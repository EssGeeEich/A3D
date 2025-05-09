#ifndef A3DSURFACECHARTENTITY_H
#define A3DSURFACECHARTENTITY_H

#include "linegroup.h"
#include "entity.h"
#include "chart.h"
#include "textbillboardentity.h"
#include <QObject>

namespace A3D {

class SurfaceChartEntity : public Entity {
	Q_OBJECT
public:
	SurfaceChartEntity(Entity* parent = nullptr);

	void setChart(MapChart3D map);
	MapChart3D const& mapChart() const;

	// Range: 0-1 -> 0-100%
	void setEnumStripThickness(float xThickness, float zThickness);
	float enumStripThicknessX() const;
	float enumStripThicknessZ() const;

	// X component means distance of labels on the X axis, and so on.
	void setLabelDistances(QVector3D distance_x_y_z);
	QVector3D labelDistances() const;

	void setMarker(QVector2D marker);
	void unsetMarker();
	bool hasMarker() const;
	QVector2D marker() const;
	void setMarkerColor(QColor markerColor);
	QColor markerColor() const;

private:
	void updateSurfaceMesh();

	void updateIndicatorLines();
	void updateIndicatorLines(Axis3D onlyUpdateAxis);

	void updateIndicatorLabels();
	void updateIndicatorLabels(Axis3D onlyUpdateAxis);

	MapChart3D m_mapChart;
	float m_enumStripThicknessX;
	float m_enumStripThicknessZ;

	QVector3D m_labelDistances;

	QPointer<LineGroup> m_indicatorLines[AXIS_COUNT][CHAXIND_COUNT];
	std::vector<QPointer<TextBillboardEntity>> m_indicatorLabels[AXIS_COUNT];

	bool m_hasMarker;
	QColor m_markerColor;
	QVector2D m_marker;
	LineGroup* m_markerLineGroup;
};

}

#endif // SURFACECHARTENTITY_H
