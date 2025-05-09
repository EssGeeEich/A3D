#include "materialcache.h"
#include "material.h"

namespace A3D {

MaterialCache::MaterialCache(Material* parent)
	: QObject{ parent },
	  m_material(parent),
	  m_isDirty(true) {
	log(LC_Debug, u"Constructor: MaterialCache");
}
MaterialCache::~MaterialCache() {
	log(LC_Debug, u"Destructor: MaterialCache");
}

Material* MaterialCache::material() const {
	return m_material;
}

void MaterialCache::markDirty() {
	m_isDirty = true;
}
void MaterialCache::markClean() {
	m_isDirty = false;
}
bool MaterialCache::isDirty() const {
	return m_isDirty;
}

}
