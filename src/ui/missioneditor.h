#ifndef MISSIONEDITOR_H
#define MISSIONEDITOR_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QStyledItemDelegate>
#include <QComboBox>
#include "models/missionmodel.h"
#include "models/vehiclemodel.h"
#include "comm/mavlinkrouter.h"

/**
 * @brief Delegate for Edit button
 */
class CommandDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit CommandDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
              const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model,
                    const QStyleOptionViewItem& option, const QModelIndex& index) override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                  const QModelIndex& index) const override;

signals:
    void editCommandRequested(int row) const;
};

/**
 * @brief Delegate for Delete button
 */
class DeleteDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit DeleteDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
              const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model,
                    const QStyleOptionViewItem& option, const QModelIndex& index) override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                  const QModelIndex& index) const override;

signals:
    void deleteRequested(int row) const;
};

/**
 * @brief Mission Editor widget for creating and managing waypoint missions
 *
 * This widget provides a table-based interface for:
 * - Adding, removing, and editing waypoints
 * - Uploading missions to the vehicle
 * - Downloading missions from the vehicle
 * - Visual feedback during mission operations
 */
class MissionEditor : public QWidget {
    Q_OBJECT

public:
    explicit MissionEditor(MissionModel* missionModel, MavlinkRouter* mavlinkRouter,
                           VehicleModel* vehicleModel, QWidget* parent = nullptr);
    ~MissionEditor() override = default;

public slots:
    /**
     * @brief Handle mission protocol responses
     */
    void onMissionCountReceived(uint16_t count, uint8_t missionType);
    void onMissionRequestIntReceived(uint16_t seq, uint8_t missionType);
    void onMissionItemIntReceived(const mavlink_mission_item_int_t& item);
    void onMissionAckReceived(uint8_t type, uint8_t missionType);

signals:
    /**
     * @brief Emitted when mission operations complete
     */
    void missionUploadComplete(bool success);
    void missionDownloadComplete(bool success);

private slots:
    void onAddWaypointClicked();
    void onRemoveWaypointClicked();
    void onClearMissionClicked();
    void onUploadMissionClicked();
    void onDownloadMissionClicked();
    void onTableItemChanged(QTableWidgetItem* item);
    void onMissionModelChanged();
    void onEditCommandRequested(int row);
    void onDeleteRequested(int row);

private:
    void setupUi();
    void updateTable();
    void updateTableRow(int row);
    void setUiEnabled(bool enabled);
    void setStatusText(const QString& text);

    // Mission upload/download state machine
    void startMissionUpload();
    void sendNextMissionItem();
    void sendMissionSetCurrent(uint16_t seq);
    void startMissionDownload();
    void requestNextMissionItem();
    void finalizeMissionDownload();

    MissionModel* m_missionModel;
    MavlinkRouter* m_mavlinkRouter;
    VehicleModel* m_vehicleModel;

    // UI elements
    QTableWidget* m_tableWidget;
    CommandDelegate* m_commandDelegate;
    DeleteDelegate* m_deleteDelegate;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;
    QPushButton* m_clearButton;
    QPushButton* m_uploadButton;
    QPushButton* m_downloadButton;
    QLabel* m_statusLabel;

    // Mission protocol state
    enum class ProtocolState {
        Idle,
        UploadingCount,
        UploadingItems,
        DownloadingCount,
        DownloadingItems
    };

    ProtocolState m_protocolState;
    uint16_t m_expectedItemSeq;
    uint16_t m_totalItemCount;
    QList<Waypoint> m_downloadBuffer;
    uint8_t m_targetSystemId;
    uint8_t m_targetComponentId;

    bool m_updatingTable;
};

#endif  // MISSIONEDITOR_H
