#include "camera.h"

namespace A3D {

inline void normalizeAngleVector(QVector3D& angle) {
	// float const fMaxVerticalAngle = 89.f;
	// if(angle.x() > fMaxVerticalAngle)
	// 		angle.setX(fMaxVerticalAngle);
	// else if(angle.x() < -fMaxVerticalAngle)
	// 		angle.setX(-fMaxVerticalAngle);

	angle.setX(fmodf(angle.x(), 360.f));
	if(angle.x() < 0.f)
		angle.setX(angle.x() + 360.f);

	angle.setY(fmodf(angle.y(), 360.f));
	if(angle.y() < 0.f)
		angle.setY(angle.y() + 360.f);

	angle.setZ(fmodf(angle.z(), 360.f));
	if(angle.z() < 0.f)
		angle.setZ(angle.z() + 360.f);
}

Camera::Camera()
	: m_viewMatrixIsDirty(true),
	  m_projMatrixIsDirty(true),
	  m_projectionMode(PM_PERSPECTIVE),
	  m_nearPlane(0.1f),
	  m_farPlane(1000.f),
	  m_perspVerticalFOV(45.f),
	  m_perspAspectRatio(16.f / 9.f) {
	log(LC_Debug, u"Constructor: Camera");
}

Camera::~Camera() {
	log(LC_Debug, u"Destructor: Camera");
}

QVector3D const& Camera::position() const {
	return m_position;
}
void Camera::setPosition(QVector3D const& pos) {
	if(m_position == pos)
		return;
	m_position          = pos;
	m_viewMatrixIsDirty = true;
}
QMatrix4x4 Camera::orientation() const {
	QMatrix4x4 matrix;
	matrix.rotate(m_angle.x(), QVector3D(1.f, 0.f, 0.f));
	matrix.rotate(m_angle.z(), QVector3D(0.f, 0.f, 1.f));
	matrix.rotate(m_angle.y(), QVector3D(0.f, 1.f, 0.f));
	return matrix;
}
QVector3D const& Camera::angle() const {
	return m_angle;
}
void Camera::setAngle(QVector3D angle) {
	normalizeAngleVector(angle);

	if(m_angle == angle)
		return;
	m_angle             = angle;
	m_viewMatrixIsDirty = true;
}
void Camera::setOrientationTarget(QVector3D const& target) {
	if(target == m_position)
		return;
	QVector3D const direction = (target - m_position);
	float const length        = direction.length();

	float const pitch = qAsin(-direction.y() / length);
	float const yaw   = qAtan2(direction.x(), -direction.z());

	QVector3D newAngle(qRadiansToDegrees(pitch), qRadiansToDegrees(yaw), 0.f);
	normalizeAngleVector(newAngle);

	if(newAngle == m_angle)
		return;
	m_angle             = newAngle;
	m_viewMatrixIsDirty = true;
}
void Camera::offsetOrientation(QVector3D const& orientation) {
	QVector3D newAngle = m_angle + orientation;
	normalizeAngleVector(newAngle);
	if(newAngle == m_angle)
		return;
	m_angle             = newAngle;
	m_viewMatrixIsDirty = true;
}

QVector3D Camera::forward() const {
	return QVector3D(orientation().inverted() * QVector4D(0.f, 0.f, -1.f, 1.f));
}

QVector3D Camera::right() const {
	return QVector3D(orientation().inverted() * QVector4D(1.f, 0.f, 0.f, 1.f));
}

QVector3D Camera::up() const {
	return QVector3D(orientation().inverted() * QVector4D(0.f, 1.f, 0.f, 1.f));
}

QMatrix4x4 const& Camera::getView() const {
	if(m_viewMatrixIsDirty) {
		m_viewMatrixIsDirty = false;
		m_viewMatrix        = orientation();
		m_viewMatrix.translate(-m_position);
	}
	return m_viewMatrix;
}

Camera::ProjectionMode Camera::projectionMode() const {
	return m_projectionMode;
}

float Camera::nearPlane() const {
	return m_nearPlane;
}
void Camera::setNearPlane(float nearPlane) {
	if(m_nearPlane == nearPlane)
		return;
	m_nearPlane         = nearPlane;
	m_projMatrixIsDirty = true;
}

float Camera::farPlane() const {
	return m_farPlane;
}
void Camera::setFarPlane(float farPlane) {
	if(m_farPlane == farPlane)
		return;
	m_farPlane          = farPlane;
	m_projMatrixIsDirty = true;
}

void Camera::setOrthogonal(QRectF const& rect) {
	if(m_projectionMode == PM_ORTHOGONAL && m_orthoView == rect)
		return;
	m_projectionMode    = PM_ORTHOGONAL;
	m_orthoView         = rect;
	m_projMatrixIsDirty = true;
}
void Camera::setPerspective(float verticalFOV, float aspectRatio) {
	if(m_projectionMode == PM_PERSPECTIVE && m_perspVerticalFOV == verticalFOV && m_perspAspectRatio == aspectRatio)
		return;

	m_projectionMode    = PM_PERSPECTIVE;
	m_perspVerticalFOV  = verticalFOV;
	m_perspAspectRatio  = aspectRatio;
	m_projMatrixIsDirty = true;
}

QMatrix4x4 const& Camera::getProjection() const {
	if(m_projMatrixIsDirty) {
		m_projMatrixIsDirty = false;
		m_projMatrix.setToIdentity();

		switch(m_projectionMode) {
		case PM_PERSPECTIVE:
			m_projMatrix.perspective(m_perspVerticalFOV, m_perspAspectRatio, m_nearPlane, m_farPlane);
			break;
		case PM_ORTHOGONAL:
			m_projMatrix.ortho(m_orthoView.left(), m_orthoView.right(), m_orthoView.bottom(), m_orthoView.top(), m_nearPlane, m_farPlane);
			break;
		}
	}
	return m_projMatrix;
}

QVector3D Camera::unprojectPoint(QPointF xy, float z) const {
	QMatrix4x4 invProj = getProjection().inverted();
	QMatrix4x4 invView = getView().inverted();

	QVector4D point = QVector4D((xy.x() * 2.f) - 1.f, (xy.y() * -2.f) + 1.f, z, 1.f);

	QVector4D eye = invProj * point;
	eye.setW(1.f);

	return (invView * eye).toVector3D();
}

}
