#include "SavePlanesAction.h"

#include "PlanesConverter.h"
#include "RlePlanesConverter.h"
#include "CsvPlanesConverter.h"
#include "PlaintextPlanesConverter.h"

#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include <fstream>
#include <memory>

namespace ntcap {

SavePlanesAction::SavePlanesAction(PlanesProvider provider,
                                   QWidget* dialogParent,
                                   QObject* parent)
    : QObject(parent)
    , m_provider(std::move(provider))
    , m_dialogParent(dialogParent)
{
    m_action = new QAction(tr("Save As\u2026"), this);
    connect(m_action, &QAction::triggered,
            this,     &SavePlanesAction::onTriggered);
}

QAction* SavePlanesAction::action() const noexcept
{
    return m_action;
}

void SavePlanesAction::onTriggered()
{
    const QString rleFilter   = tr("RLE files (*.rle)");
    const QString csvFilter   = tr("CSV files (*.csv)");
    const QString cellsFilter = tr("Plaintext cells (*.cells)");

    QString selectedFilter;
    const QString path = QFileDialog::getSaveFileName(
        m_dialogParent,
        tr("Save Planes As"),
        QString(),
        rleFilter + ";;" + csvFilter + ";;" + cellsFilter,
        &selectedFilter);

    if (path.isEmpty())
        return;

    std::unique_ptr<PlanesConverter> converter;
    QString finalPath = path;

    const bool wantCsv = path.endsWith(".csv", Qt::CaseInsensitive)
                      || selectedFilter == csvFilter;
    const bool wantCells = path.endsWith(".cells", Qt::CaseInsensitive)
                        || selectedFilter == cellsFilter;

    if (wantCells) {
        if (!finalPath.endsWith(".cells", Qt::CaseInsensitive))
            finalPath += ".cells";
        converter = std::make_unique<PlaintextPlanesConverter>();
    } else if (wantCsv) {
        if (!finalPath.endsWith(".csv", Qt::CaseInsensitive))
            finalPath += ".csv";
        converter = std::make_unique<CsvPlanesConverter>();
    } else {
        if (!finalPath.endsWith(".rle", Qt::CaseInsensitive))
            finalPath += ".rle";
        converter = std::make_unique<RlePlanesConverter>();
    }

    try {
        const Planes planes = m_provider();

        std::ofstream out(finalPath.toStdString(),
                          std::ios::out | std::ios::binary);
        if (!out) {
            QMessageBox::critical(
                m_dialogParent, tr("Error"),
                tr("Cannot open file for writing:\n%1").arg(finalPath));
            return;
        }

        converter->serialize(planes, out);

    } catch (const PlanesConverterException& e) {
        QMessageBox::critical(
            m_dialogParent, tr("Save Error"),
            QStringLiteral("%1\n\n%2")
                .arg(QString::fromStdString(e.message()),
                     QString::fromStdString(e.detail())));
    } catch (const std::exception& e) {
        QMessageBox::critical(
            m_dialogParent, tr("Save Error"),
            QString::fromStdString(e.what()));
    }
}

} // namespace ntcap