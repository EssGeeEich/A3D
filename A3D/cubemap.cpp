#include "cubemap.h"
#include "renderer.h"

namespace A3D {

Cubemap::Cubemap(ResourceManager* resourceManager)
	: Resource{ resourceManager } {
	log(LC_Debug, u"Constructor: Cubemap");
}
Cubemap::~Cubemap() {
	log(LC_Debug, u"Destructor: Cubemap (start)");
	for(auto it = m_cubemapCache.begin(); it != m_cubemapCache.end(); ++it) {
		if(it->second.isNull())
			continue;

		Renderer* r = Renderer::getRenderer(it->first);
		if(!r) {
			log(LC_Info, u"Cubemap::~Cubemap: Potential memory leak? Renderer not available.");
			continue;
		}

		r->Delete(it->second);
	}
	log(LC_Debug, u"Destructor: Cubemap (end)");
}

Cubemap* Cubemap::clone() const {
	Cubemap* newCubemap = new Cubemap(resourceManager());
	for(std::size_t i = 0; i < CF_COUNT; ++i)
		newCubemap->m_faces[i] = m_faces[i];
	return newCubemap;
}

void Cubemap::setCubemapFace(CubemapFace face, Image const& image) {
	if(face >= CF_COUNT)
		return;
	m_faces[face] = image;
}

Image const& Cubemap::cubemapFace(CubemapFace face) const {
	if(face < CF_COUNT)
		return m_faces[face];
	static Image invalid;
	return invalid;
}

bool Cubemap::isValid() const {
	if(m_faces[0].isNull())
		return false;

	QSize const s = m_faces[0].size();
	if(s.width() != s.height())
		return false;

	bool const isQImage = m_faces[0].isQImage();
	bool const isHDR    = m_faces[0].isHDR();

	for(std::size_t i = 1; i < CF_COUNT; ++i) {
		if(m_faces[i].isNull())
			return false;

		if(m_faces[i].size() != s)
			return false;
		if(m_faces[i].isQImage() != isQImage)
			return false;
		if(m_faces[i].isHDR() != isHDR)
			return false;
	}

	return true;
}

void Cubemap::invalidateCache(std::uintptr_t rendererID) {
	if(rendererID == std::numeric_limits<std::uintptr_t>::max()) {
		for(auto it = m_cubemapCache.begin(); it != m_cubemapCache.end();) {
			if(it->second.isNull()) {
				it = m_cubemapCache.erase(it);
				continue;
			}

			it->second->markDirty();
			++it;
		}
	}
	else {
		auto it = m_cubemapCache.find(rendererID);
		if(it == m_cubemapCache.end())
			return;
		if(it->second.isNull())
			m_cubemapCache.erase(it);
		else
			it->second->markDirty();
	}
}

}
