#ifndef INCOMPATIBILITYDIALOG_H
#define INCOMPATIBILITYDIALOG_H

#include <QDialog>

#include "../../A3D/chart.h"
#include "../Widgets/chartaxissettings.h"

namespace Ui {
class IncompatibilityDialog;
}

class IncompatibilityDialog : public QDialog {
	Q_OBJECT
public:
    enum UserChoice { UC_DISCARD, UC_CONTINUE, UC_CHOOSE, UC_UNDEFINED };
    explicit IncompatibilityDialog(QWidget* parent, std::vector<IndicatorInfo>&);
	~IncompatibilityDialog();

    inline UserChoice getUserChoice() const { return m_choice; }
    inline IndicatorInfo getInfo() const { return m_info; }
private slots:
    void continueButtonClicked();
    void discardButtonClicked();

private:
    Ui::IncompatibilityDialog* ui;
    UserChoice m_choice;
    IndicatorInfo m_info;
};

#endif // INCOMPATIBILITYDIALOG_H
