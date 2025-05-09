/// @file
/// @brief Declares the Image class used for handling standard and HDR images in A3D.

#ifndef A3DIMAGE_H
#define A3DIMAGE_H

#include "common.h"
#include <QImage>

namespace A3D {

/// @brief Represents a 2D image that can be either a standard QImage or HDR data.
class Image {
public:
	/// @struct HDRData
	/// @brief Stores high dynamic range image data.
	struct HDRData {
		/// @brief Default constructor.
		HDRData();
		HDRData(HDRData const& o)            = default;
		HDRData(HDRData&& o)                 = default;
		HDRData& operator=(HDRData const& o) = default;
		HDRData& operator=(HDRData&& o)      = default;

		std::vector<float> m_data; ///< Raw HDR float data
		std::size_t w;             ///< Width of the HDR image
		std::size_t h;             ///< Height of the HDR image
		std::size_t nrComponents;  ///< Number of components per pixel (e.g. RGB = 3, RGBA = 4)
	};

	/// @brief Creates an Image from an HDR file.
	/// @param[in] path The path to the HDR file.
	/// @return An Image instance with HDR data.
	static Image HDR(QString path);

	/// @brief Default constructor.
	Image();

	/// @brief Constructs an Image from HDR data.
	/// @param[in] data The HDR data.
	Image(HDRData data);

	/// @brief Constructs an Image from a QImage.
	/// @param[in] image The QImage to wrap.
	Image(QImage image);

	/// @brief Replaces current image data with a new QImage.
	/// @param[in] image The QImage to set.
	void setFromQImage(QImage image);

	/// @brief Sets image data from an HDR source.
	/// @param[in] device The input file/device containing HDR data.
	void setFromHDR(QIODevice& device);

	/// @brief Sets image data from HDRData.
	/// @param[in] data The HDR data to set.
	void setFromHDR(HDRData data);

	/// @brief Saves the image to a file.
	/// @param[in] device The output device to write the image to.
	void saveToFile(QIODevice& device);

	/// @brief Checks if the current image is a QImage.
	/// @return True if the image is a QImage.
	bool isQImage() const;

	/// @brief Checks if the current image is HDR.
	/// @return True if the image is HDR.
	bool isHDR() const;

	/// @brief Returns the dimensions of the image.
	/// @return A QSize representing the image size.
	QSize size() const;

	/// @brief Checks if the image is null (no content).
	/// @return True if the image is null.
	bool isNull() const;

	/// @brief Checks if the image has an alpha channel.
	/// @return True if the image has transparency.
	bool hasAlphaChannel() const;

	/// @brief Retrieves the QImage representation.
	/// @return Const reference to the QImage.
	QImage const& qimage() const;

	/// @brief Retrieves the HDR data.
	/// @return Const reference to the HDRData.
	HDRData const& hdr() const;

	/// @brief Assignment operator from QImage.
	/// @param[in] image The QImage to assign from.
	/// @return Reference to this Image.
	Image& operator=(QImage const& image);

private:
	/// @brief Determines if the image is currently transparent.
	/// @return True if any transparency is detected.
	bool isCurrentlyTransparent() const;

	/// @brief Triggers transparency detection logic.
	void checkTransparency() const;

	/// @brief Type of the internal image storage.
	enum Type {
		T_QIMAGE,  ///< QImage type
		T_HDRDATA, ///< HDR type
	};

	/// @brief Status of transparency detection.
	enum TransparencyDetection {
		TD_NOT_DONE_YET = 0, ///< Transparency check not performed
		TD_OPAQUE,           ///< Image is opaque
		TD_TRANSPARENT,      ///< Image has transparency
	};

	Type m_type;                                 ///< Current image type
	mutable TransparencyDetection m_transparent; ///< Cached transparency state

	QImage m_qimage; ///< The QImage data
	HDRData m_hdr;   ///< The HDR image data
};

}

#endif // A3DIMAGE_H
