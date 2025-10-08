#ifndef COMMANDEDITORDIALOG_H
#define COMMANDEDITORDIALOG_H

#include <QDialog>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include "models/waypoint.h"

/**
 * @brief Command Editor Dialog - Two-step wizard for configuring mission commands
 *
 * Step 1: Select command type from categorized list
 * Step 2: Configure command-specific parameters
 */
class CommandEditorDialog : public QDialog {
    Q_OBJECT

public:
    explicit CommandEditorDialog(const Waypoint& waypoint, QWidget* parent = nullptr);
    ~CommandEditorDialog() override = default;

    /**
     * @brief Get the configured waypoint
     */
    Waypoint getWaypoint() const { return m_waypoint; }

private slots:
    void onCommandSelected(uint16_t command);
    void onSaveClicked();
    void onCancelClicked();

private:
    void setupUi();
    void createCommandSelectionView();
    void createParameterEditorView(uint16_t command);
    QPushButton* createCommandButton(const QString& name, const QString& description,
                                     uint16_t command);

    Waypoint m_waypoint;

    // UI Elements
    QStackedWidget* m_stackedWidget;
    QWidget* m_selectionView;
    QWidget* m_editorView;
    QDialogButtonBox* m_buttonBox;
    QPushButton* m_saveButton;
    QPushButton* m_cancelButton;

    // Parameter widgets (for current command)
    QDoubleSpinBox* m_param1Spin;
    QDoubleSpinBox* m_param2Spin;
    QDoubleSpinBox* m_param3Spin;
    QDoubleSpinBox* m_param4Spin;
    QLabel* m_param1Label;
    QLabel* m_param2Label;
    QLabel* m_param3Label;
    QLabel* m_param4Label;
    QWidget* m_param1Widget;
    QWidget* m_param2Widget;
    QWidget* m_param3Widget;
    QWidget* m_param4Widget;
};

#endif  // COMMANDEDITORDIALOG_H
