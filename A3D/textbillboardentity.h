/// @file
/// @brief Defines TextBillboardEntity for rendering dynamic text as a billboard in 3D scenes.

#ifndef A3DTEXTBILLBOARDENTITY_H
#define A3DTEXTBILLBOARDENTITY_H

#include "common.h"
#include "entity.h"
#include "texture.h"
#include <QPainter>
#include <QFont>
#include <QColor>

namespace A3D {

/// @brief Entity that displays text on a 2D billboard facing the camera.
class TextBillboardEntity : public Entity {
	Q_OBJECT
public:
	/// @brief Constructs a TextBillboardEntity with optional parent.
	/// @param[in] parent Parent entity in the scene graph.
	TextBillboardEntity(Entity* parent = nullptr);

	/// @brief Sets the displayed text.
	/// @param[in] text The string to render on the billboard.
	void setText(QString text);

	/// @brief Sets the font used for rendering text.
	/// @param[in] font QFont specifying typeface and size.
	void setFont(QFont font);

	/// @brief Sets the text color.
	/// @param[in] color QColor to use for text.
	void setColor(QColor color);

	/// @brief Retrieves the current text.
	/// @return The string displayed by the entity.
	QString text() const;

	/// @brief Retrieves the current font.
	/// @return QFont used for rendering.
	QFont font() const;

	/// @brief Retrieves the current text color.
	/// @return QColor of the text.
	QColor color() const;

protected:
	/// @brief Updates the entity state each frame.
	/// @param[in] t Time elapsed since last update.
	/// @return true if the entity was modified.
	virtual bool updateEntity(std::chrono::milliseconds t) override;

private:
	/// @brief Regenerates the texture when text, font, or color changes.
	void refresh();

	QString m_text;      ///< Text string to render
	QFont m_font;        ///< Font used for rendering
	QColor m_color;      ///< Color of the text
	bool m_textureDirty; ///< Flag indicating texture needs update

	Group* m_group;             ///< Scene group for the billboard mesh
	Texture* m_texture;         ///< Texture holding rendered text
	QFontMetrics m_fontMetrics; ///< Metrics for calculating text dimensions
	QPainter m_painter;         ///< Painter used to draw text onto the image
	QImage m_image;             ///< Image buffer for text rendering
};
}

#endif // A3DTEXTBILLBOARDENTITY_H
