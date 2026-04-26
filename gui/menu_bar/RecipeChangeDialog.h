#pragma once

#include <QDialog>
#include <functional>
#include <memory>

#include "AutomatonController.h"
#include "RecipeDescriptor.h"
#include "Planes.h"

class QListWidget;
class QStackedWidget;
class QSpinBox;
class QLabel;

namespace ntcap {

class RecipeParameterWidget;

/**
 * @brief Result returned from RecipeChangeDialog on accept.
 */
struct RecipeChangeResult {
    AutomatonFactory factory;
    size_t width;
    size_t height;
    size_t numPlanes;
};

/**
 * @brief Dialog to select a recipe and configure its parameters.
 */
class RecipeChangeDialog final : public QDialog {
    Q_OBJECT
public:
    explicit RecipeChangeDialog(size_t currentWidth,
                                size_t currentHeight,
                                size_t currentPlanes,
                                QWidget* parent = nullptr);

    RecipeChangeResult result() const;

private slots:
    void onRecipeSelected(int row);
    void onDimensionsChanged();

private:
    void buildUi();
    void updateWarning();
    AutomatonFactory buildFactory(int recipeIndex) const;

    QList<RecipeDescriptor> m_descriptors;

    QListWidget*    m_recipeList   = nullptr;
    QStackedWidget* m_paramStack   = nullptr;
    QSpinBox*       m_widthSpin    = nullptr;
    QSpinBox*       m_heightSpin   = nullptr;
    QSpinBox*       m_planesSpin   = nullptr;
    QLabel*         m_warningLabel = nullptr;

    size_t m_currentWidth;
    size_t m_currentHeight;
    size_t m_currentPlanes;

    QList<int> m_stackIndices;
};

} // namespace ntcap