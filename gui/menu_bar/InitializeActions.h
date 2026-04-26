#pragma once

#include <QObject>
#include <functional>

#include "Planes.h"

class QAction;
class QMenu;
class QWidget;

namespace ntcap {

class AutomatonController;

/**
 * @brief Manages the Planes → Initialize… submenu.
 *
 * Items: This Recipe… (stub), Random…, Square…, Circle….
 */
class InitializeActions final : public QObject {
    Q_OBJECT
public:
    using PlanesProvider = std::function<Planes()>;

    /**
     * @param controller     automaton controller (not owned).
     * @param provider       callable returning a snapshot of current Planes.
     * @param dialogParent   parent widget for dialogs.
     * @param parent         QObject parent.
     */
    explicit InitializeActions(AutomatonController* controller,
                               PlanesProvider provider,
                               QWidget* dialogParent,
                               QObject* parent = nullptr);

    QMenu* menu() const noexcept;

private slots:
    void onRandomTriggered();
    void onSquareTriggered();
    void onCircleTriggered();

private:
    AutomatonController* m_controller   = nullptr;
    PlanesProvider       m_provider;
    QWidget*             m_dialogParent = nullptr;

    QMenu*   m_menu          = nullptr;
    QAction* m_recipeAction  = nullptr;
    QAction* m_randomAction  = nullptr;
    QAction* m_squareAction  = nullptr;
    QAction* m_circleAction  = nullptr;
};

} // namespace ntcap