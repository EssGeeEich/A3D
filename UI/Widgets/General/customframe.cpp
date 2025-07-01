#include "customframe.h"
#include <QStyleOptionButton>
#include <QMouseEvent>
#include <QStyle>
#include <QPainterPath>

CustomFrame::CustomFrame(QWidget* parent, QColor borderColor, A3D::ChartAxisIndicatorType type)
    : QFrame(parent),
      m_borderColor(borderColor),
      m_isButton(false),
      m_isHighlighted(false) {

    QFont font;
    font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(FR_High));
    m_numberLabel.setFont(font);
    QFormLayout* lay = new QFormLayout;
    setLayout(lay);
    layout()->setSpacing(0);
    lay->addRow(&m_numberLabel, &m_label);
    setBorder(borderColor, type);
    m_numberLabel.hide();
    m_label.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    m_numberLabel.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
}

void CustomFrame::setBorder(QColor color, A3D::ChartAxisIndicatorType type) {
    size_t borderSize = MinorWidth;
    if(type == A3D::CHAXIND_MAJOR_INDICATOR)
        borderSize = MajorWidth;

    m_borderColor = color;
    m_borderSize  = borderSize;
    update();
}

void CustomFrame::setText(QString text) {
    m_label.setText(text);
}
void CustomFrame::setNumber(size_t n) {
    m_numberLabel.show();
    m_numberLabel.setText(QString::number(n, 10).append(":  "));
}

void CustomFrame::setAsButton() {
    m_isButton = true;
}

void CustomFrame::setFormats(QColor color, QFont font) {
    QPalette palette = m_label.palette();
    // m_label.setForegroundRole(QPalette::WindowText);
    palette.setColor(QPalette::WindowText, color);
    m_label.setPalette(palette);

    FontResolutions res = ChartAxisGeneralSettings::getFontResoulution(font.pointSize());
    font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(res));
    m_label.setFont(font);
}

void CustomFrame::setValues(QString text, size_t n) {
    setText(text);
    setNumber(n);
}

void CustomFrame::mousePressEvent(QMouseEvent* event) {
    if(event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QFrame::mousePressEvent(event);
}

void CustomFrame::setHighlighted(bool state) {
    if(state == m_isHighlighted)
        return;
    m_isHighlighted = state;
    //setAutoFillBackground(state);
    if(state) {
        QPalette pal = m_enterPalette = palette();

        QColor defaultBackgroundColor = pal.button().color().lighter();
        pal.setColor(QPalette::Button, defaultBackgroundColor);
        setPalette(pal);
        return;
    }
    setPalette(m_enterPalette);
}

void CustomFrame::enterEvent(QEnterEvent* event) {

    if(!m_isButton) {
        QFrame::enterEvent(event);
        return;
    }

    setHighlighted(true);
    QFrame::enterEvent(event);
}

void CustomFrame::leaveEvent(QEvent* event) {
    if(!m_isButton) {
        QFrame::leaveEvent(event);
        return;
    }
    setHighlighted(false);

    QFrame::leaveEvent(event);
}

void CustomFrame::paintEvent(QPaintEvent* event) {

	// Disegna il bordo
	QFrame::paintEvent(event); // Chiama il paintEvent base per mantenere il comportamento predefinito

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Crea il path stondato
	QPainterPath path;
    const int radius  = 10; // Raggio degli angoli stondati
	QRectF borderRect = rect().adjusted(1, 1, -2, -2);
	path.addRoundedRect(borderRect, radius, radius);

	// Disegna il bordo stondato
	painter.setPen(QPen(m_borderColor, m_borderSize));
	painter.setBrush(Qt::NoBrush);
	painter.drawPath(path);
}
