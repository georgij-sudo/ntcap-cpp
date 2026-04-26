#pragma once

#include "Automaton.h"
#include "TorusTopology.h"

namespace ntcap {

class Life final : public Automaton {
    Q_OBJECT
public:
    explicit Life(Planes& current,
                  QObject* parent = nullptr);
    ~Life() override;

    void step() override;
    QString name() const override;
    QString description() const override;

private:
    TorusTopology m_topo;
};

} // namespace ntcap