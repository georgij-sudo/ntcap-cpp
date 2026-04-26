#pragma once

#include <QString>
#include <functional>
#include <memory>

class QWidget;

namespace ntcap {

class Automaton;
class Planes;
class RecipeParameterWidget;

/**
 * @brief Metadata for a single automaton recipe.
 */
struct RecipeDescriptor {
    /** Display name shown in the recipe list. */
    QString name;

    /** HTML description for the Info dialog. */
    QString description;

    /** Default number of planes required by this recipe. */
    size_t defaultNumPlanes = 1;

    /**
     * @brief Factory for the parameter widget.
     *
     * Returns nullptr for parameterless recipes (a default "No parameters"
     * widget is shown instead).
     */
    std::function<RecipeParameterWidget*(QWidget* parent)> createWidget;

    /**
     * @brief Simple factory for parameterless recipes.
     *
     * Ignored when createWidget returns a non-null widget
     * (the widget's own createAutomaton is used instead).
     */
    std::function<std::unique_ptr<Automaton>(Planes&)> factory;
};

} // namespace ntcap