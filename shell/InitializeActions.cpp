#include "InitializeActions.h"
#include "AutomatonController.h"
#include "PlaneInitializer.h"
#include "RandomInitDialog.h"
#include "SquareInitDialog.h"
#include "CircleInitDialog.h"
#include "RecipePresetDialog.h"
#include "RecipePresetRegistry.h"

#include <QMenu>
#include <QAction>
#include <QMessageBox>

#include <random>

namespace ntcap {

InitializeActions::InitializeActions(
    AutomatonController* controller,
    PlanesProvider provider,
    QWidget* dialogParent,
    QObject* parent)
    : QObject(parent)
    , m_controller(controller)
    , m_provider(std::move(provider))
    , m_dialogParent(dialogParent)
{
    std::random_device rd;
    m_randomState.fillParams.seed = rd() & 0x7FFFFFFFu;

    m_menu = new QMenu(tr("Initialize..."), m_dialogParent);

    m_recipeAction = m_menu->addAction(tr("This Recipe..."));
    m_menu->addSeparator();
    m_randomAction = m_menu->addAction(tr("Random..."));
    m_squareAction = m_menu->addAction(tr("Square..."));
    m_circleAction = m_menu->addAction(tr("Circle..."));

    connect(m_recipeAction, &QAction::triggered,
            this, &InitializeActions::onRecipeTriggered);
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

void InitializeActions::onRecipeTriggered()
{
    QString recipeName = m_controller->automatonName();
    auto presets = RecipePresetRegistry::presetsFor(recipeName);

    if (presets.isEmpty()) {
        QMessageBox::information(
            m_dialogParent,
            tr("This Recipe"),
            tr("No presets available for \"%1\".")
                .arg(recipeName));
        return;
    }

    RecipePresetDialog dlg(recipeName, presets, m_dialogParent);
    if (dlg.exec() != QDialog::Accepted)
        return;

    int idx = dlg.selectedIndex();
    if (idx < 0 || idx >= presets.size())
        return;

    bool wasRunning = m_controller->isRunning();
    if (wasRunning)
        m_controller->stop();

    m_controller->resetPlanes(std::move(presets[idx].planes));

    if (wasRunning)
        m_controller->start();
}

void InitializeActions::onRandomTriggered()
{
    Planes snapshot = m_provider();

    RandomInitDialog dlg(
        snapshot.numPlanes(), m_randomState, m_dialogParent);

    if (dlg.exec() != QDialog::Accepted)
        return;

    m_randomState = dlg.state();

    if (m_randomState.allPlanes) {
        PlaneInitializer::randomFillAll(
            snapshot, m_randomState.fillParams);
    } else {
        PlaneInitializer::randomFill(
            snapshot,
            m_randomState.planeIndex,
            m_randomState.fillParams);
    }

    bool wasRunning = m_controller->isRunning();
    if (wasRunning)
        m_controller->stop();

    m_controller->resetPlanes(std::move(snapshot));

    if (wasRunning)
        m_controller->start();
}

void InitializeActions::onSquareTriggered()
{
    Planes snapshot = m_provider();

    SquareInitDialog dlg(
        snapshot.width(), snapshot.height(),
        snapshot.numPlanes(), m_dialogParent);

    if (dlg.exec() != QDialog::Accepted)
        return;

    PlaneInitializer::fillSquare(
        snapshot, dlg.planeIndex(),
        dlg.squareWidth(), dlg.squareHeight(),
        dlg.fillValue());

    bool wasRunning = m_controller->isRunning();
    if (wasRunning)
        m_controller->stop();

    m_controller->resetPlanes(std::move(snapshot));

    if (wasRunning)
        m_controller->start();
}

void InitializeActions::onCircleTriggered()
{
    Planes snapshot = m_provider();

    CircleInitDialog dlg(
        snapshot.width(), snapshot.height(),
        snapshot.numPlanes(), m_dialogParent);

    if (dlg.exec() != QDialog::Accepted)
        return;

    PlaneInitializer::fillCircle(
        snapshot, dlg.planeIndex(),
        dlg.diameterX(), dlg.diameterY(),
        dlg.fillValue());

    bool wasRunning = m_controller->isRunning();
    if (wasRunning)
        m_controller->stop();

    m_controller->resetPlanes(std::move(snapshot));

    if (wasRunning)
        m_controller->start();
}

} // namespace ntcap