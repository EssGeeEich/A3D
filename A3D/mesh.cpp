#include "mesh.h"
#include "renderer.h"

namespace A3D {

Mesh* Mesh::standardMesh(StandardMesh stdMesh) {
	static std::map<StandardMesh, Mesh> standardMeshes;

	auto it = standardMeshes.find(stdMesh);
	if(it != standardMeshes.end())
		return &it->second;

	Mesh& newMesh = standardMeshes[stdMesh];
	switch(stdMesh) {
	case Triangle2DMesh:
		newMesh.setDrawMode(Triangles);

		newMesh.vertices().resize(3);
		newMesh.vertices()[0].Position3D = QVector3D(+0.0f, +0.5f, 0.f);
		newMesh.vertices()[1].Position3D = QVector3D(-0.5f, -0.5f, 0.f);
		newMesh.vertices()[2].Position3D = QVector3D(+0.5f, -0.5f, 0.f);

		newMesh.setContents(Position3D);
		break;
	case ScreenQuadMesh:
		newMesh.setDrawMode(TriangleStrips);

		newMesh.vertices().resize(4);
		newMesh.vertices()[0].Position3D = QVector3D(-1.f, +1.f, 0.f);
		newMesh.vertices()[1].Position3D = QVector3D(-1.f, -1.f, 0.f);
		newMesh.vertices()[2].Position3D = QVector3D(+1.f, +1.f, 0.f);
		newMesh.vertices()[3].Position3D = QVector3D(+1.f, -1.f, 0.f);

		newMesh.vertices()[0].TextureCoord2D = QVector2D(0.f, 1.f);
		newMesh.vertices()[1].TextureCoord2D = QVector2D(0.f, 0.f);
		newMesh.vertices()[2].TextureCoord2D = QVector2D(1.f, 1.f);
		newMesh.vertices()[3].TextureCoord2D = QVector2D(1.f, 0.f);

		newMesh.vertices()[0].Normal3D = QVector3D(0.f, 0.f, 1.f);
		newMesh.vertices()[1].Normal3D = QVector3D(0.f, 0.f, 1.f);
		newMesh.vertices()[2].Normal3D = QVector3D(0.f, 0.f, 1.f);
		newMesh.vertices()[3].Normal3D = QVector3D(0.f, 0.f, 1.f);

		newMesh.setContents(Position3D | TextureCoord2D | Normal3D);
		break;
	case UnitQuadMesh:
		newMesh.setDrawMode(TriangleStrips);

		newMesh.vertices().resize(4);
		newMesh.vertices()[0].Position3D = QVector3D(-0.5f, +0.5f, 0.f);
		newMesh.vertices()[1].Position3D = QVector3D(-0.5f, -0.5f, 0.f);
		newMesh.vertices()[2].Position3D = QVector3D(+0.5f, +0.5f, 0.f);
		newMesh.vertices()[3].Position3D = QVector3D(+0.5f, -0.5f, 0.f);

		newMesh.vertices()[0].TextureCoord2D = QVector2D(0.f, 1.f);
		newMesh.vertices()[1].TextureCoord2D = QVector2D(0.f, 0.f);
		newMesh.vertices()[2].TextureCoord2D = QVector2D(1.f, 1.f);
		newMesh.vertices()[3].TextureCoord2D = QVector2D(1.f, 0.f);

		newMesh.vertices()[0].Normal3D = QVector3D(0.f, 0.f, 1.f);
		newMesh.vertices()[1].Normal3D = QVector3D(0.f, 0.f, 1.f);
		newMesh.vertices()[2].Normal3D = QVector3D(0.f, 0.f, 1.f);
		newMesh.vertices()[3].Normal3D = QVector3D(0.f, 0.f, 1.f);

		newMesh.setContents(Position3D | TextureCoord2D | Normal3D);
		break;
	case CubeIndexedMesh:
		newMesh.setDrawMode(IndexedTriangles);

		{
			struct {
				QVector3D pos;
				QVector3D norm;
				QVector2D tex;
			} vertices[36] = {
				{{ -1.0f, -1.0f, -1.0f }, { +0.0f, +0.0f, -1.0f }, { 0.0f, 0.0f }},
                {{ +1.0f, +1.0f, -1.0f }, { +0.0f, +0.0f, -1.0f }, { 1.0f, 1.0f }},
				{{ +1.0f, -1.0f, -1.0f }, { +0.0f, +0.0f, -1.0f }, { 1.0f, 0.0f }},
                {{ +1.0f, +1.0f, -1.0f }, { +0.0f, +0.0f, -1.0f }, { 1.0f, 1.0f }},
				{{ -1.0f, -1.0f, -1.0f }, { +0.0f, +0.0f, -1.0f }, { 0.0f, 0.0f }},
                {{ -1.0f, +1.0f, -1.0f }, { +0.0f, +0.0f, -1.0f }, { 0.0f, 1.0f }},
				{{ -1.0f, -1.0f, +1.0f }, { +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }},
                {{ +1.0f, -1.0f, +1.0f }, { +0.0f, +0.0f, +1.0f }, { 1.0f, 0.0f }},
				{{ +1.0f, +1.0f, +1.0f }, { +0.0f, +0.0f, +1.0f }, { 1.0f, 1.0f }},
                {{ +1.0f, +1.0f, +1.0f }, { +0.0f, +0.0f, +1.0f }, { 1.0f, 1.0f }},
				{{ -1.0f, +1.0f, +1.0f }, { +0.0f, +0.0f, +1.0f }, { 0.0f, 1.0f }},
                {{ -1.0f, -1.0f, +1.0f }, { +0.0f, +0.0f, +1.0f }, { 0.0f, 0.0f }},
				{{ -1.0f, +1.0f, +1.0f }, { -1.0f, +0.0f, +0.0f }, { 1.0f, 0.0f }},
                {{ -1.0f, +1.0f, -1.0f }, { -1.0f, +0.0f, +0.0f }, { 1.0f, 1.0f }},
				{{ -1.0f, -1.0f, -1.0f }, { -1.0f, +0.0f, +0.0f }, { 0.0f, 1.0f }},
                {{ -1.0f, -1.0f, -1.0f }, { -1.0f, +0.0f, +0.0f }, { 0.0f, 1.0f }},
				{{ -1.0f, -1.0f, +1.0f }, { -1.0f, +0.0f, +0.0f }, { 0.0f, 0.0f }},
                {{ -1.0f, +1.0f, +1.0f }, { -1.0f, +0.0f, +0.0f }, { 1.0f, 0.0f }},
				{{ +1.0f, +1.0f, +1.0f }, { +1.0f, +0.0f, +0.0f }, { 1.0f, 0.0f }},
                {{ +1.0f, -1.0f, -1.0f }, { +1.0f, +0.0f, +0.0f }, { 0.0f, 1.0f }},
				{{ +1.0f, +1.0f, -1.0f }, { +1.0f, +0.0f, +0.0f }, { 1.0f, 1.0f }},
                {{ +1.0f, -1.0f, -1.0f }, { +1.0f, +0.0f, +0.0f }, { 0.0f, 1.0f }},
				{{ +1.0f, +1.0f, +1.0f }, { +1.0f, +0.0f, +0.0f }, { 1.0f, 0.0f }},
                {{ +1.0f, -1.0f, +1.0f }, { +1.0f, +0.0f, +0.0f }, { 0.0f, 0.0f }},
				{{ -1.0f, -1.0f, -1.0f }, { +0.0f, -1.0f, +0.0f }, { 0.0f, 1.0f }},
                {{ +1.0f, -1.0f, -1.0f }, { +0.0f, -1.0f, +0.0f }, { 1.0f, 1.0f }},
				{{ +1.0f, -1.0f, +1.0f }, { +0.0f, -1.0f, +0.0f }, { 1.0f, 0.0f }},
                {{ +1.0f, -1.0f, +1.0f }, { +0.0f, -1.0f, +0.0f }, { 1.0f, 0.0f }},
				{{ -1.0f, -1.0f, +1.0f }, { +0.0f, -1.0f, +0.0f }, { 0.0f, 0.0f }},
                {{ -1.0f, -1.0f, -1.0f }, { +0.0f, -1.0f, +0.0f }, { 0.0f, 1.0f }},
				{{ -1.0f, +1.0f, -1.0f }, { +0.0f, +1.0f, +0.0f }, { 0.0f, 1.0f }},
                {{ +1.0f, +1.0f, +1.0f }, { +0.0f, +1.0f, +0.0f }, { 1.0f, 0.0f }},
				{{ +1.0f, +1.0f, -1.0f }, { +0.0f, +1.0f, +0.0f }, { 1.0f, 1.0f }},
                {{ +1.0f, +1.0f, +1.0f }, { +0.0f, +1.0f, +0.0f }, { 1.0f, 0.0f }},
				{{ -1.0f, +1.0f, -1.0f }, { +0.0f, +1.0f, +0.0f }, { 0.0f, 1.0f }},
                {{ -1.0f, +1.0f, +1.0f }, { +0.0f, +1.0f, +0.0f }, { 0.0f, 0.0f }}
			};

			newMesh.vertices().reserve(36);

			for(auto it = std::begin(vertices); it != std::end(vertices); ++it) {
				newMesh.indices().push_back(static_cast<std::uint32_t>(newMesh.vertices().size()));
				newMesh.vertices().push_back(A3D::Mesh::Vertex());

				A3D::Mesh::Vertex& v = newMesh.vertices().back();
				v.Position3D         = it->pos;
				v.Normal3D           = it->norm;
				v.TextureCoord2D     = it->tex;
			}
		}

		newMesh.setContents(Position3D | Normal3D | TextureCoord2D);
		newMesh.optimizeIndices();
		break;
	};
	newMesh.invalidateCache();
	return &newMesh;
}
Mesh::Mesh(ResourceManager* resourceManager)
	: Resource{ resourceManager },
	  m_drawMode(Triangles),
	  m_renderOptions(NoOptions) {
	log(LC_Debug, u"Constructor: Mesh");
}

Mesh::~Mesh() {
	log(LC_Debug, u"Destructor: Mesh (begin)");
	for(auto it = m_meshCache.begin(); it != m_meshCache.end(); ++it) {
		if(it->second.isNull())
			continue;

		Renderer* r = Renderer::getRenderer(it->first);
		if(!r) {
			log(LC_Info, u"Mesh::~Mesh: Potential memory leak? Renderer not available.");
			continue;
		}

		r->Delete(it->second);
	}
	log(LC_Debug, u"Destructor: Mesh (end)");
}

Mesh* Mesh::clone() const {
	Mesh* newMesh            = new Mesh(resourceManager());
	newMesh->m_drawMode      = m_drawMode;
	newMesh->m_vertices      = m_vertices;
	newMesh->m_indices       = m_indices;
	newMesh->m_renderOptions = m_renderOptions;
	newMesh->m_contents      = m_contents;
	newMesh->m_packedData    = m_packedData;
	return newMesh;
}

void Mesh::setRenderOptions(RenderOptions renderOptions) {
	m_renderOptions = renderOptions;
}

Mesh::RenderOptions Mesh::renderOptions() const {
	return m_renderOptions;
}

void Mesh::setContents(Contents contents) {
	if(m_contents == contents)
		return;

	m_contents = contents;
	m_packedData.clear();
	invalidateCache();
}

Mesh::Contents Mesh::contents() const {
	return m_contents;
}

std::optional<QVector3D> Mesh::intersect(QVector3D origin, QVector3D dir) const {
	if((contents() & Position3D) != Position3D)
		return std::nullopt;

	QVector3D nearestHit;
	double nearestHitDistance = -1;

	QVector3D hit;

	switch(drawMode()) {

	case Triangles:
		for(std::size_t i = 0; i + 2 < m_vertices.size(); i += 3) {
			QVector3D const& v0 = m_vertices[i].Position3D;
			QVector3D const& v1 = m_vertices[i + 1].Position3D;
			QVector3D const& v2 = m_vertices[i + 2].Position3D;

			if(intersectTriangle(origin, dir, v0, v1, v2, hit)) {
				double currentDistance = origin.distanceToPoint(hit);
				if(nearestHitDistance < 0.0 || currentDistance < nearestHitDistance) {
					nearestHitDistance = currentDistance;
					nearestHit         = hit;
				}
			}
		}
		break;

	case IndexedTriangles:
		for(std::size_t i = 0; i + 2 < m_indices.size(); i += 3) {
			QVector3D const& v0 = m_vertices[m_indices[i]].Position3D;
			QVector3D const& v1 = m_vertices[m_indices[i + 1]].Position3D;
			QVector3D const& v2 = m_vertices[m_indices[i + 2]].Position3D;

			if(intersectTriangle(origin, dir, v0, v1, v2, hit)) {
				double currentDistance = origin.distanceToPoint(hit);
				if(nearestHitDistance < 0.0 || currentDistance < nearestHitDistance) {
					nearestHitDistance = currentDistance;
					nearestHit         = hit;
				}
			}
		}
		break;

	case TriangleStrips:
		for(int i = 2; i < m_vertices.size(); ++i) {
			QVector3D v0 = m_vertices[i - 2].Position3D;
			QVector3D v1 = m_vertices[i - 1].Position3D;
			QVector3D v2 = m_vertices[i].Position3D;

			QVector3D hit;
			if(intersectTriangle(origin, dir, v0, v1, v2, hit)) {
				double currentDistance = origin.distanceToPoint(hit);
				if(nearestHitDistance < 0.0 || currentDistance < nearestHitDistance) {
					nearestHitDistance = currentDistance;
					nearestHit         = hit;
				}
			}
		}
		break;

	case IndexedTriangleStrips:
		for(int i = 2; i < m_indices.size(); ++i) {
			QVector3D v0 = m_vertices[m_indices[i - 2]].Position3D;
			QVector3D v1 = m_vertices[m_indices[i - 1]].Position3D;
			QVector3D v2 = m_vertices[m_indices[i]].Position3D;

			QVector3D hit;
			if(intersectTriangle(origin, dir, v0, v1, v2, hit)) {
				double currentDistance = origin.distanceToPoint(hit);
				if(nearestHitDistance < 0.0 || currentDistance < nearestHitDistance) {
					nearestHitDistance = currentDistance;
					nearestHit         = hit;
				}
			}
		}
		break;

	default:
		break;
	}

	if(nearestHitDistance < 0.0)
		return std::nullopt;

	return nearestHit;
}

std::size_t Mesh::packedVertexSize(Contents contents) {
	std::size_t vCount = 0;
	if(contents & Position2D)
		vCount += sizeof(Vertex().Position2D);
	if(contents & Position3D)
		vCount += sizeof(Vertex().Position3D);
	if(contents & TextureCoord2D)
		vCount += sizeof(Vertex().TextureCoord2D);
	if(contents & Normal3D)
		vCount += sizeof(Vertex().Normal3D);
	if(contents & Color3D)
		vCount += sizeof(Vertex().Color3D);
	if(contents & Color4D)
		vCount += sizeof(Vertex().Color4D);
	if(contents & BoneIDs)
		vCount += sizeof(Vertex().BoneIDs);
	if(contents & BoneWeights)
		vCount += sizeof(Vertex().BoneWeights);
	if(contents & SmoothingGroup)
		vCount += sizeof(Vertex().SmoothingGroup);
	return vCount;
}

std::vector<std::uint8_t> const& Mesh::packedData() const {
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
			if(m_contents & TextureCoord2D) {
				std::memcpy(pDst, &v.TextureCoord2D, sizeof(v.TextureCoord2D));
				pDst += sizeof(v.TextureCoord2D);
			}
			if(m_contents & Normal3D) {
				std::memcpy(pDst, &v.Normal3D, sizeof(v.Normal3D));
				pDst += sizeof(v.Normal3D);
			}
			if(m_contents & Color3D) {
				std::memcpy(pDst, &v.Color3D, sizeof(v.Color3D));
				pDst += sizeof(v.Color3D);
			}
			if(m_contents & Color4D) {
				std::memcpy(pDst, &v.Color4D, sizeof(v.Color4D));
				pDst += sizeof(v.Color4D);
			}
			if(m_contents & BoneIDs) {
				std::memcpy(pDst, v.BoneIDs, sizeof(v.BoneIDs));
				pDst += sizeof(v.BoneIDs);
			}
			if(m_contents & BoneWeights) {
				std::memcpy(pDst, &v.BoneWeights, sizeof(v.BoneWeights));
				pDst += sizeof(v.BoneWeights);
			}
			if(m_contents & SmoothingGroup) {
				std::memcpy(pDst, &v.SmoothingGroup, sizeof(v.SmoothingGroup));
				pDst += sizeof(v.SmoothingGroup);
			}
			Q_UNUSED(pDst)
		}
	}
	return m_packedData;
}

