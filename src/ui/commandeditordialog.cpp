#include "commandeditordialog.h"
#include <QScrollArea>
#include <QGroupBox>

CommandEditorDialog::CommandEditorDialog(const Waypoint& waypoint, QWidget* parent)
    : QDialog(parent),
      m_waypoint(waypoint),
      m_stackedWidget(nullptr),
      m_selectionView(nullptr),
      m_editorView(nullptr),
      m_buttonBox(nullptr),
      m_saveButton(nullptr),
      m_cancelButton(nullptr),
      m_param1Spin(nullptr),
      m_param2Spin(nullptr),
      m_param3Spin(nullptr),
      m_param4Spin(nullptr) {
    setupUi();
}

void CommandEditorDialog::setupUi() {
    setObjectName("CommandEditorDialog");
    setWindowTitle("Command Editor");
    setMinimumSize(500, 600);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Stacked widget for two views
    m_stackedWidget = new QStackedWidget(this);

    // Create both views
    createCommandSelectionView();

    mainLayout->addWidget(m_stackedWidget);

    // Dialog buttons
    m_buttonBox = new QDialogButtonBox(this);
    m_saveButton = m_buttonBox->addButton("Save", QDialogButtonBox::AcceptRole);
    m_saveButton->setObjectName("saveButton");
    m_saveButton->setVisible(false);  // Hidden until command selected

    m_cancelButton = m_buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);
    m_cancelButton->setObjectName("cancelButton");

    mainLayout->addWidget(m_buttonBox);

    connect(m_saveButton, &QPushButton::clicked, this, &CommandEditorDialog::onSaveClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &CommandEditorDialog::onCancelClicked);
}

void CommandEditorDialog::createCommandSelectionView() {
    m_selectionView = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(m_selectionView);

    // Title
    QLabel* title = new QLabel("Select Mission Command", m_selectionView);
    title->setObjectName("dialogTitle");
    QFont titleFont = title->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);
    layout->addWidget(title);

    // Scroll area for commands
    QScrollArea* scrollArea = new QScrollArea(m_selectionView);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollContent = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);

    // Navigation Commands
    QLabel* navHeader = new QLabel("Navigation Commands", scrollContent);
    navHeader->setObjectName("categoryHeader");
    scrollLayout->addWidget(navHeader);

    scrollLayout->addWidget(createCommandButton(
        "Waypoint", "Fly to a specific GPS location", MAV_CMD_NAV_WAYPOINT));
    scrollLayout->addWidget(createCommandButton(
        "Takeoff", "Ascend to specified altitude and await further commands", MAV_CMD_NAV_TAKEOFF));
    scrollLayout->addWidget(createCommandButton(
        "Land", "Descend and land at current or specified location", MAV_CMD_NAV_LAND));
    scrollLayout->addWidget(createCommandButton(
        "Return to Launch", "Return to home position and land", MAV_CMD_NAV_RETURN_TO_LAUNCH));

    // Loiter Commands
    QLabel* loiterHeader = new QLabel("Loiter Commands", scrollContent);
    loiterHeader->setObjectName("categoryHeader");
    scrollLayout->addWidget(loiterHeader);

    scrollLayout->addWidget(createCommandButton(
        "Loiter Unlimited", "Circle at location indefinitely", MAV_CMD_NAV_LOITER_UNLIM));
    scrollLayout->addWidget(createCommandButton(
        "Loiter Time", "Circle at location for specified seconds", MAV_CMD_NAV_LOITER_TIME));
    scrollLayout->addWidget(createCommandButton(
        "Loiter Turns", "Circle at location for specified number of turns", MAV_CMD_NAV_LOITER_TURNS));

    // Action Commands
    QLabel* actionHeader = new QLabel("Action Commands", scrollContent);
    actionHeader->setObjectName("categoryHeader");
    scrollLayout->addWidget(actionHeader);

    scrollLayout->addWidget(createCommandButton(
        "Set ROI", "Point camera/gimbal at specified location", MAV_CMD_DO_SET_ROI));
    scrollLayout->addWidget(createCommandButton(
        "Change Speed", "Change vehicle airspeed or groundspeed", MAV_CMD_DO_CHANGE_SPEED));
    scrollLayout->addWidget(createCommandButton(
        "Jump to Waypoint", "Jump to another waypoint in the sequence", MAV_CMD_DO_JUMP));

    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);

    m_stackedWidget->addWidget(m_selectionView);
}

QPushButton* CommandEditorDialog::createCommandButton(const QString& name,
                                                       const QString& description,
                                                       uint16_t command) {
    QPushButton* button = new QPushButton(m_selectionView);
    button->setObjectName("commandButton");
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    button->setMinimumHeight(60);

    // Use plain text with name on top, description below
    QString buttonText = QString("%1\n%2").arg(name).arg(description);
    button->setText(buttonText);
    button->setProperty("command", command);

    connect(button, &QPushButton::clicked, this, [this, command]() { onCommandSelected(command); });

    return button;
}

void CommandEditorDialog::onCommandSelected(uint16_t command) {
    m_waypoint.setCommand(command);

    // Remove old editor view if it exists
    if (m_editorView) {
        m_stackedWidget->removeWidget(m_editorView);
        delete m_editorView;
    }

    createParameterEditorView(command);
    m_stackedWidget->setCurrentWidget(m_editorView);

    // Show save button
    m_saveButton->setVisible(true);
}

