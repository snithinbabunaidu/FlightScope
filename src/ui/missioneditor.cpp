#include "missioneditor.h"
#include "commandeditordialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

// ============================================================================
// CommandDelegate Implementation
// ============================================================================

CommandDelegate::CommandDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {
}

void CommandDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                            const QModelIndex& index) const {
    painter->save();

    // Draw background
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, option.palette.midlight());
    }

    // Calculate button rect (centered in cell)
    QRect buttonRect = option.rect.adjusted(8, 4, -8, -4);

    // Draw Edit button
    QStyleOptionButton buttonOption;
    buttonOption.rect = buttonRect;
    buttonOption.text = "Edit";
    buttonOption.state = QStyle::State_Enabled;

    if (option.state & QStyle::State_Selected) {
        buttonOption.state |= QStyle::State_HasFocus;
    }

    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);

    painter->restore();
}

bool CommandDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index) {
    Q_UNUSED(model);

    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        // Calculate button rect
        QRect buttonRect = option.rect.adjusted(8, 4, -8, -4);

        // Check if click is on button
        if (buttonRect.contains(mouseEvent->pos())) {
            emit editCommandRequested(index.row());
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize CommandDelegate::sizeHint(const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(qMax(size.height(), 32));  // Minimum height for button
    return size;
}

// ============================================================================
// DeleteDelegate Implementation
// ============================================================================

DeleteDelegate::DeleteDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {
}

void DeleteDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const {
    painter->save();

    // Draw background
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, option.palette.midlight());
    }

    // Calculate button rect (centered in cell)
    QRect buttonRect = option.rect.adjusted(8, 4, -8, -4);

    // Draw Delete button with red styling
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#F44336"));  // Red color
    painter->drawRoundedRect(buttonRect, 6, 6);

    // Draw text
    painter->setPen(Qt::white);
    QFont font = painter->font();
    font.setWeight(QFont::Bold);
    painter->setFont(font);
    painter->drawText(buttonRect, Qt::AlignCenter, "Delete");

    painter->restore();
}

