#pragma once

#include "Automaton.h"

namespace ntcap {

class Life final : public Automaton {
    Q_OBJECT
public:
    explicit Life(Planes& current,
                  std::unique_ptr<Topology> topology,
                  QObject* parent = nullptr);
    ~Life() override;

    void step() override;
};

} // namespace ntcap