#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include "ui_calibrationwidget.h"
#include "../Dialogs/settingsdialog.h"
#include "../../A3D/chart.h"

class CalibrationWidget : public QWidget {
    Q_OBJECT

public:
    explicit CalibrationWidget(QWidget* parent = nullptr);

    ChartWidget* chartWidget() const;
    void setKeyBindings(std::map<Qt::Key, A3D::KeyboardCameraController::Action>);


	void setName(QString);
	QString name() const;

private slots:
    void onSettingsButtonClicked();
    void onGeneralInfoChanged(GeneralInfo const&);
    void onDialogFinished(int);

private:
    Ui::CalibrationWidget ui;
    SettingsDialog* m_settingsDialog;
};

#endif // CALIBRATIONWIDGET_H
