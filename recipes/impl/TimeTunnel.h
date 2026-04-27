#pragma once

#include "Automaton.h"
#include "TorusTopology.h"

namespace ntcap {

class TimeTunnel final : public Automaton {
    Q_OBJECT
public:
    explicit TimeTunnel(Planes& current, QObject* parent = nullptr);
    ~TimeTunnel() override;

    void step() override;
    void setCurrent(Planes& current) override;
    QString name() const override;
    QString description() const override;

private:
    TorusTopology m_topo;
};

} // namespace ntcap