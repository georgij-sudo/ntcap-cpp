#pragma once

#include <QObject>
#include <functional>

#include "Planes.h"

class QAction;
class QWidget;

namespace ntcap {

class SavePlanesAction final : public QObject {
    Q_OBJECT
public:
    using PlanesProvider = std::function<Planes()>;

    explicit SavePlanesAction(PlanesProvider provider,
                              QWidget* dialogParent,
                              QObject* parent = nullptr);

    QAction* action() const noexcept;

private slots:
    void onTriggered();

private:
    PlanesProvider m_provider;
    QWidget*       m_dialogParent;
    QAction*       m_action;
};

} // namespace ntcap