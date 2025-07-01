#ifndef CUSTOMFRAME_H
#define CUSTOMFRAME_H

#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QLayout>
#include <QFormLayout>
#include "chartaxisgeneralsettings.h"

enum {
    MajorWidth = 4,
    MinorWidth = 2,
};

// This is a general widget used as button and as a display frame.

class CustomFrame : public QFrame {
    Q_OBJECT
public:
    explicit CustomFrame(QWidget* parent = nullptr, QColor borderColor = Qt::black, A3D::ChartAxisIndicatorType type = A3D::CHAXIND_MAJOR_INDICATOR);

    void setBorder(QColor color, A3D::ChartAxisIndicatorType type);
    void setText(QString text);
    void setNumber(size_t n);

    void setAsButton();

    void setFormats(QColor color, QFont font);

    void setValues(QString text, size_t n);

    void setHighlighted(bool);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;

    void enterEvent(QEnterEvent* event) override;

    void leaveEvent(QEvent* event) override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPalette m_enterPalette;
    QColor m_borderColor;
    size_t m_borderSize;
    QLabel m_numberLabel;
    QLabel m_label;
    bool m_isButton;
    bool m_isHighlighted;
};

#endif // CUSTOMFRAME_H
