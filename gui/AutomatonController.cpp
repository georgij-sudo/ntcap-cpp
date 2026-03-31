#include "AutomatonController.h"
#include "VonNeumannN.h"
#include "Coord2D.h"
#include "UCoord2D.h"
#include <QMutexLocker>
#include <cstring>

namespace ntcap {

// ─── Конструктор / деструктор ───────────────────────────────────────

AutomatonController::AutomatonController(Planes planes,
                                         TorusTopology topology,
                                         TableRule<6> rule,
                                         QObject* parent)
    : QObject(parent)
    , m_current(std::make_unique<Planes>(std::move(planes)))
    , m_tmp(std::make_unique<Planes>(m_current->width(),
                                     m_current->height(),
                                     1))
    , m_snapshot(std::make_unique<Planes>(m_current->width(),
                                         m_current->height(),
                                         1))
    , m_topology(std::move(topology))
    , m_rule(std::move(rule))
{
    std::memcpy(m_snapshot->data(),
                m_current->data(),
                m_current->size());
}

AutomatonController::~AutomatonController()
{
    stop();
}

// ─── Геттеры ────────────────────────────────────────────────────────

const Planes* AutomatonController::snapshot() const noexcept
{
    return m_snapshot.get();
}

QMutex* AutomatonController::snapshotMutex() noexcept
{
    return &m_snapshotMutex;
}

size_t AutomatonController::generation() const noexcept
{
    return m_generation.load(std::memory_order_relaxed);
}

size_t AutomatonController::renderInterval() const noexcept
{
    return m_renderInterval.load(std::memory_order_relaxed);
}

// ─── Пропуск отрисовки ─────────────────────────────────────────────

void AutomatonController::setRenderInterval(size_t n)
{
    m_renderInterval.store(n < 1 ? 1 : n, std::memory_order_relaxed);
}

void AutomatonController::skipSteps(size_t n)
{
    m_skipRemaining.fetch_add(n, std::memory_order_relaxed);
}

// ─── Управление потоком ─────────────────────────────────────────────

void AutomatonController::start()
{
    if (m_running.load())
        return;

    m_running.store(true);

    QObject::connect(&m_thread, &QThread::started,
                     this,      &AutomatonController::run,
                     Qt::DirectConnection);
    m_thread.start();
}

void AutomatonController::stop()
{
    if (!m_running.load())
        return;

    m_running.store(false);
    m_thread.quit();
    m_thread.wait();

    disconnect(&m_thread, &QThread::started,
               this,      &AutomatonController::run);
}

// ─── Рабочий цикл (выполняется в m_thread) ─────────────────────────

void AutomatonController::run()
{
    while (m_running.load(std::memory_order_relaxed)) {
        // 1. Вычислить следующее поколение.
        step();

        // 2. Поменять буферы местами.
        m_current.swap(m_tmp);

        // 3. Инкремент поколения (всегда, даже при пропуске).
        const size_t gen =
            m_generation.fetch_add(1, std::memory_order_relaxed) + 1;

        // 4. Решить, нужна ли отрисовка.
        if (!shouldRender(gen))
            continue;

        // 5. Скопировать результат в снапшот.
        {
            QMutexLocker lock(&m_snapshotMutex);
            std::memcpy(m_snapshot->data(),
                        m_current->data(),
                        m_current->size());
        }

        // 6. Уведомить главный поток.
        emit stepped();
    }
}

// ─── Проверка: нужна ли отрисовка на этом шаге ─────────────────────

bool AutomatonController::shouldRender(size_t gen) noexcept
{
    // Приоритет 1: разовый пропуск (skipSteps).
    // CAS-цикл: атомарно декрементируем, если > 0.
    size_t skip = m_skipRemaining.load(std::memory_order_relaxed);
    while (skip > 0) {
        if (m_skipRemaining.compare_exchange_weak(
                skip, skip - 1, std::memory_order_relaxed)) {
            return false;          // пропускаем этот шаг
        }
        // skip обновлён compare_exchange_weak при неудаче — повторяем
    }

    // Приоритет 2: постоянный интервал.
    const size_t interval = m_renderInterval.load(std::memory_order_relaxed);
    if (interval > 1 && (gen % interval) != 0)
        return false;

    return true;
}

// ─── Один шаг симуляции ─────────────────────────────────────────────

void AutomatonController::step()
{
    const size_t W = m_current->width();
    const size_t H = m_current->height();

    const uint8_t* cur  = m_current->data();
    uint8_t*       next = m_tmp->data();

    for (size_t y = 0; y < H; ++y) {
        for (size_t x = 0; x < W; ++x) {
            const auto px = static_cast<ptrdiff_t>(x);
            const auto py = static_cast<ptrdiff_t>(y);

            const UCoord2D nc = m_topology.normalize({px,     py - 1});
            const UCoord2D ec = m_topology.normalize({px + 1, py    });
            const UCoord2D sc = m_topology.normalize({px,     py + 1});
            const UCoord2D wc = m_topology.normalize({px - 1, py    });

            const size_t idx = y * W + x;

            const VonNeumannN neighborhood = VonNeumannNBuilder()
                .center(cur[idx])
                .north (cur[nc.y * W + nc.x])
                .east  (cur[ec.y * W + ec.x])
                .south (cur[sc.y * W + sc.x])
                .west  (cur[wc.y * W + wc.x])
                .build();

            next[idx] = m_rule(neighborhood.count());
        }
    }
}

} // namespace ntcap