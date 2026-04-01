// AutomatonController.cpp
#include "AutomatonController.h"
#include <cstring>
#include <algorithm>

namespace ntcap {

AutomatonController::AutomatonController(Planes planes,
                                         std::unique_ptr<Automaton> automaton,
                                         QObject* parent)
    : QObject(parent)
    , m_current(std::move(planes))
    , m_automaton(std::move(automaton))
    , m_triple(m_current)             // три копии начального состояния
{
    m_automaton->setCurrent(m_current);
}

AutomatonController::~AutomatonController() { stop(); }

size_t AutomatonController::generation()     const noexcept { return m_generation.load(std::memory_order_relaxed); }
size_t AutomatonController::renderInterval() const noexcept { return m_renderInterval.load(std::memory_order_relaxed); }
size_t AutomatonController::stepDelayMs()    const noexcept { return m_stepDelayMs.load(std::memory_order_relaxed); }
bool   AutomatonController::isRunning()      const noexcept { return m_running.load(); }

void AutomatonController::setRenderInterval(size_t n) { m_renderInterval.store(n < 1 ? 1 : n, std::memory_order_relaxed); }
void AutomatonController::skipSteps(size_t n)         { m_skipRemaining.fetch_add(n, std::memory_order_relaxed); }
void AutomatonController::setStepDelay(size_t ms)     { m_stepDelayMs.store(ms, std::memory_order_relaxed); }

void AutomatonController::setAutomaton(std::unique_ptr<Automaton> automaton)
{
    m_automaton = std::move(automaton);
    m_automaton->setCurrent(m_current);
}

void AutomatonController::resetPlanes(Planes planes)
{
    m_current = std::move(planes);
    m_generation = 0;

    // Публикуем через triple buffer — без мьютекса
    std::memcpy(m_triple.writeBuffer().data(),
                m_current.data(),
                m_current.size());
    m_triple.publish();
}

// ─── stepOnce (вызывается из GUI, когда симуляция остановлена) ───
void AutomatonController::stepOnce()
{
    m_automaton->step();
    m_automaton->commit();
    m_generation.fetch_add(1, std::memory_order_relaxed);

    std::memcpy(m_triple.writeBuffer().data(),
                m_current.data(),
                m_current.size());
    m_triple.publish();

    emit stepped();
}

// ─── start / stop ───────────────────────────────────────────────
void AutomatonController::start()
{
    if (m_running.load()) return;
    if (!m_automaton) return;
    m_running.store(true);
    QObject::connect(&m_thread, &QThread::started,
                     this,      &AutomatonController::run,
                     Qt::DirectConnection);
    m_thread.start();
}

void AutomatonController::stop()
{
    if (!m_running.load()) return;
    m_running.store(false);
    m_thread.quit();
    m_thread.wait();
    disconnect(&m_thread, &QThread::started,
               this,      &AutomatonController::run);
}

// ─── Главный цикл (worker thread) ──────────────────────────────
void AutomatonController::run()
{
    while (m_running.load(std::memory_order_relaxed)) {
        // Delay
        const size_t delay = m_stepDelayMs.load(std::memory_order_relaxed);
        if (delay > 0) {
            constexpr size_t kChunkMs = 50;
            size_t waited = 0;
            while (waited < delay && m_running.load(std::memory_order_relaxed)) {
                QThread::msleep(static_cast<unsigned long>(
                    std::min(kChunkMs, delay - waited)));
                waited += std::min(kChunkMs, delay - waited);
            }
            if (!m_running.load(std::memory_order_relaxed)) break;
        }

        // Step
        m_automaton->step();
        m_automaton->commit();
        const size_t gen = m_generation.fetch_add(1, std::memory_order_relaxed) + 1;

        if (!shouldRender(gen))
            continue;

        // ── Публикация: memcpy в write-буфер + атомарный swap ──
        std::memcpy(m_triple.writeBuffer().data(),
                    m_current.data(),
                    m_current.size());
        m_triple.publish();   // lock-free, ~наносекунды

        emit stepped();
    }
}

bool AutomatonController::shouldRender(size_t gen) noexcept
{
    size_t skip = m_skipRemaining.load(std::memory_order_relaxed);
    while (skip > 0) {
        if (m_skipRemaining.compare_exchange_weak(
                skip, skip - 1, std::memory_order_relaxed))
            return false;
    }
    const size_t interval = m_renderInterval.load(std::memory_order_relaxed);
    return interval <= 1 || (gen % interval) == 0;
}

} // namespace ntcap