#pragma once

#include <QDialog>
#include <QList>
#include "RecipePreset.h"

class QListWidget;
class QTextEdit;

namespace ntcap {

/**
 * @brief Dialog for selecting a recipe-specific initialisation preset.
 *
 * Displays a list of preset names on the left,
 * description of the selected preset on the right.
 * Returns the index of the chosen preset on accept, or -1 on reject.
 */
class RecipePresetDialog final : public QDialog {
    Q_OBJECT
public:
    /**
     * @param recipeName  human-readable recipe name (shown in title).
     * @param presets     available presets (must not be empty).
     * @param parent      parent widget.
     */
    explicit RecipePresetDialog(const QString& recipeName,
                                const QList<RecipePreset>& presets,
                                QWidget* parent = nullptr);

    /**
     * @brief Index of the selected preset, or -1 if dialog was rejected.
     */
    int selectedIndex() const noexcept;

private slots:
    void onSelectionChanged(int row);

private:
    void buildUi(const QString& recipeName,
                 const QList<RecipePreset>& presets);

    QListWidget* m_list        = nullptr;
    QTextEdit*   m_description = nullptr;
    int          m_selected    = -1;

    QList<QString> m_descriptions;
};

} // namespace ntcap