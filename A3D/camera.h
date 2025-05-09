/// @file
/// @brief Declaration of the A3D::Camera class for handling 3D camera view and projection.
///
/// This file defines the Camera class which encapsulates the view and projection
/// matrices, camera positioning, orientation, and projection parameters.

#ifndef A3DCAMERAVIEW_H
#define A3DCAMERAVIEW_H

#include "common.h"

namespace A3D {

/// @brief Represents a camera in 3D space with view and projection transformations.
///
/// The Camera class manages positioning and orientation of a virtual camera,
/// and constructs view and projection matrices for rendering scenes.
class Camera {
public:
	/// @brief Defines the projection mode of the camera.
	enum ProjectionMode {
		PM_PERSPECTIVE, ///< Perspective projection mode.
		PM_ORTHOGONAL,  ///< Orthographic projection mode.
	};

	/// @brief Constructs a default Camera.
	///
	/// Initializes the camera at the origin with no rotation,
	/// default near/far planes, and perspective projection.
	Camera();

	/// @brief Destroys the Camera.
	~Camera();

	/// @name View Matrix Functions
	/// @{

	/// @brief Retrieves the position of the camera in world coordinates.
	/// @return Constant reference to the camera position vector.
	QVector3D const& position() const;

	/// @brief Sets the camera position.
	/// @param[in] pos New position vector for the camera.
	void setPosition(QVector3D const& pos);

	/// @brief Offsets the camera position by a given vector.
	/// @param[in] pos Offset vector to add to the current position.
	inline void offsetPosition(QVector3D const& pos) { setPosition(position() + pos); }

	/// @brief Returns the orientation matrix (rotation only) of the camera.
	/// @return View orientation matrix.
	QMatrix4x4 orientation() const;

	/// @brief Retrieves the Euler angles (pitch, yaw, roll) of the camera.
	/// @return Constant reference to the XYZ rotation angles vector.
	QVector3D const& angle() const;

	/// @brief Sets the Euler angles (pitch, yaw, roll) of the camera.
	/// @param[in] angle New rotation angles vector (XYZ).
	void setAngle(QVector3D angle);

	/// @brief Orients the camera to look at a target point.
	///
	/// Assumes the world's up vector is (0,1,0).
	/// Cancels any Z component of the up direction.
	/// @param[in] target Target point in world coordinates.
	void setOrientationTarget(QVector3D const& target);

	/// @brief Offsets the camera orientation by given Euler angles.
	/// @param[in] orientation Euler angles to add to the current orientation.
	void offsetOrientation(QVector3D const& orientation);

	/// @brief Returns the forward direction vector of the camera.
	/// @return Forward direction vector.
	QVector3D forward() const;

	/// @brief Returns the right direction vector of the camera.
	/// @return Right direction vector.
	QVector3D right() const;

	/// @brief Returns the up direction vector of the camera.
	/// @return Up direction vector.
	QVector3D up() const;

	/// @brief Retrieves the view matrix of the camera.
	/// @return Constant reference to the view matrix.
	QMatrix4x4 const& getView() const;

	/// @}

	/// @name Projection Matrix Functions
	/// @{

	/// @brief Gets the current projection mode.
	/// @return Current ProjectionMode (perspective or orthogonal).
	ProjectionMode projectionMode() const;

	/// @brief Retrieves the near clipping plane distance.
	/// @return Near plane distance.
	float nearPlane() const;

	/// @brief Sets the near clipping plane distance.
	/// @param[in] nearPlane New near plane distance.
	void setNearPlane(float nearPlane);

	/// @brief Retrieves the far clipping plane distance.
	/// @return Far plane distance.
	float farPlane() const;

	/// @brief Sets the far clipping plane distance.
	/// @param[in] farPlane New far plane distance.
	void setFarPlane(float farPlane);

	/// @brief Sets both near and far clipping planes.
	/// @param[in] nearPlane New near plane distance.
	/// @param[in] farPlane New far plane distance.
	inline void setPlanes(float nearPlane, float farPlane) {
		setNearPlane(nearPlane);
		setFarPlane(farPlane);
	}

	/// @brief Configures the camera for orthographic projection.
	///
	/// Maps display coordinates to the specified rectangle.
	/// @param[in] rect Rectangle defining the orthographic view volume.
	void setOrthogonal(QRectF const& rect);

	/// @brief Configures the camera for perspective projection.
	/// @param[in] verticalFOV Vertical field of view angle in degrees.
	/// @param[in] aspectRatio Aspect ratio (width/height) of the view.
	void setPerspective(float verticalFOV, float aspectRatio);

	/// @brief Retrieves the projection matrix.
	/// @return Constant reference to the projection matrix.
	QMatrix4x4 const& getProjection() const;

	/// @}

	/// @brief Unprojects a 2D screen point to a 3D world coordinate.
	/// @param[in] xy 2D point on the near plane in screen coordinates.
	/// @param[in] z  Depth value in normalized device coordinates.
	/// @return Unprojected 3D point in world coordinates.
	QVector3D unprojectPoint(QPointF xy, float z = 1.f) const;

private:
	mutable bool m_viewMatrixIsDirty; ///< Flag to indicate if view matrix needs updating.
	mutable QMatrix4x4 m_viewMatrix;  ///< Cached view matrix.

	QVector3D m_position; ///< Camera position vector.
	QVector3D m_angle;    ///< Camera rotation angles (Euler).

	mutable bool m_projMatrixIsDirty; ///< Flag to indicate if projection matrix needs updating.
	mutable QMatrix4x4 m_projMatrix;  ///< Cached projection matrix.
	ProjectionMode m_projectionMode;  ///< Current projection mode.
	float m_nearPlane;                ///< Near clip plane distance.
	float m_farPlane;                 ///< Far clip plane distance.
	QRectF m_orthoView;               ///< Orthographic view rectangle.
	float m_perspVerticalFOV;         ///< Perspective vertical field of view.
	float m_perspAspectRatio;         ///< Perspective aspect ratio.
};

}

#endif // A3DCAMERAVIEW_H
