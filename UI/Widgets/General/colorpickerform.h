#ifndef COLORPICKERFORM_H
#define COLORPICKERFORM_H

#include <QWidget>

// Warning, on Linux, the QColorDialog doesn't work if QOpelGLWidget is running

namespace Ui {
class ColorPickerForm;
}

class ColorPickerForm : public QWidget {
	Q_OBJECT

public:
    explicit ColorPickerForm(QWidget* parent = nullptr);
	~ColorPickerForm();

    QColor color() const;

    void setColor(QColor color);

    void setText(QString);
private slots:
    void onChangeColorButtonClicked();

private:
    QColor m_color;
    Ui::ColorPickerForm* ui;
};

#endif // COLORPICKERFORM_H
