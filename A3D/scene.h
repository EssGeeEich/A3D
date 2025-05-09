/// @file
/// @brief Defines the Scene class, managing entities, lights, and controllers.

#ifndef A3DSCENE_H
#define A3DSCENE_H

#include "common.h"
#include <QObject>
#include "entity.h"
#include "resourcemanager.h"
#include "cubemap.h"
#include "scenecontroller.h"

namespace A3D {

/// Information for a point light in the scene.
struct PointLightInfo {
	QVector4D color;    ///< RGBA color, A channel can serve as intensity multiplier
	QVector3D position; ///< World-space position of the light
};

/// @brief Root scene node containing entities, lights, and controllers.
class Scene : public Entity {
	Q_OBJECT
public:
	/// @brief Constructs a Scene with an optional parent.
	/// @param[in] parent Parent QObject, nullptr for no parent.
	explicit Scene(QObject* parent = nullptr);

	/// @brief Destructor.
	~Scene();

	/// @brief Retrieves the resource manager of this scene.
	/// @return Pointer to the ResourceManager.
	ResourceManager* resourceManager();

	/// @brief Retrieves the const resource manager of this scene.
	/// @return Const pointer to the ResourceManager.
	ResourceManager const* resourceManager() const;

	/// @brief Gets or creates a point light by ID.
	/// @param[in] id Unique identifier for the light.
	/// @return Reference to the PointLightInfo.
	PointLightInfo& getOrCreateLight(std::size_t id);

	/// @brief Retrieves a point light by ID.
	/// @param[in] id Unique identifier for the light.
	/// @return Pointer to the PointLightInfo or nullptr if not found.
	PointLightInfo const* getLight(std::size_t id) const;

	/// @brief Gets all point lights in the scene.
	/// @return Const reference to the lights map.
	std::map<std::size_t, PointLightInfo> const& lights() const;

	/// @brief Retrieves the skybox cubemap.
	/// @return Pointer to the Cubemap representing the skybox.
	Cubemap* skybox() const;

	/// @brief Sets the scene's skybox.
	/// @param[in] cubemap Pointer to the Cubemap to use.
	void setSkybox(Cubemap* cubemap);

	/// @brief Adds a SceneController to the scene.
	/// @param[in] controller Pointer to the SceneController.
	void addController(SceneController* controller);

	/// @brief Removes a SceneController from the scene.
	/// @param[in] controller Pointer to the SceneController.
	void removeController(SceneController* controller);

	/// @brief Gets the runtime multiplier for time-based updates.
	/// @return Multiplier applied to elapsed time.
	float runTimeMultiplier() const;

	/// @brief Sets the runtime multiplier for time-based updates.
	/// @param[in] multiplier Factor to apply to elapsed time.
	void setRunTimeMultiplier(float multiplier);

	/// @brief Checks if the scene is currently running (updating).
	/// @return True if running; false otherwise.
	bool isRunning() const;

	/// @brief Sets whether the scene should run (start or stop updates).
	/// @param[in] running True to enable updates; false to disable.
	void setRunning(bool running);

	/// @brief Starts the scene updates.
	inline void run() { setRunning(true); }

	/// @brief Stops the scene updates.
	inline void stop() { setRunning(false); }

signals:
	/// Emitted when the scene is updated (triggered by updateScene()).
	void sceneUpdated();

public slots:
	/// @brief Slot to update the entire scene (shoule be called each user-defined tick).
	void updateScene();

private:
	QElapsedTimer m_sceneRunTimer; ///< Timer tracking scene delta time calculation
	float m_runTimeMultiplier;     ///< Time scaling factor for updates

	std::vector<QPointer<SceneController>> m_sceneControllers; ///< Controllers attached to the scene
	ResourceManager m_resourceManager;                         ///< Manager for scene resources
	std::map<std::size_t, PointLightInfo> m_lights;            ///< Map of point lights by ID

	QPointer<Cubemap> m_skybox; ///< Skybox cubemap
};

}

#endif // A3DSCENE_H
