#ifndef EDITSHORTCUTSDIALOG_H
#define EDITSHORTCUTSDIALOG_H

#include <QDialog>
#include "../../A3D/keyboardcameracontroller.h"
#include <QKeySequenceEdit>

namespace Ui {
class EditShortcutsDialog;
}

class EditShortcutsDialog : public QDialog {
	Q_OBJECT

public:
    explicit EditShortcutsDialog(QWidget* parent = nullptr);
	~EditShortcutsDialog();
    void setSequence(std::map<A3D::KeyboardCameraController::Action, Qt::Key>);
signals:
    void shortcutsChanged(std::map<Qt::Key, A3D::KeyboardCameraController::Action>);
private slots:
    void exportShortcuts();

private:
    Ui::EditShortcutsDialog* ui;
    std::map<A3D::KeyboardCameraController::Action, Qt::Key> m_sequence;
    std::map<A3D::KeyboardCameraController::Action, QKeySequenceEdit*> m_editors;
};

#endif // EDITSHORTCUTSDIALOG_H
