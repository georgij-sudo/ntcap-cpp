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
    /**
     * @param planes     начальное состояние (одноплоскостной Planes).
     * @param topology   топология тора.
     * @param rule       таблица правил.
     * @param parent     родительский QObject.
     */
    explicit AutomatonController(Planes planes,
                                 TorusTopology topology,
                                 TableRule<6> rule,
                                 QObject* parent = nullptr);
    ~AutomatonController() override;

    /**
     * Указатель на снапшот (копия актуального поколения).
     * Безопасен для чтения из главного потока
     * только под snapshotMutex().
     */
    const Planes* snapshot() const noexcept;

    /**
     * Мьютекс, защищающий снапшот.
     * CellularCanvas должен блокировать его при чтении.
     */
    QMutex* snapshotMutex() noexcept;

    size_t generation() const noexcept;

public slots:
    void start();
    void stop();

signals:
    void stepped();

private:
    void run();
    void step();

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
};

} // namespace ntcap