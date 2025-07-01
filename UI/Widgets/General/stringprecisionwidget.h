#ifndef STRINGPRECISIONWIDGET_H
#define STRINGPRECISIONWIDGET_H

#include <QWidget>

namespace Ui {
class StringPrecisionWidget;
}

class StringPrecisionWidget : public QWidget {
	Q_OBJECT

public:
    explicit StringPrecisionWidget(QWidget* parent = nullptr);
	~StringPrecisionWidget();

    size_t getPrecision();

    void setPrecision(size_t);

signals:
    void stringPrecisionChanged(size_t);
private slots:
    void onValueChanged(int);

private:
    Ui::StringPrecisionWidget* ui;
};

#endif // STRINGPRECISIONWIDGET_H
