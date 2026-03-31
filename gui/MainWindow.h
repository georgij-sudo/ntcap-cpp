#pragma once

#include <QMainWindow>
#include <QMenu>

namespace ntcap {

class CellularCanvas;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(size_t fieldWidth, size_t fieldHeight,
                        QWidget* parent = nullptr);
    ~MainWindow() override = default;

    CellularCanvas* canvas() const noexcept;

protected:
    void showEvent(QShowEvent* event) override;

private:
    void buildMenuBar();
    QMenu* addPlaceholderMenu(const QString& title);

    CellularCanvas* m_canvas = nullptr;
    size_t m_fieldWidth  = 0;
    size_t m_fieldHeight = 0;

    QMenu* m_planesMenu     = nullptr;
    QMenu* m_recipesMenu    = nullptr;
    QMenu* m_simulationMenu = nullptr;

    bool m_firstShow = true;
};

} // namespace ntcap