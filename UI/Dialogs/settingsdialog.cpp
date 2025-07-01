#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent) {
    ui.setupUi(this);
    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onSettingsAccepted);
    ui.worldColorPicker->setText("World color:");
    ui.markerColorPicker->setText("Marker color:");
    ui.chartNameLineEdit->setMaxLength(32);
}

void SettingsDialog::setGeneralInfo(GeneralInfo info) {
    m_info = info;
    ui.histRadioButton->setChecked(false);
    ui.normalRadioButton->setChecked(false);
    if(m_info.m_variants == A3D::SurfaceChartEntity::RV_NONE)
        ui.normalRadioButton->setChecked(true);
    else if((m_info.m_variants & A3D::SurfaceChartEntity::RV_HISTOGRAM_ENUMERATIONS) == A3D::SurfaceChartEntity::RV_HISTOGRAM_ENUMERATIONS)
        ui.histRadioButton->setChecked(true);

    ui.chartNameLineEdit->setText(info.m_chartName);

    ui.worldColorPicker->setColor(m_info.m_worldColor);
    ui.markerColorPicker->setColor(m_info.m_markerColor);
}

void SettingsDialog::setMap(A3D::MapChart3D const& map) {
    ui.xAxisSettings->setAxisData(map.axisData(A3D::AXIS_X));
    ui.yAxisSettings->setAxisData(map.axisData(A3D::AXIS_Y));
    ui.zAxisSettings->setAxisData(map.axisData(A3D::AXIS_Z));
    m_map = map;
}

void SettingsDialog::onSettingsAccepted() {
    A3D::ChartAxisData xAxisData = ui.xAxisSettings->axisData();
    A3D::ChartAxisData yAxisData = ui.yAxisSettings->axisData();
    A3D::ChartAxisData zAxisData = ui.zAxisSettings->axisData();
    m_map.setAxisData(A3D::AXIS_X, xAxisData);
    m_map.setAxisData(A3D::AXIS_Y, yAxisData);
    m_map.setAxisData(A3D::AXIS_Z, zAxisData);

    A3D::SurfaceChartEntity::RenderVariants variants;
    if(ui.normalRadioButton->isChecked())
        variants |= A3D::SurfaceChartEntity::RV_NONE;
    if(ui.histRadioButton->isChecked())
        variants |= A3D::SurfaceChartEntity::RV_HISTOGRAM_ENUMERATIONS;
    if(m_info.m_worldColor != ui.worldColorPicker->color()
       || m_info.m_markerColor != ui.markerColorPicker->color()
       || variants != m_info.m_variants
       || m_info.m_chartName != ui.chartNameLineEdit->text()) {
        m_info.m_markerColor = ui.markerColorPicker->color();
        m_info.m_worldColor  = ui.worldColorPicker->color();
        m_info.m_variants    = variants;
        m_info.m_chartName = ui.chartNameLineEdit->text();
        emit generalInfoChanged(m_info);
    }

    emit mapUpdated(m_map);
}
