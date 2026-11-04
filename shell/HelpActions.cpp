#include "HelpActions.h"

#include "AboutDialog.h"

#include <QAction>
#include <QMenu>

namespace ntcap {

HelpActions::HelpActions(QWidget* dialogParent, QObject* parent)
    : QObject(parent)
    , m_dialogParent(dialogParent)
{
    m_menu = new QMenu(tr("Help"), dialogParent);

    m_keyMappingsAction = m_menu->addAction(tr("Key Mappings\u2026"));
    connect(m_keyMappingsAction, &QAction::triggered,
            this, &HelpActions::showKeyMappings);

    m_aboutAction = m_menu->addAction(tr("About\u2026"));
    connect(m_aboutAction, &QAction::triggered,
            this, &HelpActions::showAbout);
}

QMenu* HelpActions::menu() const noexcept
{
    return m_menu;
}

void HelpActions::showKeyMappings()
{
    if (!m_keyMappingsDialog) {
        m_keyMappingsDialog = new KeyMappingsDialog(m_dialogParent);
    }

    m_keyMappingsDialog->show();
    m_keyMappingsDialog->raise();
    m_keyMappingsDialog->activateWindow();
}

void HelpActions::showAbout()
{
    AboutDialog dialog(m_dialogParent);
    dialog.exec();
}

} // namespace ntcap
