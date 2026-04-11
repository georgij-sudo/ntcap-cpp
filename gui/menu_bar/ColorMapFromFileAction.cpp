#include "ColorMapFromFileAction.h"

#include "CellularCanvas.h"

#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

namespace ntcap {

ColorMapFromFileAction::ColorMapFromFileAction(CellularCanvas* canvas,
                                               QWidget* dialogParent,
                                               QObject* parent)
    : QObject(parent)
    , m_canvas(canvas)
    , m_dialogParent(dialogParent)
{
    m_action = new QAction(tr("Color Map from file\u2026"), this);
    connect(m_action, &QAction::triggered,
            this,     &ColorMapFromFileAction::onTriggered);
}

void ColorMapFromFileAction::onTriggered()
{
    const QString startDir = QCoreApplication::applicationDirPath();

    QString path = QFileDialog::getOpenFileName(
        m_dialogParent,
        tr("Open Color Map CSV"),
        startDir,
        tr("CSV files (*.csv)"));

    if (path.isEmpty())
        return;

    QString error;
    ColorMap map = parseCsv(path, &error);

    if (map.isEmpty()) {
        QMessageBox::warning(
            m_dialogParent,
            tr("Color Map Error"),
            error.isEmpty() ? tr("File contains no valid entries.") : error);
        return;
    }

    m_canvas->setColorMap(map);
}

ColorMap ColorMapFromFileAction::parseCsv(const QString& filePath,
                                          QString* errorOut)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorOut)
            *errorOut = QObject::tr("Cannot open file: %1").arg(file.errorString());
        return {};
    }

    ColorMap map;
    QTextStream in(&file);
    int lineNo = 0;

    while (!in.atEnd()) {
        ++lineNo;
        QString line = in.readLine().trimmed();

        // пропускаем пустые строки и комментарии
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        // пропускаем заголовок вида "state,r,g,b"
        if (lineNo == 1 && !line[0].isDigit())
            continue;

        QStringList parts = line.split(',');
        if (parts.size() < 4) {
            if (errorOut) {
                *errorOut = QObject::tr("Line %1: expected 4 columns (state,r,g,b), got %2.")
                                .arg(lineNo).arg(parts.size());
            }
            return {};
        }

        bool okS, okR, okG, okB;
        int state = parts[0].trimmed().toInt(&okS);
        int r     = parts[1].trimmed().toInt(&okR);
        int g     = parts[2].trimmed().toInt(&okG);
        int b     = parts[3].trimmed().toInt(&okB);

        if (!okS || !okR || !okG || !okB) {
            if (errorOut) {
                *errorOut = QObject::tr("Line %1: non-integer value.").arg(lineNo);
            }
            return {};
        }

        if (state < 0 || state > 255 ||
            r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
        {
            if (errorOut) {
                *errorOut = QObject::tr("Line %1: values must be in [0..255].").arg(lineNo);
            }
            return {};
        }

        map[static_cast<uint8_t>(state)] = qRgb(r, g, b);
    }

    return map;
}

} // namespace ntcap
