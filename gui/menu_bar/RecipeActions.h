#pragma once

#include <QObject>

class QAction;
class QMenu;
class QWidget;

namespace ntcap {

class AutomatonController;
class CellularCanvas;

/**
 * @brief Manages the Recipes menu: Change, Load, Info.
 */
class RecipeActions final : public QObject {
    Q_OBJECT
public:
    /**
     * @param controller     automaton controller (not owned).
     * @param canvas         cellular canvas (not owned).
     * @param dialogParent   parent widget for dialogs.
     * @param parent         QObject parent.
     */
    explicit RecipeActions(AutomatonController* controller,
                           CellularCanvas* canvas,
                           QWidget* dialogParent,
                           QObject* parent = nullptr);

    QMenu* menu() const noexcept;

private slots:
    void onChangeTriggered();
    void onLoadTriggered();
    void onInfoTriggered();

private:
    AutomatonController* m_controller   = nullptr;
    CellularCanvas*      m_canvas       = nullptr;
    QWidget*             m_dialogParent = nullptr;

    QMenu*   m_menu          = nullptr;
    QAction* m_changeAction  = nullptr;
    QAction* m_loadAction    = nullptr;
    QAction* m_infoAction    = nullptr;
};

} // namespace ntcap