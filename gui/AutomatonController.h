#pragma once
#include <QObject>
#include <QThread>
#include <QString>
#include <atomic>
#include <memory>
#include "Planes.h"
#include "Automaton.h"
#include "TripleBuffer.h"

namespace ntcap {

class AutomatonController final : public QObject {
    Q_OBJECT
public:
    explicit AutomatonController(Planes planes,
                                 std::unique_ptr<Automaton> automaton,
                                 QObject* parent = nullptr);
    ~AutomatonController() override;

    TripleBuffer<Planes>& tripleBuffer() noexcept { return m_triple; }

    size_t generation()     const noexcept;
    size_t renderInterval() const noexcept;
    size_t stepDelayMs()    const noexcept;
    bool   isRunning()      const noexcept;

    /**
     * @brief Name of the current automaton recipe (GUI-thread safe).
     */
    QString automatonName() const noexcept;

    /**
     * @brief Field width of the current planes.
     */
    size_t fieldWidth() const noexcept;

    /**
     * @brief Field height of the current planes.
     */
    size_t fieldHeight() const noexcept;

    /**
     * @brief Number of planes in the current configuration.
     */
    size_t numPlanes() const noexcept;

    void setAutomaton(std::unique_ptr<Automaton> automaton);

public slots:
    void start();
    void stop();
    void stepOnce();
    void setRenderInterval(size_t n);
    void skipSteps(size_t n);
    void setStepDelay(size_t ms);
    void resetPlanes(Planes planes);

signals:
    void planesReset(const Planes& planes);

private:
    void run();
    bool shouldRender(size_t gen) noexcept;
    void publishSnapshot();
    void cacheAutomatonName();

    Planes                     m_current;
    std::unique_ptr<Automaton> m_automaton;
    TripleBuffer<Planes>       m_triple;

    QThread                    m_thread;
    std::atomic<bool>          m_running{false};
    std::atomic<size_t>        m_generation{0};
    std::atomic<size_t>        m_renderInterval{1};
    std::atomic<size_t>        m_skipRemaining{0};
    std::atomic<size_t>        m_stepDelayMs{0};

    QString m_cachedAutomatonName;
};

} // namespace ntcap