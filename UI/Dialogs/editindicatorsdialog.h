#ifndef EDITINDICATORSDIALOG_H
#define EDITINDICATORSDIALOG_H

#include <QDialog>
#include "../../A3D/chart.h"

namespace Ui {
class EditIndicatorsDialog;
}

class EditIndicatorsDialog : public QDialog {
	Q_OBJECT

public:
    explicit EditIndicatorsDialog(QWidget* parent = nullptr);
    ~EditIndicatorsDialog();

    void reset();

    std::vector<A3D::ChartAxisIndicator> indicators();
    void setStyle(A3D::ChartAxisIndicatorStyle style);
    void setChartIndicatorsType(A3D::ChartAxisIndicatorType type);

    void editIndicators(std::vector<A3D::ChartAxisIndicator> const& indicators);

private:
    void setStyleNeeded(bool);
    bool m_styleNeeded;
    std::vector<A3D::ChartAxisIndicator> m_indicators;

    Ui::EditIndicatorsDialog* ui;
};

#endif // ADDLIINDICATORSDIALOG_H
