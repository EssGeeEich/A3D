#ifndef CHARTINDICATORTYPEWIDGET_H
#define CHARTINDICATORTYPEWIDGET_H

#include <QWidget>
#include "../../../A3D/chart.h"

namespace Ui {
class ChartIndicatorTypeWidget;
}

class ChartIndicatorTypeWidget : public QWidget {
	Q_OBJECT

public:
    explicit ChartIndicatorTypeWidget(QWidget* parent = nullptr);
	~ChartIndicatorTypeWidget();

    A3D::ChartAxisIndicatorType type() const;

    void setType(A3D::ChartAxisIndicatorType);

private:
    Ui::ChartIndicatorTypeWidget* ui;
};

#endif // CHARTINDICATORTYPEWIDGET_H