bool DeleteDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
                                 const QStyleOptionViewItem& option,
                                 const QModelIndex& index) {
    Q_UNUSED(model);

    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        // Calculate button rect
        QRect buttonRect = option.rect.adjusted(8, 4, -8, -4);

        // Check if click is on button
        if (buttonRect.contains(mouseEvent->pos())) {
            emit deleteRequested(index.row());
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize DeleteDelegate::sizeHint(const QStyleOptionViewItem& option,
                               const QModelIndex& index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(qMax(size.height(), 32));  // Minimum height for button
    return size;
}

// ============================================================================
// MissionEditor Implementation
// ============================================================================

MissionEditor::MissionEditor(MissionModel* missionModel, MavlinkRouter* mavlinkRouter,
                             VehicleModel* vehicleModel, QWidget* parent)
    : QWidget(parent),
      m_missionModel(missionModel),
      m_mavlinkRouter(mavlinkRouter),
      m_vehicleModel(vehicleModel),
      m_tableWidget(nullptr),
      m_commandDelegate(nullptr),
      m_deleteDelegate(nullptr),
      m_addButton(nullptr),
      m_removeButton(nullptr),
      m_clearButton(nullptr),
      m_uploadButton(nullptr),
      m_downloadButton(nullptr),
      m_statusLabel(nullptr),
      m_protocolState(ProtocolState::Idle),
      m_expectedItemSeq(0),
      m_totalItemCount(0),
      m_targetSystemId(1),
      m_targetComponentId(1),
      m_updatingTable(false) {

    setupUi();

    // Connect mission model signals
    connect(m_missionModel, &MissionModel::missionChanged, this,
            &MissionEditor::onMissionModelChanged);

    // Connect MAVLink router signals for mission protocol
    connect(m_mavlinkRouter, &MavlinkRouter::missionCountReceived, this,
            &MissionEditor::onMissionCountReceived);
    connect(m_mavlinkRouter, &MavlinkRouter::missionRequestReceived, this,
            &MissionEditor::onMissionRequestIntReceived);  // Handle legacy MISSION_REQUEST
    connect(m_mavlinkRouter, &MavlinkRouter::missionRequestIntReceived, this,
            &MissionEditor::onMissionRequestIntReceived);
    connect(m_mavlinkRouter, &MavlinkRouter::missionItemIntReceived, this,
            &MissionEditor::onMissionItemIntReceived);
    connect(m_mavlinkRouter, &MavlinkRouter::missionAckReceived, this,
            &MissionEditor::onMissionAckReceived);
}

void MissionEditor::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Toolbar
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    m_addButton = new QPushButton("Add Waypoint", this);
    m_addButton->setObjectName("missionButton");

    m_removeButton = new QPushButton("Remove", this);
    m_removeButton->setObjectName("missionButton");

    m_clearButton = new QPushButton("Clear All", this);
    m_clearButton->setObjectName("missionButton");

    m_uploadButton = new QPushButton("Upload to Vehicle", this);
    m_uploadButton->setObjectName("missionButton");

    m_downloadButton = new QPushButton("Download from Vehicle", this);
    m_downloadButton->setObjectName("missionButton");

    toolbarLayout->addWidget(m_addButton);
    toolbarLayout->addWidget(m_removeButton);
    toolbarLayout->addWidget(m_clearButton);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_uploadButton);
    toolbarLayout->addWidget(m_downloadButton);

    mainLayout->addLayout(toolbarLayout);

    // Table - Enable alternating row colors
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(7);  // Added Edit Mission and Delete columns
    m_tableWidget->setHorizontalHeaderLabels(
        {"#", "Command", "Latitude", "Longitude", "Altitude (m)", "Edit Mission", "Delete"});
    m_tableWidget->horizontalHeader()->setStretchLastSection(false);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setAlternatingRowColors(true);  // Enable alternating row colors

    // Set column widths
    m_tableWidget->setColumnWidth(0, 40);  // # column - narrow
    m_tableWidget->setColumnWidth(5, 100);  // Edit Mission column
    m_tableWidget->setColumnWidth(6, 100);  // Delete column

    // Set command delegate for column 5 (Edit Mission button column)
    m_commandDelegate = new CommandDelegate(this);
    m_tableWidget->setItemDelegateForColumn(5, m_commandDelegate);
    connect(m_commandDelegate, &CommandDelegate::editCommandRequested, this,
            &MissionEditor::onEditCommandRequested);

    // Set delete delegate for column 6 (Delete button column)
    m_deleteDelegate = new DeleteDelegate(this);
    m_tableWidget->setItemDelegateForColumn(6, m_deleteDelegate);
    connect(m_deleteDelegate, &DeleteDelegate::deleteRequested, this,
            &MissionEditor::onDeleteRequested);

    mainLayout->addWidget(m_tableWidget);

    // Status label
    m_statusLabel = new QLabel("Ready", this);
    mainLayout->addWidget(m_statusLabel);

    // Connect button signals
    connect(m_addButton, &QPushButton::clicked, this, &MissionEditor::onAddWaypointClicked);
    connect(m_removeButton, &QPushButton::clicked, this, &MissionEditor::onRemoveWaypointClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &MissionEditor::onClearMissionClicked);
    connect(m_uploadButton, &QPushButton::clicked, this, &MissionEditor::onUploadMissionClicked);
    connect(m_downloadButton, &QPushButton::clicked, this,
            &MissionEditor::onDownloadMissionClicked);

    // Connect table signals
    connect(m_tableWidget, &QTableWidget::itemChanged, this,
            &MissionEditor::onTableItemChanged);
}

void MissionEditor::updateTable() {
    m_updatingTable = true;

    m_tableWidget->setRowCount(m_missionModel->count());

    for (int i = 0; i < m_missionModel->count(); ++i) {
        updateTableRow(i);
    }

    m_updatingTable = false;
}

