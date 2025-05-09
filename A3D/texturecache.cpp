#include "texturecache.h"
#include "texture.h"

namespace A3D {

TextureCache::TextureCache(Texture* parent)
	: QObject{ parent },
	  m_texture(parent),
	  m_isDirty(true) {
	log(LC_Debug, u"Constructor: TextureCache");
}
TextureCache::~TextureCache() {
	log(LC_Debug, u"Destructor: TextureCache");
}

Texture* TextureCache::texture() const {
	return m_texture;
}

void TextureCache::markDirty() {
	m_isDirty = true;
}
void TextureCache::markClean() {
	m_isDirty = false;
}
bool TextureCache::isDirty() const {
	return m_isDirty;
}

}
