#ifndef CHARTAXISSETTINGS_H
#define CHARTAXISSETTINGS_H

#include "ui_chartaxissettings.h"
#include "../../A3D/chart.h"

struct IndicatorInfo {
    A3D::ChartAxisIndicatorStyle m_style;
    A3D::ChartAxisIndicatorType m_type;
    bool operator==(IndicatorInfo const& i) const { return m_style == i.m_style && m_type == i.m_type; }
    bool operator!=(IndicatorInfo const& i) const { return !(*this == i); }
    IndicatorInfo(A3D::ChartAxisIndicator const& indicator) {
        m_style = indicator.m_style;
        m_type  = indicator.m_type;
    }
};

class ChartAxisSettings : public QWidget {
    Q_OBJECT

public:
    explicit ChartAxisSettings(QWidget* parent = nullptr);

    A3D::ChartAxisIndicatorStyle style() const;

    void setAxisData(A3D::ChartAxisData);

    A3D::ChartAxisData axisData();

private slots:
    void onLinearInterpolatedRadioButtonClicked();
    void onEnumeratedRadioButtonClicked();

private:
    void setChartAxisType(A3D::ChartAxisType);
    // This is only to decide the window
    A3D::ChartAxisType m_type;

    Ui::ChartAxisSettings ui;
};

#endif // CHARTAXISSETTINGS_H
