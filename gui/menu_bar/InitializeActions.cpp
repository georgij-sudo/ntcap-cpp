#include "InitializeActions.h"
#include "AutomatonController.h"
#include "PlaneInitializer.h"
#include "RandomInitDialog.h"
#include "SquareInitDialog.h"
#include "CircleInitDialog.h"

#include <QAction>
#include <QMenu>

namespace ntcap {

InitializeActions::InitializeActions(AutomatonController* controller,
                                     PlanesProvider provider,
                                     QWidget* dialogParent,
                                     QObject* parent)
    : QObject(parent)
    , m_controller(controller)
    , m_provider(std::move(provider))
    , m_dialogParent(dialogParent)
{
    m_menu = new QMenu(tr("Initialize"), m_dialogParent);

    m_recipeAction = m_menu->addAction(tr("This Recipe..."));
    m_recipeAction->setEnabled(false);
    m_recipeAction->setToolTip(tr("Not implemented yet"));

    m_menu->addSeparator();

    m_randomAction = m_menu->addAction(tr("Random Fill..."));
    m_squareAction = m_menu->addAction(tr("Square..."));
    m_circleAction = m_menu->addAction(tr("Circle..."));

    connect(m_randomAction, &QAction::triggered,
            this, &InitializeActions::onRandomTriggered);
    connect(m_squareAction, &QAction::triggered,
            this, &InitializeActions::onSquareTriggered);
    connect(m_circleAction, &QAction::triggered,
            this, &InitializeActions::onCircleTriggered);
}

QMenu* InitializeActions::menu() const noexcept
{
    return m_menu;
}

void InitializeActions::onRandomTriggered()
{
    const size_t np = m_controller->numPlanes();

    RandomInitDialog dlg(np, m_dialogParent);
    if (dlg.exec() != QDialog::Accepted)
        return;

    Planes snapshot = m_provider();

    if (dlg.allPlanes()) {
        PlaneInitializer::randomFillAll(snapshot, dlg.density(), dlg.fillValue());
    } else {
        PlaneInitializer::randomFill(snapshot, dlg.planeIndex(),
                                     dlg.density(), dlg.fillValue());
    }

    m_controller->resetPlanes(std::move(snapshot));
}

void InitializeActions::onSquareTriggered()
{
    const size_t fw = m_controller->fieldWidth();
    const size_t fh = m_controller->fieldHeight();
    const size_t np = m_controller->numPlanes();

    SquareInitDialog dlg(fw, fh, np, m_dialogParent);
    if (dlg.exec() != QDialog::Accepted)
        return;

    Planes snapshot = m_provider();

    PlaneInitializer::fillSquare(snapshot, dlg.planeIndex(),
                                 dlg.squareWidth(), dlg.squareHeight(),
                                 dlg.fillValue());

    m_controller->resetPlanes(std::move(snapshot));
}

void InitializeActions::onCircleTriggered()
{
    const size_t fw = m_controller->fieldWidth();
    const size_t fh = m_controller->fieldHeight();
    const size_t np = m_controller->numPlanes();

    CircleInitDialog dlg(fw, fh, np, m_dialogParent);
    if (dlg.exec() != QDialog::Accepted)
        return;

    Planes snapshot = m_provider();

    PlaneInitializer::fillCircle(snapshot, dlg.planeIndex(),
                                 dlg.diameterX(), dlg.diameterY(),
                                 dlg.fillValue());

    m_controller->resetPlanes(std::move(snapshot));
}

} // namespace ntcap