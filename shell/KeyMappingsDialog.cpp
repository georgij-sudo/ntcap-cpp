#include "KeyMappingsDialog.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>

namespace ntcap {
namespace {

QString groupTitle(KeyMappingGroup group)
{
    switch (group) {
    case KeyMappingGroup::Simulation:
        return KeyMappingsDialog::tr("Simulation");
    case KeyMappingGroup::Canvas:
        return KeyMappingsDialog::tr("Canvas");
    case KeyMappingGroup::Mouse:
        return KeyMappingsDialog::tr("Mouse");
    }

    return {};
}

QString actionTitle(KeyMappingAction action)
{
    switch (action) {
    case KeyMappingAction::PlayPause:
        return KeyMappingsDialog::tr("Play / Pause");
    case KeyMappingAction::StepForward:
        return KeyMappingsDialog::tr("Step Forward (while paused)");
    case KeyMappingAction::FitPlaneToWindow:
        return KeyMappingsDialog::tr("Fit Plane to Window");
    case KeyMappingAction::ZoomIn:
        return KeyMappingsDialog::tr("Zoom In");
    case KeyMappingAction::ZoomOut:
        return KeyMappingsDialog::tr("Zoom Out");
    case KeyMappingAction::PanLeft:
        return KeyMappingsDialog::tr("Pan Left");
    case KeyMappingAction::PanRight:
        return KeyMappingsDialog::tr("Pan Right");
    case KeyMappingAction::PanUp:
        return KeyMappingsDialog::tr("Pan Up");
    case KeyMappingAction::PanDown:
        return KeyMappingsDialog::tr("Pan Down");
    case KeyMappingAction::NextDisplayMode:
        return KeyMappingsDialog::tr("Next Display Mode");
    case KeyMappingAction::PreviousDisplayMode:
        return KeyMappingsDialog::tr("Previous Display Mode");
    case KeyMappingAction::ZoomAtCursor:
        return KeyMappingsDialog::tr("Zoom at cursor");
    case KeyMappingAction::PanByDragging:
        return KeyMappingsDialog::tr("Pan the plane");
    }

    return {};
}

} // namespace

KeyMappingsDialog::KeyMappingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Key Mappings"));
    // Own top-level window so it can stay open beside the main window.
    setWindowFlag(Qt::Window, true);
    setMinimumSize(460, 400);
    resize(560, 680);

    auto* content = new QWidget(this);
    auto* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(8);

    for (KeyMappingGroup group : keyMappingGroups()) {
        auto* box = new QGroupBox(groupTitle(group), content);
        auto* boxLayout = new QVBoxLayout(box);
        boxLayout->addWidget(createTable(group, box));
        contentLayout->addWidget(box);
    }
    contentLayout->addStretch(1);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(content);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(scroll, 1);
    layout->addWidget(buttons);
}

QTableWidget* KeyMappingsDialog::createTable(KeyMappingGroup group, QWidget* parent)
{
    const std::span<const KeyMappingRow> rows = keyMappingRows(group);
    const int rowCount = static_cast<int>(rows.size());

    auto* table = new QTableWidget(rowCount, 2, parent);
    table->setHorizontalHeaderLabels({tr("Key"), tr("Action")});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setFocusPolicy(Qt::NoFocus);
    table->setWordWrap(false);
    table->setAlternatingRowColors(true);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    for (int row = 0; row < rowCount; ++row) {
        const KeyMappingRow& mapping = rows[static_cast<std::size_t>(row)];

        auto* keyItem = new QTableWidgetItem(
            QString::fromUtf8(mapping.key.data(),
                              static_cast<int>(mapping.key.size())));
        keyItem->setFlags(Qt::ItemIsEnabled);
        table->setItem(row, 0, keyItem);

        auto* actionItem = new QTableWidgetItem(actionTitle(mapping.action));
        actionItem->setFlags(Qt::ItemIsEnabled);
        table->setItem(row, 1, actionItem);
    }

    // Show every row without an inner scrollbar; the dialog itself scrolls.
    const int rowHeight    = table->verticalHeader()->defaultSectionSize();
    const int headerHeight = table->horizontalHeader()->sizeHint().height();
    table->setFixedHeight(headerHeight + rowHeight * rowCount + 2 * table->frameWidth());

    return table;
}

} // namespace ntcap
