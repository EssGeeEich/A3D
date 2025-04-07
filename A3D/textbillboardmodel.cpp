#include "textbillboardmodel.h"
#include "entity.h"

namespace A3D {

TextBillboard::TextBillboard(QObject* parent)
	: Model(parent) {

	Group* group = getOrAddGroup("Text");
	group->setMesh(Mesh::standardMesh(Mesh::ScreenQuadMesh));
	group->setMaterial(Material::standardMaterial(Material::BillboardMaterial));

	Texture* textTexture = new Texture(nullptr);
	textTexture->setParent(this);
	MaterialProperties* materialProperties = new MaterialProperties(nullptr);
	materialProperties->setParent(this);
	group->setMaterialProperties(materialProperties);
	materialProperties->setTexture(textTexture, MaterialProperties::AlbedoTextureSlot);
}

void TextBillboard::setText(QString text, QFont const& font, QColor const& color) {
	m_image = QImage(400, 400, QImage::Format_RGBA8888);
	m_image.fill(Qt::transparent);

	m_painter.begin(&m_image);
	m_painter.setFont(font);
	m_painter.setPen(color);
	m_painter.drawText(50, 100, text);
	m_painter.end();

	Texture* texture = getOrAddGroup("Text")->materialProperties()->texture(MaterialProperties::AlbedoTextureSlot);
	texture->setImage(std::move(m_image));
	texture->invalidateCache();
}

}
