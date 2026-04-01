#pragma once
#include "Automaton.h"

namespace ntcap {

class Parity final : public Automaton {
    Q_OBJECT
public:
    explicit Parity(Planes& current,
                    std::unique_ptr<Topology> topology,
                    QObject* parent = nullptr);
    ~Parity() override;

    void step() override;
};

} // namespace ntcap