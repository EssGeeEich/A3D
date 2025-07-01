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
	enum RenderVariant {
		RV_NONE                   = 0,
		RV_HISTOGRAM_ENUMERATIONS = 0x1,
	};
	Q_DECLARE_FLAGS(RenderVariants, RenderVariant)

	SurfaceChartEntity(Entity* parent = nullptr);

	// Store the name in this class in case it will be displayed one day
	void setName(QString);
	QString name() const;

	void setChart(MapChart3D map);
	MapChart3D const& mapChart() const;
	MapChart3D& mapChart();

	void setRenderVariants(RenderVariants);
	RenderVariants renderVariants() const;

	// (if editFilterMask & charteditorcontroller.editFilterMask) != 0 then it's editable
	void setEditFilterMask(std::uint32_t mask);
	std::uint32_t editFilterMask() const;

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

protected:
	/// @brief Updates Entity logic for a tick.
	/// @param[in] deltaMs Delta time in milliseconds.
	/// @return True if the Entity state changed and matrix must be updated.
	virtual bool updateEntity(std::chrono::milliseconds deltaMs) override;

signals:
	void markerMoved();

private:
	void updateSurfaceMesh();

	void updateIndicatorLines();
	void updateIndicatorLines(Axis3D onlyUpdateAxis);

	void updateIndicatorLabels();
	void updateIndicatorLabels(Axis3D onlyUpdateAxis);

	RenderVariants m_renderVariants;
	MapChart3D m_mapChart;
	float m_enumStripThicknessX;
	float m_enumStripThicknessZ;

	QVector3D m_labelDistances;

	QPointer<LineGroup> m_indicatorLines[AXIS_COUNT][CHAXIND_COUNT];
	std::vector<QPointer<TextBillboardEntity>> m_indicatorLabels[AXIS_COUNT];

    size_t m_chartSurfaceSyncRevision;
    size_t m_chartIndicatorsSyncRevision;
    size_t m_chartLabelsSyncRevision;
	bool m_hasMarker;
	QColor m_markerColor;
	QVector2D m_marker;
	LineGroup* m_markerLineGroup;
	std::uint32_t m_editFilterMask;
	QString m_name;
};

}

#endif // SURFACECHARTENTITY_H
