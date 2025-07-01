#include "editshortcutsdialog.h"
#include "ui_editshortcutsdialog.h"
#include <QKeySequenceEdit>
#include <QMessageBox>

static std::map<A3D::KeyboardCameraController::Action, QString> actionString;

EditShortcutsDialog::EditShortcutsDialog(QWidget* parent)
    : QDialog(parent),
      ui(new Ui::EditShortcutsDialog) {
	ui->setupUi(this);

    actionString[A3D::KeyboardCameraController::ACT_MOVE_FORWARD]                = "Move the camera forward";
    actionString[A3D::KeyboardCameraController::ACT_MOVE_BACKWARD]               = "Move the camera backward";
    actionString[A3D::KeyboardCameraController::ACT_MOVE_LEFT]                   = "Strafe the camera left";
    actionString[A3D::KeyboardCameraController::ACT_MOVE_RIGHT]                  = "Strafe the camera right";
    actionString[A3D::KeyboardCameraController::ACT_MOVE_UPWARD]                 = "Move the camera upward";
    actionString[A3D::KeyboardCameraController::ACT_MOVE_DOWNWARD]               = "Move the camera downward";
    actionString[A3D::KeyboardCameraController::ACT_MOVE_PRECISE]                = "Apply precise (slower) movement modifier";
    actionString[A3D::KeyboardCameraController::ACT_MOVE_QUICK]                  = "Apply quick (faster) movement modifier";
    actionString[A3D::KeyboardCameraController::ACT_LOOK_LEFT]                   = "Rotate the camera to look left";
    actionString[A3D::KeyboardCameraController::ACT_LOOK_RIGHT]                  = "Rotate the camera to look right";
    actionString[A3D::KeyboardCameraController::ACT_LOOK_UP]                     = "Rotate the camera to look up";
    actionString[A3D::KeyboardCameraController::ACT_LOOK_DOWN]                   = "Rotate the camera to look down";
    actionString[A3D::KeyboardCameraController::ACT_LOOK_TILTLEFT]               = "Tilt the camera left";
    actionString[A3D::KeyboardCameraController::ACT_LOOK_TILTRIGHT]              = "Tilt the camera right";
    actionString[A3D::KeyboardCameraController::ACT_ROTATE_LEFT_AROUND_HOME]     = "Rotate left around home position";
    actionString[A3D::KeyboardCameraController::ACT_ROTATE_RIGHT_AROUND_HOME]    = "Rotate right around home position";
    actionString[A3D::KeyboardCameraController::ACT_ROTATE_UPWARD_AROUND_HOME]   = "Rotate upward around home position";
    actionString[A3D::KeyboardCameraController::ACT_ROTATE_DOWNWARD_AROUND_HOME] = "Rotate downward around home position";
    actionString[A3D::KeyboardCameraController::ACT_LOOK_HOME]                   = "Reset camera to home position";

    for(const auto& [key, value]: actionString) {
        QKeySequenceEdit* kSE = new QKeySequenceEdit(ui->widget);
        QFormLayout* layout   = qobject_cast<QFormLayout*>(ui->widget->layout());
        kSE->setMaximumSequenceLength(1);
        m_editors[key] = kSE;
        layout->addRow(value, kSE);
        connect(kSE, &QKeySequenceEdit::keySequenceChanged, this, [=]() {
            QPalette pal          = kSE->palette();
            QPalette titlePalette = ui->label->palette();
            pal.setColor(QPalette::Text, titlePalette.color(QPalette::Text));
            kSE->setPalette(pal);
        });
    }

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &EditShortcutsDialog::exportShortcuts);
}

EditShortcutsDialog::~EditShortcutsDialog() {
	delete ui;
}

void EditShortcutsDialog::exportShortcuts() {

    std::map<Qt::Key, A3D::KeyboardCameraController::Action> seq;
    bool changed = false;
    for(const auto& [action, editor]: m_editors) {
        QKeySequence temp          = editor->keySequence();
        bool const actionIsPresent = m_sequence.find(action) != m_sequence.end();
        if(!temp.count()) {
            if(actionIsPresent)
                changed = true;
            m_sequence.erase(action);
            continue;
        }
        Qt::Key const newKey = temp[0].key();
        if(!actionIsPresent)
            changed = true;
        else {
            Qt::Key actualKey = m_sequence[action];
            if(actualKey != newKey)
                changed = true;
        }

        m_sequence[action] = newKey;

        if(seq.find(newKey) != seq.end()) {
            QMessageBox::critical(this, "Error, duplicate", "Duplicate key in:\n" + actionString[action]);
            QPalette pal = editor->palette();
            pal.setColor(QPalette::Text, Qt::red); // Sfondo per campi input
            editor->setPalette(pal);
            return;
        }
        seq[newKey] = action;
    }

    if(changed)
        emit shortcutsChanged(seq);
}

void EditShortcutsDialog::setSequence(std::map<A3D::KeyboardCameraController::Action, Qt::Key> sequence) {
    for(const auto& [action, key]: sequence) {
        m_sequence[action] = key;
        QKeySequence temp(key);
        m_editors[action]->setKeySequence(temp);
    }
}
