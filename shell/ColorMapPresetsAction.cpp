#include "ColorMapPresetsAction.h"
#include "ColorMapPresetsDialog.h"
#include "CellularCanvas.h"

#include <QAction>

namespace ntcap {

ColorMapPresetsAction::ColorMapPresetsAction(CellularCanvas* canvas,
                                             QWidget* dialogParent,
                                             QObject* parent)
    : QObject(parent)
    , m_canvas(canvas)
    , m_dialogParent(dialogParent)
{
    m_action = new QAction(tr("Color Map\u2026"), this);
    connect(m_action, &QAction::triggered,
            this,     &ColorMapPresetsAction::onTriggered);
}

void ColorMapPresetsAction::onTriggered()
{
    ColorMapPresetsDialog dlg(m_dialogParent);
    if (dlg.exec() == QDialog::Accepted) {
        ColorMap map = dlg.selectedColorMap();
        if (!map.isEmpty())
            m_canvas->setColorMap(map);
    }
}

} // namespace ntcap