void Mesh::setDrawMode(DrawMode drawMode) {
	m_drawMode = drawMode;
}
Mesh::DrawMode Mesh::drawMode() const {
	return m_drawMode;
}

bool Mesh::Vertex::Equals(Vertex const& o, Contents c) const {
	if(c & Mesh::Position2D && Position2D != o.Position2D)
		return false;
	if(c & Mesh::Position3D && Position3D != o.Position3D)
		return false;
	if(c & Mesh::TextureCoord2D && TextureCoord2D != o.TextureCoord2D)
		return false;
	if(c & Mesh::Normal3D && Normal3D != o.Normal3D)
		return false;
	if(c & Mesh::Color3D && Color3D != o.Color3D)
		return false;
	if(c & Mesh::Color4D && Color4D != o.Color4D)
		return false;
	if(c & Mesh::BoneIDs && std::memcmp(BoneIDs, o.BoneIDs, sizeof(BoneIDs)) != 0)
		return false;
	if(c & Mesh::BoneWeights && BoneWeights != o.BoneWeights)
		return false;
	if(c & Mesh::SmoothingGroup && SmoothingGroup != o.SmoothingGroup)
		return false;
	return true;
}

void Mesh::optimizeIndices() {
	if(drawMode() != Mesh::IndexedTriangles && drawMode() != Mesh::IndexedTriangleStrips)
		return;

	std::vector<Mesh::Vertex> optimizedVertices;
	std::vector<std::uint32_t> optimizedIndices;

	optimizedVertices.reserve(m_indices.size() / 2);
	optimizedIndices.reserve(m_indices.size());

	for(auto it = m_indices.begin(); it != m_indices.end(); ++it) {
		Mesh::Vertex* currentVertex = &m_vertices[*it];

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

std::vector<Mesh::Vertex>& Mesh::vertices() {
	return m_vertices;
}
std::vector<Mesh::Vertex> const& Mesh::vertices() const {
	return m_vertices;
}

std::vector<std::uint32_t>& Mesh::indices() {
	return m_indices;
}
std::vector<std::uint32_t> const& Mesh::indices() const {
	return m_indices;
}

void Mesh::invalidateCache(std::uintptr_t rendererID) {
	m_packedData.clear();
	if(rendererID == std::numeric_limits<std::uintptr_t>::max()) {
		m_packedData.clear();
		for(auto it = m_meshCache.begin(); it != m_meshCache.end();) {
			if(it->second.isNull()) {
				it = m_meshCache.erase(it);
				continue;
			}

			it->second->markDirty();
			++it;
		}
	}
	else {
		auto it = m_meshCache.find(rendererID);
		if(it == m_meshCache.end())
			return;
		if(it->second.isNull())
			m_meshCache.erase(it);
		else
			it->second->markDirty();
	}
}

}
