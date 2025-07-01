#include "stringprecisionwidget.h"
#include "ui_stringprecisionwidget.h"

StringPrecisionWidget::StringPrecisionWidget(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::StringPrecisionWidget) {
	ui->setupUi(this);

    connect(ui->stringPrecisionSpinBox, &QSpinBox::valueChanged, this, &StringPrecisionWidget::onValueChanged);
}

size_t StringPrecisionWidget::getPrecision() {
    return ui->stringPrecisionSpinBox->value();
}

void StringPrecisionWidget::setPrecision(size_t val) {
    ui->stringPrecisionSpinBox->setValue(val);
}

void StringPrecisionWidget::onValueChanged(int val) {
    emit stringPrecisionChanged(val);
}

StringPrecisionWidget::~StringPrecisionWidget() {
	delete ui;
}
