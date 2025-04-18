#ifndef A3DTEXTBILLBOARDENTITY_H
#define A3DTEXTBILLBOARDENTITY_H
#include "common.h"
#include "entity.h"
#include "texture.h"
#include <QPainter>
#include <QFont>
#include <QColor>

namespace A3D {

class TextBillboardEntity : public Entity {
	Q_OBJECT
public:
	TextBillboardEntity(Entity* parent = nullptr);

	void setText(QString text);
	void setFont(QFont font);
	void setColor(QColor color);

	QString text() const;
	QFont font() const;
	QColor color() const;

protected:
	virtual bool updateEntity(std::chrono::milliseconds t) override;

private:
	void refresh();

	QString m_text;
	QFont m_font;
	QColor m_color;
	bool m_textureDirty;

	Group* m_group;
	Texture* m_texture;
	QFontMetrics m_fontMetrics;
	QPainter m_painter;
	QImage m_image;
};
}

#endif // A3DTEXTBILLBOARDENTITY_H
