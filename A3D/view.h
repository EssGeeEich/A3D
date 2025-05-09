/// @file
/// @brief OpenGL widget for rendering and interacting with an A3D scene.

#ifndef A3DVIEW_H
#define A3DVIEW_H

#include "common.h"
#include <QOpenGLWidget>
#include "camera.h"
#include "rendererogl.h"
#include "scene.h"
#include "viewcontroller.h"

namespace A3D {

/// @brief OpenGL widget that displays a 3D scene and manages its camera, controller, and rendering.
///
/// Manages scene graph, camera control, view updating, and runtime settings.
class View : public QOpenGLWidget, public CoreGLFunctions {
	Q_OBJECT
public:
	/// @brief Constructs the View with an optional parent widget.
	/// @param parent The Qt parent widget.
	explicit View(QWidget* parent = nullptr);

	/// @brief Destroys the View and releases GL resources.
	~View();

	/// @brief Gets the current camera (const).
	/// @return Constant reference to the internal Camera.
	Camera const& camera() const;

	/// @brief Gets the current camera (modifiable).
	/// @return Reference to the internal Camera for modification.
	Camera& camera();

	/// @brief Retrieves the current scene.
	/// @return Pointer to the active Scene, or nullptr if none is set.
	Scene* scene() const;

	/// @brief Sets the scene to be rendered.
	/// @param[in] scene Pointer to the Scene object.
	void setScene(Scene* scene);

	/// @brief Gets the current view controller.
	/// @return Pointer to the ViewController, or nullptr if none is set.
	ViewController* controller() const;

	/// @brief Sets the view controller responsible for updating the view.
	/// @param[in] controller Pointer to the ViewController.
	void setController(ViewController* controller);

	/// @brief Checks if automatic view refreshing is enabled.
	/// @return True if auto-refresh is active; false otherwise.
	bool isAutoRefreshEnabled() const;

	/// @brief Enables or disables automatic view refreshing.
	/// @param enabled True to start auto-refresh; false to stop.
	void setAutoRefreshEnabled(bool enabled);

	/// @brief Gets the renderer interface.
	/// @return Pointer to the Renderer used for drawing.
	Renderer* renderer();

	/// @brief Provides a recommended minimum size for the widget.
	/// @return Minimum size hint for layout purposes.
	QSize minimumSizeHint() const override;

	/// @brief Provides a recommended size for the widget.
	/// @return Preferred size hint for layout purposes.
	QSize sizeHint() const override;

	/// @brief Converts a point in widget-local coordinates to normalized device coordinates.
	/// @param viewLocalCoordinate Point in widget-local pixel coordinates.
	/// @return Point in normalized coordinates ([0,1] × [0,1]).
	/// @remark Use mapFromGlobal to convert global coordinates to local
	QPointF toNormalizedPoint(QPointF viewLocalCoordinate) const;

public slots:
	/// @brief Slot to trigger a view update and repaint.
	void updateView();

signals:
	/// @brief Emitted after a frame has been rendered.
	void frameRendered();

protected:
	/// @brief Initializes OpenGL state and resources.
	void initializeGL() override;

	/// @brief Handles resizing of the OpenGL viewport.
	/// @param[in] w New width of the widget.
	/// @param[in] h New height of the widget.
	void resizeGL(int w, int h) override;

	/// @brief Renders the current frame.
	void paintGL() override;

private slots:
	/// @brief Called when the scene is changed, causes a view redraw.
	void sceneChanged();

private:
	bool m_initDoneGL;                         ///< True if GL initialization is complete.
	QElapsedTimer m_refreshTimer;              ///< Timer tracking elapsed time for auto-refresh scheduling.
	QPointer<ViewController> m_viewController; ///< Weak pointer to the view controller.
	Camera m_camera;                           ///< Camera used to view the scene.
	std::unique_ptr<RendererOGL> m_renderer;   ///< OpenGL renderer instance.
	QPointer<Scene> m_scene;                   ///< Weak pointer to the current scene.
	QMetaObject::Connection m_sceneConnection; ///< Connection handle for scene change signals.
};

}

#endif // A3DVIEW_H
