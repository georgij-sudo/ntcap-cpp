#pragma once

#include <QDialog>
#include <cstddef>
#include <cstdint>

#include "PlaneInitializer.h"

class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QPushButton;
class QSlider;
class QSpinBox;
class QStackedWidget;
class QFormLayout;

namespace ntcap {

/**
 * @brief Persisted state of the random fill dialog between opens.
 */
struct RandomInitState {
    RandomFillParams fillParams;
    bool             allPlanes  = true;
    size_t           planeIndex = 0;
};

/**
 * @brief Dialog for configuring random fill of planes.
 *
 * Supports Bernoulli, Uniform and Normal distributions
 * with explicit seed control for reproducibility.
 * Accepts previous state to restore widget values between opens.
 */
class RandomInitDialog final : public QDialog {
    Q_OBJECT
public:
    /**
     * @param numPlanes     number of available planes.
     * @param initialState  previous dialog state to restore.
     * @param parent        parent widget.
     */
    explicit RandomInitDialog(size_t numPlanes,
                              const RandomInitState& initialState,
                              QWidget* parent = nullptr);

    /**
     * @brief Collect full dialog state for persistence.
     */
    RandomInitState state() const;

    bool             allPlanes()  const;
    size_t           planeIndex() const;
    RandomFillParams params()     const;

private slots:
    void onAllPlanesToggled(bool checked);
    void onPlaneSliderChanged(int value);
    void onDistributionChanged(int index);
    void onCopySeed();

private:
    void buildGeneralSection(QFormLayout* form, size_t numPlanes,
                             const RandomInitState& initial);
    void buildDistributionSection(QFormLayout* form,
                                  const RandomFillParams& initial);
    void buildSeedSection(QFormLayout* form, uint32_t seed);
    void buildDistributionPages(const DistributionParams& initial);

    QCheckBox*      m_allPlanesCb     = nullptr;
    QSlider*        m_planeSlider     = nullptr;
    QLabel*         m_planeLabel      = nullptr;

    QComboBox*      m_distributionCb  = nullptr;
    QDoubleSpinBox* m_densitySpin     = nullptr;
    QSpinBox*       m_seedSpin        = nullptr;
    QPushButton*    m_copySeedBtn     = nullptr;

    QStackedWidget* m_paramStack      = nullptr;

    QSpinBox*       m_bernoulliValue  = nullptr;

    QSpinBox*       m_uniformMin      = nullptr;
    QSpinBox*       m_uniformMax      = nullptr;

    QDoubleSpinBox* m_normalMean      = nullptr;
    QDoubleSpinBox* m_normalStddev    = nullptr;
};

} // namespace ntcap