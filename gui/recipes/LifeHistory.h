#pragma once

#include "Automaton.h"
#include "TorusTopology.h"

namespace ntcap {

class LifeHistory final : public Automaton {
    Q_OBJECT
public:
    explicit LifeHistory(Planes& current,
                         QObject* parent = nullptr);
    ~LifeHistory() override;

    void step() override;
    void setCurrent(Planes& current) override;
    QString name() const override;
    QString description() const override;

    std::span<const uint8_t> previous() const noexcept;

private:
    TorusTopology m_topo;
};

} // namespace ntcap