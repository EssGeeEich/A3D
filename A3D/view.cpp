#include "view.h"
#include "scene.h"
#include <QTimer>
#include <QKeyEvent>

namespace A3D {

View::View(QWidget* parent)
	: QOpenGLWidget{ parent },
	  m_initDoneGL(false),
	  m_renderer(nullptr),
	  m_scene(nullptr) {
	log(LC_Debug, u"Constructor: View");
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_AlwaysStackOnTop);

	QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
	fmt.setSamples(4);
	fmt.setDepthBufferSize(24);
	fmt.setVersion(3, 3);
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	fmt.setAlphaBufferSize(8);
	fmt.setSwapInterval(1);
	fmt.setSwapBehavior(QSurfaceFormat::TripleBuffer);
	setFormat(fmt);
}

View::~View() {
	log(LC_Debug, u"Destructor: View");
}

Camera& View::camera() {
	return m_camera;
}

Camera const& View::camera() const {
	return m_camera;
}

Renderer* View::renderer() {
	return m_renderer.get();
}

Scene* View::scene() const {
	return m_scene;
}

void View::setScene(Scene* newScene) {
	if(m_sceneConnection) {
		disconnect(m_sceneConnection);
		m_sceneConnection = QMetaObject::Connection();
	}

	m_scene = newScene;

	if(m_scene)
		m_sceneConnection = connect(m_scene, &Scene::sceneUpdated, this, &View::sceneChanged);
}

ViewController* View::controller() const {
	return m_viewController;
}

void View::setController(ViewController* viewController) {
	if(m_viewController)
		removeEventFilter(m_viewController);

	m_viewController = viewController;

	if(m_viewController)
		installEventFilter(m_viewController);
}

bool View::isAutoRefreshEnabled() const {
	return m_refreshTimer.isValid();
}

void View::setAutoRefreshEnabled(bool enabled) {
	if(m_refreshTimer.isValid() == enabled)
		return;

	if(enabled)
		m_refreshTimer.restart();
	else
		m_refreshTimer.invalidate();
}

QSize View::minimumSizeHint() const {
	return QSize(50, 50);
}

QSize View::sizeHint() const {
	return QSize(240, 160);
}

void View::initializeGL() {
	if(m_initDoneGL)
		return;

	if(!initializeOpenGLFunctions()) {
		log(LC_Debug, u"initializeOpenGLFunctions failed.");
		log(LC_Fatal, u"OpenGL 3.3 Core is required to run this software.");
		return;
	}

	m_renderer   = std::make_unique<RendererOGL>(context(), this);
	m_initDoneGL = true;
}

void View::resizeGL(int w, int h) {
	if(!m_initDoneGL)
		return;

	// Update the Camera
	if(m_camera.projectionMode() == Camera::PM_PERSPECTIVE) {
		float fWidth  = static_cast<float>(w);
		float fHeight = static_cast<float>(h);
		if(fWidth <= 1.f)
			fWidth = 1.f;
		if(fHeight <= 1.f)
			fHeight = 1.f;
		m_camera.setPerspective(45.f, fWidth / fHeight);
	}
}

void View::sceneChanged() {
	update();
}

void View::paintGL() {
	if(!m_initDoneGL)
		return;

	m_renderer->DrawAll(m_scene, camera());
	m_renderer->CleanupRenderCache();

	emit frameRendered();
}

void View::updateView() {
	std::chrono::milliseconds t = std::chrono::milliseconds(m_refreshTimer.restart());
	bool hasChanges             = m_viewController->update(t);
	Q_UNUSED(hasChanges)

	this->update(); // Redraw
}

QPointF View::toNormalizedPoint(QPointF viewLocalCoordinate) const {
	return QPointF(viewLocalCoordinate.x() / static_cast<float>(width()), viewLocalCoordinate.y() / static_cast<float>(height()));
}

}
