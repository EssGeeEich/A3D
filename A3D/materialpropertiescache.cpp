#include "materialpropertiescache.h"
#include "materialproperties.h"

namespace A3D {

MaterialPropertiesCache::MaterialPropertiesCache(MaterialProperties* parent)
	: QObject{ parent },
	  m_materialProperties(parent),
	  m_isDirty(true) {
	log(LC_Debug, u"Constructor: MaterialPropertiesCache");
}
MaterialPropertiesCache::~MaterialPropertiesCache() {
	log(LC_Debug, u"Destructor: MaterialPropertiesCache");
}

MaterialProperties* MaterialPropertiesCache::materialProperties() const {
	return m_materialProperties;
}

void MaterialPropertiesCache::markDirty() {
	m_isDirty = true;
}
void MaterialPropertiesCache::markClean() {
	m_isDirty = false;
}
bool MaterialPropertiesCache::isDirty() const {
	return m_isDirty;
}

}
