#pragma once

#include <QString>
#include "Planes.h"

namespace ntcap {

/**
 * @brief A ready-made initialisation preset for a specific recipe.
 *
 * Contains a fully populated Planes object with fixed dimensions,
 * a human-readable name and description.
 */
struct RecipePreset {
    /** Short display name (e.g. "Glider"). */
    QString name;

    /** Description shown in the selection dialog. */
    QString description;

    /** Ready-made field state. */
    Planes planes;
};

} // namespace ntcap