/// @file
/// @brief Defines an interface for controllers that update a Scene over time.

#ifndef A3DSCENECONTROLLER_H
#define A3DSCENECONTROLLER_H

#include "common.h"
#include <QObject>

namespace A3D {

class Scene;

/// @brief Base class for objects that control and update a Scene.
class SceneController : public QObject {
	Q_OBJECT
public:
	/// @brief Constructs a SceneController for the given Scene.
	/// @param[in] scene Pointer to the Scene to control.
	explicit SceneController(Scene* scene);

	/// @brief Updates the Scene for a simulation tick.
	/// @param[in] deltaT Time elapsed since the last update.
	/// @return true if the Scene was modified.
	virtual bool update(std::chrono::milliseconds deltaT) = 0;

protected:
	/// @brief Retrieves the controlled Scene.
	/// @return Pointer to the Scene.
	Scene* scene() const;

private:
	QPointer<Scene> m_scene; ///< Weak pointer to the associated Scene
};

}

#endif // A3DSCENECONTROLLER_H
