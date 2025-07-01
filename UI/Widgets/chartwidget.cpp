#include "chartwidget.h"
#include "calibrationwidget.h"
#include "../../A3D/cubemap.h"

ChartWidget::ChartWidget(QWidget* parent)
	: QWidget(parent) {
    setLayout(new QVBoxLayout);

	m_scene = new A3D::Scene(this);

	A3D::PointLightInfo& light = m_scene->getOrCreateLight(0);
	light.position             = QVector3D(0.5f, 10.f, 0.5f);
	light.color                = QVector4D(1.f, 1.f, 1.f, 500.f);

	m_view = new A3D::View(this);
	m_view->camera().setPosition(QVector3D(2.f, 2.f, 2.f));
	m_view->camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));
	m_view->setScene(m_scene);

	m_cubemap = new A3D::Cubemap(m_scene->resourceManager());
	setWorldColor(Qt::black);

	if(m_cubemap->isValid())
		m_scene->setSkybox(m_cubemap);

	m_surfaceChart = m_scene->emplaceChildEntity<A3D::SurfaceChartEntity>();
	m_surfaceChart->setPosition(QVector3D(0.f, 0.f, 0.f));
	m_surfaceChart->setLabelDistances(QVector3D(0.1f, 0.1f, 0.1f));

	m_chartEditorController = new A3D::ChartEditorController(m_view);

    m_keyCamController = new A3D::KeyboardCameraController(m_view);
    m_keyCamController->setBaseMovementSpeed(QVector3D(9.f, 9.f, 9.f));
    m_keyCamController->addKeyBinding(Qt::Key_Space, A3D::KeyboardCameraController::ACT_MOVE_UPWARD);
    m_keyCamController->addKeyBinding(Qt::Key_Shift, A3D::KeyboardCameraController::ACT_MOVE_DOWNWARD);

    m_keyCamController->addKeyBinding(Qt::Key_Left, A3D::KeyboardCameraController::ACT_ROTATE_LEFT_AROUND_HOME);
    m_keyCamController->addKeyBinding(Qt::Key_Right, A3D::KeyboardCameraController::ACT_ROTATE_RIGHT_AROUND_HOME);
    m_keyCamController->addKeyBinding(Qt::Key_Up, A3D::KeyboardCameraController::ACT_ROTATE_UPWARD_AROUND_HOME);
    m_keyCamController->addKeyBinding(Qt::Key_Down, A3D::KeyboardCameraController::ACT_ROTATE_DOWNWARD_AROUND_HOME);

    // Setto la home del keyCamController al centro del grafico
    QMatrix4x4 mat   = m_view->calculateFullMatrix(m_surfaceChart, m_surfaceChart->model(), m_surfaceChart->model()->getGroup("Chart"));
    QVector3D center = QVector3D(0.5f, 0.5f, 0.5f);
    center           = mat.map(center);
    m_keyCamController->setHomePosition(center);



    if(m_view->format().swapInterval() > 0)
        m_timer.setInterval(0);
    else
        m_timer.setInterval(20);
    m_timer.start();

    QObject::connect(&m_timer, &QTimer::timeout, m_view, &A3D::View::updateView);
    QObject::connect(&m_timer, &QTimer::timeout, m_scene, &A3D::Scene::updateScene);

    m_view->setAutoRefreshEnabled(true);

    m_scene->run();

    layout()->addWidget(m_view);
}

A3D::SurfaceChartEntity::RenderVariants ChartWidget::renderVariants() const {
	return m_surfaceChart->renderVariants();
}

void ChartWidget::setRenderVariants(A3D::SurfaceChartEntity::RenderVariants renderVariants) {
	m_surfaceChart->setRenderVariants(renderVariants);
}

QColor ChartWidget::markerColor() const {
	return m_surfaceChart->markerColor();
}

void ChartWidget::setMarkerColor(QColor color) {
	if(color.isValid())
		m_surfaceChart->setMarkerColor(color);
}

QColor ChartWidget::worldColor() const {
	return m_worldColor;
}

std::map<A3D::KeyboardCameraController::Action, Qt::Key> ChartWidget::getKeyBindings() {
    return m_keyCamController->getKeyBindings();
}

void ChartWidget::setWorldColor(QColor color) {
    if(color == m_worldColor || !color.isValid())
        return;
    m_worldColor = color;
    m_cubemap->setCubemapFace(A3D::Cubemap::CF_NEG_X, m_worldColor);
    m_cubemap->setCubemapFace(A3D::Cubemap::CF_NEG_Y, m_worldColor);
    m_cubemap->setCubemapFace(A3D::Cubemap::CF_NEG_Z, m_worldColor);
    m_cubemap->setCubemapFace(A3D::Cubemap::CF_POS_X, m_worldColor);
    m_cubemap->setCubemapFace(A3D::Cubemap::CF_POS_Y, m_worldColor);
    m_cubemap->setCubemapFace(A3D::Cubemap::CF_POS_Z, m_worldColor);
    m_cubemap->invalidateCache();
}

void ChartWidget::setKeyBindings(std::map<Qt::Key, A3D::KeyboardCameraController::Action> bindings) {
    m_keyCamController->setKeyBindings(bindings);
}

A3D::MapChart3D ChartWidget::map() const {
    return m_surfaceChart->mapChart();
}

void ChartWidget::setMap(A3D::MapChart3D const& map) {
	if(map.isValid())
		m_surfaceChart->setChart(map);
}

void ChartWidget::stop() {
	m_timer.stop();
}

void ChartWidget::restart() {
	m_timer.start(20);
}

void ChartWidget::setName(QString name)
{
	m_surfaceChart->setName(name);
}

QString ChartWidget::name() const
{
	return m_surfaceChart->name();
}

