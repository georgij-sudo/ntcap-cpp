#pragma once

#include <QObject>
#include "TubeWorms.h"

namespace ntcap {

/**
 * @brief StableTubeWorms — TubeWorms with predefined alarm mask.
 *
 * Constructor signature matches LifeHistory: StableTubeWorms(Planes& current, QObject* parent = nullptr).
 */
class StableTubeWorms final : public TubeWorms {
    Q_OBJECT
public:
    /**
     * @brief Construct StableTubeWorms bound to externally managed Planes.
     *
     * @param current Planes reference (must have numPlanes() >= 3).
     * @param parent optional QObject parent.
     */
    explicit StableTubeWorms(Planes& current, QObject* parent = nullptr);
};

} // namespace ntcap