#pragma once

#include <QWidget>
#include <memory>

namespace ntcap {

class Automaton;
class Planes;

/**
 * @brief Base class for recipe-specific parameter editors.
 *
 * Subclasses add their own controls and implement createAutomaton()
 * that captures the current widget state.
 */
class RecipeParameterWidget : public QWidget {
    Q_OBJECT
public:
    explicit RecipeParameterWidget(QWidget* parent = nullptr);
    ~RecipeParameterWidget() override;

    /**
     * @brief Create an Automaton with the current parameter values.
     * @param planes  reference to the active Planes (owned by controller).
     */
    virtual std::unique_ptr<Automaton> createAutomaton(Planes& planes) = 0;
};

} // namespace ntcap