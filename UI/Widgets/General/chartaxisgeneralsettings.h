#ifndef CHARTAXISGENERALSETTINGS_H
#define CHARTAXISGENERALSETTINGS_H

#include "ui_chartaxisgeneralsettings.h"
#include "../../../A3D/chart.h"

enum FontResolutions { FR_Low = 0, FR_Medium, FR_High, FR_Count };

class ChartAxisGeneralSettings : public QWidget {
    Q_OBJECT

public:
    explicit ChartAxisGeneralSettings(QWidget* parent = nullptr);

    A3D::ChartAxisIndicatorStyle style() const;

    void setStyle(A3D::ChartAxisIndicatorStyle const&);

    // Interpret the font resolution from the real size
    static FontResolutions getFontResoulutionFromDisplaySize(size_t displaySize);
    // Interpret the font resolution from the real size
	static FontResolutions getFontResoulution(size_t fontPointSize);
    // Returns the fake size in order to display a normal label
	static size_t getDisplaySize(FontResolutions);
    // Return the real point size fot the A3D use
    static size_t getSizeFromResolution(FontResolutions);

private slots:

    void onChangeLabelColorButtonClicked();

    void onChangeFontButtonClicked();

    void onHighResolutionRadioButtonClicked();

    void onMediumResolutionRadioButtonClicked();

    void onLowResolutionRadioButtonClicked();

    void onResetButtonClicked();

    void onScaleSliderValueChanged(int);

private:
    // setters
    void setExampleColor(QColor const&);
    // Always set resoultion before setting font
    void setExampleFont(QFont);
    void setResoulution(FontResolutions const& res);
    // getters
    QColor getExampleColor() const;
    // it return the final font with the fake point size
    QFont getExampleFont() const;
    // it return the final font with the real point size
    QFont getActualFont() const;

    FontResolutions m_resolution;

    Ui::ChartAxisGeneralSettings ui;
};

#endif // CHARTAXISSETTINGS_H
