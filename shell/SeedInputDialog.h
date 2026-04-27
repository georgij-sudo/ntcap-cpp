#pragma once

#include <QDialog>
#include <cstdint>

class QSpinBox;
class QPushButton;

namespace ntcap {

/**
 * @brief Small dialog for entering an automaton RNG seed.
 *
 * Provides a numeric input, a Copy button for reproducibility,
 * and standard Ok / Cancel buttons.
 */
class SeedInputDialog final : public QDialog {
    Q_OBJECT
public:
    /**
     * @param currentSeed  value to display initially.
     * @param parent       parent widget.
     */
    explicit SeedInputDialog(uint32_t currentSeed,
                             QWidget* parent = nullptr);

    /**
     * @brief Seed value entered by the user.
     */
    uint32_t seedValue() const;

private slots:
    void onCopy();

private:
    QSpinBox*    m_seedSpin    = nullptr;
    QPushButton* m_copyBtn     = nullptr;
};

} // namespace ntcap