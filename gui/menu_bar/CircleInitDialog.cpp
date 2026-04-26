#include "CircleInitDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

namespace ntcap {

CircleInitDialog::CircleInitDialog(size_t fieldWidth,
                                   size_t fieldHeight,
                                   size_t numPlanes,
                                   QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Circle Initialization"));

    m_planeSlider = new QSlider(Qt::Horizontal);
    m_planeSlider->setRange(0, static_cast<int>(numPlanes) - 1);
    m_planeSlider->setTickPosition(QSlider::TicksBelow);
    m_planeSlider->setTickInterval(1);

    m_planeLabel = new QLabel(QStringLiteral("0"));
    m_planeLabel->setMinimumWidth(24);
    m_planeLabel->setAlignment(Qt::AlignCenter);

    auto* sliderRow = new QHBoxLayout;
    sliderRow->addWidget(m_planeSlider, 1);
    sliderRow->addWidget(m_planeLabel);

    m_diamXSpin = new QSpinBox;
    m_diamXSpin->setRange(1, static_cast<int>(fieldWidth));
    m_diamXSpin->setValue(static_cast<int>(fieldWidth / 4));

    m_diamYSpin = new QSpinBox;
    m_diamYSpin->setRange(1, static_cast<int>(fieldHeight));
    m_diamYSpin->setValue(static_cast<int>(fieldHeight / 4));

    m_valueSpin = new QSpinBox;
    m_valueSpin->setRange(1, 255);
    m_valueSpin->setValue(1);

    auto* form = new QFormLayout;
    form->addRow(tr("Plane index:"), sliderRow);
    form->addRow(tr("Diameter X:"),  m_diamXSpin);
    form->addRow(tr("Diameter Y:"),  m_diamYSpin);
    form->addRow(tr("Fill value:"),  m_valueSpin);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto* root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addWidget(buttons);

    connect(m_planeSlider, &QSlider::valueChanged,
            this, &CircleInitDialog::onSliderChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

size_t CircleInitDialog::planeIndex() const
{
    return static_cast<size_t>(m_planeSlider->value());
}

size_t CircleInitDialog::diameterX() const
{
    return static_cast<size_t>(m_diamXSpin->value());
}

size_t CircleInitDialog::diameterY() const
{
    return static_cast<size_t>(m_diamYSpin->value());
}

uint8_t CircleInitDialog::fillValue() const
{
    return static_cast<uint8_t>(m_valueSpin->value());
}

void CircleInitDialog::onSliderChanged(int value)
{
    m_planeLabel->setText(QString::number(value));
}

} // namespace ntcap