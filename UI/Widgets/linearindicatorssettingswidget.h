#ifndef LINEARINDICATORSSETTINGSWIDGET_H
#define LINEARINDICATORSSETTINGSWIDGET_H

#include "ui_linearindicatorssettingswidget.h"

class LinearIndicatorsSettingsWidget : public QWidget {
	Q_OBJECT

public:
    explicit LinearIndicatorsSettingsWidget(QWidget* parent = nullptr);

    IndicatorsPreviewWidget* indicatorPreviewWidget() const;

    inline void setBounds(float min, float max) {
        ui.minValueDoubleSpinBox->setValue(min);
        ui.maxValueDoubleSpinBox->setValue(max);
    }

    float min() const;
    float max() const;

private:
	Ui::LinearIndicatorsSettingsWidget ui;
};

#endif // LINEARINDICATORSSETTINGSWIDGET_H
