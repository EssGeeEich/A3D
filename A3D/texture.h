#ifndef A3DTEXTURE_H
#define A3DTEXTURE_H

#include "A3D/common.h"
#include <QObject>
#include <QImage>
#include "A3D/texturecache.h"

namespace A3D {

class Texture : public QObject {
	Q_OBJECT
public:
	enum RenderOption {
		NoOptions = 0x0,

		GenerateMipMaps = 0x1,
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)

	enum StandardTexture {
		MissingTexture,
	};
	static Texture* standardTexture(StandardTexture);

	enum Filter {
		Nearest,
		Linear,
		NearestMipMapNearest,
		NearestMipMapLinear,
		LinearMipMapNearest,
		LinearMipMapLinear,
	};

	enum WrapDirection {
		WrapDirectionX,
		WrapDirectionY,
		WrapDirectionZ,
	};

	enum WrapMode {
		Repeat,
		MirroredRepeat,
		Clamp,
	};

	explicit Texture(QObject* parent = nullptr);
	explicit Texture(QImage image, QObject* parent = nullptr);
	~Texture();

	RenderOptions renderOptions() const;
	void setRenderOptions(RenderOptions);

	QImage const& image() const;
	void setImage(QImage);

	WrapMode wrapMode(WrapDirection) const;
	void setWrapMode(WrapDirection, WrapMode);

	float maxAnisotropy() const;
	void setMaxAnisotropy(float);

	Filter minFilter() const;
	void setMinFilter(Filter);

	Filter magFilter() const;
	void setMagFilter(Filter);

	float lodBias() const;
	// Lower = Nicer
	// Higher = Faster
	void setLodBias(float);

	void invalidateCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());

	template <typename T>
	T* getTextureCacheT(std::uintptr_t rendererID) const {
		auto it = m_textureCache.find(rendererID);
		if(it == m_textureCache.end() || it->second.isNull())
			return nullptr;

		return qobject_cast<T*>(it->second);
	}
	template <typename T>
	std::pair<T*, bool> getOrEmplaceTextureCache(std::uintptr_t rendererID) {
		auto it = m_textureCache.find(rendererID);
		if(it == m_textureCache.end() || it->second.isNull()) {
			T* c                       = new T(this);
			m_textureCache[rendererID] = QPointer<TextureCache>(c);
			return std::make_pair(c, true);
		}

		T* c = qobject_cast<T*>(it->second);
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for Texture.");

		return std::make_pair(c, false);
	}

private:
	QImage m_image;
	std::map<WrapDirection, WrapMode> m_wrapMode;
	Filter m_minFilter;
	Filter m_magFilter;
	float m_lodBias;
	float m_maxAnisotropy;
	RenderOptions m_renderOptions;

	std::map<std::uintptr_t, QPointer<TextureCache>> m_textureCache;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Texture::RenderOptions)

}

#endif // A3DTEXTURE_H