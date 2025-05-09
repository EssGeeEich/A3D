#include "image.h"
#include <QFile>
#include <QIODevice>

namespace A3D {

Image::HDRData::HDRData()
	: w(0),
	  h(0),
	  nrComponents(0) {}

Image Image::HDR(QString path) {
	QFile f(path);
	if(!f.open(QFile::ReadOnly))
		return Image();
	Image rv;
	rv.setFromHDR(f);
	return rv;
}

Image::Image()
	: Image(QImage()) {}

Image::Image(QImage i)
	: m_type(T_QIMAGE),
	  m_transparent(TD_NOT_DONE_YET),
	  m_qimage(std::move(i)) {}

Image::Image(HDRData data)
	: m_type(T_HDRDATA),
	  m_transparent(TD_NOT_DONE_YET),
	  m_hdr(std::move(data)) {}

void Image::setFromQImage(QImage i) {
	m_hdr.m_data.clear();
	m_qimage      = std::move(i);
	m_type        = T_QIMAGE;
	m_transparent = TD_NOT_DONE_YET;
}

bool Image::isQImage() const {
	return m_type == T_QIMAGE;
}

bool Image::isHDR() const {
	return m_type == T_HDRDATA;
}

QSize Image::size() const {
	if(isQImage())
		return m_qimage.size();
	if(isHDR())
		return QSize(m_hdr.w, m_hdr.h);
	return QSize();
}

bool Image::isNull() const {
	if(isQImage())
		return m_qimage.isNull();
	if(isHDR())
		return m_hdr.m_data.empty();
	return true;
}

bool Image::isCurrentlyTransparent() const {
	if(isQImage()) {
		if(!m_qimage.hasAlphaChannel())
			return false;

		QImage temp;
		QImage const* ref = imageWithFormat(QImage::Format_RGBA8888, m_qimage, temp);

		std::uint8_t const* beg = ref->constBits();
		std::uint8_t const* end = beg + ref->sizeInBytes();

		for(std::uint8_t const* it = beg; it < end; it += 4) {
			if(it[3] < 0xF0)
				return true;
		}
		return false;
	}
	if(isHDR()) {
		if(m_hdr.nrComponents < 4)
			return false;

		float const* beg = m_hdr.m_data.data();
		float const* end = beg + m_hdr.m_data.size();

		for(float const* it = beg; it < end; it += 4) {
			if(it[3] < 1.f)
				return true;
		}
		return false;
	}
	return false;
}

void Image::checkTransparency() const {
	if(m_transparent == TD_NOT_DONE_YET)
		m_transparent = isCurrentlyTransparent() ? TD_TRANSPARENT : TD_OPAQUE;
}

bool Image::hasAlphaChannel() const {
	checkTransparency();
	return m_transparent == TD_TRANSPARENT;
}

QImage const& Image::qimage() const {
	return m_qimage;
}

Image::HDRData const& Image::hdr() const {
	return m_hdr;
}

Image& Image::operator=(QImage const& i) {
	setFromQImage(i);
	return *this;
}

}

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

namespace A3D {

static int cb_qt_read(void* user, char* data, int size) {
	if(!user || size <= 0)
		return 0;
	QIODevice* d = reinterpret_cast<QIODevice*>(user);
	return static_cast<int>(d->read(data, size));
}
static void cb_qt_skip(void* user, int n) {
	if(!user || n == 0)
		return;
	QIODevice* d = reinterpret_cast<QIODevice*>(user);
	d->seek(d->pos() + n);
}
static int cb_qt_eof(void* user) {
	if(!user)
		return 1;
	QIODevice* d = reinterpret_cast<QIODevice*>(user);
	return d->atEnd() ? 1 : 0;
}

static void cb_qt_write(void* user, void* data, int size) {
	if(!user)
		return;
	QIODevice* d = reinterpret_cast<QIODevice*>(user);
	d->write(reinterpret_cast<char const*>(data), size);
}

void Image::saveToFile(QIODevice& dest) {
	if(isQImage()) {
		qimage().save(&dest, "PNG");
	}
	else if(isHDR()) {
		stbi_write_hdr_to_func(cb_qt_write, reinterpret_cast<void*>(&dest), m_hdr.w, m_hdr.h, m_hdr.nrComponents, m_hdr.m_data.data());
	}
}

void Image::setFromHDR(HDRData data) {
	m_hdr         = std::move(data);
	m_type        = T_HDRDATA;
	m_transparent = TD_NOT_DONE_YET;
}

void Image::setFromHDR(QIODevice& d) {
	stbi_io_callbacks callbacks = { cb_qt_read, cb_qt_skip, cb_qt_eof };

	int const requiredComponents = 0;

	int dstX    = 0;
	int dstY    = 0;
	int dstComp = 0;
	float* data = stbi_loadf_from_callbacks(&callbacks, reinterpret_cast<void*>(&d), &dstX, &dstY, &dstComp, requiredComponents);

	if(requiredComponents)
		dstComp = requiredComponents;

	if(data) {
		m_hdr.m_data.resize(dstX * dstY * dstComp);
		std::memcpy(m_hdr.m_data.data(), data, m_hdr.m_data.size() * sizeof(float));
		for(std::size_t i = 0; i < m_hdr.m_data.size(); ++i) {
			if(m_hdr.m_data[i] > 65000.f) {
				qDebug().noquote() << "Too bright pixel at index " << i << ": " << m_hdr.m_data[i];
				m_hdr.m_data[i] = 65000.f;
			}
		}
		m_hdr.w            = static_cast<std::size_t>(dstX);
		m_hdr.h            = static_cast<std::size_t>(dstY);
		m_hdr.nrComponents = static_cast<std::size_t>(dstComp);
		stbi_image_free(data);
	}
	else {
		m_hdr.m_data.clear();
		m_hdr.w            = 0;
		m_hdr.h            = 0;
		m_hdr.nrComponents = 0;
	}
	m_type        = T_HDRDATA;
	m_transparent = TD_NOT_DONE_YET;
}

}
