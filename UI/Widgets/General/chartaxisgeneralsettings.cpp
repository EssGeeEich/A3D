#include "chartaxisgeneralsettings.h"
#include <QColorDialog>
#include <QFontDialog>

enum FS {
	FS_Low    = 16,
	FS_Medium = 32,
	FS_High   = 64,
};

uint8_t const FontSizes[FR_Count] = { FS_Low, FS_Medium, FS_High };

ChartAxisGeneralSettings::ChartAxisGeneralSettings(QWidget* parent)
	: QWidget(parent) {
	ui.setupUi(this);

    ui.indicatorColorPicker->setText("Indicator color:");
    ui.indicatorColorPicker->setColor(Qt::white);

    ui.scaleValueLabel->setMinimumWidth(ui.scaleValueLabel->fontMetrics().boundingRect("1,000").width());
    ui.scaleValueLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    setExampleColor(Qt::white);
    setResoulution(FR_High);
    setExampleFont(QFont());

    connect(ui.changeLabelColorButton, &QPushButton::clicked, this, &ChartAxisGeneralSettings::onChangeLabelColorButtonClicked);
    connect(ui.changeFontButton, &QPushButton::clicked, this, &ChartAxisGeneralSettings::onChangeFontButtonClicked);
    connect(ui.highResolutionRadioButton, &QRadioButton::clicked, this, &ChartAxisGeneralSettings::onHighResolutionRadioButtonClicked);
    connect(ui.mediumResolutionRadioButton, &QRadioButton::clicked, this, &ChartAxisGeneralSettings::onMediumResolutionRadioButtonClicked);
    connect(ui.lowResolutionRadioButton, &QRadioButton::clicked, this, &ChartAxisGeneralSettings::onLowResolutionRadioButtonClicked);
    connect(ui.resetButton, &QPushButton::clicked, this, &ChartAxisGeneralSettings::onResetButtonClicked);
    connect(ui.scaleSlider, &QSlider::valueChanged, this, &ChartAxisGeneralSettings::onScaleSliderValueChanged);
}

A3D::ChartAxisIndicatorStyle ChartAxisGeneralSettings::style() const {

    A3D::ChartAxisIndicatorStyle style;

    style.m_indicatorColor = ui.indicatorColorPicker->color();
    style.m_labelColor     = getExampleColor();
    style.m_labelFont      = getActualFont();
    style.m_labelSize      = ui.scaleSlider->value() / 100.f;

    return style;
}

void ChartAxisGeneralSettings::setStyle(A3D::ChartAxisIndicatorStyle const& style) {

    ui.indicatorColorPicker->setColor(style.m_indicatorColor);
    setExampleColor(style.m_labelColor);

    size_t fontSize = style.m_labelFont.pointSize();

    setResoulution(ChartAxisGeneralSettings::getFontResoulution(fontSize));
    setExampleFont(style.m_labelFont);

    ui.scaleSlider->setValue(style.m_labelSize * 100.f);
}

FontResolutions ChartAxisGeneralSettings::getFontResoulution(size_t fontPointSize) {

	FontResolutions resolution = FR_High;
    for(size_t i = 0; i < FR_Count; i++) {
		if(fontPointSize == FontSizes[i])
			resolution = static_cast<FontResolutions>(i);
	}
	return resolution;
}

// Interpret the font resolution from the real size
FontResolutions ChartAxisGeneralSettings::getFontResoulutionFromDisplaySize(size_t displaySize) {
    FontResolutions resolution = FR_High;
    for(size_t i = 0; i < FR_Count; i++) {
        if(displaySize == FontSizes[i] / 4)
            resolution = static_cast<FontResolutions>(i);
    }
    return resolution;
}

size_t ChartAxisGeneralSettings::getDisplaySize(FontResolutions res) {
	if(res >= FR_Count || res < FR_Low)
		return FontSizes[FR_High];
	return FontSizes[res] / 4;
}

size_t ChartAxisGeneralSettings::getSizeFromResolution(FontResolutions res) {
    if(res >= FR_Count || res < FR_Low)
        return FontSizes[FR_High];
    return FontSizes[res];
}

void ChartAxisGeneralSettings::onChangeLabelColorButtonClicked() {
	QColor color = QColorDialog::getColor();

	if(!color.isValid())
		return;
    setExampleColor(color);
}

void ChartAxisGeneralSettings::onChangeFontButtonClicked() {

    bool result = false;
    QFont font  = QFontDialog::getFont(&result, getExampleFont());

	if(!result)
		return;

    setExampleFont(font);
}

void ChartAxisGeneralSettings::setExampleColor(QColor const& color) {
    QPalette palette = ui.exampleOfColorLabel->palette();
    palette.setColor(QPalette::WindowText, color);
    ui.exampleOfColorLabel->setPalette(palette);
}

void ChartAxisGeneralSettings::setExampleFont(QFont font) {
    font.setPointSize(getDisplaySize(m_resolution));
    ui.exampleOfFontLabel->setText(font.family());
    ui.exampleOfFontLabel->setFont(font);
}

void ChartAxisGeneralSettings::setResoulution(FontResolutions const& res) {
	m_resolution = res;
    // Reload display font
    setExampleFont(getExampleFont());
}

QColor ChartAxisGeneralSettings::getExampleColor() const {
    return ui.exampleOfColorLabel->palette().color(QPalette::WindowText);
}

QFont ChartAxisGeneralSettings::getExampleFont() const {
    return ui.exampleOfFontLabel->font();
}

QFont ChartAxisGeneralSettings::getActualFont() const {
    QFont font = getExampleFont();
    font.setPointSize(getSizeFromResolution(m_resolution));
    return font;
}

void ChartAxisGeneralSettings::onHighResolutionRadioButtonClicked() {
	setResoulution(FR_High);
}

void ChartAxisGeneralSettings::onMediumResolutionRadioButtonClicked() {
	setResoulution(FR_Medium);
}

void ChartAxisGeneralSettings::onLowResolutionRadioButtonClicked() {
	setResoulution(FR_Low);
}

void ChartAxisGeneralSettings::onResetButtonClicked() {
    ui.scaleSlider->setValue(100);
}

void ChartAxisGeneralSettings::onScaleSliderValueChanged(int value) {
    ui.scaleValueLabel->setText(QString::number(value / 100.f, 'g', 2));
}
