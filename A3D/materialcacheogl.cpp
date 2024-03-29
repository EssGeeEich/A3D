#include "A3D/materialcacheogl.h"
#include "A3D/materialpropertiescacheogl.h"
#include "A3D/material.h"
#include "A3D/rendererogl.h"
#include <QColor>
#include <QTransform>

namespace A3D {

MaterialCacheOGL::MaterialCacheOGL(Material* parent)
	: MaterialCache{ parent },
	  m_meshUBO_index(GL_INVALID_INDEX),
	  m_matpropUBO_index(GL_INVALID_INDEX),
	  m_sceneUBO_index(GL_INVALID_INDEX) {
	log(LC_Debug, "Constructor: MaterialCacheOGL");
}

MaterialCacheOGL::~MaterialCacheOGL() {
	log(LC_Debug, "Destructor: MaterialCacheOGL");
}

void MaterialCacheOGL::applyUniform(QString const& name, QVariant const& value) {
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

void MaterialCacheOGL::applyUniforms(std::map<QString, QVariant> const& uniforms) {
	if(!m_program)
		return;

	for(auto it = uniforms.begin(); it != uniforms.end(); ++it) {
		applyUniform(it->first, it->second);
	}
}

void MaterialCacheOGL::install(CoreGLFunctions* gl) {
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
}

void MaterialCacheOGL::update(RendererOGL*, CoreGLFunctions* gl) {
	Material* m = material();
	if(!m) {
		m_program.reset();
		return;
	}

	QString vxShader = m->shader(Material::GLSL, Material::VertexShader);
	QString fxShader = m->shader(Material::GLSL, Material::FragmentShader);

	if(vxShader.isEmpty() || fxShader.isEmpty()) {
		m_program.reset();
		return;
	}

	m_program = std::make_unique<QOpenGLShaderProgram>();

	// TODO: Evaluate caching?

	if(!vxShader.isEmpty())
		m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vxShader);
	if(!fxShader.isEmpty())
		m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fxShader);
	if(!m_program->link()) {
		log(LC_Warning, "Couldn't link QOpenGLShader: " + m_program->log());
		m_program.reset();
		return;
	}

	m_uniformCachedInfo.clear();
	m_program->bind();

	for(GLuint i = 0; i < MaterialProperties::MaxTextures; ++i) {
		applyUniform(QString("TextureSlot") + QString::number(i), GLuint(i));
	}

	// PBR mode
	applyUniform("AlbedoTexture", GLuint(MaterialProperties::AlbedoTextureSlot));
	applyUniform("NormalTexture", GLuint(MaterialProperties::NormalTextureSlot));
	applyUniform("MetallicTexture", GLuint(MaterialProperties::MetallicTextureSlot));
	applyUniform("RoughnessTexture", GLuint(MaterialProperties::RoughnessTextureSlot));
	applyUniform("AOTexture", GLuint(MaterialProperties::AOTextureSlot));

	// Phong mode
	applyUniform("DiffuseTexture", GLuint(MaterialProperties::AlbedoTextureSlot));
	applyUniform("EmissiveTexture", GLuint(MaterialProperties::MetallicTextureSlot));
	applyUniform("BumpMapTexture", GLuint(MaterialProperties::NormalTextureSlot));

	applyUniform("EnvironmentMapTexture", GLuint(MaterialProperties::EnvironmentTextureSlot));
	applyUniform("CubeMapTexture", GLuint(MaterialProperties::EnvironmentTextureSlot));
	applyUniform("IrradianceTexture", GLuint(MaterialProperties::EnvironmentTextureSlot));
	applyUniform("PrefilterTexture", GLuint(MaterialProperties::PrefilterTextureSlot));
	applyUniform("BrdfTexture", GLuint(MaterialProperties::BrdfTextureSlot));

	m_meshUBO_index    = gl->glGetUniformBlockIndex(m_program->programId(), "MeshUBO_Data");
	m_matpropUBO_index = gl->glGetUniformBlockIndex(m_program->programId(), "MaterialUBO_Data");
	m_sceneUBO_index = gl->glGetUniformBlockIndex(m_program->programId(), "SceneUBO_Data");
	m_program->release();

	markClean();
}

}