void CommandEditorDialog::createParameterEditorView(uint16_t command) {
    m_editorView = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(m_editorView);

    // Title with command name
    QLabel* title = new QLabel(QString("Configure: %1").arg(Waypoint::commandName(command)),
                               m_editorView);
    title->setObjectName("dialogTitle");
    QFont titleFont = title->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);
    layout->addWidget(title);

    // Back button
    QPushButton* backButton = new QPushButton("← Back to Command Selection", m_editorView);
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, [this]() {
        m_stackedWidget->setCurrentWidget(m_selectionView);
        m_saveButton->setVisible(false);
    });
    layout->addWidget(backButton);

    // Parameter form
    QGroupBox* paramGroup = new QGroupBox("Parameters", m_editorView);
    paramGroup->setObjectName("parameterGroup");
    QVBoxLayout* paramLayout = new QVBoxLayout(paramGroup);

    // Helper lambda to create parameter row
    auto createParamRow = [&](const QString& labelText, QDoubleSpinBox*& spinBox,
                              QLabel*& label, QWidget*& widget) {
        widget = new QWidget(paramGroup);
        QHBoxLayout* rowLayout = new QHBoxLayout(widget);
        rowLayout->setContentsMargins(0, 0, 0, 0);

        label = new QLabel(labelText, widget);
        label->setObjectName("parameterLabel");
        label->setMinimumWidth(150);

        spinBox = new QDoubleSpinBox(widget);
        spinBox->setRange(-10000, 10000);
        spinBox->setDecimals(2);
        spinBox->setSingleStep(0.1);

        rowLayout->addWidget(label);
        rowLayout->addWidget(spinBox, 1);

        paramLayout->addWidget(widget);
    };

    // Create all 4 parameter rows
    createParamRow("Parameter 1:", m_param1Spin, m_param1Label, m_param1Widget);
    createParamRow("Parameter 2:", m_param2Spin, m_param2Label, m_param2Widget);
    createParamRow("Parameter 3:", m_param3Spin, m_param3Label, m_param3Widget);
    createParamRow("Parameter 4:", m_param4Spin, m_param4Label, m_param4Widget);

    // Set current values
    m_param1Spin->setValue(m_waypoint.param1());
    m_param2Spin->setValue(m_waypoint.param2());
    m_param3Spin->setValue(m_waypoint.param3());
    m_param4Spin->setValue(m_waypoint.param4());

    // Configure labels and visibility based on command type
    switch (command) {
        case MAV_CMD_NAV_WAYPOINT:
            m_param1Label->setText("Hold Time (s):");
            m_param2Label->setText("Acceptance Radius (m):");
            m_param3Label->setText("Pass Radius (m):");
            m_param4Label->setText("Yaw Angle (°):");
            break;

        case MAV_CMD_NAV_LOITER_TIME:
            m_param1Label->setText("Hold Time (s):");
            m_param2Widget->setVisible(false);
            m_param3Label->setText("Loiter Radius (m):");
            m_param4Label->setText("Loiter Direction:");
            break;

        case MAV_CMD_NAV_LOITER_TURNS:
            m_param1Label->setText("Number of Turns:");
            m_param2Widget->setVisible(false);
            m_param3Label->setText("Loiter Radius (m):");
            m_param4Label->setText("Loiter Direction:");
            break;

        case MAV_CMD_NAV_LOITER_UNLIM:
            m_param1Widget->setVisible(false);
            m_param2Widget->setVisible(false);
            m_param3Label->setText("Loiter Radius (m):");
            m_param4Label->setText("Loiter Direction:");
            break;

        case MAV_CMD_NAV_TAKEOFF:
            m_param1Label->setText("Pitch Angle (°):");
            m_param2Widget->setVisible(false);
            m_param3Widget->setVisible(false);
            m_param4Label->setText("Yaw Angle (°):");
            break;

        case MAV_CMD_NAV_LAND:
            m_param1Label->setText("Abort Altitude (m):");
            m_param2Label->setText("Land Mode:");
            m_param3Widget->setVisible(false);
            m_param4Label->setText("Yaw Angle (°):");
            break;

        case MAV_CMD_NAV_RETURN_TO_LAUNCH:
            m_param1Widget->setVisible(false);
            m_param2Widget->setVisible(false);
            m_param3Widget->setVisible(false);
            m_param4Widget->setVisible(false);
            break;

        case MAV_CMD_DO_SET_ROI:
            m_param1Label->setText("ROI Mode:");
            m_param2Label->setText("Mission Index:");
            m_param3Label->setText("ROI Index:");
            m_param4Widget->setVisible(false);
            break;

        case MAV_CMD_DO_CHANGE_SPEED:
            m_param1Label->setText("Speed Type (0=Air, 1=Ground):");
            m_param2Label->setText("Speed (m/s):");
            m_param3Label->setText("Throttle (%):");
            m_param4Widget->setVisible(false);
            break;

        case MAV_CMD_DO_JUMP:
            m_param1Label->setText("Target Waypoint #:");
            m_param2Label->setText("Repeat Count:");
            m_param3Widget->setVisible(false);
            m_param4Widget->setVisible(false);
            break;

        default:
            // Default generic labels
            break;
    }

    layout->addWidget(paramGroup);
    layout->addStretch();

    m_stackedWidget->addWidget(m_editorView);
}

void CommandEditorDialog::onSaveClicked() {
    // Save parameter values
    if (m_param1Spin && m_param1Widget->isVisible()) {
        m_waypoint.setParam1(m_param1Spin->value());
    }
    if (m_param2Spin && m_param2Widget->isVisible()) {
        m_waypoint.setParam2(m_param2Spin->value());
    }
    if (m_param3Spin && m_param3Widget->isVisible()) {
        m_waypoint.setParam3(m_param3Spin->value());
    }
    if (m_param4Spin && m_param4Widget->isVisible()) {
        m_waypoint.setParam4(m_param4Spin->value());
    }

    accept();
}

void CommandEditorDialog::onCancelClicked() {
    reject();
}
