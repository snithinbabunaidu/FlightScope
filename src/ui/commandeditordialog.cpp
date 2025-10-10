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
    qDebug() << "=== CommandEditorDialog::onCommandSelected ===";
    qDebug() << "Selected command:" << command << "(" << Waypoint::commandName(command) << ")";
    qDebug() << "BEFORE - Waypoint state:";
    qDebug() << "  Lat:" << m_waypoint.latitude() << "Lon:" << m_waypoint.longitude() << "Alt:" << m_waypoint.altitude();
    qDebug() << "  Params:" << m_waypoint.param1() << m_waypoint.param2() << m_waypoint.param3() << m_waypoint.param4();

    m_waypoint.setCommand(command);

    // Set proper default values based on command type
    // Only set defaults if current values are 0.0 (indicating new waypoint)
    bool isNewWaypoint = (m_waypoint.param1() == 0.0f && m_waypoint.param2() == 0.0f &&
                          m_waypoint.param3() == 0.0f && m_waypoint.param4() == 0.0f);

    qDebug() << "  isNewWaypoint:" << isNewWaypoint;

    if (isNewWaypoint) {
        switch (command) {
            case MAV_CMD_NAV_WAYPOINT:
                // param1: Hold time - 0s (fly-through)
                // param2: Acceptance radius - 2m (predictable)
                // param3: Pass radius - 0 (use acceptance)
                // param4: Yaw - 0 (hold current)
                m_waypoint.setParam1(0.0f);
                m_waypoint.setParam2(2.0f);
                m_waypoint.setParam3(0.0f);
                m_waypoint.setParam4(0.0f);
                break;

            case MAV_CMD_NAV_TAKEOFF:
                // param1: Pitch - 0 (vertical)
                // param4: Yaw - NaN means "hold current"
                m_waypoint.setParam1(0.0f);
                m_waypoint.setParam4(NAN);
                break;

            case MAV_CMD_NAV_LAND:
                // param1: Abort altitude - 0 (land immediately)
                // param2: Land mode - 0 (normal)
                // param4: Yaw - 0 (hold current)
                m_waypoint.setParam1(0.0f);
                m_waypoint.setParam2(0.0f);
                m_waypoint.setParam4(0.0f);
                break;

            case MAV_CMD_NAV_LOITER_TIME:
                // param1: Hold time - 30s (useful default)
                // param3: Radius - 25m (visible circle)
                // param4: Direction - 1 (clockwise)
                m_waypoint.setParam1(30.0f);
                m_waypoint.setParam3(25.0f);
                m_waypoint.setParam4(1.0f);
                break;

            case MAV_CMD_NAV_LOITER_TURNS:
                // param1: Turns - 2 (observable)
                // param3: Radius - 25m
                // param4: Direction - 1 (clockwise)
                m_waypoint.setParam1(2.0f);
                m_waypoint.setParam3(25.0f);
                m_waypoint.setParam4(1.0f);
                break;

            case MAV_CMD_NAV_LOITER_UNLIM:
                // param3: Radius - 25m
                // param4: Direction - 1 (clockwise)
                m_waypoint.setParam3(25.0f);
                m_waypoint.setParam4(1.0f);
                break;

            case MAV_CMD_DO_SET_ROI:
                // param1: ROI mode - 0 (location)
                // param2: Mission index - 0 (unused)
                // param3: ROI index - 0 (unused)
                m_waypoint.setParam1(0.0f);
                m_waypoint.setParam2(0.0f);
                m_waypoint.setParam3(0.0f);
                break;

            case MAV_CMD_DO_CHANGE_SPEED:
                // param1: Speed type - 1 (ground speed for copters)
                // param2: Speed - 5.0 m/s (typical cruise)
                // param3: Throttle - -1 (no change)
                m_waypoint.setParam1(1.0f);
                m_waypoint.setParam2(5.0f);
                m_waypoint.setParam3(-1.0f);
                break;

            case MAV_CMD_DO_JUMP:
                // param1: Target WP - 1 (first waypoint)
                // param2: Repeat count - 1 (one loop)
                m_waypoint.setParam1(1.0f);
                m_waypoint.setParam2(1.0f);
                break;

            case MAV_CMD_NAV_RETURN_TO_LAUNCH:
                // No parameters needed
                break;

            default:
                break;
        }
    }

    qDebug() << "AFTER defaults - Waypoint state:";
    qDebug() << "  Lat:" << m_waypoint.latitude() << "Lon:" << m_waypoint.longitude() << "Alt:" << m_waypoint.altitude();
    qDebug() << "  Params:" << m_waypoint.param1() << m_waypoint.param2() << m_waypoint.param3() << m_waypoint.param4();

    // Remove old editor view if it exists
    if (m_editorView) {
        m_stackedWidget->removeWidget(m_editorView);
        delete m_editorView;
    }

    createParameterEditorView(command);
    m_stackedWidget->setCurrentWidget(m_editorView);

    // Show save button
    m_saveButton->setVisible(true);
    qDebug() << "=== End onCommandSelected ===";
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

    // Helper lambda to create parameter row with validation
    auto createParamRow = [&](const QString& labelText, QDoubleSpinBox*& spinBox,
                              QLabel*& label, QWidget*& widget,
                              double minVal, double maxVal, double step = 0.1, int decimals = 2) {
        widget = new QWidget(paramGroup);
        QHBoxLayout* rowLayout = new QHBoxLayout(widget);
        rowLayout->setContentsMargins(0, 0, 0, 0);

        label = new QLabel(labelText, widget);
        label->setObjectName("parameterLabel");
        label->setMinimumWidth(200);

        spinBox = new QDoubleSpinBox(widget);
        spinBox->setRange(minVal, maxVal);
        spinBox->setDecimals(decimals);
        spinBox->setSingleStep(step);
        spinBox->setMinimumWidth(120);

        rowLayout->addWidget(label);
        rowLayout->addWidget(spinBox, 1);

        paramLayout->addWidget(widget);
    };

    // Create parameter rows with command-specific validation ranges
    // Default ranges (will be overridden per command)
    createParamRow("Parameter 1:", m_param1Spin, m_param1Label, m_param1Widget, -1000, 1000);
    createParamRow("Parameter 2:", m_param2Spin, m_param2Label, m_param2Widget, -1000, 1000);
    createParamRow("Parameter 3:", m_param3Spin, m_param3Label, m_param3Widget, -1000, 1000);
    createParamRow("Parameter 4:", m_param4Spin, m_param4Label, m_param4Widget, -1000, 1000);

    // Set current values
    m_param1Spin->setValue(m_waypoint.param1());
    m_param2Spin->setValue(m_waypoint.param2());
    m_param3Spin->setValue(m_waypoint.param3());
    m_param4Spin->setValue(m_waypoint.param4());

    // Configure labels, visibility, and validation ranges based on command type
    switch (command) {
        case MAV_CMD_NAV_WAYPOINT:
            m_param1Label->setText("Hold Time (s):");
            m_param1Spin->setRange(0, 3600);  // 0s to 1 hour
            m_param1Spin->setSingleStep(1.0);

            m_param2Label->setText("Acceptance Radius (m):");
            m_param2Spin->setRange(0.1, 100);  // Min 10cm, max 100m
            m_param2Spin->setSingleStep(0.5);

            m_param3Label->setText("Pass Radius (m):");
            m_param3Spin->setRange(0, 100);  // 0 = use acceptance radius
            m_param3Spin->setSingleStep(0.5);

            m_param4Label->setText("Yaw Angle (°):");
            m_param4Spin->setRange(-360, 360);  // Full rotation
            m_param4Spin->setSingleStep(5.0);
            m_param4Spin->setDecimals(1);
            break;

        case MAV_CMD_NAV_LOITER_TIME:
            m_param1Label->setText("Hold Time (s):");
            m_param1Spin->setRange(1, 3600);  // 1s to 1 hour
            m_param1Spin->setSingleStep(5.0);

            m_param2Widget->setVisible(false);

            m_param3Label->setText("Loiter Radius (m):");
            m_param3Spin->setRange(1, 500);  // Min 1m, max 500m
            m_param3Spin->setSingleStep(5.0);

            m_param4Label->setText("Direction (1=CW, -1=CCW):");
            m_param4Spin->setRange(-1, 1);
            m_param4Spin->setSingleStep(2.0);  // Jump between -1 and 1
            m_param4Spin->setDecimals(0);
            break;

        case MAV_CMD_NAV_LOITER_TURNS:
            m_param1Label->setText("Number of Turns:");
            m_param1Spin->setRange(1, 100);  // Min 1 turn, max 100
            m_param1Spin->setSingleStep(1.0);
            m_param1Spin->setDecimals(0);

            m_param2Widget->setVisible(false);

            m_param3Label->setText("Loiter Radius (m):");
            m_param3Spin->setRange(1, 500);
            m_param3Spin->setSingleStep(5.0);

            m_param4Label->setText("Direction (1=CW, -1=CCW):");
            m_param4Spin->setRange(-1, 1);
            m_param4Spin->setSingleStep(2.0);
            m_param4Spin->setDecimals(0);
            break;

        case MAV_CMD_NAV_LOITER_UNLIM:
            m_param1Widget->setVisible(false);
            m_param2Widget->setVisible(false);

            m_param3Label->setText("Loiter Radius (m):");
            m_param3Spin->setRange(1, 500);
            m_param3Spin->setSingleStep(5.0);

            m_param4Label->setText("Direction (1=CW, -1=CCW):");
            m_param4Spin->setRange(-1, 1);
            m_param4Spin->setSingleStep(2.0);
            m_param4Spin->setDecimals(0);
            break;

        case MAV_CMD_NAV_TAKEOFF:
            m_param1Label->setText("Pitch Angle (°):");
            m_param1Spin->setRange(-45, 45);  // Reasonable pitch range
            m_param1Spin->setSingleStep(5.0);
            m_param1Spin->setDecimals(1);

            m_param2Widget->setVisible(false);
            m_param3Widget->setVisible(false);

            m_param4Label->setText("Yaw Angle (°) [NaN=Hold]:");
            m_param4Spin->setRange(-360, 360);
            m_param4Spin->setSingleStep(5.0);
            m_param4Spin->setDecimals(1);
            m_param4Spin->setSpecialValueText("NaN (Hold Current)");
            break;

        case MAV_CMD_NAV_LAND:
            m_param1Label->setText("Abort Altitude (m):");
            m_param1Spin->setRange(0, 100);  // 0 = land immediately
            m_param1Spin->setSingleStep(1.0);

            m_param2Label->setText("Land Mode:");
            m_param2Spin->setRange(0, 3);  // ArduPilot land modes
            m_param2Spin->setSingleStep(1.0);
            m_param2Spin->setDecimals(0);

            m_param3Widget->setVisible(false);

            m_param4Label->setText("Yaw Angle (°):");
            m_param4Spin->setRange(-360, 360);
            m_param4Spin->setSingleStep(5.0);
            m_param4Spin->setDecimals(1);
            break;

        case MAV_CMD_NAV_RETURN_TO_LAUNCH:
            m_param1Widget->setVisible(false);
            m_param2Widget->setVisible(false);
            m_param3Widget->setVisible(false);
            m_param4Widget->setVisible(false);
            break;

        case MAV_CMD_DO_SET_ROI:
            m_param1Label->setText("ROI Mode (0=Location):");
            m_param1Spin->setRange(0, 4);  // MAV_ROI enum
            m_param1Spin->setSingleStep(1.0);
            m_param1Spin->setDecimals(0);

            m_param2Label->setText("Mission Index:");
            m_param2Spin->setRange(0, 255);  // WP sequence range
            m_param2Spin->setSingleStep(1.0);
            m_param2Spin->setDecimals(0);

            m_param3Label->setText("ROI Index:");
            m_param3Spin->setRange(0, 255);
            m_param3Spin->setSingleStep(1.0);
            m_param3Spin->setDecimals(0);

            m_param4Widget->setVisible(false);
            break;

        case MAV_CMD_DO_CHANGE_SPEED:
            m_param1Label->setText("Speed Type (0=Air, 1=Ground):");
            m_param1Spin->setRange(0, 1);
            m_param1Spin->setSingleStep(1.0);
            m_param1Spin->setDecimals(0);

            m_param2Label->setText("Speed (m/s):");
            m_param2Spin->setRange(0.5, 30);  // Copter speed range
            m_param2Spin->setSingleStep(0.5);

            m_param3Label->setText("Throttle (-1=No Change):");
            m_param3Spin->setRange(-1, 100);  // -1 or 0-100%
            m_param3Spin->setSingleStep(5.0);
            m_param3Spin->setDecimals(0);

            m_param4Widget->setVisible(false);
            break;

        case MAV_CMD_DO_JUMP:
            m_param1Label->setText("Target Waypoint #:");
            m_param1Spin->setRange(0, 255);  // Mission sequence range
            m_param1Spin->setSingleStep(1.0);
            m_param1Spin->setDecimals(0);

            m_param2Label->setText("Repeat Count (-1=Forever):");
            m_param2Spin->setRange(-1, 100);  // -1 = infinite, 0-100 repeats
            m_param2Spin->setSingleStep(1.0);
            m_param2Spin->setDecimals(0);

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
    qDebug() << "=== CommandEditorDialog::onSaveClicked ===";
    qDebug() << "BEFORE param save - Waypoint state:";
    qDebug() << "  Command:" << m_waypoint.command() << "(" << Waypoint::commandName(m_waypoint.command()) << ")";
    qDebug() << "  Lat:" << m_waypoint.latitude() << "Lon:" << m_waypoint.longitude() << "Alt:" << m_waypoint.altitude();
    qDebug() << "  Params:" << m_waypoint.param1() << m_waypoint.param2() << m_waypoint.param3() << m_waypoint.param4();

    // Save parameter values
    if (m_param1Spin && m_param1Widget->isVisible()) {
        m_waypoint.setParam1(m_param1Spin->value());
        qDebug() << "  Set param1 to:" << m_param1Spin->value();
    }
    if (m_param2Spin && m_param2Widget->isVisible()) {
        m_waypoint.setParam2(m_param2Spin->value());
        qDebug() << "  Set param2 to:" << m_param2Spin->value();
    }
    if (m_param3Spin && m_param3Widget->isVisible()) {
        m_waypoint.setParam3(m_param3Spin->value());
        qDebug() << "  Set param3 to:" << m_param3Spin->value();
    }
    if (m_param4Spin && m_param4Widget->isVisible()) {
        m_waypoint.setParam4(m_param4Spin->value());
        qDebug() << "  Set param4 to:" << m_param4Spin->value();
    }

    qDebug() << "AFTER param save - Waypoint state:";
    qDebug() << "  Command:" << m_waypoint.command() << "(" << Waypoint::commandName(m_waypoint.command()) << ")";
    qDebug() << "  Lat:" << m_waypoint.latitude() << "Lon:" << m_waypoint.longitude() << "Alt:" << m_waypoint.altitude();
    qDebug() << "  Params:" << m_waypoint.param1() << m_waypoint.param2() << m_waypoint.param3() << m_waypoint.param4();
    qDebug() << "=== End onSaveClicked - Accepting dialog ===";

    accept();
}

void CommandEditorDialog::onCancelClicked() {
    reject();
}
