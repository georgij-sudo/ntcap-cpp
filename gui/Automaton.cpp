#include "Automaton.h"
#include <cstring>

namespace ntcap {

Automaton::Automaton(Planes& current,
                     std::unique_ptr<Topology> topology,
                     QObject* parent)
    : QObject(parent)
    , m_current(&current)
    , m_tmp(std::make_unique<Planes>(current.width(),
                                     current.height(),
                                     1))
    , m_topology(std::move(topology))
{
}

Automaton::~Automaton() = default;

void Automaton::commit()
{
    std::memcpy(m_current->data(),
                m_tmp->data(),
                m_current->size());
}

void Automaton::setCurrent(Planes& current)
{
    m_current = &current;
    m_tmp = std::make_unique<Planes>(current.width(),
                                     current.height(),
                                     1);
}

Planes& Automaton::current() noexcept
{
    return *m_current;
}

const Planes& Automaton::current() const noexcept
{
    return *m_current;
}

Planes& Automaton::tmp() noexcept
{
    return *m_tmp;
}

Topology& Automaton::topology() const noexcept
{
    return *m_topology;
}

} // namespace ntcap