#include "connectdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>

ConnectDialog::ConnectDialog(QWidget* parent)
    : QDialog(parent), m_presetCombo(nullptr), m_connectTypeCombo(nullptr),
      m_localAddressEdit(nullptr), m_localPortSpin(nullptr), m_remoteAddressEdit(nullptr),
      m_remotePortSpin(nullptr), m_connectButton(nullptr), m_cancelButton(nullptr) {
    setupUi();
    loadPresets();
}

void ConnectDialog::setupUi() {
    setWindowTitle("Connect to Vehicle");
    setModal(true);
    resize(450, 350);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Preset selection
    QGroupBox* presetGroup = new QGroupBox("Quick Connect", this);
    QFormLayout* presetLayout = new QFormLayout(presetGroup);

    m_presetCombo = new QComboBox(this);
    m_presetCombo->addItem("Custom Configuration");
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &ConnectDialog::onPresetChanged);
    presetLayout->addRow("Preset:", m_presetCombo);

    mainLayout->addWidget(presetGroup);

    // Connection type
    QGroupBox* connectionGroup = new QGroupBox("Connection Settings", this);
    QFormLayout* connectionLayout = new QFormLayout(connectionGroup);

    m_connectTypeCombo = new QComboBox(this);
    m_connectTypeCombo->addItem("UDP");
    m_connectTypeCombo->addItem("Serial (Coming Soon)");
    m_connectTypeCombo->addItem("TCP (Coming Soon)");
    m_connectTypeCombo->setCurrentIndex(0);
    m_connectTypeCombo->setEnabled(true);
    connect(m_connectTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &ConnectDialog::onConnectTypeChanged);
    connectionLayout->addRow("Type:", m_connectTypeCombo);

    // UDP Configuration
    m_localAddressEdit = new QLineEdit("0.0.0.0", this);
    connectionLayout->addRow("Local Address:", m_localAddressEdit);

    m_localPortSpin = new QSpinBox(this);
    m_localPortSpin->setRange(1, 65535);
    m_localPortSpin->setValue(14551);  // Different from Mission Planner's 14550
    connectionLayout->addRow("Local Port:", m_localPortSpin);

    m_remoteAddressEdit = new QLineEdit("127.0.0.1", this);
    connectionLayout->addRow("Remote Address:", m_remoteAddressEdit);

    m_remotePortSpin = new QSpinBox(this);
    m_remotePortSpin->setRange(1, 65535);
    m_remotePortSpin->setValue(14550);
    connectionLayout->addRow("Remote Port:", m_remotePortSpin);

    mainLayout->addWidget(connectionGroup);

    // Info label
    QLabel* infoLabel = new QLabel(
        "<b>Important:</b> FlightScope listens on a different port to avoid conflicts.<br>"
        "<i>In Mission Planner, go to: Ctrl+F → \"MAVLink\" → Add output: UDP Client 127.0.0.1:14551</i>",
        this);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { background-color: #ffffcc; padding: 5px; border: 1px solid #999; }");
    mainLayout->addWidget(infoLabel);

    // Spacer
    mainLayout->addStretch();

    // Buttons
    QDialogButtonBox* buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_connectButton = buttonBox->button(QDialogButtonBox::Ok);
    m_connectButton->setText("Connect");
    m_cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

void ConnectDialog::loadPresets() {
    // Clear existing presets
    m_presets.clear();

    // Add ArduPilot SITL preset
    Preset arduPilot;
    arduPilot.name = "ArduPilot SITL (Listen on 14551)";
    arduPilot.localAddress = "0.0.0.0";
    arduPilot.localPort = 14551;  // Listen on different port to avoid conflict
    arduPilot.remoteAddress = "127.0.0.1";
    arduPilot.remotePort = 14550;  // Mission Planner broadcasts on 14550
    m_presets.append(arduPilot);

    // Add PX4 SITL preset
    Preset px4;
    px4.name = "PX4 SITL (Listen on 14541)";
    px4.localAddress = "0.0.0.0";
    px4.localPort = 14541;  // Different from PX4's 14540
    px4.remoteAddress = "127.0.0.1";
    px4.remotePort = 14540;
    m_presets.append(px4);

    // Add MAVProxy preset
    Preset mavproxy;
    mavproxy.name = "MAVProxy (Listen on 14552)";
    mavproxy.localAddress = "0.0.0.0";
    mavproxy.localPort = 14552;
    mavproxy.remoteAddress = "127.0.0.1";
    mavproxy.remotePort = 14550;
    m_presets.append(mavproxy);

    // Populate combo box
    for (const Preset& preset : m_presets) {
        m_presetCombo->addItem(preset.name);
    }
}

void ConnectDialog::onPresetChanged(int index) {
    if (index <= 0) {
        // Custom configuration - do nothing
        return;
    }

    // Apply preset (index - 1 because first item is "Custom Configuration")
    const Preset& preset = m_presets[index - 1];

    m_localAddressEdit->setText(preset.localAddress);
    m_localPortSpin->setValue(preset.localPort);
    m_remoteAddressEdit->setText(preset.remoteAddress);
    m_remotePortSpin->setValue(preset.remotePort);
}

void ConnectDialog::onConnectTypeChanged(int index) {
    // Currently only UDP is supported
    bool isUdp = (index == 0);

    m_localAddressEdit->setEnabled(isUdp);
    m_localPortSpin->setEnabled(isUdp);
    m_remoteAddressEdit->setEnabled(isUdp);
    m_remotePortSpin->setEnabled(isUdp);
    m_connectButton->setEnabled(isUdp);
}

UdpLink* ConnectDialog::getConfiguredLink() {
    UdpLink::Configuration config;
    config.name = QString("UDP %1:%2").arg(m_localAddressEdit->text()).arg(m_localPortSpin->value());
    config.localAddress = QHostAddress(m_localAddressEdit->text());
    config.localPort = static_cast<quint16>(m_localPortSpin->value());
    config.remoteAddress = QHostAddress(m_remoteAddressEdit->text());
    config.remotePort = static_cast<quint16>(m_remotePortSpin->value());
    config.isServer = true;  // Server mode (listen)

    return new UdpLink(config);
}
