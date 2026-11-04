#pragma once

#include <QObject>
#include <QPointer>

#include "KeyMappingsDialog.h"

class QAction;
class QMenu;
class QWidget;

namespace ntcap {

// Owns the top-level Help menu: the non-modal "Key Mappings..." window and the
// "About..." dialog.
class HelpActions final : public QObject {
    Q_OBJECT

public:
    explicit HelpActions(QWidget* dialogParent, QObject* parent = nullptr);

    QMenu* menu() const noexcept;

private slots:
    void showKeyMappings();
    void showAbout();

private:
    QWidget* m_dialogParent = nullptr;

    QMenu*   m_menu              = nullptr;
    QAction* m_keyMappingsAction = nullptr;
    QAction* m_aboutAction       = nullptr;

    // Single instance: re-opening raises the existing window instead of
    // creating a duplicate.
    QPointer<KeyMappingsDialog> m_keyMappingsDialog;
};

} // namespace ntcap
