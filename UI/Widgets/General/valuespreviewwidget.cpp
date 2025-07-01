#include "valuespreviewwidget.h"
#include <QDoubleSpinBox>
#include <QListWidget>

ValuesPreviewWidget::ValuesPreviewWidget(QWidget* parent)
    : GeneralPreview(parent) {
    GeneralPreview::onItemSelectionChanged();
    previewWidget()->setFlow(QListWidget::LeftToRight);
}

void ValuesPreviewWidget::setStringPrecision(size_t prec) {
    for(size_t i = 0; i < previewWidget()->count(); i++) {
        QListWidgetItem* item      = previewWidget()->item(i);
        QWidget* tempDoubleSpinBox = previewWidget()->itemWidget(item);
        qobject_cast<QDoubleSpinBox*>(tempDoubleSpinBox)->setDecimals(prec);
    }
}

std::vector<float> ValuesPreviewWidget::getValues() const {
    std::vector<float> values;

    for(size_t i = 0; i < previewWidget()->count(); i++) {
        QListWidgetItem* item      = previewWidget()->item(i);
        QWidget* tempDoubleSpinBox = previewWidget()->itemWidget(item);
        values.push_back(qobject_cast<QDoubleSpinBox*>(tempDoubleSpinBox)->value());
    }

    return values;
}

void ValuesPreviewWidget::addValues(std::vector<float> values) {
    for(int i = 0; i < values.size(); i++) {
		addValue(values.at(i));
	}
}

void ValuesPreviewWidget::clear() {
    previewWidget()->clear();
}

void ValuesPreviewWidget::addValue(float value) {

    QListWidgetItem* newWidget    = new QListWidgetItem(previewWidget());
    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(previewWidget());

    doubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    doubleSpinBox->setMinimum(std::numeric_limits<double>::lowest());
    doubleSpinBox->setValue(value);
    newWidget->setSizeHint(doubleSpinBox->sizeHint());

    previewWidget()->setItemWidget(newWidget, doubleSpinBox);
    previewWidget()->addItem(newWidget);
}

void ValuesPreviewWidget::onAddButtonClicked() {

    float defaultValue = 0.f;
    if(previewWidget()->count()) {
        QListWidgetItem* item      = previewWidget()->item(0);
        QWidget* tempDoubleSpinBox = previewWidget()->itemWidget(item);
        defaultValue               = qobject_cast<QDoubleSpinBox*>(tempDoubleSpinBox)->value() + 1;
	}
	addValue(defaultValue);
}

bool ValuesPreviewWidget::isAddEnabled() const {
    return true;
}

bool ValuesPreviewWidget::isEditEnabled() const {
    return false;
}

bool ValuesPreviewWidget::isMultiEditEnabled() const {
    return false;
}

bool ValuesPreviewWidget::isRemoveEnabled() const {
    return true;
}
