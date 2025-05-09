#include "meshcache.h"
#include "mesh.h"

namespace A3D {

MeshCache::MeshCache(Mesh* parent)
	: QObject{ parent },
	  m_mesh(parent),
	  m_isDirty(true) {
	log(LC_Debug, u"Constructor: MeshCache");
}
MeshCache::~MeshCache() {
	log(LC_Debug, u"Destructor: MeshCache");
}

Mesh* MeshCache::mesh() const {
	return m_mesh;
}

void MeshCache::markDirty() {
	m_isDirty = true;
}
void MeshCache::markClean() {
	m_isDirty = false;
}
bool MeshCache::isDirty() const {
	return m_isDirty;
}

}
