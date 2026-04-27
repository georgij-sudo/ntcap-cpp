#pragma once

#include <QDialog>

class QTextBrowser;

namespace ntcap {

/**
 * @brief Dialog that displays the current recipe description in rich HTML.
 */
class RecipeInfoDialog final : public QDialog {
    Q_OBJECT
public:
    /**
     * @param recipeName   human-readable recipe name.
     * @param descriptionHtml  HTML description (may contain formulas, images).
     * @param parent       parent widget.
     */
    explicit RecipeInfoDialog(const QString& recipeName,
                              const QString& descriptionHtml,
                              QWidget* parent = nullptr);

private:
    QTextBrowser* m_browser = nullptr;
};

} // namespace ntcap