#include "SquareInitDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

namespace ntcap {

SquareInitDialog::SquareInitDialog(size_t fieldWidth,
                                   size_t fieldHeight,
                                   size_t numPlanes,
                                   QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Square Initialization"));

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

    m_widthSpin = new QSpinBox;
    m_widthSpin->setRange(1, static_cast<int>(fieldWidth));
    m_widthSpin->setValue(static_cast<int>(fieldWidth / 4));

    m_heightSpin = new QSpinBox;
    m_heightSpin->setRange(1, static_cast<int>(fieldHeight));
    m_heightSpin->setValue(static_cast<int>(fieldHeight / 4));

    m_valueSpin = new QSpinBox;
    m_valueSpin->setRange(1, 255);
    m_valueSpin->setValue(1);

    auto* form = new QFormLayout;
    form->addRow(tr("Plane index:"),   sliderRow);
    form->addRow(tr("Square width:"),  m_widthSpin);
    form->addRow(tr("Square height:"), m_heightSpin);
    form->addRow(tr("Fill value:"),    m_valueSpin);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto* root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addWidget(buttons);

    connect(m_planeSlider, &QSlider::valueChanged,
            this, &SquareInitDialog::onSliderChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

size_t SquareInitDialog::planeIndex() const
{
    return static_cast<size_t>(m_planeSlider->value());
}

size_t SquareInitDialog::squareWidth() const
{
    return static_cast<size_t>(m_widthSpin->value());
}

size_t SquareInitDialog::squareHeight() const
{
    return static_cast<size_t>(m_heightSpin->value());
}

uint8_t SquareInitDialog::fillValue() const
{
    return static_cast<uint8_t>(m_valueSpin->value());
}

void SquareInitDialog::onSliderChanged(int value)
{
    m_planeLabel->setText(QString::number(value));
}

} // namespace ntcap