#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include "../comm/udplink.h"

/**
 * @brief Dialog for configuring and initiating connections
 *
 * Provides UI for:
 * - Selecting connection type (UDP, Serial, TCP)
 * - Configuring connection parameters
 * - Preset configurations for SITL
 */
class ConnectDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget* parent = nullptr);
    ~ConnectDialog() override = default;

    /**
     * @brief Get the configured UDP link
     * @return Configured UdpLink (caller takes ownership)
     */
    UdpLink* getConfiguredLink();

private slots:
    void onPresetChanged(int index);
    void onConnectTypeChanged(int index);

private:
    void setupUi();
    void loadPresets();

    // UI Elements
    QComboBox* m_presetCombo;
    QComboBox* m_connectTypeCombo;

    // UDP specific
    QLineEdit* m_localAddressEdit;
    QSpinBox* m_localPortSpin;
    QLineEdit* m_remoteAddressEdit;
    QSpinBox* m_remotePortSpin;

    QPushButton* m_connectButton;
    QPushButton* m_cancelButton;

    // Presets
    struct Preset {
        QString name;
        QString localAddress;
        quint16 localPort;
        QString remoteAddress;
        quint16 remotePort;
    };
    QList<Preset> m_presets;
};

#endif  // CONNECTDIALOG_H
