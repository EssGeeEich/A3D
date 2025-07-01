#ifndef _CALIBRATIONWIDGET_HPP_
#define _CALIBRATIONWIDGET_HPP_

#include <QWidget>
#include <QTimer>
#include "../../A3D/scene.h"
#include "../../A3D/view.h"
#include "../../A3D/surfacechartentity.h"
#include "../../A3D/keyboardcameracontroller.h"
#include "../../A3D/charteditorcontroller.h"

// This class is meant to be a wall between other projects and A3D
// Data enters and settings enter and exit through here
// The sanity of the input is checked here
class ChartWidget : public QWidget {
	Q_OBJECT
public:
    ChartWidget(QWidget* parent = nullptr);

	A3D::SurfaceChartEntity::RenderVariants renderVariants() const;
	void setRenderVariants(A3D::SurfaceChartEntity::RenderVariants);
	QColor markerColor() const;
	void setMarkerColor(QColor);
    QColor worldColor() const;
    void setWorldColor(QColor);

    A3D::MapChart3D map() const;
    void stop();
    void restart();
    void setKeyBindings(std::map<Qt::Key, A3D::KeyboardCameraController::Action>);
    std::map<A3D::KeyboardCameraController::Action, Qt::Key> getKeyBindings();

	void setName(QString name);
	QString name() const;

    //A3D::SurfaceChartEntity * surfaceChart() const;
public slots:

    void setMap(A3D::MapChart3D const&);

private:
    A3D::Scene* m_scene;
    A3D::SurfaceChartEntity* m_surfaceChart;
    A3D::View* m_view;
    A3D::KeyboardCameraController* m_keyCamController;
    A3D::ChartEditorController* m_chartEditorController;
    A3D::MapChart3D m_map;
    A3D::Cubemap* m_cubemap;

    QColor m_worldColor;
    QTimer m_timer;
};

#endif
