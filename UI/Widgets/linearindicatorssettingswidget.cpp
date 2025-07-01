#include "linearindicatorssettingswidget.h"

LinearIndicatorsSettingsWidget::LinearIndicatorsSettingsWidget(QWidget* parent)
    : QWidget(parent) {
    ui.setupUi(this);
    ui.maxValueDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui.minValueDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui.maxValueDoubleSpinBox->setMinimum(std::numeric_limits<double>::lowest());
    ui.minValueDoubleSpinBox->setMinimum(std::numeric_limits<double>::lowest());
}

IndicatorsPreviewWidget* LinearIndicatorsSettingsWidget::indicatorPreviewWidget() const {
    return ui.previewWidget;
}

float LinearIndicatorsSettingsWidget::min() const {
    return ui.minValueDoubleSpinBox->value();
}
float LinearIndicatorsSettingsWidget::max() const {
    return ui.maxValueDoubleSpinBox->value();
}
