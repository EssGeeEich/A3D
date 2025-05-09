#include "linegroup.h"
#include "renderer.h"

namespace A3D {

LineGroup::LineGroup(ResourceManager* resourceManager)
	: Resource{ resourceManager },
	  m_drawMode(Lines),
	  m_thickness(1.f) {
	log(LC_Debug, u"Constructor: LineGroup");
}

LineGroup::~LineGroup() {
	log(LC_Debug, u"Destructor: LineGroup (begin)");
	for(auto it = m_lineGroupCache.begin(); it != m_lineGroupCache.end(); ++it) {
		if(it->second.isNull())
			continue;

		Renderer* r = Renderer::getRenderer(it->first);
		if(!r) {
			log(LC_Info, u"LineGroup::~LineGroup: Potential memory leak? Renderer not available.");
			continue;
		}

		r->Delete(it->second);
	}
	log(LC_Debug, u"Destructor: LineGroup (end)");
}

LineGroup* LineGroup::clone() const {
	LineGroup* newLineGroup    = new LineGroup(resourceManager());
	newLineGroup->m_drawMode   = m_drawMode;
	newLineGroup->m_vertices   = m_vertices;
	newLineGroup->m_indices    = m_indices;
	newLineGroup->m_thickness  = m_thickness;
	newLineGroup->m_contents   = m_contents;
	newLineGroup->m_packedData = m_packedData;
	return newLineGroup;
}

void LineGroup::setContents(Contents contents) {
	if(m_contents == contents)
		return;

	m_contents = contents;
	m_packedData.clear();
	invalidateCache();
}

LineGroup::Contents LineGroup::contents() const {
	return m_contents;
}

void LineGroup::setThickness(float value) {
	m_thickness = value;
}

float LineGroup::thickness() const {
	return m_thickness;
}

void LineGroup::optimizeIndices() {
	if(drawMode() != LineGroup::IndexedLines && drawMode() != LineGroup::IndexedLineStrips)
		return;

	std::vector<LineGroup::Vertex> optimizedVertices;
	std::vector<std::uint32_t> optimizedIndices;

	optimizedVertices.reserve(m_indices.size() / 2);
	optimizedIndices.reserve(m_indices.size());

	for(auto it = m_indices.begin(); it != m_indices.end(); ++it) {
		LineGroup::Vertex* currentVertex = &m_vertices[*it];

		optimizedIndices.push_back(static_cast<std::uint32_t>(optimizedVertices.size()));

		for(std::uint32_t prevIndex = 0; prevIndex < optimizedVertices.size(); ++prevIndex) {
			if(currentVertex->Equals(optimizedVertices[prevIndex], m_contents)) {
				optimizedIndices.back() = prevIndex;
				break;
			}
		}

		if(optimizedIndices.back() == optimizedVertices.size())
			optimizedVertices.push_back(*currentVertex);
	}

	m_vertices = std::move(optimizedVertices);
	m_indices  = std::move(optimizedIndices);
	invalidateCache();
}

std::vector<LineGroup::Vertex>& LineGroup::vertices() {
	return m_vertices;
}
std::vector<LineGroup::Vertex> const& LineGroup::vertices() const {
	return m_vertices;
}

std::vector<std::uint32_t>& LineGroup::indices() {
	return m_indices;
}
std::vector<std::uint32_t> const& LineGroup::indices() const {
	return m_indices;
}

void LineGroup::setDrawMode(DrawMode drawMode) {
	m_drawMode = drawMode;
}
LineGroup::DrawMode LineGroup::drawMode() const {
	return m_drawMode;
}

std::vector<std::uint8_t> const& LineGroup::packedData() const {
	if(m_packedData.empty()) {
		std::size_t const vertexSize = packedVertexSize(m_contents);
		m_packedData.resize(vertexSize * m_vertices.size());

		std::uint8_t* pDstBase = m_packedData.data();
		for(auto it = m_vertices.begin(); it != m_vertices.end(); ++it, pDstBase += vertexSize) {
			std::uint8_t* pDst = pDstBase;
			Vertex const& v    = *it;

			if(m_contents & Position2D) {
				std::memcpy(pDst, &v.Position2D, sizeof(v.Position2D));
				pDst += sizeof(v.Position2D);
			}
			if(m_contents & Position3D) {
				std::memcpy(pDst, &v.Position3D, sizeof(v.Position3D));
				pDst += sizeof(v.Position3D);
			}
			if(m_contents & Color3D) {
				std::memcpy(pDst, &v.Color3D, sizeof(v.Color3D));
				pDst += sizeof(v.Color3D);
			}
			if(m_contents & Color4D) {
				std::memcpy(pDst, &v.Color4D, sizeof(v.Color4D));
				pDst += sizeof(v.Color4D);
			}
			Q_UNUSED(pDst)
		}
	}

	return m_packedData;
}

void LineGroup::invalidateCache(std::uintptr_t rendererID) {
	m_packedData.clear();

	if(rendererID == std::numeric_limits<std::uintptr_t>::max()) {
		for(auto it = m_lineGroupCache.begin(); it != m_lineGroupCache.end();) {
			if(it->second.isNull()) {
				it = m_lineGroupCache.erase(it);
				continue;
			}

			it->second->markDirty();
			++it;
		}
	}
	else {
		auto it = m_lineGroupCache.find(rendererID);
		if(it == m_lineGroupCache.end())
			return;
		if(it->second.isNull())
			m_lineGroupCache.erase(it);
		else
			it->second->markDirty();
	}
}

std::size_t LineGroup::packedVertexSize(Contents contents) {
	std::size_t vCount = 0;
	if(contents & Position2D)
		vCount += sizeof(Vertex().Position2D);
	if(contents & Position3D)
		vCount += sizeof(Vertex().Position3D);
	if(contents & Color3D)
		vCount += sizeof(Vertex().Color3D);
	if(contents & Color4D)
		vCount += sizeof(Vertex().Color4D);
	return vCount;
}

bool LineGroup::Vertex::Equals(Vertex const& o, Contents c) const {
	if(c & LineGroup::Position2D && Position2D != o.Position2D)
		return false;
	if(c & LineGroup::Position3D && Position3D != o.Position3D)
		return false;
	if(c & LineGroup::Color3D && Color3D != o.Color3D)
		return false;
	if(c & LineGroup::Color4D && Color4D != o.Color4D)
		return false;
	return true;
}

}
