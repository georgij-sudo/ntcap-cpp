#include "RecipeActions.h"
#include "RecipeChangeDialog.h"
#include "RecipeInfoDialog.h"
#include "AutomatonController.h"
#include "CellularCanvas.h"

#include <QMenu>
#include <QAction>
#include <QMessageBox>

namespace ntcap {

RecipeActions::RecipeActions(AutomatonController* controller,
                             CellularCanvas* canvas,
                             QWidget* dialogParent,
                             QObject* parent)
    : QObject(parent)
    , m_controller(controller)
    , m_canvas(canvas)
    , m_dialogParent(dialogParent)
{
    m_menu = new QMenu(tr("&Recipes"), m_dialogParent);

    m_changeAction = m_menu->addAction(tr("Change..."));
    m_loadAction   = m_menu->addAction(tr("Load TableRule..."));
    m_menu->addSeparator();
    m_infoAction   = m_menu->addAction(tr("Info"));

    m_loadAction->setEnabled(false);
    m_loadAction->setToolTip(tr("Not yet implemented"));

    connect(m_changeAction, &QAction::triggered,
            this, &RecipeActions::onChangeTriggered);
    connect(m_loadAction, &QAction::triggered,
            this, &RecipeActions::onLoadTriggered);
    connect(m_infoAction, &QAction::triggered,
            this, &RecipeActions::onInfoTriggered);
}

QMenu* RecipeActions::menu() const noexcept
{
    return m_menu;
}

void RecipeActions::onChangeTriggered()
{
    RecipeChangeDialog dlg(
        m_controller->fieldWidth(),
        m_controller->fieldHeight(),
        m_controller->numPlanes(),
        m_dialogParent);

    if (dlg.exec() != QDialog::Accepted)
        return;

    auto res = dlg.result();

    m_controller->changeRecipe(
        std::move(res.factory),
        res.width,
        res.height,
        res.numPlanes);
}

void RecipeActions::onLoadTriggered()
{
    QMessageBox::information(
        m_dialogParent,
        tr("Load TableRule"),
        tr("This feature is not yet implemented."));
}

void RecipeActions::onInfoTriggered()
{
    RecipeInfoDialog dlg(
        m_controller->automatonName(),
        m_controller->automatonDescription(),
        m_dialogParent);
    dlg.exec();
}

} // namespace ntcap