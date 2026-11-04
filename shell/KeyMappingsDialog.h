#pragma once

#include <QDialog>

#include "KeyMappingsData.h"

class QTableWidget;

namespace ntcap {

// Non-modal reference window listing every working keyboard and mouse control.
// It is meant to stay open beside the canvas while the simulation is running.
class KeyMappingsDialog final : public QDialog {
    Q_OBJECT

public:
    explicit KeyMappingsDialog(QWidget* parent = nullptr);

private:
    QTableWidget* createTable(KeyMappingGroup group, QWidget* parent);
};

} // namespace ntcap
