/// @file
/// @brief Declares the KeyboardCameraController for keyboard-driven camera navigation.
/// @details Maps keyboard input to movement, looking, and speed-modification actions.

#ifndef A3DKEYBOARDCAMERACONTROLLER_H
#define A3DKEYBOARDCAMERACONTROLLER_H

#include "common.h"
#include "viewcontroller.h"
#include <QObject>
#include <set>

namespace A3D {

/// @brief Controller handling keyboard input to manipulate a camera view.
/// @details Supports customizable key bindings for movement, looking, and speed adjustments.
class KeyboardCameraController : public ViewController {
	Q_OBJECT

public:
	/// @brief Enumerates all possible keyboard-driven camera actions.
	enum Action {
		ACT_MOVE_FORWARD,  ///< Move the camera forward
		ACT_MOVE_BACKWARD, ///< Move the camera backward
		ACT_MOVE_LEFT,     ///< Strafe the camera left
		ACT_MOVE_RIGHT,    ///< Strafe the camera right
		ACT_MOVE_UPWARD,   ///< Move the camera upward
		ACT_MOVE_DOWNWARD, ///< Move the camera downward

		ACT_MOVE_PRECISE, ///< Apply precise (slower) movement modifier
		ACT_MOVE_QUICK,   ///< Apply quick (faster) movement modifier

		ACT_LOOK_LEFT,      ///< Rotate the camera to look left
		ACT_LOOK_RIGHT,     ///< Rotate the camera to look right
		ACT_LOOK_UP,        ///< Rotate the camera to look up
		ACT_LOOK_DOWN,      ///< Rotate the camera to look down
		ACT_LOOK_TILTLEFT,  ///< Tilt the camera left
		ACT_LOOK_TILTRIGHT, ///< Tilt the camera right

		ACT_LOOK_HOME, ///< Reset camera to home position

		ACT_COUNT, ///< Total number of actions
	};

	/// @brief Constructs a KeyboardCameraController.
	/// @param[in] view Pointer to the View to control.
	explicit KeyboardCameraController(View* view);

	/// @brief Binds a key to a camera action.
	/// @param[in] key The Qt::Key to bind.
	/// @param[in] action The action to invoke when the key is pressed.
	void addKeyBinding(Qt::Key key, Action action);

	/// @brief Sets multiple key-to-action bindings.
	/// @param[in] bindings Map of Qt::Key to Action pairs.
	void setKeyBindings(std::map<Qt::Key, Action> bindings);

	/// @brief Sets the precise movement speed multiplier.
	/// @param[in] factor Factor to apply when precise movement is active.
	void setPreciseMovementFactor(float factor = 0.50f);

	/// @brief Sets the quick movement speed multiplier.
	/// @param[in] factor Factor to apply when quick movement is active.
	void setQuickMovementFactor(float factor = 2.0f);

	/// @brief Sets the base movement speed vector.
	/// @param[in] speed Base movement speed along X, Y, Z axes.
	void setBaseMovementSpeed(QVector3D speed = QVector3D(1.f, 1.f, 1.f));

	/// @brief Sets the base rotation speed vector.
	/// @param[in] speed Base rotation speed around each axis.
	void setBaseRotationSpeed(QVector3D speed = QVector3D(1.f, 1.f, 1.f));

	/// @brief Sets the camera's home position.
	/// @param[in] position World-space position to return to on "home" action.
	void setHomePosition(QVector3D position);

	/// @brief Updates controller state and applies camera motion.
	/// @param[in] deltaT Time elapsed since last update in milliseconds.
	/// @return true if camera was moved or rotated; false otherwise.
	virtual bool update(std::chrono::milliseconds deltaT) override;

	/// @brief Event filter for capturing key events.
	/// @param[in] obj The object receiving the event.
	/// @param[in] event The event to process.
	/// @return true if event was handled; false otherwise.
	virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
	/// @brief Evaluates current key statuses to update action flags.
	void updateActions();

	bool m_actions[ACT_COUNT];               ///< Active state of each action
	std::map<Qt::Key, Action> m_keyBindings; ///< Map of key bindings to actions
	std::map<Qt::Key, bool> m_keyStatus;     ///< Current pressed state of keys

	QVector3D m_movementBaseSpeed; ///< Base speed for translations
	float m_movementPreciseFactor; ///< Precise movement multiplier
	float m_movementQuickFactor;   ///< Quick movement multiplier

	QVector3D m_rotationBaseSpeed; ///< Base speed for rotations
	QVector3D m_homePosition;      ///< Home position for camera reset
};

}

#endif // A3DKEYBOARDCAMERACONTROLLER_H
