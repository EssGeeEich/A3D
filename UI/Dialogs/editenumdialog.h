#ifndef EDITENUMDIALOG_H
#define EDITENUMDIALOG_H

#include <QDialog>
#include "../../A3D/chart.h"

namespace Ui {
class EditEnumDialog;
}

class EditEnumDialog : public QDialog {
	Q_OBJECT

public:
    struct IndicatorIterator {
        size_t index;
        A3D::ChartAxisIndicator indicator;
    };

    explicit EditEnumDialog(QWidget* parent = nullptr);
    void editIndicator(IndicatorIterator const& indicator);
	~EditEnumDialog();

    IndicatorIterator indicator();

private:
    void setChartIndicatorsType(A3D::ChartAxisIndicatorType type);
    IndicatorIterator m_iterator;
    Ui::EditEnumDialog* ui;
};

#endif // EDITENUMDIALOG_H
