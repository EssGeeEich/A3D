#include "meshcacheogl.h"
#include "rendererogl.h"
#include <QOpenGLFunctions>

namespace A3D {

MeshCacheOGL::MeshCacheOGL(Mesh* parent)
	: MeshCache{ parent },
	  m_drawMode(Mesh::Triangles),
	  m_vbo(QOpenGLBuffer::VertexBuffer),
	  m_ibo(QOpenGLBuffer::IndexBuffer),
	  m_elementCount(0),
	  m_iboFormat(GL_UNSIGNED_INT),
	  m_meshUBO(0) {
	log(LC_Debug, u"Constructor: MeshCacheOGL");
}

MeshCacheOGL::~MeshCacheOGL() {
	log(LC_Debug, u"Destructor: MeshCacheOGL");

	if(m_meshUBO) {
		QOpenGLContext::currentContext()->functions()->glDeleteBuffers(1, &m_meshUBO);
		m_meshUBO = 0;
	}
}

void MeshCacheOGL::render(RendererOGL* r, CoreGLFunctions* gl, QMatrix4x4 const& modelMatrix, QMatrix4x4 const& viewMatrix, QMatrix4x4 const& projMatrix) {
	auto glErrorCheck = r->checkGlErrors("MeshCacheOGL::render");
	Q_UNUSED(glErrorCheck)

	if(!m_elementCount || !m_meshUBO)
		return;

	m_vao.bind();

	if(m_meshUBO_data.mMatrix != modelMatrix || m_meshUBO_data.vMatrix != viewMatrix || m_meshUBO_data.pMatrix != projMatrix) {
		QMatrix4x4 const mvMatrix  = (viewMatrix * modelMatrix);
		QMatrix4x4 const mvpMatrix = (projMatrix * mvMatrix);

		m_meshUBO_data.mMatrix = modelMatrix;
		m_meshUBO_data.vMatrix = viewMatrix;
		m_meshUBO_data.pMatrix = projMatrix;

		m_meshUBO_data.mvMatrix  = mvMatrix;
		m_meshUBO_data.mvpMatrix = mvpMatrix;

		m_meshUBO_data.mNormalMatrix   = modelMatrix.inverted().transposed();
		m_meshUBO_data.mvNormalMatrix  = mvMatrix.inverted().transposed();
		m_meshUBO_data.mvpNormalMatrix = mvpMatrix.inverted().transposed();

		gl->glBindBuffer(GL_UNIFORM_BUFFER, m_meshUBO);
		gl->glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MeshUBO_Data), &m_meshUBO_data);
		gl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	gl->glBindBufferBase(GL_UNIFORM_BUFFER, RendererOGL::UBO_MeshBinding, m_meshUBO);

	switch(m_drawMode) {
	case Mesh::Triangles:
		gl->glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_elementCount));
		break;
	case Mesh::TriangleStrips:
		gl->glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_elementCount));
		break;
	case Mesh::IndexedTriangles:
		gl->glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_elementCount), m_iboFormat, 0);
		break;
	case Mesh::IndexedTriangleStrips:
		gl->glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(m_elementCount), m_iboFormat, 0);
		break;
	}

	m_vao.release();
}