void MissionEditor::updateTableRow(int row) {
    const Waypoint* wp = m_missionModel->waypointAt(row);
    if (!wp) {
        return;
    }

    // Sequence number
    QTableWidgetItem* seqItem = new QTableWidgetItem(QString::number(wp->sequence()));
    seqItem->setFlags(seqItem->flags() & ~Qt::ItemIsEditable);
    m_tableWidget->setItem(row, 0, seqItem);

    // Command - Read-only
    QTableWidgetItem* cmdItem = new QTableWidgetItem(Waypoint::commandName(wp->command()));
    cmdItem->setFlags(cmdItem->flags() & ~Qt::ItemIsEditable);
    cmdItem->setData(Qt::UserRole, wp->command());  // Store command ID
    m_tableWidget->setItem(row, 1, cmdItem);

    // Latitude
    QTableWidgetItem* latItem = new QTableWidgetItem(QString::number(wp->latitude(), 'f', 7));
    m_tableWidget->setItem(row, 2, latItem);

    // Longitude
    QTableWidgetItem* lonItem = new QTableWidgetItem(QString::number(wp->longitude(), 'f', 7));
    m_tableWidget->setItem(row, 3, lonItem);

    // Altitude
    QTableWidgetItem* altItem = new QTableWidgetItem(QString::number(wp->altitude(), 'f', 2));
    m_tableWidget->setItem(row, 4, altItem);

    // Edit Mission button (column 5) - will be rendered by delegate
    QTableWidgetItem* editItem = new QTableWidgetItem("");  // Empty text, button drawn by delegate
    editItem->setFlags(editItem->flags() & ~Qt::ItemIsEditable);
    m_tableWidget->setItem(row, 5, editItem);

    // Delete button (column 6) - will be rendered by delegate
    QTableWidgetItem* deleteItem = new QTableWidgetItem("");  // Empty text, button drawn by delegate
    deleteItem->setFlags(deleteItem->flags() & ~Qt::ItemIsEditable);
    m_tableWidget->setItem(row, 6, deleteItem);
}

void MissionEditor::setUiEnabled(bool enabled) {
    m_addButton->setEnabled(enabled);
    m_removeButton->setEnabled(enabled);
    m_clearButton->setEnabled(enabled);
    m_uploadButton->setEnabled(enabled);
    m_downloadButton->setEnabled(enabled);
    m_tableWidget->setEnabled(enabled);
}

void MissionEditor::setStatusText(const QString& text) {
    m_statusLabel->setText(text);
}

void MissionEditor::onAddWaypointClicked() {
    Waypoint wp;
    wp.setSequence(m_missionModel->count());
    wp.setCommand(MAV_CMD_NAV_WAYPOINT);
    wp.setFrame(MAV_FRAME_GLOBAL_RELATIVE_ALT_INT);
    wp.setLatitude(0.0);
    wp.setLongitude(0.0);
    wp.setAltitude(50.0);
    wp.setAutocontinue(1);

    m_missionModel->addWaypoint(wp);
    setStatusText(QString("Added waypoint #%1").arg(wp.sequence()));
}

void MissionEditor::onRemoveWaypointClicked() {
    int currentRow = m_tableWidget->currentRow();
    if (currentRow >= 0 && currentRow < m_missionModel->count()) {
        m_missionModel->removeWaypoint(currentRow);
        setStatusText(QString("Removed waypoint #%1").arg(currentRow));
    }
}

void MissionEditor::onClearMissionClicked() {
    if (m_missionModel->count() == 0) {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Clear Mission", "Are you sure you want to clear all waypoints?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_missionModel->clearMission();
        setStatusText("Mission cleared");
    }
}

void MissionEditor::onUploadMissionClicked() {
    if (m_missionModel->isEmpty()) {
        QMessageBox::warning(this, "Upload Mission", "Mission is empty. Add waypoints first.");
        return;
    }

    startMissionUpload();
}

void MissionEditor::onDownloadMissionClicked() {
    startMissionDownload();
}

void MissionEditor::onTableItemChanged(QTableWidgetItem* item) {
    if (m_updatingTable) {
        return;
    }

    int row = item->row();
    int col = item->column();

    const Waypoint* wp = m_missionModel->waypointAt(row);
    if (!wp) {
        return;
    }

    Waypoint updated = *wp;
    bool ok = false;

    switch (col) {
        case 1:  // Command
            {
                uint16_t commandId = item->data(Qt::UserRole).toUInt(&ok);
                if (ok) {
                    updated.setCommand(commandId);
                    qDebug() << "MissionEditor: Changed waypoint" << row << "command to"
                             << Waypoint::commandName(commandId);
                }
            }
            break;
        case 2:  // Latitude
            updated.setLatitude(item->text().toDouble(&ok));
            break;
        case 3:  // Longitude
            updated.setLongitude(item->text().toDouble(&ok));
            break;
        case 4:  // Altitude
            updated.setAltitude(item->text().toFloat(&ok));
            break;
        default:
            return;
    }

    if (ok) {
        m_missionModel->updateWaypoint(row, updated);
        setStatusText(QString("Updated waypoint #%1").arg(row));
    } else {
        // Revert invalid input
        m_updatingTable = true;
        updateTableRow(row);
        m_updatingTable = false;
        setStatusText("Invalid value entered");
    }
}

void MissionEditor::onMissionModelChanged() {
    updateTable();
}

