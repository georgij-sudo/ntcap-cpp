//menu_bar/LoadPlanesAction.h
#pragma once

#include <QObject>

#include "Planes.h"

class QAction;
class QWidget;

namespace ntcap {

class LoadPlanesAction final : public QObject {
    Q_OBJECT
public:
    explicit LoadPlanesAction(QWidget* dialogParent,
                              QObject* parent = nullptr);

    QAction* action() const noexcept;

signals:
    void planesLoaded(const ntcap::Planes& planes);

private slots:
    void onTriggered();

private:
    QWidget* m_dialogParent;
    QAction* m_action;
};

} // namespace ntcap