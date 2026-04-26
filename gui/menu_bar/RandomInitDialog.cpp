#include "RandomInitDialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

namespace ntcap {

RandomInitDialog::RandomInitDialog(size_t numPlanes, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Random Fill"));

    m_allPlanesCb = new QCheckBox(tr("All planes"));
    m_allPlanesCb->setChecked(true);

    m_planeSlider = new QSlider(Qt::Horizontal);
    m_planeSlider->setRange(0, static_cast<int>(numPlanes) - 1);
    m_planeSlider->setTickPosition(QSlider::TicksBelow);
    m_planeSlider->setTickInterval(1);
    m_planeSlider->setEnabled(false);

    m_planeLabel = new QLabel(QStringLiteral("0"));
    m_planeLabel->setMinimumWidth(24);
    m_planeLabel->setAlignment(Qt::AlignCenter);
    m_planeLabel->setEnabled(false);

    auto* sliderRow = new QHBoxLayout;
    sliderRow->addWidget(m_planeSlider, 1);
    sliderRow->addWidget(m_planeLabel);

    m_densitySpin = new QDoubleSpinBox;
    m_densitySpin->setRange(0.0, 1.0);
    m_densitySpin->setSingleStep(0.05);
    m_densitySpin->setDecimals(2);
    m_densitySpin->setValue(0.50);

    m_valueSpin = new QSpinBox;
    m_valueSpin->setRange(1, 255);
    m_valueSpin->setValue(1);

    auto* form = new QFormLayout;
    form->addRow(m_allPlanesCb);
    form->addRow(tr("Plane index:"), sliderRow);
    form->addRow(tr("Density:"),     m_densitySpin);
    form->addRow(tr("Fill value:"),  m_valueSpin);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto* root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addWidget(buttons);

    connect(m_allPlanesCb, &QCheckBox::toggled,
            this, &RandomInitDialog::onAllPlanesToggled);
    connect(m_planeSlider, &QSlider::valueChanged,
            this, &RandomInitDialog::onSliderChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

bool RandomInitDialog::allPlanes() const
{
    return m_allPlanesCb->isChecked();
}

size_t RandomInitDialog::planeIndex() const
{
    return static_cast<size_t>(m_planeSlider->value());
}

double RandomInitDialog::density() const
{
    return m_densitySpin->value();
}

uint8_t RandomInitDialog::fillValue() const
{
    return static_cast<uint8_t>(m_valueSpin->value());
}

void RandomInitDialog::onAllPlanesToggled(bool checked)
{
    m_planeSlider->setEnabled(!checked);
    m_planeLabel->setEnabled(!checked);
}

void RandomInitDialog::onSliderChanged(int value)
{
    m_planeLabel->setText(QString::number(value));
}

} // namespace ntcap