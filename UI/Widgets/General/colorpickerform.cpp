#include "colorpickerform.h"
#include "ui_colorpickerform.h"
#include <QColorDialog>

ColorPickerForm::ColorPickerForm(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::ColorPickerForm) {
	ui->setupUi(this);
    ui->colorWidget->setAutoFillBackground(true);

    connect(ui->changeColorButton, &QPushButton::clicked, this, &ColorPickerForm::onChangeColorButtonClicked);
    setColor(Qt::white);
}

ColorPickerForm::~ColorPickerForm() {
	delete ui;
}

QColor ColorPickerForm::color() const {
    return m_color;
}

void ColorPickerForm::setText(QString text) {
    ui->colorLabel->setText(text);
}

void ColorPickerForm::setColor(QColor color) {
    m_color          = color;
    QPalette palette = ui->colorWidget->palette();
    palette.setColor(ui->colorWidget->backgroundRole(), m_color);
    ui->colorWidget->setPalette(palette);
}

void ColorPickerForm::onChangeColorButtonClicked() {
    QColor color = QColorDialog::getColor();

    if(!color.isValid())
        return;

    setColor(color);
}
