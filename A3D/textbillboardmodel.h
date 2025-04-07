#ifndef TEXTBILLBOARDMODEL_H
#define TEXTBILLBOARDMODEL_H
#include "model.h"
#include <QPainter>
#include <QFont>
#include <QColor>

namespace A3D {

class TextBillboard : public Model {
	Q_OBJECT
public:
	TextBillboard(QObject* parent = nullptr);

	void setText(QString text, QFont const& font = QFont(), QColor const& color = QColor(Qt::black));

private:
	QPainter m_painter;
	QImage m_image;
};

}

#endif // TEXTBILLBOARDMODEL_H
