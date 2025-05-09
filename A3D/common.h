/// @file
/// @brief Core definitions, utilities, and common types for the A3D engine.

#ifndef A3DCOMMON_H
#define A3DCOMMON_H

#include <QGenericMatrix>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QPointF>
#include <QPointer>
#include <QtMath>
#include <QString>
#include <QStringView>
#include <QVariant>
#include <QObject>
#include <QImage>

#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>

#include <map>
#include <vector>

/// @namespace A3D
/// @brief Root namespace for the A3D engine, providing foundational types, utilities, and interfaces.
namespace A3D {

/// @brief Severity levels for engine logging.
enum LogChannel {
	LC_Debug = 0, ///< Debug-level messages
	LC_Info,      ///< Informational messages
	LC_Warning,   ///< Warning messages
	LC_Critical,  ///< Error messages
	LC_Fatal      ///< Fatal errors
};

/// @brief Logs a message to the engine console or output.
/// @param[in] channel The severity channel.
/// @param[in] msg     The message string view.
void log(LogChannel channel, QStringView msg);

/// @brief Enumerates six cardinal 3D directions and a count.
enum Direction3D {
	D_PositiveX = 0, ///< +X axis direction
	D_PositiveY,     ///< +Y axis direction
	D_PositiveZ,     ///< +Z axis direction
	D_NegativeX,     ///< -X axis direction
	D_NegativeY,     ///< -Y axis direction
	D_NegativeZ,     ///< -Z axis direction
	D_Count          ///< Number of directions
};

/// @brief Identifies the three principal axes in 3D space.
enum Axis3D {
	AXIS_X,    ///< X axis
	AXIS_Y,    ///< Y axis
	AXIS_Z,    ///< Z axis
	AXIS_COUNT ///< Number of axes
};

/// @brief Returns the unit vector corresponding to a principal axis.
/// @param[in] axis One of the values of Axis3D.
/// @return A QVector3D of length 1 along the given axis.
QVector3D axisVector(Axis3D axis);

/// @brief Sets a specific axis component of a vector.
/// @param[in,out] vector The vector to modify.
/// @param[in]     axis   The axis to set (AXIS_X, AXIS_Y, AXIS_Z).
/// @param[in]     value  New value for the specified axis.
void setVectorAxis(QVector3D& vector, Axis3D axis, float value);

/// @brief Retrieves a specific axis component of a vector.
/// @param[in] vector The vector to query.
/// @param[in] axis   The axis to get (AXIS_X, AXIS_Y, AXIS_Z).
/// @return The component value along the given axis.
float getVectorAxis(QVector3D const& vector, Axis3D axis);

/// @brief Utility struct to hold raw matrix data.
/// @details Stores a 4x4 matrix as a flat float array, with interoperability for QMatrix4x4.
struct RawMatrix4x4 {
	/// Default constructor initializes to identity matrix.
	RawMatrix4x4();

	/// Constructs from a QMatrix4x4.
	/// @param[in] m The QMatrix4x4 to convert into raw format.
	RawMatrix4x4(QMatrix4x4 const& m);

	/// Assigns from a QMatrix4x4.
	/// @param[in] m The QMatrix4x4 to assign from.
	/// @return Reference to this RawMatrix4x4 after assignment.
	RawMatrix4x4& operator=(QMatrix4x4 const& m);

	RawMatrix4x4(RawMatrix4x4 const& o)            = default;
	RawMatrix4x4& operator=(RawMatrix4x4 const& m) = default;

	/// @brief Compares this matrix with a QMatrix4x4 for equality.
	/// @param[in] o The QMatrix4x4 to compare with.
	/// @return True if all elements are equal; false otherwise.
	bool operator==(QMatrix4x4 const& o) const;

	/// @brief Compares this matrix with a QMatrix4x4 for inequality.
	/// @param[in] o The QMatrix4x4 to compare with.
	/// @return True if any element differs; false otherwise.
	bool operator!=(QMatrix4x4 const& o) const;

