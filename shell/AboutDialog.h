#pragma once

#include <QDialog>

namespace ntcap {

// Standard information dialog: application name, version, one-line
// description, the Qt version the application runs on, and a
// copyright/license line.
class AboutDialog final : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget* parent = nullptr);
};

} // namespace ntcap
