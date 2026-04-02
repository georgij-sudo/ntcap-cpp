#pragma once

#include <QObject>
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

    void commit();
    void setCurrent(Planes& current);

protected:
    Planes&       current() noexcept;
    const Planes& current() const noexcept;
    Planes&       tmp() noexcept;

private:
    Planes*                 m_current;
    std::unique_ptr<Planes> m_tmp;
};

} // namespace ntcap