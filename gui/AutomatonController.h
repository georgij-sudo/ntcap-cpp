#pragma once
#include <QObject>
#include <QThread>
#include <QString>
#include <atomic>
#include <memory>
#include <functional>
#include "Planes.h"
#include "Automaton.h"
#include "TripleBuffer.h"

namespace ntcap {

/**
 * @brief Factory that creates an Automaton bound to the given Planes.
 */
using AutomatonFactory = std::function<std::unique_ptr<Automaton>(Planes&)>;

class AutomatonController final : public QObject {
    Q_OBJECT
public:
    explicit AutomatonController(Planes planes,
                                 std::unique_ptr<Automaton> automaton,
                                 QObject* parent = nullptr);
    ~AutomatonController() override;

    TripleBuffer<Planes>& tripleBuffer() noexcept { return *m_triple; }

    size_t generation()     const noexcept;
    size_t renderInterval() const noexcept;
    size_t stepDelayMs()    const noexcept;
    bool   isRunning()      const noexcept;

    /**
     * @brief Name of the current automaton recipe (GUI-thread safe).
     */
    QString automatonName() const noexcept;

    /**
     * @brief HTML description of the current automaton.
     */
    QString automatonDescription() const noexcept;

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

    /**
     * @brief Change the recipe, optionally resizing the field.
     *
     * @param factory     creates the new Automaton bound to internal Planes.
     * @param width       desired field width (0 = keep current).
     * @param height      desired field height (0 = keep current).
     * @param numPlanes   desired plane count (0 = keep current).
     */
    void changeRecipe(AutomatonFactory factory,
                      size_t width = 0,
                      size_t height = 0,
                      size_t numPlanes = 0);

public slots:
    void start();
    void stop();
    void stepOnce();
    void setRenderInterval(size_t n);
    void skipSteps(size_t n);
    void setStepDelay(size_t ms);
    void resetPlanes(Planes planes);

    /**
     * @brief Swap two planes by index. Thread-safe (deferred to worker).
     */
    void requestSwapPlanes(size_t a, size_t b);

signals:
    void planesReset(const Planes& planes);
    void recipeChanged();

private:
    void run();
    bool shouldRender(size_t gen) noexcept;
    void publishSnapshot();
    void cacheAutomatonInfo();
    void rebuildTripleBuffer();

    Planes                              m_current;
    std::unique_ptr<Automaton>          m_automaton;
    std::unique_ptr<TripleBuffer<Planes>> m_triple;

    QThread                    m_thread;
    std::atomic<bool>          m_running{false};
    std::atomic<size_t>        m_generation{0};
    std::atomic<size_t>        m_renderInterval{1};
    std::atomic<size_t>        m_skipRemaining{0};
    std::atomic<size_t>        m_stepDelayMs{0};

    struct SwapRequest {
        std::atomic<bool> pending{false};
        size_t a = 0;
        size_t b = 0;
    } m_swapRequest;

    QString m_cachedAutomatonName;
    QString m_cachedDescription;
};

} // namespace ntcap