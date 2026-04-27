#pragma once

#include <QObject>
#include <QString>
#include <atomic>
#include <cstdint>
#include <memory>
#include "Planes.h"

namespace ntcap {

class Automaton : public QObject {
    Q_OBJECT
public:
    explicit Automaton(Planes& current,
                       QObject* parent = nullptr);
    ~Automaton() override;

    virtual void step() = 0;

    /**
     * @brief Human-readable name of the automaton recipe.
     */
    virtual QString name() const;

    /**
     * @brief HTML description of the automaton for the Info dialog.
     */
    virtual QString description() const;

    void commit();

    /**
     * @brief Rebind the automaton to a (possibly resized) Planes.
     *
     * Subclasses that cache geometry (e.g. TorusTopology) must override
     * this to rebuild their caches.  Always call the base implementation.
     */
    virtual void setCurrent(Planes& current);

    /**
     * @brief Set the RNG seed for stochastic automata.
     *
     * Subclasses that use randomness in step() should override this
     * to reseed their RNG.  The base implementation stores the value
     * atomically so it can be read from the worker thread.
     *
     * @param seed  new seed value.
     */
    virtual void setSeed(uint32_t seed);

    /**
     * @brief Current seed value (thread-safe).
     */
    uint32_t seed() const noexcept;

protected:
    Planes&       current() noexcept;
    const Planes& current() const noexcept;
    Planes&       tmp() noexcept;

private:
    Planes*                 m_current;
    std::unique_ptr<Planes> m_tmp;
    std::atomic<uint32_t>   m_seed{0};
};

} // namespace ntcap