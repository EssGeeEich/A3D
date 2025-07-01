#include "calibrationwidget.h"

CalibrationWidget::CalibrationWidget(QWidget* parent)
    : QWidget(parent) {
    ui.setupUi(this);
    m_settingsDialog = new SettingsDialog(this);

    connect(m_settingsDialog, &SettingsDialog::mapUpdated, ui.chartWidget, &ChartWidget::setMap);
    connect(m_settingsDialog, &SettingsDialog::generalInfoChanged, this, &CalibrationWidget::onGeneralInfoChanged);
    connect(m_settingsDialog, &SettingsDialog::finished, this, &CalibrationWidget::onDialogFinished);
    connect(ui.settingsButton, &QPushButton::clicked, this, &CalibrationWidget::onSettingsButtonClicked);
}

ChartWidget* CalibrationWidget::chartWidget() const {
    return ui.chartWidget;
}

void CalibrationWidget::setKeyBindings(std::map<Qt::Key, A3D::KeyboardCameraController::Action> sequences) {
    ui.chartWidget->setKeyBindings(sequences);
}

void CalibrationWidget::setName(QString name)
{
	ui.nameLabel->setText(name);
	ui.chartWidget->setName(name);
}

QString CalibrationWidget::name() const
{
    return ui.chartWidget->name();
}

void CalibrationWidget::onSettingsButtonClicked() {
    ui.chartWidget->stop();
    m_settingsDialog->setMap(ui.chartWidget->map());

    GeneralInfo info;
    info.m_worldColor  = ui.chartWidget->worldColor();
    info.m_markerColor = ui.chartWidget->markerColor();
    info.m_variants    = ui.chartWidget->renderVariants();
    m_settingsDialog->setGeneralInfo(info);
    m_settingsDialog->open();
}

void CalibrationWidget::onDialogFinished(int) {
    ui.chartWidget->restart();
}

void CalibrationWidget::onGeneralInfoChanged(GeneralInfo const& info) {
    ui.chartWidget->setMarkerColor(info.m_markerColor);
    ui.chartWidget->setRenderVariants(info.m_variants);
    ui.chartWidget->setWorldColor(info.m_worldColor);
}