void MeshCacheOGL::update(RendererOGL* r, CoreGLFunctions* gl) {
	auto glErrorCheck = r->checkGlErrors("MeshCacheOGL::render");
	Q_UNUSED(glErrorCheck)

	Mesh* m = mesh();
	if(!m) {
		m_elementCount = 0;
		return;
	}

	bool isIndexed = (m->drawMode() == Mesh::IndexedTriangles || m->drawMode() == Mesh::IndexedTriangleStrips);

	if(isIndexed)
		m_elementCount = m->indices().size();
	else
		m_elementCount = m->vertices().size();

	if(!m_elementCount)
		return;

	m_drawMode = m->drawMode();

	if(!m_vao.isCreated())
		m_vao.create();
	if(!m_vbo.isCreated())
		m_vbo.create();
	if(isIndexed && !m_ibo.isCreated())
		m_ibo.create();

	m_vao.bind();

	{
		std::vector<std::uint8_t> const& data = m->packedData();
		m_vbo.bind();
		m_vbo.allocate(data.data(), static_cast<int>(data.size() * sizeof(*data.data())));
	}

	if(isIndexed) {
		// GLuint != uint32_t, but technically they should always be the same.
		// If that is not the case, change the GLxxxx type and update GL_UNSIGNED_INT in render().
		std::vector<GLuint> const& indices = m->indices();
		auto it                            = std::max_element(indices.begin(), indices.end());

		m_ibo.bind();

		// Reduce VRAM usage if possible...
		// PS. Don't use the <= comparison here.
		// We want to keep the last index free for the "Triangle Strip Restart Index" feature if we'll ever use it...
		if(it != indices.end() && *it < std::numeric_limits<GLubyte>::max()) {
			std::vector<GLubyte> byteIndices;
			byteIndices.reserve(indices.size());
			std::copy(indices.begin(), indices.end(), std::back_inserter(byteIndices));
			m_ibo.allocate(byteIndices.data(), static_cast<int>(byteIndices.size() * sizeof(*byteIndices.data())));
			m_iboFormat = GL_UNSIGNED_BYTE;
		}
		else if(it != indices.end() && *it < std::numeric_limits<GLushort>::max()) {
			std::vector<GLushort> shortIndices;
			shortIndices.reserve(indices.size());
			std::copy(indices.begin(), indices.end(), std::back_inserter(shortIndices));
			m_ibo.allocate(shortIndices.data(), static_cast<int>(shortIndices.size() * sizeof(*shortIndices.data())));
			m_iboFormat = GL_UNSIGNED_SHORT;
		}
		else {
			m_ibo.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(*indices.data())));
			m_iboFormat = GL_UNSIGNED_INT;
		}
	}

	Mesh::Contents contents        = m->contents();
	GLsizei stride                 = static_cast<GLsizei>(Mesh::packedVertexSize(contents));
	std::uint8_t const* dataOffset = 0;

	if(contents & Mesh::Position2D) {
		gl->glVertexAttribPointer(Position2DAttribute, 2, GL_FLOAT, false, stride, dataOffset);
		gl->glEnableVertexAttribArray(Position2DAttribute);
		dataOffset += sizeof(Mesh::Vertex().Position2D);
	}

	if(contents & Mesh::Position3D) {
		gl->glVertexAttribPointer(Position3DAttribute, 3, GL_FLOAT, false, stride, dataOffset);
		gl->glEnableVertexAttribArray(Position3DAttribute);
		dataOffset += sizeof(Mesh::Vertex().Position3D);
	}

	if(contents & Mesh::TextureCoord2D) {
		gl->glVertexAttribPointer(TextureCoord2DAttribute, 2, GL_FLOAT, false, stride, dataOffset);
		gl->glEnableVertexAttribArray(TextureCoord2DAttribute);
		dataOffset += sizeof(Mesh::Vertex().TextureCoord2D);
	}

	if(contents & Mesh::Normal3D) {
		gl->glVertexAttribPointer(Normal3DAttribute, 3, GL_FLOAT, false, stride, dataOffset);
		gl->glEnableVertexAttribArray(Normal3DAttribute);
		dataOffset += sizeof(Mesh::Vertex().Normal3D);
	}

	if(contents & Mesh::Color3D) {
		gl->glVertexAttribPointer(Color3DAttribute, 3, GL_FLOAT, false, stride, dataOffset);
		gl->glEnableVertexAttribArray(Color3DAttribute);
		dataOffset += sizeof(Mesh::Vertex().Color3D);
	}

	if(contents & Mesh::Color4D) {
		gl->glVertexAttribPointer(Color4DAttribute, 4, GL_FLOAT, false, stride, dataOffset);
		gl->glEnableVertexAttribArray(Color4DAttribute);
		dataOffset += sizeof(Mesh::Vertex().Color4D);
	}

	if(contents & Mesh::BoneIDs) {
		gl->glVertexAttribIPointer(BoneIDAttribute, 4, GL_UNSIGNED_BYTE, stride, dataOffset);
		gl->glEnableVertexAttribArray(BoneIDAttribute);
		dataOffset += sizeof(Mesh::Vertex().BoneIDs);
	}

	if(contents & Mesh::BoneWeights) {
		gl->glVertexAttribPointer(BoneWeightsAttribute, 4, GL_FLOAT, false, stride, dataOffset);
		gl->glEnableVertexAttribArray(BoneWeightsAttribute);
		dataOffset += sizeof(Mesh::Vertex().BoneWeights);
	}

	if(contents & Mesh::SmoothingGroup) {
		gl->glVertexAttribIPointer(SmoothingGroupAttribute, 1, GL_UNSIGNED_BYTE, stride, dataOffset);
		gl->glEnableVertexAttribArray(SmoothingGroupAttribute);
		dataOffset += sizeof(Mesh::Vertex().SmoothingGroup);
	}

	Q_UNUSED(dataOffset)

	m_vao.release();
	m_vbo.release();
	if(isIndexed)
		m_ibo.release();

	if(!m_meshUBO) {
		gl->glGenBuffers(1, &m_meshUBO);

		if(m_meshUBO) {
			gl->glBindBuffer(GL_UNIFORM_BUFFER, m_meshUBO);
			gl->glBufferData(GL_UNIFORM_BUFFER, sizeof(MeshUBO_Data), nullptr, GL_DYNAMIC_DRAW);
			gl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}

	markClean();
}

}
