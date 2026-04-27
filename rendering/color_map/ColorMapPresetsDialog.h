#pragma once

#include <QDialog>
#include "ColorMapPresets.h"

class QListWidget;

namespace ntcap {

class ColorMapPresetsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ColorMapPresetsDialog(QWidget* parent = nullptr);

    ColorMap selectedColorMap() const;
    QString  selectedName()     const;

private:
    QList<ColorMapPreset> m_presets;
    QListWidget*          m_list = nullptr;
};

} // namespace ntcap