// Mission Protocol Implementation

void MissionEditor::startMissionUpload() {
    m_protocolState = ProtocolState::UploadingCount;
    m_expectedItemSeq = 0;

    // CRITICAL: Add 1 for HOME waypoint (item 0) - ArduPilot requires this!
    m_totalItemCount = m_missionModel->count() + 1;

    // Update target IDs from vehicle model
    m_targetSystemId = m_vehicleModel->systemId();
    m_targetComponentId = m_vehicleModel->componentId();

    setUiEnabled(false);
    setStatusText(QString("Uploading mission (%1 waypoints + HOME)...").arg(m_missionModel->count()));

    // Send MISSION_COUNT (includes HOME waypoint)
    mavlink_message_t msg;
    mavlink_mission_count_t missionCount{};
    missionCount.target_system = m_targetSystemId;
    missionCount.target_component = m_targetComponentId;
    missionCount.count = m_totalItemCount;
    missionCount.mission_type = MAV_MISSION_TYPE_MISSION;

    mavlink_msg_mission_count_encode(255, 190, &msg, &missionCount);
    m_mavlinkRouter->sendMessage(msg);

    qInfo() << "MissionEditor: Sent MISSION_COUNT:" << m_totalItemCount
            << "to system" << m_targetSystemId << "component" << m_targetComponentId;
}

void MissionEditor::onMissionRequestIntReceived(uint16_t seq, uint8_t missionType) {
    if (missionType != MAV_MISSION_TYPE_MISSION) {
        return;
    }

    if (m_protocolState != ProtocolState::UploadingCount &&
        m_protocolState != ProtocolState::UploadingItems) {
        return;
    }

    m_protocolState = ProtocolState::UploadingItems;
    m_expectedItemSeq = seq;

    qDebug() << "MissionEditor: Vehicle requested item" << seq;

    sendNextMissionItem();
}

void MissionEditor::sendNextMissionItem() {
    mavlink_mission_item_int_t item{};
    item.target_system = m_targetSystemId;
    item.target_component = m_targetComponentId;
    item.mission_type = MAV_MISSION_TYPE_MISSION;
    item.seq = m_expectedItemSeq;

    // Item 0 is always HOME waypoint
    if (m_expectedItemSeq == 0) {
        item.command = MAV_CMD_NAV_WAYPOINT;
        item.frame = MAV_FRAME_GLOBAL;
        item.current = 1;  // HOME is current
        item.autocontinue = 1;
        // Use current vehicle position (0,0,0 means use current position)
        item.x = 0;
        item.y = 0;
        item.z = 0;

        qInfo() << "MissionEditor: Sending HOME waypoint (seq 0)";
    } else {
        // Real waypoints start from seq 1
        int wpIndex = m_expectedItemSeq - 1;
        if (wpIndex >= m_missionModel->count()) {
            qWarning() << "MissionEditor: Requested seq" << m_expectedItemSeq << "out of range";
            return;
        }

        const Waypoint* wp = m_missionModel->waypointAt(wpIndex);
        if (!wp) {
            return;
        }

        item = wp->toMavlinkMissionItemInt();
        item.seq = m_expectedItemSeq;  // Renumber (user waypoint 0 becomes seq 1)
        item.current = 0;
        item.target_system = m_targetSystemId;
        item.target_component = m_targetComponentId;
        item.mission_type = MAV_MISSION_TYPE_MISSION;

        qDebug() << "MissionEditor: Sending waypoint" << wpIndex << "as seq" << m_expectedItemSeq
                 << "- Lat:" << wp->latitude() << "Lon:" << wp->longitude() << "Alt:" << wp->altitude()
                 << "Command:" << Waypoint::commandName(wp->command());
    }

    mavlink_message_t msg;
    mavlink_msg_mission_item_int_encode(255, 190, &msg, &item);
    m_mavlinkRouter->sendMessage(msg);

    setStatusText(QString("Uploading item %1/%2...").arg(m_expectedItemSeq + 1).arg(m_totalItemCount));
}

void MissionEditor::sendMissionSetCurrent(uint16_t seq) {
    mavlink_message_t msg;
    mavlink_mission_set_current_t setCurrent{};

    setCurrent.target_system = m_targetSystemId;
    setCurrent.target_component = m_targetComponentId;
    setCurrent.seq = seq;

    mavlink_msg_mission_set_current_encode(255, 190, &msg, &setCurrent);
    m_mavlinkRouter->sendMessage(msg);

    qInfo() << "MissionEditor: Sent MISSION_SET_CURRENT, seq:" << seq;
}

