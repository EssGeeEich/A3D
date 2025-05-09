#include "materialproperties.h"
#include "renderer.h"

namespace A3D {

MaterialProperties::MaterialProperties(ResourceManager* resourceManager)
	: Resource{ resourceManager },
	  m_alwaysTranslucent(false) {
	log(LC_Debug, u"Constructor: MaterialProperties");
}

MaterialProperties::~MaterialProperties() {
	log(LC_Debug, u"Destructor: MaterialProperties (start)");
	for(auto it = m_materialPropertiesCache.begin(); it != m_materialPropertiesCache.end(); ++it) {
		if(it->second.isNull())
			continue;

		Renderer* r = Renderer::getRenderer(it->first);
		if(!r) {
			log(LC_Info, u"MaterialProperties::~MaterialProperties: Potential memory leak? Renderer not available.");
			continue;
		}

		r->Delete(it->second);
	}
	log(LC_Debug, u"Destructor: MaterialProperties (end)");
}

MaterialProperties* MaterialProperties::clone() const {
	MaterialProperties* newMatProp  = new MaterialProperties(resourceManager());
	newMatProp->m_alwaysTranslucent = m_alwaysTranslucent;
	newMatProp->m_rawValues         = m_rawValues;
	for(std::size_t i = 0; i < MaxTextures; ++i)
		newMatProp->m_textures[i] = m_textures[i];
	return newMatProp;
}

Texture* MaterialProperties::texture(TextureSlot slot) const {
	if(slot >= MaxTextures)
		return nullptr;
	return m_textures[slot];
}

void MaterialProperties::setTexture(Texture* texture, TextureSlot slot) {
	if(slot >= MaxTextures)
		return;
	if(texture == m_textures[slot])
		return;
	if(m_textures[slot] && m_textures[slot]->parent() == this)
		delete m_textures[slot];
	m_textures[slot] = texture;
}

QVariant MaterialProperties::rawValue(QString name, QVariant fallback) {
	auto val = m_rawValues.find(name);
	if(val != m_rawValues.end())
		return val->second;

	return fallback;
}

void MaterialProperties::setRawValue(QString name, QVariant value) {
	m_rawValues[std::move(name)] = std::move(value);
}

std::map<QString, QVariant> const& MaterialProperties::rawValues() const {
	return m_rawValues;
}
std::map<QString, QVariant>& MaterialProperties::rawValues() {
	return m_rawValues;
}

void MaterialProperties::setAlwaysTranslucent(bool always) {
	m_alwaysTranslucent = always;
}
bool MaterialProperties::isTranslucent() const {
	if(m_alwaysTranslucent)
		return true;

	for(std::size_t i = 0; i < MaxTextures; ++i) {
		if(m_textures[i] && m_textures[i]->image().hasAlphaChannel())
			return true;
	}

	return false;
}

}
