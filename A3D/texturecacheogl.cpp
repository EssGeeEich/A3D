#include "texturecacheogl.h"
#include "texture.h"
#include "rendererogl.h"
#include <QOpenGLPixelTransferOptions>

namespace A3D {

TextureCacheOGL::TextureCacheOGL(Texture* parent)
	: TextureCache{ parent } {
	log(LC_Debug, u"Constructor: TextureCacheOGL");
}

TextureCacheOGL::~TextureCacheOGL() {
	log(LC_Debug, u"Destructor: TextureCacheOGL");
}

inline QOpenGLTexture::WrapMode translateWrapMode(Texture::WrapMode wm) {
	switch(wm) {
	default:
	case Texture::Repeat:
		return QOpenGLTexture::Repeat;
	case Texture::MirroredRepeat:
		return QOpenGLTexture::MirroredRepeat;
	case Texture::Clamp:
		return QOpenGLTexture::ClampToEdge;
	}
}
inline QOpenGLTexture::Filter translateFilter(Texture::Filter f) {
	switch(f) {
	default:
	case Texture::Nearest:
		return QOpenGLTexture::Nearest;
	case Texture::Linear:
		return QOpenGLTexture::Linear;
	case Texture::NearestMipMapNearest:
		return QOpenGLTexture::NearestMipMapNearest;
	case Texture::NearestMipMapLinear:
		return QOpenGLTexture::NearestMipMapLinear;
	case Texture::LinearMipMapNearest:
		return QOpenGLTexture::LinearMipMapNearest;
	case Texture::LinearMipMapLinear:
		return QOpenGLTexture::LinearMipMapLinear;
	}
}
void TextureCacheOGL::update(RendererOGL* r, CoreGLFunctions* gl) {
	auto glErrorCheck = r->checkGlErrors("TextureCacheOGL::update");
	Q_UNUSED(glErrorCheck)

	Texture* t = texture();
	if(!t || t->image().isNull()) {
		m_texture.reset();
		return;
	}

	m_texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
	m_texture->setWrapMode(QOpenGLTexture::DirectionS, translateWrapMode(t->wrapMode(Texture::WrapDirectionX)));
	m_texture->setWrapMode(QOpenGLTexture::DirectionT, translateWrapMode(t->wrapMode(Texture::WrapDirectionY)));
	// m_texture->setWrapMode(QOpenGLTexture::DirectionR, translateWrapMode(t->wrapMode(Texture::WrapDirectionZ)));

	m_texture->setMaximumAnisotropy(t->maxAnisotropy());
	m_texture->setMinMagFilters(translateFilter(t->minFilter()), translateFilter(t->magFilter()));
	m_texture->setLevelofDetailBias(t->lodBias());

	Image const& i = t->image();
	if(i.isQImage()) {
		if(i.hasAlphaChannel()) {
			QImage temp;
			QImage const* glImage = imageWithFormat(QImage::Format_RGBA8888, i.qimage(), temp);

			m_texture->bind();
			m_texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
			m_texture->setSize(glImage->width(), glImage->height());
			m_texture->setMipLevels((t->renderOptions() & Texture::GenerateMipMaps) ? m_texture->maximumMipLevels() : 1);
			m_texture->allocateStorage();

			QOpenGLPixelTransferOptions uploadOptions;
			uploadOptions.setAlignment(1);
			m_texture->setData(0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, glImage->bits(), &uploadOptions);
		}
		else {
			QImage temp;
			QImage const* glImage = imageWithFormat(QImage::Format_RGB888, i.qimage(), temp);

			m_texture->bind();
			m_texture->setFormat(QOpenGLTexture::RGB8_UNorm);
			m_texture->setSize(glImage->width(), glImage->height());
			m_texture->setMipLevels((t->renderOptions() & Texture::GenerateMipMaps) ? m_texture->maximumMipLevels() : 1);
			m_texture->allocateStorage();

			QOpenGLPixelTransferOptions uploadOptions;
			uploadOptions.setAlignment(1);
			m_texture->setData(0, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, glImage->bits(), &uploadOptions);
		}
	}
	else if(i.isHDR()) {
		m_texture->bind();
		switch(i.hdr().nrComponents) {
		case 1:
			gl->glTexSubImage2D(m_texture->target(), 0, 0, 0, i.hdr().w, i.hdr().h, QOpenGLTexture::Red, QOpenGLTexture::Float32, i.hdr().m_data.data());
			break;
		case 2:
			gl->glTexSubImage2D(m_texture->target(), 0, 0, 0, i.hdr().w, i.hdr().h, QOpenGLTexture::RG, QOpenGLTexture::Float32, i.hdr().m_data.data());
			break;
		case 3:
			gl->glTexSubImage2D(m_texture->target(), 0, 0, 0, i.hdr().w, i.hdr().h, QOpenGLTexture::RGB, QOpenGLTexture::Float32, i.hdr().m_data.data());
			break;
		case 4:
			gl->glTexSubImage2D(m_texture->target(), 0, 0, 0, i.hdr().w, i.hdr().h, QOpenGLTexture::RGBA, QOpenGLTexture::Float32, i.hdr().m_data.data());
			break;
		}
		m_texture->release();
	}

	markClean();
}

void TextureCacheOGL::applyToSlot(RendererOGL* r, CoreGLFunctions* gl, GLuint slot) {
	auto glErrorCheck = r->checkGlErrors("TextureCacheOGL::applyToSlot");
	Q_UNUSED(glErrorCheck)

	if(!m_texture)
		return;
	gl->glActiveTexture(GL_TEXTURE0 + slot);
	gl->glBindTexture(GL_TEXTURE_2D, m_texture->textureId());
}
}
