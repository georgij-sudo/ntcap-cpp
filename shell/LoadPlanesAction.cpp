#include "LoadPlanesAction.h"

#include "PlanesConverter.h"
#include "RlePlanesConverter.h"
#include "CsvPlanesConverter.h"
#include "PlaintextPlanesConverter.h"
#include "LifeRlePlanesConverter.h"

#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include <fstream>
#include <memory>

namespace ntcap {

LoadPlanesAction::LoadPlanesAction(QWidget* dialogParent,
                                   QObject* parent)
    : QObject(parent)
    , m_dialogParent(dialogParent)
{
    m_action = new QAction(tr("Open\u2026"), this);
    connect(m_action, &QAction::triggered,
            this,     &LoadPlanesAction::onTriggered);
}

QAction* LoadPlanesAction::action() const noexcept
{
    return m_action;
}

void LoadPlanesAction::onTriggered()
{
    const QString rleFilter   = tr("RLE files (*.rle)");
    const QString csvFilter   = tr("CSV files (*.csv)");
    const QString cellsFilter = tr("Plaintext cells (*.cells)");
    const QString allFilter   = tr("All supported (*.rle *.csv *.cells)");

    QString selectedFilter;
    const QString path = QFileDialog::getOpenFileName(
        m_dialogParent,
        tr("Open Planes"),
        QString(),
        allFilter + ";;" + rleFilter + ";;" + csvFilter + ";;" + cellsFilter,
        &selectedFilter);

    if (path.isEmpty())
        return;

    std::unique_ptr<PlanesConverter> converter;

    if (path.endsWith(".csv", Qt::CaseInsensitive)) {
        converter = std::make_unique<CsvPlanesConverter>();
    } else if (path.endsWith(".rle", Qt::CaseInsensitive)) {
        std::ifstream probe(path.toStdString(),
                            std::ios::in | std::ios::binary);
        if (probe && LifeRlePlanesConverter::looksLikeTextRle(probe))
            converter = std::make_unique<LifeRlePlanesConverter>();
        else
            converter = std::make_unique<RlePlanesConverter>();
    } else if (path.endsWith(".cells", Qt::CaseInsensitive)) {
        converter = std::make_unique<PlaintextPlanesConverter>();
    } else {
        QMessageBox::warning(
            m_dialogParent, tr("Unknown Format"),
            tr("Cannot determine format from extension:\n%1").arg(path));
        return;
    }

    try {
        std::ifstream in(path.toStdString(),
                         std::ios::in | std::ios::binary);
        if (!in) {
            QMessageBox::critical(
                m_dialogParent, tr("Error"),
                tr("Cannot open file for reading:\n%1").arg(path));
            return;
        }

        Planes planes = converter->deserialize(in);
        emit planesLoaded(planes);

    } catch (const PlanesConverterException& e) {
        QMessageBox::critical(
            m_dialogParent, tr("Load Error"),
            QStringLiteral("%1\n\n%2")
                .arg(QString::fromStdString(e.message()),
                     QString::fromStdString(e.detail())));
    } catch (const std::exception& e) {
        QMessageBox::critical(
            m_dialogParent, tr("Load Error"),
            QString::fromStdString(e.what()));
    }
}

} // namespace ntcap