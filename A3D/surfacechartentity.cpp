#include "surfacechartentity.h"

namespace A3D {

SurfaceChartEntity::SurfaceChartEntity(Entity* parent)
	: Entity(parent),
	  m_renderVariants(RV_NONE),
	  m_enumStripThicknessX(0.8f),
	  m_enumStripThicknessZ(0.8f),
	  m_editFilterMask(0xFFFFFFFF) {

	A3D::Model* m = new A3D::Model(this);
	setModel(m);

	{
		A3D::Group* meshGroup = m->getOrAddGroup("Chart");
		meshGroup->setMaterialProperties(new MaterialProperties);
	}

	for(std::size_t j = 0; j < AXIS_COUNT; ++j) {
		for(std::size_t i = 0; i < CHAXIND_COUNT; ++i) {
			A3D::Group* group      = m->getOrAddGroup(QStringLiteral("Indicators %1 for axis %2").arg(QString::number(i + 1), QString::number(j + 1)));
			LineGroup* lineGroup   = new LineGroup();
			m_indicatorLines[j][i] = lineGroup;
			group->setLineGroup(lineGroup);

			switch(i) {
			case CHAXIND_MAJOR_INDICATOR:
				lineGroup->setThickness(0.01f);
				break;
			case CHAXIND_MINOR_INDICATOR:
				lineGroup->setThickness(0.003f);
				break;
			default:
				break;
			}
		}
	}

	{
		A3D::Group* group = m->getOrAddGroup("Marker");
		group->setLineGroup(m_markerLineGroup = new LineGroup());
		m_markerLineGroup->setContents(A3D::LineGroup::Position3D | A3D::LineGroup::Color4D);
		m_markerLineGroup->setThickness(0.01f);
	}
}

void SurfaceChartEntity::setName(QString name)
{
	m_name = name;
}

QString SurfaceChartEntity::name() const
{
	return m_name;
}

void SurfaceChartEntity::setChart(MapChart3D map) {
	m_mapChart = std::move(map);
	updateSurfaceMesh();
	updateIndicatorLines();
	updateIndicatorLabels();
    m_chartSurfaceSyncRevision    = m_mapChart.surfaceRevision();
    m_chartIndicatorsSyncRevision = m_mapChart.indicatorsRevision();
    m_chartLabelsSyncRevision     = m_mapChart.labelsRevision();
}

MapChart3D const& SurfaceChartEntity::mapChart() const {
	return m_mapChart;
}

MapChart3D& SurfaceChartEntity::mapChart() {
	return m_mapChart;
}

void SurfaceChartEntity::setRenderVariants(RenderVariants variants) {
	if(m_renderVariants != variants) {
		m_renderVariants = variants;
		updateSurfaceMesh();
	}
}

SurfaceChartEntity::RenderVariants SurfaceChartEntity::renderVariants() const {
	return m_renderVariants;
}

void SurfaceChartEntity::setEditFilterMask(std::uint32_t mask) {
	m_editFilterMask = mask;
}

std::uint32_t SurfaceChartEntity::editFilterMask() const {
	return m_editFilterMask;
}

void SurfaceChartEntity::setEnumStripThickness(float xThickness, float zThickness) {
	m_enumStripThicknessX = xThickness;
	m_enumStripThicknessZ = zThickness;

	if(m_mapChart.isValid()) {
		updateSurfaceMesh();
	}
}

float SurfaceChartEntity::enumStripThicknessX() const {
	return m_enumStripThicknessX;
}

float SurfaceChartEntity::enumStripThicknessZ() const {
	return m_enumStripThicknessZ;
}

void SurfaceChartEntity::setLabelDistances(QVector3D distance_x_y_z) {
	m_labelDistances = distance_x_y_z;

	if(m_mapChart.isValid())
		updateIndicatorLabels();
}

QVector3D SurfaceChartEntity::labelDistances() const {
	return m_labelDistances;
}

static void
getVertexFromCoordinate(A3D::Mesh::Vertex& vx, std::vector<float> const& xAxis, std::vector<float> const& zAxis, std::vector<float> const& yData, std::size_t x, std::size_t z) {
	vx.Position3D     = QVector3D(xAxis[x], yData[x + (z * xAxis.size())], zAxis[z]);
	vx.TextureCoord2D = QVector2D(static_cast<float>(x), static_cast<float>(z));
}

static void generateNormal(A3D::Mesh::Vertex& triangleVertexA, A3D::Mesh::Vertex& triangleVertexB, A3D::Mesh::Vertex& triangleVertexC) {
	triangleVertexA.Normal3D = triangleVertexB.Normal3D = triangleVertexC.Normal3D
		= QVector3D::normal(triangleVertexA.Position3D, triangleVertexB.Position3D, triangleVertexC.Position3D);
}

void SurfaceChartEntity::updateSurfaceMesh() {
	if(!model())
		return;

	Group* chartGroup = model()->getOrAddGroup("Chart");
	Mesh* chartMesh   = chartGroup->mesh();

	if(!m_mapChart.isValid()) {
		if(chartMesh) {
			chartMesh->deleteLater();
			chartGroup->setMesh(nullptr);
		}
		return;
	}

	if(!chartMesh) {
		chartMesh = new A3D::Mesh();
		chartMesh->setParent(chartGroup);
		chartGroup->setMesh(chartMesh);
	}

	chartGroup->setMaterial(A3D::Material::standardMaterial(A3D::Material::MapChart3DMaterial));

	std::vector<float> const& xAxisPos     = m_mapChart.normalizedValuesForAxis(AXIS_X);
	std::vector<float> const& zAxisPos     = m_mapChart.normalizedValuesForAxis(AXIS_Z);
	std::vector<float> const& yValueHeight = m_mapChart.normalizedValuesForAxis(AXIS_Y);

	ChartAxisType const xAxisType = m_mapChart.axisData(AXIS_X).type();
	ChartAxisType const zAxisType = m_mapChart.axisData(AXIS_Z).type();

	size_t const xAxisSize = xAxisPos.size();
	size_t const zAxisSize = zAxisPos.size();

	// x*z = 0 should return an empty mesh right here...

	float const calcXEnumStripThickness = (1.f / xAxisSize) * m_enumStripThicknessX;
	float const calcZEnumStripThickness = (1.f / zAxisSize) * m_enumStripThicknessZ;

	chartMesh->setDrawMode(A3D::Mesh::Triangles);

	std::vector<Mesh::Vertex>& vertices = chartMesh->vertices();
	vertices.clear();

	size_t xIterations = xAxisSize - 1;
	if(xAxisType == CHAXIS_ENUMERATED)
		++xIterations;

	size_t zIterations = zAxisSize - 1;
	if(zAxisType == CHAXIS_ENUMERATED)
		++zIterations;

	enum TriangulationType {
		TRI_5_VERTICES,
		TRI_4_VERTICES,
	};

	bool const drawAsHistogram = (m_renderVariants & RV_HISTOGRAM_ENUMERATIONS) == RV_HISTOGRAM_ENUMERATIONS;
	float const yHistogramRoot = 0.f;

	TriangulationType triangulationType = TRI_5_VERTICES;
	if(xAxisType == CHAXIS_ENUMERATED || zAxisType == CHAXIS_ENUMERATED)
		triangulationType = TRI_4_VERTICES;

	std::size_t targetTriangleCount = 0;

	switch(triangulationType) {
	case TRI_4_VERTICES:
		targetTriangleCount = zIterations * xIterations * 2;
		break;
	case TRI_5_VERTICES:
		targetTriangleCount = zIterations * xIterations * 4;
		break;
	}

	if(drawAsHistogram) {
		if(xAxisType == CHAXIS_ENUMERATED || zAxisType == CHAXIS_ENUMERATED)
			targetTriangleCount += 2;
		if(xAxisType == CHAXIS_ENUMERATED)
			targetTriangleCount += 4;
		if(zAxisType == CHAXIS_ENUMERATED)
			targetTriangleCount += 4;
	}

	vertices.reserve(targetTriangleCount * 3);
	bool const swapVertexOrder = (xAxisType == CHAXIS_ENUMERATED) != (zAxisType == CHAXIS_ENUMERATED);

	auto addTriangle = [&](A3D::Mesh::Vertex& a, A3D::Mesh::Vertex& b, A3D::Mesh::Vertex& c) {
		if(swapVertexOrder) {
			generateNormal(b, a, c);
			vertices.push_back(b);
			vertices.push_back(a);
			vertices.push_back(c);
		}
		else {
			generateNormal(a, b, c);
			vertices.push_back(a);
			vertices.push_back(b);
			vertices.push_back(c);
		}
	};

	auto addTriangleWithTexCoordOffset = [&](A3D::Mesh::Vertex a, A3D::Mesh::Vertex b, A3D::Mesh::Vertex c) {
		if(swapVertexOrder) {
			generateNormal(b, a, c);
		}
		else {
			generateNormal(a, b, c);
		}

		a.TextureCoord2D.setX(a.TextureCoord2D.x() - a.Normal3D.x() * 0.1f);
		a.TextureCoord2D.setY(a.TextureCoord2D.y() - a.Normal3D.z() * 0.1f);
		b.TextureCoord2D.setX(b.TextureCoord2D.x() - b.Normal3D.x() * 0.1f);
		b.TextureCoord2D.setY(b.TextureCoord2D.y() - b.Normal3D.z() * 0.1f);
		c.TextureCoord2D.setX(c.TextureCoord2D.x() - c.Normal3D.x() * 0.1f);
		c.TextureCoord2D.setY(c.TextureCoord2D.y() - c.Normal3D.z() * 0.1f);

		if(swapVertexOrder) {
			vertices.push_back(b);
			vertices.push_back(a);
			vertices.push_back(c);
		}
		else {
			vertices.push_back(a);
			vertices.push_back(b);
			vertices.push_back(c);
		}
	};

	for(size_t z = 0; z < zIterations; ++z) {
		for(size_t x = 0; x < xIterations; ++x) {
			A3D::Mesh::Vertex xLow_zLow;   // Top Left
			A3D::Mesh::Vertex xHigh_zLow;  // Top Right
			A3D::Mesh::Vertex xLow_zHigh;  // Bottom Left
			A3D::Mesh::Vertex xHigh_zHigh; // Bottom Right

			size_t xLow_index  = x;
			size_t xHigh_index = x + 1;
			size_t zLow_index  = z;
			size_t zHigh_index = z + 1;

			if(xAxisType == CHAXIS_ENUMERATED)
				--xHigh_index;
			if(zAxisType == CHAXIS_ENUMERATED)
				--zHigh_index;

			getVertexFromCoordinate(xLow_zLow, xAxisPos, zAxisPos, yValueHeight, xLow_index, zLow_index);
			getVertexFromCoordinate(xHigh_zLow, xAxisPos, zAxisPos, yValueHeight, xHigh_index, zLow_index);
			getVertexFromCoordinate(xLow_zHigh, xAxisPos, zAxisPos, yValueHeight, xLow_index, zHigh_index);
			getVertexFromCoordinate(xHigh_zHigh, xAxisPos, zAxisPos, yValueHeight, xHigh_index, zHigh_index);

			if(xAxisType == CHAXIS_ENUMERATED) {
				float const beforeThickness = xLow_zLow.Position3D.x() * calcXEnumStripThickness;
				float const afterThickness  = calcXEnumStripThickness - beforeThickness;

				xLow_zLow.Position3D.setX(xLow_zLow.Position3D.x() - beforeThickness);
				xLow_zHigh.Position3D.setX(xLow_zHigh.Position3D.x() - beforeThickness);
				xHigh_zLow.Position3D.setX(xHigh_zLow.Position3D.x() + afterThickness);
				xHigh_zHigh.Position3D.setX(xHigh_zHigh.Position3D.x() + afterThickness);

				float const xBase = xLow_zLow.TextureCoord2D.x();
				float const xLow  = xBase;
				float const xHigh = xBase + 1.f;

				xLow_zLow.TextureCoord2D.setX(xLow);
				xLow_zHigh.TextureCoord2D.setX(xLow);
				xHigh_zLow.TextureCoord2D.setX(xHigh);
				xHigh_zHigh.TextureCoord2D.setX(xHigh);
			}

			if(zAxisType == CHAXIS_ENUMERATED) {
				float const beforeThickness = xLow_zLow.Position3D.z() * calcZEnumStripThickness;
				float const afterThickness  = calcZEnumStripThickness - beforeThickness;

				xLow_zLow.Position3D.setZ(xLow_zLow.Position3D.z() - beforeThickness);
				xHigh_zLow.Position3D.setZ(xHigh_zLow.Position3D.z() - beforeThickness);
				xLow_zHigh.Position3D.setZ(xLow_zHigh.Position3D.z() + afterThickness);
				xHigh_zHigh.Position3D.setZ(xHigh_zHigh.Position3D.z() + afterThickness);

				float const zBase = xLow_zLow.TextureCoord2D.y();
				float const zLow  = zBase;
				float const zHigh = zBase + 1.f;

				xLow_zLow.TextureCoord2D.setY(zLow);
				xHigh_zLow.TextureCoord2D.setY(zLow);
				xLow_zHigh.TextureCoord2D.setY(zHigh);
				xHigh_zHigh.TextureCoord2D.setY(zHigh);
			}

			if(triangulationType == TRI_4_VERTICES) {
				// X or Z is enumerated: No need to generate the central vertex, the 2-triangle approximation is more than enough.
				addTriangle(xLow_zLow, xLow_zHigh, xHigh_zHigh);
				addTriangle(xLow_zLow, xHigh_zHigh, xHigh_zLow);
			}
			else {
				// Both X and Z are interpolated: We need the center vertex too.
				A3D::Mesh::Vertex center;
				center.Position2D     = (xLow_zLow.Position2D + xHigh_zLow.Position2D + xLow_zHigh.Position2D + xHigh_zHigh.Position2D) * 0.25f;
				center.Position3D     = (xLow_zLow.Position3D + xHigh_zLow.Position3D + xLow_zHigh.Position3D + xHigh_zHigh.Position3D) * 0.25f;
				center.TextureCoord2D = (xLow_zLow.TextureCoord2D + xHigh_zLow.TextureCoord2D + xLow_zHigh.TextureCoord2D + xHigh_zHigh.TextureCoord2D) * 0.25f;
				center.Color3D        = (xLow_zLow.Color3D + xHigh_zLow.Color3D + xLow_zHigh.Color3D + xHigh_zHigh.Color3D) * 0.25f;
				center.Color4D        = (xLow_zLow.Color4D + xHigh_zLow.Color4D + xLow_zHigh.Color4D + xHigh_zHigh.Color4D) * 0.25f;

				addTriangle(xLow_zLow, xLow_zHigh, center);
				addTriangle(xLow_zHigh, xHigh_zHigh, center);
				addTriangle(xHigh_zHigh, xHigh_zLow, center);
				addTriangle(xHigh_zLow, xLow_zLow, center);
			}

			if(drawAsHistogram) {
				if(xAxisType == CHAXIS_LINEAR_INTERPOLATED || zAxisType == CHAXIS_LINEAR_INTERPOLATED) {
					xLow_zLow.TextureCoord2D   = QVector2D(0.05f, 0.05f);
					xHigh_zLow.TextureCoord2D  = QVector2D(0.95f, 0.05f);
					xLow_zHigh.TextureCoord2D  = QVector2D(0.05f, 0.95f);
					xHigh_zHigh.TextureCoord2D = QVector2D(0.95f, 0.95f);
				}

				A3D::Mesh::Vertex xLow_zLow_yRoot   = xLow_zLow;
				A3D::Mesh::Vertex xHigh_zLow_yRoot  = xHigh_zLow;
				A3D::Mesh::Vertex xLow_zHigh_yRoot  = xLow_zHigh;
				A3D::Mesh::Vertex xHigh_zHigh_yRoot = xHigh_zHigh;

				xLow_zLow_yRoot.Position3D.setY(yHistogramRoot);
				xHigh_zLow_yRoot.Position3D.setY(yHistogramRoot);
				xLow_zHigh_yRoot.Position3D.setY(yHistogramRoot);
				xHigh_zHigh_yRoot.Position3D.setY(yHistogramRoot);

				if(xAxisType == CHAXIS_ENUMERATED || zAxisType == CHAXIS_ENUMERATED) {
					// Bottom square
					addTriangle(xLow_zHigh_yRoot, xLow_zLow_yRoot, xHigh_zLow_yRoot);
					addTriangle(xLow_zHigh_yRoot, xHigh_zLow_yRoot, xHigh_zHigh_yRoot);
				}

				if(xAxisType == CHAXIS_ENUMERATED) {
					// Left square
					addTriangleWithTexCoordOffset(xLow_zLow, xLow_zLow_yRoot, xLow_zHigh_yRoot);
					addTriangleWithTexCoordOffset(xLow_zLow, xLow_zHigh_yRoot, xLow_zHigh);

					// Right square
					addTriangleWithTexCoordOffset(xHigh_zHigh, xHigh_zHigh_yRoot, xHigh_zLow_yRoot);
					addTriangleWithTexCoordOffset(xHigh_zHigh, xHigh_zLow_yRoot, xHigh_zLow);
				}

				if(zAxisType == CHAXIS_ENUMERATED) {
					// Front square
					addTriangleWithTexCoordOffset(xLow_zHigh, xLow_zHigh_yRoot, xHigh_zHigh_yRoot);
					addTriangleWithTexCoordOffset(xLow_zHigh, xHigh_zHigh_yRoot, xHigh_zHigh);

					// Back square
					addTriangleWithTexCoordOffset(xHigh_zLow, xHigh_zLow_yRoot, xLow_zLow_yRoot);
					addTriangleWithTexCoordOffset(xHigh_zLow, xLow_zLow_yRoot, xLow_zLow);
				}

				// End caps...
				if(xAxisType != CHAXIS_ENUMERATED) {
					if(x == 0) {
						// Left square
						addTriangleWithTexCoordOffset(xLow_zLow, xLow_zLow_yRoot, xLow_zHigh_yRoot);
						addTriangleWithTexCoordOffset(xLow_zLow, xLow_zHigh_yRoot, xLow_zHigh);
					}
					else if(x == (xIterations - 1)) {
						// Right square
						addTriangleWithTexCoordOffset(xHigh_zHigh, xHigh_zHigh_yRoot, xHigh_zLow_yRoot);
						addTriangleWithTexCoordOffset(xHigh_zHigh, xHigh_zLow_yRoot, xHigh_zLow);
					}
				}

				if(zAxisType != CHAXIS_ENUMERATED) {
					if(z == 0) {
						// Back square
						addTriangleWithTexCoordOffset(xHigh_zLow, xHigh_zLow_yRoot, xLow_zLow_yRoot);
						addTriangleWithTexCoordOffset(xHigh_zLow, xLow_zLow_yRoot, xLow_zLow);
					}
					else if(z == (zIterations - 1)) {
						// Front square
						addTriangleWithTexCoordOffset(xLow_zHigh, xLow_zHigh_yRoot, xHigh_zHigh_yRoot);
						addTriangleWithTexCoordOffset(xLow_zHigh, xHigh_zHigh_yRoot, xHigh_zHigh);
					}
				}
			}
		}
	}

	chartMesh->setContents(A3D::Mesh::Position3D | A3D::Mesh::Normal3D | A3D::Mesh::TextureCoord2D);
	chartMesh->setRenderOptions(A3D::Mesh::DisableCulling);
	chartMesh->invalidateCache();
}

void SurfaceChartEntity::updateIndicatorLines() {
	updateIndicatorLines(AXIS_X);
	updateIndicatorLines(AXIS_Y);
	updateIndicatorLines(AXIS_Z);
}
void SurfaceChartEntity::updateIndicatorLines(Axis3D axis) {
	if(axis >= AXIS_COUNT)
		return;

	QPointer<LineGroup>(&indicatorLines)[CHAXIND_COUNT] = m_indicatorLines[axis];
	Axis3D otherAxisA, otherAxisB;

	switch(axis) {
	case AXIS_X:
		otherAxisA = AXIS_Y;
		otherAxisB = AXIS_Z;
		break;
	case AXIS_Y:
		otherAxisA = AXIS_X;
		otherAxisB = AXIS_Z;
		break;
	case AXIS_Z:
		otherAxisA = AXIS_X;
		otherAxisB = AXIS_Y;
		break;
	default:
		return;
	}

	for(QPointer<LineGroup>& lineGroup: indicatorLines) {
		lineGroup->vertices().clear();
	}

	for(ChartAxisIndicator const& indicator: m_mapChart.axisData(axis).indicators()) {
		if(indicator.m_type >= CHAXIND_COUNT)
			continue;

		LineGroup::Vertex vertexBase;
		vertexBase.Color4D = colorToVector(indicator.m_style.m_indicatorColor);
		setVectorAxis(vertexBase.Position3D, axis, indicator.m_normalizedValue);

		LineGroup::Vertex vxAxisA = vertexBase;
		LineGroup::Vertex vxAxisB = vertexBase;
		setVectorAxis(vxAxisA.Position3D, otherAxisA, 1.f);
		setVectorAxis(vxAxisB.Position3D, otherAxisB, 1.f);

		std::vector<A3D::LineGroup::Vertex>& vertices = indicatorLines[indicator.m_type]->vertices();
		vertices.push_back(vertexBase);
		vertices.push_back(vxAxisA);
		vertices.push_back(vertexBase);
		vertices.push_back(vxAxisB);
	}

	for(QPointer<LineGroup>& lineGroup: indicatorLines) {
		lineGroup->setContents(LineGroup::Position3D | LineGroup::Color4D);
		lineGroup->invalidateCache();
	}
}

void SurfaceChartEntity::updateIndicatorLabels() {
	updateIndicatorLabels(AXIS_X);
	updateIndicatorLabels(AXIS_Y);
	updateIndicatorLabels(AXIS_Z);
}

void SurfaceChartEntity::updateIndicatorLabels(Axis3D axis) {
	if(axis >= AXIS_COUNT)
		return;

	std::vector<QPointer<TextBillboardEntity>>& indicatorLabels = m_indicatorLabels[axis];

	Axis3D otherAxisA, otherAxisB;

	switch(axis) {
	case AXIS_X:
		otherAxisA = AXIS_Y;
		otherAxisB = AXIS_Z;
		break;
	case AXIS_Y:
		otherAxisA = AXIS_X;
		otherAxisB = AXIS_Z;
		break;
	case AXIS_Z:
		otherAxisA = AXIS_X;
		otherAxisB = AXIS_Y;
		break;
	default:
		return;
	}

	float const otherAxisDistanceA = getVectorAxis(m_labelDistances, otherAxisA);
	float const otherAxisDistanceB = getVectorAxis(m_labelDistances, otherAxisB);

	for(QPointer<TextBillboardEntity>& previousLabel: indicatorLabels) {
		if(previousLabel) {
			delete previousLabel;
		}
	}
	indicatorLabels.clear();

	for(ChartAxisIndicator const& indicator: m_mapChart.axisData(axis).indicators()) {
		if(indicator.m_label.isEmpty())
			continue;

		QVector3D vertexBase;
		setVectorAxis(vertexBase, axis, indicator.m_normalizedValue);

		QVector3D vxAxisA = vertexBase;
		QVector3D vxAxisB = vertexBase;
		setVectorAxis(vxAxisA, otherAxisA, 1.f + otherAxisDistanceA);
		setVectorAxis(vxAxisB, otherAxisB, 1.f + otherAxisDistanceB);

		TextBillboardEntity* newBillboardA = emplaceChildEntity<TextBillboardEntity>();
		TextBillboardEntity* newBillboardB = emplaceChildEntity<TextBillboardEntity>();

		newBillboardA->setText(indicator.m_label);
		newBillboardA->setColor(indicator.m_style.m_labelColor);
		newBillboardA->setFont(indicator.m_style.m_labelFont);
		newBillboardA->setScale(QVector3D(0.05f, 0.05f, 0.05f) * indicator.m_style.m_labelSize);
		newBillboardA->setPosition(vxAxisA);

		newBillboardB->setText(indicator.m_label);
		newBillboardB->setColor(indicator.m_style.m_labelColor);
		newBillboardB->setFont(indicator.m_style.m_labelFont);
		newBillboardB->setScale(QVector3D(0.05f, 0.05f, 0.05f) * indicator.m_style.m_labelSize);
		newBillboardB->setPosition(vxAxisB);

		indicatorLabels.push_back(newBillboardA);
		indicatorLabels.push_back(newBillboardB);
	}
}

void SurfaceChartEntity::setMarker(QVector2D marker) {
	if(m_hasMarker && m_marker == marker)
		return;

	m_marker    = marker;
	m_hasMarker = true;

	LineGroup::Vertex start;
	LineGroup::Vertex end;
	start.Position3D = QVector3D(marker.x(), 0.f, marker.y());
	start.Color4D    = colorToVector(m_markerColor);
	end.Position3D   = start.Position3D;
	end.Position3D.setY(1.f);
	end.Color4D = start.Color4D;

	m_markerLineGroup->vertices().clear();
	m_markerLineGroup->vertices().push_back(start);
	m_markerLineGroup->vertices().push_back(end);
	m_markerLineGroup->invalidateCache();

	emit markerMoved();
}

void SurfaceChartEntity::unsetMarker() {
	m_marker    = QVector2D(0.f, 0.f);
	m_hasMarker = false;
	m_markerLineGroup->vertices().clear();
	m_markerLineGroup->invalidateCache();
}

bool SurfaceChartEntity::hasMarker() const {
	return m_hasMarker;
}

QVector2D SurfaceChartEntity::marker() const {
	return m_marker;
}

void SurfaceChartEntity::setMarkerColor(QColor markerColor) {
	if(m_markerColor == markerColor)
		return;
	m_markerColor = markerColor;
	if(!m_hasMarker)
		return;

	QVector4D vectorColor = colorToVector(m_markerColor);
	for(LineGroup::Vertex& vertices: m_markerLineGroup->vertices())
		vertices.Color4D = vectorColor;

	m_markerLineGroup->invalidateCache();
}

QColor SurfaceChartEntity::markerColor() const {
	return m_markerColor;
}

bool SurfaceChartEntity::updateEntity(std::chrono::milliseconds deltaMs) {
    if(m_chartSurfaceSyncRevision != m_mapChart.surfaceRevision()) {
		updateSurfaceMesh();
        m_chartSurfaceSyncRevision = m_mapChart.surfaceRevision();
	}

    if(m_chartIndicatorsSyncRevision != m_mapChart.indicatorsRevision()) {
        updateIndicatorLines();
        m_chartIndicatorsSyncRevision = m_mapChart.indicatorsRevision();
    }

    if(m_chartLabelsSyncRevision != m_mapChart.labelsRevision()) {
        updateIndicatorLabels();
        m_chartLabelsSyncRevision = m_mapChart.labelsRevision();
    }

	return Entity::updateEntity(deltaMs);
}

}