void MissionEditor::onMissionAckReceived(uint8_t type, uint8_t missionType) {
    qDebug() << "=== MissionEditor::onMissionAckReceived ===";
    qDebug() << "Type:" << type << "MissionType:" << missionType;

    if (missionType != MAV_MISSION_TYPE_MISSION) {
        qDebug() << "Not a mission type, ignoring";
        return;
    }

    if (m_protocolState == ProtocolState::UploadingItems) {
        if (type == MAV_MISSION_ACCEPTED) {
            qDebug() << "Mission accepted (type 0)";
            setStatusText("Mission upload complete!");
            m_missionModel->markSaved();

            // Set current mission item to 1 (first actual waypoint, not HOME)
            // HOME is seq 0, first waypoint is seq 1
            qDebug() << "Setting current waypoint to seq 1 (first user waypoint)";
            sendMissionSetCurrent(1);

            emit missionUploadComplete(true);
        } else if (type == 13) {
            // Error 13 (MAV_MISSION_INVALID_SEQUENCE) is a false error - mission uploaded successfully
            // Just ignore it and treat as success
            qDebug() << "Received error 13 (false error), treating as success";
            setStatusText("Mission upload complete!");
            m_missionModel->markSaved();

            // CRITICAL FIX: Set to seq 1, not seq 0!
            // Seq 0 is HOME - we want to start at first waypoint (seq 1)
            qDebug() << "Setting current waypoint to seq 1 (first user waypoint)";
            sendMissionSetCurrent(1);

            emit missionUploadComplete(true);
        } else {
            qDebug() << "Mission upload failed with error:" << type;
            setStatusText(QString("Mission upload failed (error code: %1)").arg(type));
            emit missionUploadComplete(false);
        }

        m_protocolState = ProtocolState::Idle;
        setUiEnabled(true);
    } else if (m_protocolState == ProtocolState::DownloadingItems) {
        if (type == MAV_MISSION_ACCEPTED) {
            finalizeMissionDownload();
        } else {
            setStatusText(QString("Mission download failed (error code: %1)").arg(type));
            emit missionDownloadComplete(false);
            m_protocolState = ProtocolState::Idle;
            setUiEnabled(true);
        }
    }
}

void MissionEditor::startMissionDownload() {
    m_protocolState = ProtocolState::DownloadingCount;
    m_downloadBuffer.clear();

    // Update target IDs from vehicle model
    m_targetSystemId = m_vehicleModel->systemId();
    m_targetComponentId = m_vehicleModel->componentId();

    setUiEnabled(false);
    setStatusText("Requesting mission from vehicle...");

    // Send MISSION_REQUEST_LIST
    mavlink_message_t msg;
    mavlink_mission_request_list_t requestList{};
    requestList.target_system = m_targetSystemId;
    requestList.target_component = m_targetComponentId;
    requestList.mission_type = MAV_MISSION_TYPE_MISSION;

    mavlink_msg_mission_request_list_encode(255, 190, &msg, &requestList);
    m_mavlinkRouter->sendMessage(msg);

    qInfo() << "MissionEditor: Sent MISSION_REQUEST_LIST to system" << m_targetSystemId
            << "component" << m_targetComponentId;
}

void MissionEditor::onMissionCountReceived(uint16_t count, uint8_t missionType) {
    if (missionType != MAV_MISSION_TYPE_MISSION) {
        return;
    }

    if (m_protocolState != ProtocolState::DownloadingCount) {
        return;
    }

    m_totalItemCount = count;
    m_expectedItemSeq = 0;
    m_downloadBuffer.clear();

    if (count == 0) {
        setStatusText("Vehicle has no mission");
        m_missionModel->clearMission();
        m_protocolState = ProtocolState::Idle;
        setUiEnabled(true);
        emit missionDownloadComplete(true);
        return;
    }

    m_protocolState = ProtocolState::DownloadingItems;
    setStatusText(QString("Downloading mission (%1 items)...").arg(count));

    qInfo() << "MissionEditor: Vehicle has" << count << "mission items";

    requestNextMissionItem();
}

