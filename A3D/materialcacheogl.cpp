#include "materialcacheogl.h"
#include "materialpropertiescacheogl.h"
#include "material.h"
#include "rendererogl.h"
#include <QColor>
#include <QTransform>

namespace A3D {

MaterialCacheOGL::MaterialCacheOGL(Material* parent)
	: MaterialCache{ parent },
	  m_meshUBO_index(GL_INVALID_INDEX),
	  m_matpropUBO_index(GL_INVALID_INDEX),
	  m_sceneUBO_index(GL_INVALID_INDEX),
	  m_lineUBO_index(GL_INVALID_INDEX) {
	log(LC_Debug, u"Constructor: MaterialCacheOGL");
}

MaterialCacheOGL::~MaterialCacheOGL() {
	log(LC_Debug, u"Destructor: MaterialCacheOGL");
}

void MaterialCacheOGL::applyUniform(RendererOGL* r, CoreGLFunctions*, QString const& name, QVariant const& value) {
	auto glErrorCheck = r->checkGlErrors("MaterialCacheOGL::applyUniform");
	Q_UNUSED(glErrorCheck)

	if(!m_program)
		return;

	auto prevEntry         = m_uniformCachedInfo.try_emplace(name);
	UniformCachedInfo& uci = prevEntry.first->second;
	if(prevEntry.second)
		uci.m_uniformID = m_program->uniformLocation(name);

	if(uci.m_uniformID == -1)
		return;

	if(uci.m_lastValue == value)
		return;

	uci.m_lastValue = value;

	int const uniformLocation = uci.m_uniformID;

	switch(value.userType()) {
	default:
		break;
	case QMetaType::Double:
	case QMetaType::Float:
		m_program->setUniformValue(uniformLocation, value.toFloat());
		break;
	case QMetaType::LongLong:
	case QMetaType::Long:
	case QMetaType::Int:
	case QMetaType::Short:
		m_program->setUniformValue(uniformLocation, static_cast<GLint>(value.toInt()));
		break;
	case QMetaType::ULongLong:
	case QMetaType::ULong:
	case QMetaType::UInt:
	case QMetaType::UShort:
		m_program->setUniformValue(uniformLocation, static_cast<GLuint>(value.toUInt()));
		break;

#define BIND_MT(metaType, cppType)                                                                                                                                                 \
	case metaType:                                                                                                                                                                 \
		m_program->setUniformValue(uniformLocation, value.value<cppType>());                                                                                                       \
		break

		BIND_MT(QMetaType::QColor, QColor);
		BIND_MT(QMetaType::QPoint, QPoint);
		BIND_MT(QMetaType::QPointF, QPointF);
		BIND_MT(QMetaType::QSize, QSize);
		BIND_MT(QMetaType::QSizeF, QSizeF);
		BIND_MT(QMetaType::QMatrix4x4, QMatrix4x4);
		BIND_MT(QMetaType::QTransform, QTransform);
		BIND_MT(QMetaType::QVector2D, QVector2D);
		BIND_MT(QMetaType::QVector3D, QVector3D);
		BIND_MT(QMetaType::QVector4D, QVector4D);

#undef BIND_MT
	}
}

void MaterialCacheOGL::applyUniforms(RendererOGL* r, CoreGLFunctions* gl, std::map<QString, QVariant> const& uniforms) {
	auto glErrorCheck = r->checkGlErrors("MaterialCacheOGL::applyUniforms");
	Q_UNUSED(glErrorCheck)

	if(!m_program)
		return;

	for(auto it = uniforms.begin(); it != uniforms.end(); ++it) {
		applyUniform(r, gl, it->first, it->second);
	}
}

void MaterialCacheOGL::install(RendererOGL* r, CoreGLFunctions* gl) {
	auto glErrorCheck = r->checkGlErrors("MaterialCacheOGL::install");
	Q_UNUSED(glErrorCheck)

	if(!m_program)
		return;

	Material* m = material();
	if(!m)
		return;

	m_program->bind();

	if(m_meshUBO_index != GL_INVALID_INDEX)
		gl->glUniformBlockBinding(m_program->programId(), m_meshUBO_index, RendererOGL::UBO_MeshBinding);

	if(m_matpropUBO_index != GL_INVALID_INDEX)
		gl->glUniformBlockBinding(m_program->programId(), m_matpropUBO_index, RendererOGL::UBO_MaterialPropertiesBinding);

	if(m_sceneUBO_index != GL_INVALID_INDEX)
		gl->glUniformBlockBinding(m_program->programId(), m_sceneUBO_index, RendererOGL::UBO_SceneBinding);

	if(m_lineUBO_index != GL_INVALID_INDEX)
		gl->glUniformBlockBinding(m_program->programId(), m_lineUBO_index, RendererOGL::UBO_LineBinding);
}

void MaterialCacheOGL::update(RendererOGL* r, CoreGLFunctions* gl) {
	auto glErrorCheck = r->checkGlErrors("MaterialCacheOGL::update");
	Q_UNUSED(glErrorCheck)

	Material* m = material();
	if(!m) {
		m_program.reset();
		return;
	}

	QString gxShader = m->shader(Material::GLSL, Material::GeometryShader);
	QString vxShader = m->shader(Material::GLSL, Material::VertexShader);
	QString fxShader = m->shader(Material::GLSL, Material::FragmentShader);

	if(vxShader.isEmpty() || fxShader.isEmpty()) {
		m_program.reset();
		return;
	}

	m_program = std::make_unique<QOpenGLShaderProgram>();

	// TODO: Evaluate caching?

	if(!gxShader.isEmpty())
		m_program->addShaderFromSourceCode(QOpenGLShader::Geometry, gxShader);

	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vxShader);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fxShader);

	if(!m_program->link()) {
		log(LC_Warning, QStringLiteral(u"Couldn't link QOpenGLShader: %1").arg(m_program->log()));
		m_program.reset();
		return;
	}

	m_uniformCachedInfo.clear();
	m_program->bind();

	for(GLuint i = 0; i < MaterialProperties::MaxTextures; ++i) {
		applyUniform(r, gl, QString("TextureSlot") + QString::number(i), GLuint(i));
	}

	// PBR mode
	applyUniform(r, gl, "AlbedoTexture", GLuint(MaterialProperties::AlbedoTextureSlot));
	applyUniform(r, gl, "NormalTexture", GLuint(MaterialProperties::NormalTextureSlot));
	applyUniform(r, gl, "MetallicTexture", GLuint(MaterialProperties::MetallicTextureSlot));
	applyUniform(r, gl, "RoughnessTexture", GLuint(MaterialProperties::RoughnessTextureSlot));
	applyUniform(r, gl, "AOTexture", GLuint(MaterialProperties::AOTextureSlot));

	// Phong mode
	applyUniform(r, gl, "DiffuseTexture", GLuint(MaterialProperties::AlbedoTextureSlot));
	applyUniform(r, gl, "EmissiveTexture", GLuint(MaterialProperties::MetallicTextureSlot));
	applyUniform(r, gl, "BumpMapTexture", GLuint(MaterialProperties::NormalTextureSlot));

	applyUniform(r, gl, "EnvironmentMapTexture", GLuint(MaterialProperties::EnvironmentTextureSlot));
	applyUniform(r, gl, "CubeMapTexture", GLuint(MaterialProperties::EnvironmentTextureSlot));
	applyUniform(r, gl, "IrradianceTexture", GLuint(MaterialProperties::EnvironmentTextureSlot));
	applyUniform(r, gl, "PrefilterTexture", GLuint(MaterialProperties::PrefilterTextureSlot));
	applyUniform(r, gl, "BrdfTexture", GLuint(MaterialProperties::BrdfTextureSlot));

	m_meshUBO_index    = gl->glGetUniformBlockIndex(m_program->programId(), "MeshUBO_Data");
	m_matpropUBO_index = gl->glGetUniformBlockIndex(m_program->programId(), "MaterialUBO_Data");
	m_sceneUBO_index   = gl->glGetUniformBlockIndex(m_program->programId(), "SceneUBO_Data");
	m_lineUBO_index    = gl->glGetUniformBlockIndex(m_program->programId(), "LineUBO_Data");
	m_program->release();

	markClean();
}

}
