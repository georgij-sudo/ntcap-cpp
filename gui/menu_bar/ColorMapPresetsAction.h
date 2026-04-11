#pragma once

#include <QObject>
#include "ColorMapPresets.h"

class QAction;

namespace ntcap {

class CellularCanvas;

class ColorMapPresetsAction : public QObject
{
    Q_OBJECT
public:
    explicit ColorMapPresetsAction(CellularCanvas* canvas,
                                   QWidget* dialogParent,
                                   QObject* parent = nullptr);

    QAction* action() const { return m_action; }

private slots:
    void onTriggered();

private:
    QAction*         m_action       = nullptr;
    CellularCanvas*  m_canvas       = nullptr;
    QWidget*         m_dialogParent = nullptr;
};

} // namespace ntcap
