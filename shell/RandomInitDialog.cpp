#include "RandomInitDialog.h"

#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QApplication>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace ntcap {

static int distributionIndex(const DistributionParams& dist) noexcept
{
    if (std::holds_alternative<BernoulliParams>(dist)) return 0;
    if (std::holds_alternative<UniformParams>(dist))   return 1;
    if (std::holds_alternative<NormalParams>(dist))    return 2;
    return 0;
}

RandomInitDialog::RandomInitDialog(size_t numPlanes,
                                   const RandomInitState& initialState,
                                   QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Random Fill"));
    setMinimumWidth(360);

    auto* form = new QFormLayout;
    buildGeneralSection(form, numPlanes, initialState);
    buildDistributionSection(form, initialState.fillParams);
    buildSeedSection(form, initialState.fillParams.seed);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto* root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void RandomInitDialog::buildGeneralSection(QFormLayout* form,
                                           size_t numPlanes,
                                           const RandomInitState& initial)
{
    m_allPlanesCb = new QCheckBox(tr("All planes"));
    m_allPlanesCb->setChecked(initial.allPlanes);

    m_planeSlider = new QSlider(Qt::Horizontal);
    m_planeSlider->setRange(0, static_cast<int>(numPlanes) - 1);
    m_planeSlider->setTickPosition(QSlider::TicksBelow);
    m_planeSlider->setTickInterval(1);
    m_planeSlider->setValue(static_cast<int>(
        std::min(initial.planeIndex, numPlanes - 1)));
    m_planeSlider->setEnabled(!initial.allPlanes);

    m_planeLabel = new QLabel(QString::number(m_planeSlider->value()));
    m_planeLabel->setMinimumWidth(24);
    m_planeLabel->setAlignment(Qt::AlignCenter);
    m_planeLabel->setEnabled(!initial.allPlanes);

    auto* sliderRow = new QHBoxLayout;
    sliderRow->addWidget(m_planeSlider, 1);
    sliderRow->addWidget(m_planeLabel);

    m_densitySpin = new QDoubleSpinBox;
    m_densitySpin->setRange(0.0, 1.0);
    m_densitySpin->setSingleStep(0.05);
    m_densitySpin->setDecimals(2);
    m_densitySpin->setValue(initial.fillParams.density);

    form->addRow(m_allPlanesCb);
    form->addRow(tr("Plane index:"), sliderRow);
    form->addRow(tr("Density:"), m_densitySpin);

    connect(m_allPlanesCb, &QCheckBox::toggled,
            this, &RandomInitDialog::onAllPlanesToggled);
    connect(m_planeSlider, &QSlider::valueChanged,
            this, &RandomInitDialog::onPlaneSliderChanged);
}

void RandomInitDialog::buildDistributionSection(QFormLayout* form,
                                                const RandomFillParams& initial)
{
    m_distributionCb = new QComboBox;
    m_distributionCb->addItem(tr("Bernoulli"));
    m_distributionCb->addItem(tr("Uniform"));
    m_distributionCb->addItem(tr("Normal"));

    buildDistributionPages(initial.distribution);

    int idx = distributionIndex(initial.distribution);
    m_distributionCb->setCurrentIndex(idx);
    m_paramStack->setCurrentIndex(idx);

    form->addRow(tr("Distribution:"), m_distributionCb);
    form->addRow(m_paramStack);

    connect(m_distributionCb, &QComboBox::currentIndexChanged,
            this, &RandomInitDialog::onDistributionChanged);
}

void RandomInitDialog::buildDistributionPages(const DistributionParams& initial)
{
    m_paramStack = new QStackedWidget;

    auto bernoulli = std::get_if<BernoulliParams>(&initial);
    auto uniform   = std::get_if<UniformParams>(&initial);
    auto normal    = std::get_if<NormalParams>(&initial);

    auto* bernoulliPage = new QWidget;
    auto* bernoulliForm = new QFormLayout(bernoulliPage);
    m_bernoulliValue = new QSpinBox;
    m_bernoulliValue->setRange(1, 255);
    m_bernoulliValue->setValue(bernoulli ? bernoulli->value : 1);
    bernoulliForm->addRow(tr("Fill value:"), m_bernoulliValue);
    m_paramStack->addWidget(bernoulliPage);

    auto* uniformPage = new QWidget;
    auto* uniformForm = new QFormLayout(uniformPage);
    m_uniformMin = new QSpinBox;
    m_uniformMin->setRange(0, 255);
    m_uniformMin->setValue(uniform ? uniform->minValue : 0);
    m_uniformMax = new QSpinBox;
    m_uniformMax->setRange(0, 255);
    m_uniformMax->setValue(uniform ? uniform->maxValue : 255);
    uniformForm->addRow(tr("Min value:"), m_uniformMin);
    uniformForm->addRow(tr("Max value:"), m_uniformMax);
    m_paramStack->addWidget(uniformPage);

    auto* normalPage = new QWidget;
    auto* normalForm = new QFormLayout(normalPage);
    m_normalMean = new QDoubleSpinBox;
    m_normalMean->setRange(0.0, 255.0);
    m_normalMean->setSingleStep(1.0);
    m_normalMean->setDecimals(1);
    m_normalMean->setValue(normal ? normal->mean : 128.0);
    m_normalStddev = new QDoubleSpinBox;
    m_normalStddev->setRange(0.1, 128.0);
    m_normalStddev->setSingleStep(1.0);
    m_normalStddev->setDecimals(1);
    m_normalStddev->setValue(normal ? normal->stddev : 40.0);
    normalForm->addRow(tr("Mean:"), m_normalMean);
    normalForm->addRow(tr("Std Dev:"), m_normalStddev);
    m_paramStack->addWidget(normalPage);
}

void RandomInitDialog::buildSeedSection(QFormLayout* form, uint32_t seed)
{
    m_seedSpin = new QSpinBox;
    m_seedSpin->setRange(0, std::numeric_limits<int>::max());
    m_seedSpin->setValue(static_cast<int>(seed & 0x7FFFFFFFu));

    m_copySeedBtn = new QPushButton(tr("Copy"));
    m_copySeedBtn->setFixedWidth(60);

    auto* seedRow = new QHBoxLayout;
    seedRow->addWidget(m_seedSpin, 1);
    seedRow->addWidget(m_copySeedBtn);

    form->addRow(tr("Seed:"), seedRow);

    connect(m_copySeedBtn, &QPushButton::clicked,
            this, &RandomInitDialog::onCopySeed);
}

bool RandomInitDialog::allPlanes() const
{
    return m_allPlanesCb->isChecked();
}

size_t RandomInitDialog::planeIndex() const
{
    return static_cast<size_t>(m_planeSlider->value());
}

RandomFillParams RandomInitDialog::params() const
{
    RandomFillParams p;
    p.density = m_densitySpin->value();
    p.seed    = static_cast<uint32_t>(m_seedSpin->value());

    switch (m_distributionCb->currentIndex()) {
    case 0:
        p.distribution = BernoulliParams{
            static_cast<uint8_t>(m_bernoulliValue->value())
        };
        break;
    case 1:
        p.distribution = UniformParams{
            static_cast<uint8_t>(m_uniformMin->value()),
            static_cast<uint8_t>(m_uniformMax->value())
        };
        break;
    case 2:
        p.distribution = NormalParams{
            m_normalMean->value(),
            m_normalStddev->value()
        };
        break;
    default:
        p.distribution = BernoulliParams{};
        break;
    }

    return p;
}

RandomInitState RandomInitDialog::state() const
{
    return RandomInitState{
        .fillParams = params(),
        .allPlanes  = allPlanes(),
        .planeIndex = planeIndex()
    };
}

void RandomInitDialog::onAllPlanesToggled(bool checked)
{
    m_planeSlider->setEnabled(!checked);
    m_planeLabel->setEnabled(!checked);
}

void RandomInitDialog::onPlaneSliderChanged(int value)
{
    m_planeLabel->setText(QString::number(value));
}

void RandomInitDialog::onDistributionChanged(int index)
{
    m_paramStack->setCurrentIndex(index);
}

void RandomInitDialog::onCopySeed()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(QString::number(m_seedSpin->value()));
}

} // namespace ntcap