	float data[16]; ///< Raw matrix data as float array
};

class Entity;
class Model;
class Group;
class View;

/// @struct IntersectionResult
/// @brief Contains the results of a ray-scene intersection.
struct IntersectionResult {
	Entity* m_resultingEntity;      ///< The intersected entity, or nullptr if none
	Model* m_resultingModel;        ///< The specific model intersected within the entity
	Group* m_resultingGroup;        ///< The group owning the hit geometry
	QVector3D m_groupLocalHitPoint; ///< Intersection point in group-local coordinates
	QVector3D m_hitPoint;           ///< Intersection point in world coordinates
};

/// @brief Linearly maps values in `data` to the range [0,1] based on expected bounds.
/// @param[in,out] data Vector of float values to normalize in-place.
/// @param[in]     min  Expected minimum input value (maps to 0).
/// @param[in]     max  Expected maximum input value (maps to 1).
/// @remark Each element x in data is transformed via (x - min)/(max - min).
/// @remark Values outside [min, max] will map outside [0,1].
void normalizeMinMax(std::vector<float>& data, float min, float max);

/// @brief Normalizes values in `data` to the range [0,1].
/// @param[in] data Vector of float values to normalize in-place.
void normalize(std::vector<float>& data);

/// @brief Converts a QColor to a normalized QVector4D (RGBA).
/// @param[in] color The input QColor.
/// @return A vector with components in [0,1].
QVector4D colorToVector(QColor color);

/// @brief Mixin to make derived classes non-copyable.
class NonCopyable {
	NonCopyable(NonCopyable const&)            = delete;
	NonCopyable& operator=(NonCopyable const&) = delete;

public:
	/// @brief Default constructor.
	inline NonCopyable() {}
};

/// @typedef CoreGLFunctions
/// @brief Alias for QOpenGLFunctions_3_3_Core to access OpenGL 3.3 core calls.
typedef QOpenGLFunctions_3_3_Core CoreGLFunctions;

/// @brief Removes null QPointer elements from a vector container.
/// @tparam T Type of QObject pointed to.
/// @param[in,out] container Vector of QPointer<T> to clean in-place.
template <typename T>
void cleanupQPointers(std::vector<QPointer<T>>& container) {
	container.erase(
		std::remove_if(
			container.begin(), container.end(),
			[](QPointer<T> const& e) -> bool {
				return e.isNull();
			}
		),
		container.end()
	);
}

/// @brief Removes null QPointer values from a map container.
/// @tparam K Key type.
/// @tparam T QObject type pointed to.
/// @param[in,out] container Map from key to QPointer<T> to clean in-place.
template <typename K, typename T>
void cleanupQPointers(std::map<K, QPointer<T>>& container) {
	for(auto it = container.begin(); it != container.end();) {
		if(it->second.isNull())
			it = container.erase(it);
		else
			++it;
	}
}

/// @brief Returns an image in the desired format, converting if necessary.
/// @param[in]     format  The target QImage::Format.
/// @param[in]     base    The source image.
/// @param[in,out] storage Temporary storage for conversion.
/// @return Pointer to either the original or converted image.
QImage const* imageWithFormat(QImage::Format format, QImage const& base, QImage& storage);

/// @brief Tests intersection of a ray with a triangle (Möller–Trumbore algorithm).
/// @param[in] rayOrigin    Origin of the ray in world coordinates.
/// @param[in] rayDirection Normalized direction vector of the ray.
/// @param[in] triangle0    First vertex of the triangle.
/// @param[in] triangle1    Second vertex of the triangle.
/// @param[in] triangle2    Third vertex of the triangle.
/// @param[out] hitPoint    Output hit point if intersection occurs.
/// @param[in] tolerance    Intersection tolerance for parallelism.
/// @return True if the ray intersects the triangle; false otherwise.
bool intersectTriangle(
	QVector3D const& rayOrigin, QVector3D const& rayDirection, QVector3D const& triangle0, QVector3D const& triangle1, QVector3D const& triangle2, QVector3D& hitPoint,
	float const tolerance = 1e-6
);

}

#endif // A3DCOMMON_H
