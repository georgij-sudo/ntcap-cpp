#pragma once

#include <QDialog>
#include <cstddef>
#include <cstdint>

class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QSlider;
class QLabel;

namespace ntcap {

/**
 * @brief Dialog for configuring random fill of planes.
 */
class RandomInitDialog final : public QDialog {
    Q_OBJECT
public:
    explicit RandomInitDialog(size_t numPlanes,
                              QWidget* parent = nullptr);

    bool    allPlanes()  const;
    size_t  planeIndex() const;
    double  density()    const;
    uint8_t fillValue()  const;

private slots:
    void onAllPlanesToggled(bool checked);
    void onSliderChanged(int value);

private:
    QCheckBox*      m_allPlanesCb    = nullptr;
    QSlider*        m_planeSlider    = nullptr;
    QLabel*         m_planeLabel     = nullptr;
    QDoubleSpinBox* m_densitySpin    = nullptr;
    QSpinBox*       m_valueSpin      = nullptr;
};

} // namespace ntcap