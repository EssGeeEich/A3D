#ifndef A3DMATERIAL_H
#define A3DMATERIAL_H

#include "A3D/common.h"
#include <QObject>
#include <QVariant>
#include <map>
#include <cstdint>
#include <stdexcept>
#include "A3D/materialcache.h"
#include "A3D/resource.h"

namespace A3D {

class Material : public Resource {
	Q_OBJECT
public:
	enum RenderOption {
		NoOptions = 0x0,

		// Mark this material as a translucent material.
		// Will render the entity on a separate draw pass.
		Translucent = 0x1,
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)

	enum StandardMaterial {
		UnshadedMaterial,
		BillboardMaterial,
		PBRMaterial,
		SkyboxMaterial,
		IrradianceMaterial,
		PrefilterMaterial,
		BRDFMaterial,
	};
	static Material* standardMaterial(StandardMaterial);

	enum ShaderMode {
		GLSL,
	};

	enum ShaderType {
		VertexShader,
		FragmentShader,
	};

	explicit Material(ResourceManager* = nullptr);
	~Material();

	Material* clone() const;

	RenderOptions renderOptions() const;
	void setRenderOptions(RenderOptions);

	void setShader(ShaderMode mode, ShaderType type, QString shaderContents);
	void setShaderFile(ShaderMode mode, ShaderType type, QString shaderPath);

	QString shader(ShaderMode mode, ShaderType type) const;

	void invalidateCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());

	template <typename T>
	T* getMaterialCacheT(std::uintptr_t rendererID) const {
		auto it = m_materialCache.find(rendererID);
		if(it == m_materialCache.end() || it->second.isNull())
			return nullptr;

		return qobject_cast<T*>(it->second);
	}
	template <typename T>
	std::pair<T*, bool> getOrEmplaceMaterialCache(std::uintptr_t rendererID) {
		auto it = m_materialCache.find(rendererID);
		if(it == m_materialCache.end() || it->second.isNull()) {
			T* c                        = new T(this);
			m_materialCache[rendererID] = QPointer<MaterialCache>(c);
			return std::make_pair(c, true);
		}

		T* c = qobject_cast<T*>(it->second);
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for Material.");

		return std::make_pair(c, false);
	}

private:
	RenderOptions m_renderOptions;
	std::map<ShaderMode, std::map<ShaderType, QString>> m_shaders;
	std::map<std::uintptr_t, QPointer<MaterialCache>> m_materialCache;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Material::RenderOptions)

}

#endif // A3DMATERIAL_H
