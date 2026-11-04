#include "AboutDialog.h"

#include "Version.h"

#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QtGlobal>

namespace ntcap {
namespace {

QString applicationName()
{
    const QString name = QCoreApplication::applicationName();
    return name.isEmpty() ? QStringLiteral(NTCAP_APPLICATION_NAME) : name;
}

QString applicationVersion()
{
    const QString version = QCoreApplication::applicationVersion();
    return version.isEmpty() ? QStringLiteral(NTCAP_VERSION_STRING) : version;
}

QLabel* makeInfoLabel(const QString& text, QWidget* parent)
{
    auto* label = new QLabel(text, parent);
    label->setWordWrap(true);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    return label;
}

} // namespace

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    const QString name = applicationName();

    setWindowTitle(tr("About %1").arg(name));
    setMinimumWidth(420);

    auto* nameLabel = makeInfoLabel(name, this);
    QFont nameFont = nameLabel->font();
    nameFont.setPointSizeF(nameFont.pointSizeF() * 1.6);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);

    auto* versionLabel = makeInfoLabel(
        tr("Version %1").arg(applicationVersion()), this);

    auto* descriptionLabel = makeInfoLabel(
        tr("Cellular automata simulator focused on fast processing "
           "and easy experimentation."), this);

    auto* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);

    auto* qtLabel = makeInfoLabel(
        tr("Running on Qt %1").arg(QString::fromLatin1(qVersion())), this);

    auto* copyrightLabel = makeInfoLabel(
        tr("Copyright \u00A9 2026 \u041A\u0440\u0430\u0439\u043D\u044E\u043A\u043E\u0432 "
           "\u0413\u0435\u043E\u0440\u0433\u0438\u0439. License: TBD."), this);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(nameLabel);
    layout->addWidget(versionLabel);
    layout->addSpacing(8);
    layout->addWidget(descriptionLabel);
    layout->addSpacing(8);
    layout->addWidget(separator);
    layout->addWidget(qtLabel);
    layout->addWidget(copyrightLabel);
    layout->addSpacing(8);
    layout->addWidget(buttons);
}

} // namespace ntcap
