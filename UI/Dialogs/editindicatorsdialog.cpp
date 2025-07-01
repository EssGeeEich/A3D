#include "editindicatorsdialog.h"
#include "ui_editindicatorsdialog.h"

EditIndicatorsDialog::EditIndicatorsDialog(QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::EditIndicatorsDialog) {
    ui->setupUi(this);

    connect(ui->stringPrecisionWidget, &StringPrecisionWidget::stringPrecisionChanged, ui->rawEditWidget, &ValuesPreviewWidget::setStringPrecision);
    reset();
}

static size_t getPrecisionFromString(const QString text) {
    QStringList lst = text.split(".");

    if(lst.count() != 2)
        return 0;

    return lst.at(1).size();
}

void EditIndicatorsDialog::setStyleNeeded(bool styleNeeded) {
    m_styleNeeded = styleNeeded;
    ui->generalSettings->setHidden(!styleNeeded);
    ui->indicatorTypeWidget->setHidden(!styleNeeded);
}

void EditIndicatorsDialog::editIndicators(std::vector<A3D::ChartAxisIndicator> const& indicators) {
    size_t maxStringPrecision = 0;
    m_indicators              = indicators;
    for(size_t i = 0; i < indicators.size(); ++i) {
        ui->rawEditWidget->addValue(indicators.at(i).m_value);
        size_t prec = getPrecisionFromString(indicators.at(i).m_label);
        if(prec > maxStringPrecision)
            maxStringPrecision = prec;
    }
    ui->stringPrecisionWidget->setPrecision(maxStringPrecision);
    open();
}

void EditIndicatorsDialog::setChartIndicatorsType(A3D::ChartAxisIndicatorType type) {
    ui->indicatorTypeWidget->setType(type);
}

void EditIndicatorsDialog::setStyle(A3D::ChartAxisIndicatorStyle style) {
    ui->generalSettings->setStyle(style);
    setStyleNeeded(true);
}

void EditIndicatorsDialog::reset() {
    setStyle(A3D::ChartAxisIndicatorStyle());
    setChartIndicatorsType(A3D::CHAXIND_MAJOR_INDICATOR);
    setStyleNeeded(false);
    m_indicators.clear();
    ui->rawEditWidget->clear();
}

// TODO:  Normalize value when they arrive
std::vector<A3D::ChartAxisIndicator> EditIndicatorsDialog::indicators() {
    std::vector<float> values    = ui->rawEditWidget->getValues();
    size_t const stringPrecision = ui->stringPrecisionWidget->getPrecision();
    for(size_t i = 0; i < values.size(); i++) {
        m_indicators[i].m_value = values[i];
        m_indicators[i].m_label = QString::number(values.at(i), 'f', stringPrecision);
    }
    if(!m_styleNeeded)
        return m_indicators;

    A3D::ChartAxisIndicatorType type   = ui->indicatorTypeWidget->type();
    A3D::ChartAxisIndicatorStyle style = ui->generalSettings->style();

    for(A3D::ChartAxisIndicator& it: m_indicators) {
        it.m_type  = type;
        it.m_style = style;
    }

    return m_indicators;
}

EditIndicatorsDialog::~EditIndicatorsDialog() {
	delete ui;
}
