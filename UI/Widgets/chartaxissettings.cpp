#include "chartaxissettings.h"
#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>
#include "incompatibilitydialog.h"

ChartAxisSettings::ChartAxisSettings(QWidget* parent)
    : QWidget(parent),
      m_type(A3D::CHAXIS_LINEAR_INTERPOLATED) {
    ui.setupUi(this);

    connect(ui.enumeratedRadioButton, &QRadioButton::clicked, this, &ChartAxisSettings::onEnumeratedRadioButtonClicked);
    connect(ui.linearInterpolatedRadioButton, &QRadioButton::clicked, this, &ChartAxisSettings::onLinearInterpolatedRadioButtonClicked);
    setChartAxisType(m_type);
    ui.nameLineEdit->setMaxLength(32);
}

void ChartAxisSettings::setChartAxisType(A3D::ChartAxisType type) {
    switch(m_type = type) {
    case A3D::CHAXIS_ENUMERATED:
        ui.EnumeratedPreviewWidget->show();
        ui.LinearPreviewWidget->hide();
        break;
    default:
    case A3D::CHAXIS_LINEAR_INTERPOLATED:
        ui.EnumeratedPreviewWidget->hide();
        ui.LinearPreviewWidget->show();
        break;
    }
}

void ChartAxisSettings::setAxisData(A3D::ChartAxisData data) {
    ui.invertedCheckbox->setChecked(data.isInverted());

    if(data.isInverted())
        data.invert();

    setChartAxisType(data.type());
    ui.nameLineEdit->setText(data.name());
    A3D::ChartAxisType type = data.type();
    setChartAxisType(type);
    ui.chartAxisGeneralSettings->setStyle(data.defaultIndicatorStyle());

    switch(type) {
    case A3D::CHAXIS_ENUMERATED:
        ui.EnumeratedPreviewWidget->addIndicators(data.indicators());
        break;
    default:
    case A3D::CHAXIS_LINEAR_INTERPOLATED:
        ui.LinearPreviewWidget->setBounds(data.minimum(), data.maximum());
        ui.LinearPreviewWidget->indicatorPreviewWidget()->clear();
        ui.LinearPreviewWidget->indicatorPreviewWidget()->addIndicators(data.indicators());

        break;
    }
}

A3D::ChartAxisData ChartAxisSettings::axisData() {
    A3D::ChartAxisData data;

    data.setName(ui.nameLineEdit->text());
    data.setDefaultIndicatorStyle(ui.chartAxisGeneralSettings->style());

    switch(m_type) {
    case A3D::CHAXIS_ENUMERATED:
        {
            // Lo so che si può ottimizzare e così fa schifo, ma per ora mi interessa verderlo fungere!!!
            std::vector<StyledEnumeratedIndicatorsPreviewWidget::StyledIndicator> indicators = ui.EnumeratedPreviewWidget->enumeratedIndicators();
            std::vector<A3D::ChartAxisStyledEnumeratedIndicatorData> majorIndicators;
            std::vector<A3D::ChartAxisStyledEnumeratedIndicatorData> minorIndicators;
            for(size_t i = 0; i < indicators.size(); i++) {
                if(indicators[i].m_type == A3D::CHAXIND_MAJOR_INDICATOR)
                    majorIndicators.push_back(indicators[i].m_data);
                else
                    minorIndicators.push_back(indicators[i].m_data);
            }

            data.setIndicators(majorIndicators, A3D::CHAXIND_MAJOR_INDICATOR);
            data.setIndicators(minorIndicators, A3D::CHAXIND_MINOR_INDICATOR);
        }
        break;
    default:
    case A3D::CHAXIS_LINEAR_INTERPOLATED:

        data.setIndicators(ui.LinearPreviewWidget->indicatorPreviewWidget()->indicators());
        // I set the bounds so it auto normalize them
        data.setMinMax(ui.LinearPreviewWidget->min(), ui.LinearPreviewWidget->max());
        break;
    }

    if(ui.invertedCheckbox->isChecked())
        data.invert();

    return data;
}

A3D::ChartAxisIndicatorStyle ChartAxisSettings::style() const {
    return ui.chartAxisGeneralSettings->style();
}

void ChartAxisSettings::onLinearInterpolatedRadioButtonClicked() {
    setChartAxisType(A3D::CHAXIS_LINEAR_INTERPOLATED);
}

void ChartAxisSettings::onEnumeratedRadioButtonClicked() {
    setChartAxisType(A3D::CHAXIS_ENUMERATED);
}
