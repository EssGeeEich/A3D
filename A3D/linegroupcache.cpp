#include "linegroupcache.h"
#include "linegroup.h"

namespace A3D {

LineGroupCache::LineGroupCache(LineGroup* parent)
	: QObject{ parent },
	  m_lineGroup(parent),
	  m_isDirty(true) {
	log(LC_Debug, u"Constructor: LineGroup");
}
LineGroupCache::~LineGroupCache() {
	log(LC_Debug, u"Destructor: LineGroup");
}

LineGroup* LineGroupCache::lineGroup() const {
	return m_lineGroup;
}

void LineGroupCache::markDirty() {
	m_isDirty = true;
}
void LineGroupCache::markClean() {
	m_isDirty = false;
}
bool LineGroupCache::isDirty() const {
	return m_isDirty;
}

}
