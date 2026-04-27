#pragma once

#include <QObject>
#include "TubeWorms.h"

namespace ntcap {

/**
 * @brief StableTubeWorms — TubeWorms with predefined alarm mask.
 */
class StableTubeWorms final : public TubeWorms {
    Q_OBJECT
public:
    explicit StableTubeWorms(Planes& current, QObject* parent = nullptr);

    QString name() const override;
    QString description() const override;
};

} // namespace ntcap