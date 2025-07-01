#include "chartindicatortypewidget.h"
#include "ui_chartindicatortypewidget.h"

ChartIndicatorTypeWidget::ChartIndicatorTypeWidget(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::ChartIndicatorTypeWidget) {
	ui->setupUi(this);
}

ChartIndicatorTypeWidget::~ChartIndicatorTypeWidget() {
	delete ui;
}

A3D::ChartAxisIndicatorType ChartIndicatorTypeWidget::type() const {
    return ui->majorRadioButton->isChecked() ? A3D::CHAXIND_MAJOR_INDICATOR : A3D::CHAXIND_MINOR_INDICATOR;
}

void ChartIndicatorTypeWidget::setType(A3D::ChartAxisIndicatorType type) {
    bool ret = type == A3D::CHAXIND_MAJOR_INDICATOR;
    ui->majorRadioButton->setChecked(ret);
    ui->minorRadioButton->setChecked(!ret);
}
