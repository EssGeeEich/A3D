#ifndef A3DMATERIALCACHEOGL_H
#define A3DMATERIALCACHEOGL_H

#include "A3D/common.h"
#include "A3D/materialcache.h"
#include "A3D/materialproperties.h"
#include <QOpenGLShaderProgram>

namespace A3D {

class MaterialCacheOGL : public MaterialCache
{
	Q_OBJECT
public:
	MaterialCacheOGL(Material*);
	
	void update(CoreGLFunctions*);
	void render(CoreGLFunctions*, MaterialProperties const&, QMatrix4x4 const& model, QMatrix4x4 const& view, QMatrix4x4 const& proj);
	
private:
	void applyUniform(QString const& name, QVariant const& value);
	void applyUniforms(std::map<QString, QVariant> const& uniforms);
	std::unique_ptr<QOpenGLShaderProgram> m_program;
	
	struct UniformCachedInfo {
		inline UniformCachedInfo()
			: m_uniformID(-1) {}
		
		int m_uniformID;
		QVariant m_lastValue;
	};

	std::map<QString, UniformCachedInfo> m_uniformCachedInfo;
	
	int m_uidMVP;
	int m_uidM;
	int m_uidV;
	int m_uidP;
};

}

#endif // A3DMATERIALCACHEOGL_H