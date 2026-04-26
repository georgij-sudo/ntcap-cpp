#pragma once

#include "RecipeDescriptor.h"
#include <QList>

namespace ntcap {

/**
 * @brief Central registry of all known recipes.
 *
 * Returns a list of RecipeDescriptor — no singletons, no global state.
 */
class RecipeRegistry {
public:
    RecipeRegistry() = delete;

    /**
     * @brief All built-in recipes.
     */
    static QList<RecipeDescriptor> builtIn();
};

} // namespace ntcap