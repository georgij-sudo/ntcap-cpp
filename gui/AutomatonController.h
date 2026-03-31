#pragma once
#include <QObject>
#include <QThread>
#include <QMutex>
#include <atomic>
#include <memory>
#include "Planes.h"
#include "TorusTopology.h"
#include "TableRule.h"

namespace ntcap {

class AutomatonController final : public QObject {
    Q_OBJECT
public:
    explicit AutomatonController(Planes planes,
                                 TorusTopology topology,
                                 TableRule<6> rule,
                                 QObject* parent = nullptr);
    ~AutomatonController() override;

    const Planes* snapshot() const noexcept;
    QMutex*       snapshotMutex() noexcept;
    size_t        generation() const noexcept;

    size_t renderInterval() const noexcept;

public slots:
    void start();
    void stop();

    /**
     * Постоянная настройка: отрисовывать каждый n-й шаг.
     * Можно менять на лету. n == 0 трактуется как 1.
     */
    void setRenderInterval(size_t n);

    /**
     * Разовая команда: пропустить следующие n шагов без отрисовки.
     * Вызовы аддитивны: skipSteps(5) + skipSteps(3) = пропустить 8.
     * Пока счётчик > 0, renderInterval не учитывается.
     */
    void skipSteps(size_t n);

signals:
    void stepped();

private:
    void run();
    void step();
    bool shouldRender(size_t gen) noexcept;

    // ── Поля (только рабочий поток) ─────────────────────
    std::unique_ptr<Planes> m_current;
    std::unique_ptr<Planes> m_tmp;

    // ── Снапшот (общий, под мьютексом) ──────────────────
    std::unique_ptr<Planes> m_snapshot;
    mutable QMutex          m_snapshotMutex;

    // ── Правило и топология ─────────────────────────────
    TorusTopology m_topology;
    TableRule<6>  m_rule;

    // ── Поток ───────────────────────────────────────────
    QThread             m_thread;
    std::atomic<bool>   m_running{false};
    std::atomic<size_t> m_generation{0};

    // ── Пропуск отрисовки ───────────────────────────────
    std::atomic<size_t> m_renderInterval{1};
    std::atomic<size_t> m_skipRemaining{0};
};

} // namespace ntcap