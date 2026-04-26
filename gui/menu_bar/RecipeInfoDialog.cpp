#include "RecipeInfoDialog.h"

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDialogButtonBox>

namespace ntcap {

RecipeInfoDialog::RecipeInfoDialog(const QString& recipeName,
                                   const QString& descriptionHtml,
                                   QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Recipe Info — %1").arg(recipeName));
    setMinimumSize(480, 360);
    resize(560, 440);

    m_browser = new QTextBrowser(this);
    m_browser->setOpenExternalLinks(true);
    m_browser->setReadOnly(true);

    QString html = QStringLiteral(
        "<!DOCTYPE html>"
        "<html><head><style>"
        "body { font-family: sans-serif; margin: 12px; }"
        "h2 { color: #336699; }"
        "h3 { color: #555; margin-top: 16px; }"
        "code { background: #f4f4f4; padding: 2px 4px; }"
        "ul { margin-left: 20px; }"
        "</style></head><body>");
    html += descriptionHtml;
    html += QStringLiteral("</body></html>");
    m_browser->setHtml(html);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_browser, 1);
    layout->addWidget(buttons);
}

} // namespace ntcap