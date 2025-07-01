#ifndef ADDINDICATORSDIALOG_H
#define ADDINDICATORSDIALOG_H

#include <QDialog>

#include "../../A3D/chart.h"
namespace Ui {
class AddIndicatorsDialog;
}

enum AddMode {
	AM_BY_STEP = 0,
	AM_RAW_ADD,
};

class AddIndicatorsDialog : public QDialog {
	Q_OBJECT

public:
    explicit AddIndicatorsDialog(QWidget* parent = nullptr);
    ~AddIndicatorsDialog();

	std::vector<A3D::ChartAxisIndicator> indicators() const;
    void setStyle(A3D::ChartAxisIndicatorStyle style);

private:
	void setMode(AddMode);

    std::vector<A3D::ChartAxisIndicator> getIndicatorFromCount(size_t) const;

    A3D::ChartAxisIndicator getIndicatorByValue(float value) const;

    void onModeChanged(bool);

    void onCountModeChanged(bool);

    void onLabelDigitsChanged();

	AddMode m_mode;
	bool m_countMode;

    Ui::AddIndicatorsDialog* ui;
};

#endif // ADDINDICATORSDIALOG_H
