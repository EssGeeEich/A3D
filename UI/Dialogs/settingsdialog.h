#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ui_settingsdialog.h"
#include "../../A3D/chart.h"
#include "../../A3D/surfacechartentity.h"

struct GeneralInfo {
    QString m_chartName;
    QColor m_worldColor;
    QColor m_markerColor;
    A3D::SurfaceChartEntity::RenderVariants m_variants;
};

class SettingsDialog : public QDialog {
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget* parent = nullptr);

    void setMap(A3D::MapChart3D const&);
    void setGeneralInfo(GeneralInfo);

    // Here the shortcuts and other settings
    //void setControlInfo();
    void onSettingsAccepted();
signals:
    void mapUpdated(A3D::MapChart3D const&);
    void generalInfoChanged(GeneralInfo const&);

private:
    Ui::SettingsDialog ui;
    GeneralInfo m_info;

    A3D::MapChart3D m_map;
};

#endif // SETTINGSDIALOG_H
