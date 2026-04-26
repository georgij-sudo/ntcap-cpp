#pragma once

#include "Automaton.h"
#include "TorusTopology.h"

namespace ntcap {

class Parity final : public Automaton {
    Q_OBJECT
public:
    explicit Parity(Planes& current,
                    QObject* parent = nullptr);
    ~Parity() override;

    void step() override;
    QString name() const override;

private:
    TorusTopology m_topo;
};

} // namespace ntcap