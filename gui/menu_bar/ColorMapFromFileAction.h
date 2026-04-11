#pragma once

#include <QObject>
#include "ColorMapPresets.h"

class QAction;

namespace ntcap {

class CellularCanvas;

class ColorMapFromFileAction : public QObject
{
    Q_OBJECT
public:
    explicit ColorMapFromFileAction(CellularCanvas* canvas,
                                    QWidget* dialogParent,
                                    QObject* parent = nullptr);

    QAction* action() const { return m_action; }

    /// Парсит CSV → ColorMap.  Формат строки: state,r,g,b
    static ColorMap parseCsv(const QString& filePath, QString* errorOut = nullptr);

private slots:
    void onTriggered();

private:
    QAction*         m_action       = nullptr;
    CellularCanvas*  m_canvas       = nullptr;
    QWidget*         m_dialogParent = nullptr;
};

} // namespace ntcap