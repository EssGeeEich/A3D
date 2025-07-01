#include "editenumdialog.h"
#include "ui_editenumdialog.h"

EditEnumDialog::EditEnumDialog(QWidget* parent)
    : QDialog(parent),
      ui(new Ui::EditEnumDialog) {
	ui->setupUi(this);
    ui->lineEdit->setMaxLength(32);
}

void EditEnumDialog::editIndicator(IndicatorIterator const& iterator) {
    m_iterator = iterator;
    ui->lineEdit->setText(iterator.indicator.m_label);
    ui->generalSettingsWidget->setStyle(iterator.indicator.m_style);
    setChartIndicatorsType(iterator.indicator.m_type);
}

EditEnumDialog::IndicatorIterator EditEnumDialog::indicator() {
    m_iterator.indicator.m_style = ui->generalSettingsWidget->style();
    m_iterator.indicator.m_label = ui->lineEdit->text();
    m_iterator.indicator.m_type  = ui->indicatorTypeWidget->type();
    return m_iterator;
}

void EditEnumDialog::setChartIndicatorsType(A3D::ChartAxisIndicatorType type) {
    ui->indicatorTypeWidget->setType(type);
}

EditEnumDialog::~EditEnumDialog() {
	delete ui;
}
