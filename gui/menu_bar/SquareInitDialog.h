#pragma once

#include <QDialog>
#include <cstddef>
#include <cstdint>

class QSpinBox;
class QSlider;
class QLabel;

namespace ntcap {

/**
 * @brief Dialog for configuring centred-square fill.
 */
class SquareInitDialog final : public QDialog {
    Q_OBJECT
public:
    explicit SquareInitDialog(size_t fieldWidth,
                              size_t fieldHeight,
                              size_t numPlanes,
                              QWidget* parent = nullptr);

    size_t  planeIndex()   const;
    size_t  squareWidth()  const;
    size_t  squareHeight() const;
    uint8_t fillValue()    const;

private slots:
    void onSliderChanged(int value);

private:
    QSlider*  m_planeSlider = nullptr;
    QLabel*   m_planeLabel  = nullptr;
    QSpinBox* m_widthSpin   = nullptr;
    QSpinBox* m_heightSpin  = nullptr;
    QSpinBox* m_valueSpin   = nullptr;
};

} // namespace ntcap