void MissionEditor::requestNextMissionItem() {
    mavlink_message_t msg;
    mavlink_mission_request_int_t request{};
    request.target_system = m_targetSystemId;
    request.target_component = m_targetComponentId;
    request.seq = m_expectedItemSeq;
    request.mission_type = MAV_MISSION_TYPE_MISSION;

    mavlink_msg_mission_request_int_encode(255, 190, &msg, &request);
    m_mavlinkRouter->sendMessage(msg);

    qDebug() << "MissionEditor: Requested item" << m_expectedItemSeq;
}

void MissionEditor::onMissionItemIntReceived(const mavlink_mission_item_int_t& item) {
    if (m_protocolState != ProtocolState::DownloadingItems) {
        return;
    }

    if (item.seq != m_expectedItemSeq) {
        qWarning() << "MissionEditor: Received unexpected seq" << item.seq << "expected"
                   << m_expectedItemSeq;
        return;
    }

    Waypoint wp(item);
    m_downloadBuffer.append(wp);

    setStatusText(
        QString("Downloaded item %1/%2...").arg(m_expectedItemSeq + 1).arg(m_totalItemCount));

    m_expectedItemSeq++;

    if (m_expectedItemSeq < m_totalItemCount) {
        requestNextMissionItem();
    } else {
        // Send MISSION_ACK to complete download
        mavlink_message_t msg;
        mavlink_mission_ack_t ack{};
        ack.target_system = m_targetSystemId;
        ack.target_component = m_targetComponentId;
        ack.type = MAV_MISSION_ACCEPTED;
        ack.mission_type = MAV_MISSION_TYPE_MISSION;

        mavlink_msg_mission_ack_encode(255, 190, &msg, &ack);
        m_mavlinkRouter->sendMessage(msg);

        finalizeMissionDownload();
    }
}

void MissionEditor::finalizeMissionDownload() {
    m_missionModel->loadMission(m_downloadBuffer);

    setStatusText(QString("Mission download complete! (%1 waypoints)").arg(m_downloadBuffer.count()));
    m_protocolState = ProtocolState::Idle;
    setUiEnabled(true);

    emit missionDownloadComplete(true);

    qInfo() << "MissionEditor: Download complete," << m_downloadBuffer.count() << "waypoints";
}

void MissionEditor::onEditCommandRequested(int row) {
    qDebug() << "=== MissionEditor::onEditCommandRequested ===";
    qDebug() << "Editing waypoint row:" << row;

    const Waypoint* wp = m_missionModel->waypointAt(row);
    if (!wp) {
        qDebug() << "ERROR: Waypoint at row" << row << "is null!";
        return;
    }

    qDebug() << "Original waypoint:";
    qDebug() << "  Command:" << wp->command() << "(" << Waypoint::commandName(wp->command()) << ")";
    qDebug() << "  Lat:" << wp->latitude() << "Lon:" << wp->longitude() << "Alt:" << wp->altitude();
    qDebug() << "  Params:" << wp->param1() << wp->param2() << wp->param3() << wp->param4();

    // Open command editor dialog
    CommandEditorDialog dialog(*wp, this);

    if (dialog.exec() == QDialog::Accepted) {
        // Update waypoint with new command and parameters
        Waypoint updatedWp = dialog.getWaypoint();

        qDebug() << "Updated waypoint returned from dialog:";
        qDebug() << "  Command:" << updatedWp.command() << "(" << Waypoint::commandName(updatedWp.command()) << ")";
        qDebug() << "  Lat:" << updatedWp.latitude() << "Lon:" << updatedWp.longitude() << "Alt:" << updatedWp.altitude();
        qDebug() << "  Params:" << updatedWp.param1() << updatedWp.param2() << updatedWp.param3() << updatedWp.param4();

        m_missionModel->updateWaypoint(row, updatedWp);

        setStatusText(QString("Updated waypoint #%1 command to: %2")
                          .arg(row)
                          .arg(Waypoint::commandName(updatedWp.command())));

        qDebug() << "MissionEditor: Updated waypoint" << row << "to command"
                 << Waypoint::commandName(updatedWp.command());
    } else {
        qDebug() << "Dialog was cancelled - no changes made";
    }
    qDebug() << "=== End onEditCommandRequested ===";
}

void MissionEditor::onDeleteRequested(int row) {
    // Confirm deletion
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Waypoint",
        QString("Are you sure you want to delete waypoint #%1?").arg(row),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_missionModel->removeWaypoint(row);
        setStatusText(QString("Deleted waypoint #%1").arg(row));
        qDebug() << "MissionEditor: Deleted waypoint" << row;
    }
}
