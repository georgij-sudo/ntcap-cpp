#pragma once
#include <QObject>
#include <QThread>
#include <QMutex>
#include <atomic>
#include <memory>
#include "Planes.h"
#include "Automaton.h"

namespace ntcap {

class AutomatonController final : public QObject {
    Q_OBJECT
public:
    explicit AutomatonController(Planes planes,
                                 std::unique_ptr<Automaton> automaton,
                                 QObject* parent = nullptr);
    ~AutomatonController() override;

    const Planes* snapshot() const noexcept;
    QMutex*       snapshotMutex() noexcept;
    size_t        generation() const noexcept;
    size_t        renderInterval() const noexcept;
    size_t        stepDelayMs() const noexcept;

    void setAutomaton(std::unique_ptr<Automaton> automaton);

public slots:
    void start();
    void stop();
    void setRenderInterval(size_t n);
    void skipSteps(size_t n);
    void setStepDelay(size_t ms);

signals:
    void stepped();

private:
    void run();
    bool shouldRender(size_t gen) noexcept;

    Planes                     m_current;
    std::unique_ptr<Automaton> m_automaton;

    std::unique_ptr<Planes>    m_snapshot;
    mutable QMutex             m_snapshotMutex;

    QThread                    m_thread;
    std::atomic<bool>          m_running{false};
    std::atomic<size_t>        m_generation{0};

    std::atomic<size_t>        m_renderInterval{1};
    std::atomic<size_t>        m_skipRemaining{0};
    std::atomic<size_t>        m_stepDelayMs{0};
};

} // namespace ntcap