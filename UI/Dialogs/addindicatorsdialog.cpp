#include "addindicatorsdialog.h"
#include "ui_addindicatorsdialog.h"

AddIndicatorsDialog::AddIndicatorsDialog(QWidget* parent)
	: QDialog(parent),
      ui(new Ui::AddIndicatorsDialog),
      m_mode(AM_BY_STEP),
      m_countMode(true) {
	ui->setupUi(this);
	setMode(m_mode);
    onCountModeChanged(m_countMode);

    ui->stepSizeLabel->setMinimumWidth(ui->stepSizeLabel->fontMetrics().boundingRect("Indicators count").width());
    ui->stepSizeLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

	ui->stepSizeDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui->stepSizeDoubleSpinBox->setMinimum(std::numeric_limits<double>::min());

	ui->fromDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui->fromDoubleSpinBox->setMinimum(std::numeric_limits<double>::lowest());

	ui->toDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui->toDoubleSpinBox->setMinimum(std::numeric_limits<double>::lowest());

	ui->countSpinBox->setMaximum(A3D::MaxIndicators);

    connect(ui->byStepRadioButton, &QRadioButton::toggled, this, &AddIndicatorsDialog::onModeChanged);
    connect(ui->countRadioButton, &QRadioButton::toggled, this, &AddIndicatorsDialog::onCountModeChanged);
    connect(ui->labelDigitsWidget, &StringPrecisionWidget::stringPrecisionChanged, this, &AddIndicatorsDialog::onLabelDigitsChanged);
}

void AddIndicatorsDialog::onModeChanged(bool mode) {
    setMode(mode ? AM_BY_STEP : AM_RAW_ADD);
}

void AddIndicatorsDialog::onLabelDigitsChanged() {
    size_t digits = ui->labelDigitsWidget->getPrecision();
    ui->rawEditWidget->setStringPrecision(digits);
}

void AddIndicatorsDialog::onCountModeChanged(bool mode) {
	m_countMode = mode;
    ui->stepSizeLabel->setHidden(m_countMode);
    ui->stepSizeDoubleSpinBox->setHidden(m_countMode);
    ui->countSpinBox->setHidden(!m_countMode);
    ui->countLabel->setHidden(!m_countMode);
}

void AddIndicatorsDialog::setMode(AddMode mode) {
	m_mode = mode;

	bool isRawEditHidden = m_mode == AM_BY_STEP;
	ui->rawEditWidget->setHidden(isRawEditHidden);
	ui->byStepWidget->setHidden(!isRawEditHidden);
}

void AddIndicatorsDialog::setStyle(A3D::ChartAxisIndicatorStyle style) {
    ui->generalSettings->setStyle(style);
}

A3D::ChartAxisIndicator AddIndicatorsDialog::getIndicatorByValue(float value) const
{
	A3D::ChartAxisIndicatorType type   = ui->indicatorTypeWidget->type();
	size_t const stringPrecision       = ui->labelDigitsWidget->getPrecision();
	A3D::ChartAxisIndicatorStyle style = ui->generalSettings->style();
	return A3D::ChartAxisIndicator(type, value, 0.f, QString::number(value, 'f', stringPrecision), style);
}

std::vector<A3D::ChartAxisIndicator> AddIndicatorsDialog::getIndicatorFromCount(size_t count) const {

    std::vector<A3D::ChartAxisIndicator> indicators;
    float const from = ui->fromDoubleSpinBox->value();
    float const to   = ui->toDoubleSpinBox->value();

    float fInverseIndicatorCount = 1.f / static_cast<float>(count - 1);
    for(size_t i = 0; i < count; ++i) {
        float const val = from + ((to - from) * fInverseIndicatorCount * static_cast<float>(i));
        indicators.push_back(getIndicatorByValue(val));
    }

    return indicators;
}

// TODO:  Normalize value when they arrive
std::vector<A3D::ChartAxisIndicator> AddIndicatorsDialog::indicators() const {
	std::vector<A3D::ChartAxisIndicator> indicators;

    switch(m_mode) {
	default:
	case AM_BY_STEP:
		{
			if(m_countMode) {
				indicators = getIndicatorFromCount(ui->countSpinBox->value());
				return indicators;
			}

			// Copied from A3D/chart.h
            float const from = ui->fromDoubleSpinBox->value();
            float const to   = ui->toDoubleSpinBox->value();
            float const stepSize              = ui->stepSizeDoubleSpinBox->value();
            size_t const indicatorCount = static_cast<size_t>((to - from) / stepSize) + 1;

            if(indicatorCount > A3D::MaxIndicators) {
                indicators = getIndicatorFromCount(A3D::MaxIndicators);
                return indicators;
            }

            indicators.reserve(indicatorCount);

            for(size_t i = 0; i < indicatorCount; ++i) {
                float const val = from + stepSize * static_cast<float>(i);
                indicators.push_back(getIndicatorByValue(val));
            }

			indicators.push_back(getIndicatorByValue(to));
		}

    case AM_RAW_ADD:
        {
            std::vector<float> values = ui->rawEditWidget->getValues();

            for(size_t i = 0; i < values.size(); i++)
                indicators.push_back(getIndicatorByValue(values.at(i)));
        }

		break;
	}

    return indicators;
}

AddIndicatorsDialog::~AddIndicatorsDialog() {
	delete ui;
}
