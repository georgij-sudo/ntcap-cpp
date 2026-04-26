#include "Automaton.h"

namespace ntcap {

Automaton::Automaton(Planes& current, QObject* parent)
    : QObject(parent)
    , m_current(&current)
    , m_tmp(std::make_unique<Planes>(current.width(),
                                     current.height(),
                                     current.numPlanes())) {}

Automaton::~Automaton() = default;

QString Automaton::name() const
{
    return QStringLiteral("Unknown");
}

QString Automaton::description() const
{
    return QString();
}

void Automaton::commit()
{
    std::swap(*m_current, *m_tmp);
}

void Automaton::setCurrent(Planes& current)
{
    m_current = &current;
    m_tmp = std::make_unique<Planes>(current.width(),
                                     current.height(),
                                     current.numPlanes());
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

} // namespace ntcap