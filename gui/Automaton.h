#pragma once
#include <QObject>
#include <memory>
#include "Planes.h"
#include "Topology.h"

namespace ntcap {

class Automaton : public QObject {
    Q_OBJECT
public:
    explicit Automaton(Planes& current,
                       std::unique_ptr<Topology> topology,
                       QObject* parent = nullptr);
    ~Automaton() override;

    virtual void step() = 0;

    void commit();
    void setCurrent(Planes& current);

protected:
    Planes&         current() noexcept;
    const Planes&   current() const noexcept;
    Planes&         tmp() noexcept;
    Topology& topology() const noexcept;

private:
    Planes*                   m_current;
    std::unique_ptr<Planes>   m_tmp;
    std::unique_ptr<Topology> m_topology;
};

} // namespace ntcap