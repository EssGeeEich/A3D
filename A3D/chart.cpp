#include "chart.h"

namespace A3D {

ChartAxisIndicatorStyle::ChartAxisIndicatorStyle(QColor indicatorColor, QColor labelColor, QFont labelFont, float labelSize)
	: m_indicatorColor(std::move(indicatorColor)),
	  m_labelColor(std::move(labelColor)),
	  m_labelFont(std::move(labelFont)),
	  m_labelSize(labelSize) {}

ChartAxisIndicator::ChartAxisIndicator(ChartAxisIndicatorType type, float value, float normalizedValue, QString label, ChartAxisIndicatorStyle style)
	: m_type(type),
	  m_value(value),
	  m_normalizedValue(normalizedValue),
	  m_label(std::move(label)),
	  m_style(std::move(style)) {}

ChartAxisStyledEnumeratedIndicatorData::ChartAxisStyledEnumeratedIndicatorData(QString label, ChartAxisIndicatorStyle style)
	: m_label(std::move(label)),
	  m_style(std::move(style)) {}

ChartAxisData::ChartAxisData()
	: m_type(CHAXIS_LINEAR_INTERPOLATED),
	  m_axisMinimumValue(0.f),
	  m_axisMaximumValue(1.f) {}

void ChartAxisData::setName(QString name) {
	m_name = std::move(name);
}

QString ChartAxisData::name() const {
	return m_name;
}

void ChartAxisData::setDefaultIndicatorStyle(ChartAxisIndicatorStyle style) {
	m_defaultStyle = std::move(style);
}

bool ChartAxisIndicatorStyle::operator==(ChartAxisIndicatorStyle const& obj) const {
    return m_indicatorColor == obj.m_indicatorColor && m_labelColor == obj.m_labelColor && m_labelFont == obj.m_labelFont && m_labelSize == obj.m_labelSize;
}

bool ChartAxisIndicatorStyle::operator!=(ChartAxisIndicatorStyle const& obj) const {
    return !(*this == obj);
}

ChartAxisIndicatorStyle ChartAxisData::defaultIndicatorStyle() const {
	return m_defaultStyle;
}

void ChartAxisData::removeAllIndicators() {
	m_indicators.clear();
}

void ChartAxisData::removeIndicators(ChartAxisIndicatorType indicatorType) {
	m_indicators.erase(
		std::remove_if(
			m_indicators.begin(), m_indicators.end(),
			[indicatorType](ChartAxisIndicator const& indicator) -> bool {
				return indicator.m_type == indicatorType;
			}
		),
		m_indicators.end()
	);
}

void ChartAxisData::setIndicators(std::vector<ChartAxisIndicator> points) {
	m_type = CHAXIS_LINEAR_INTERPOLATED;
	m_indicators.clear();
	m_indicators.reserve(points.size());

	for(ChartAxisIndicator& indicator: points)
		m_indicators.emplace_back(std::move(indicator));
}

void ChartAxisData::setIndicators(QStringList enumeratedPoints, ChartAxisIndicatorType indicatorType) {
	if(enumeratedPoints.isEmpty())
		return;

	m_type = CHAXIS_ENUMERATED;
	m_indicators.clear();
	m_indicators.reserve(enumeratedPoints.size());

	m_axisMinimumValue = 0.f;
	m_axisMaximumValue = static_cast<float>(enumeratedPoints.size() - 1);
	for(std::size_t i = 0; i < enumeratedPoints.size(); ++i) {
		m_indicators.push_back(ChartAxisIndicator(indicatorType, static_cast<float>(i), static_cast<float>(i) / m_axisMaximumValue, std::move(enumeratedPoints[i]), m_defaultStyle)
		);
	}
}

void ChartAxisData::setIndicators(std::vector<ChartAxisStyledEnumeratedIndicatorData> styledEnumeratedPoints, ChartAxisIndicatorType indicatorType) {
	if(styledEnumeratedPoints.empty())
		return;

	m_type = CHAXIS_ENUMERATED;
	m_indicators.clear();
	m_indicators.reserve(styledEnumeratedPoints.size());

	m_axisMinimumValue = 0.f;
	m_axisMaximumValue = static_cast<float>(styledEnumeratedPoints.size() - 1);
	for(std::size_t i = 0; i < styledEnumeratedPoints.size(); ++i) {
		m_indicators.push_back(ChartAxisIndicator(
			indicatorType, static_cast<float>(i), static_cast<float>(i) / m_axisMaximumValue, std::move(styledEnumeratedPoints[i].m_label),
			std::move(styledEnumeratedPoints[i].m_style)
		));
	}
}

void ChartAxisData::addIndicators(std::vector<float> const& points, int toStringPrecision, ChartAxisIndicatorType indicatorType) {
	if(m_type != CHAXIS_LINEAR_INTERPOLATED)
		return;
	m_indicators.reserve(m_indicators.size() + points.size());

	for(float const& val: points)
		m_indicators.push_back(ChartAxisIndicator(indicatorType, val, normalizedIndicatorValue(val), QString::number(val, 'f', toStringPrecision), m_defaultStyle));
}

void ChartAxisData::addEquidistantIndicatorsByIndicatorCount(float from, float to, size_t indicatorCount, int toStringPrecision, ChartAxisIndicatorType indicatorType) {
	if(m_type != CHAXIS_LINEAR_INTERPOLATED || indicatorCount < 2)
		return;

	m_indicators.reserve(m_indicators.size() + indicatorCount);

	float fInverseIndicatorCount = 1.f / static_cast<float>(indicatorCount - 1);
	for(size_t i = 0; i < indicatorCount; ++i) {
		float const val = from + ((to - from) * fInverseIndicatorCount * static_cast<float>(i));
		m_indicators.push_back(ChartAxisIndicator(indicatorType, val, normalizedIndicatorValue(val), QString::number(val, 'f', toStringPrecision), m_defaultStyle));
	}
}

void ChartAxisData::addEquidistantIndicatorsByStepSize(float from, float to, float stepSize, int toStringPrecision, ChartAxisIndicatorType indicatorType) {
	if(m_type != CHAXIS_LINEAR_INTERPOLATED)
		return;

	size_t const indicatorCount = static_cast<size_t>((to - from) / stepSize);

    if(indicatorCount > MaxIndicators) // Too many, limit to 1000 points...
	{
		addEquidistantIndicatorsByIndicatorCount(from, to, 1000, toStringPrecision, indicatorType);
		return;
	}

	m_indicators.reserve(m_indicators.size() + indicatorCount + 1);

	for(size_t i = 0; i < indicatorCount; ++i) {
		float const val = from + stepSize * static_cast<float>(i);
		m_indicators.push_back(ChartAxisIndicator(indicatorType, val, normalizedIndicatorValue(val), QString::number(val, 'f', toStringPrecision), m_defaultStyle));
	}

	float const val = to;
	m_indicators.push_back(ChartAxisIndicator(indicatorType, val, normalizedIndicatorValue(val), QString::number(val, 'f', toStringPrecision), m_defaultStyle));
}

QString ChartAxisData::getEnumerationName(std::size_t index) const {
	if(m_type != CHAXIS_ENUMERATED)
		return 0;

	if(index >= m_indicators.size())
		return 0;

	return m_indicators[index].m_label;
}

std::size_t ChartAxisData::getEnumerationsCount() const {
	if(m_type != CHAXIS_ENUMERATED)
		return 0;

	return m_indicators.size();
}

float ChartAxisData::denormalizeValue(float normalizedValue) const {
	if(m_type == CHAXIS_LINEAR_INTERPOLATED) {
		return (normalizedValue * (m_axisMaximumValue - m_axisMinimumValue)) + m_axisMinimumValue;
	}
	else if(m_type == CHAXIS_ENUMERATED) {
		float range = (m_axisMaximumValue - m_axisMinimumValue);

		if(range >= 0) {
			range += 1.f;
			float val = (normalizedValue * range) + m_axisMinimumValue;
			return std::floor(val);
		}
		else {
			range -= 1.f;
			float val = (normalizedValue * range) + m_axisMinimumValue;
			return std::ceil(val);
		}
	}
	return 0.f;
}

float ChartAxisData::normalizeValue(float denormalizedValue) const {
	float const fZeroOffset = minimum();
	float const fInvDelta   = 1.f / (maximum() - minimum());

	return (denormalizedValue - fZeroOffset) * fInvDelta;
}

float ChartAxisData::normalizeDelta(float denormalizedValue) const {
	float const fMin = std::min(minimum(), maximum());
	float const fMax = std::max(minimum(), maximum());

	float const fZeroOffset = fMin;
	float const fInvDelta   = 1.f / (fMax - fMin);

	return (denormalizedValue - fZeroOffset) * fInvDelta;
}

void ChartAxisData::setMinMax(float minimum, float maximum) {
	if(m_type != CHAXIS_LINEAR_INTERPOLATED)
		return;

	m_axisMinimumValue = minimum;
	m_axisMaximumValue = maximum;

	normalizeIndicatorValues();
}

void ChartAxisData::invert() {
	std::swap(m_axisMaximumValue, m_axisMinimumValue);

	for(ChartAxisIndicator& indicator: m_indicators) {
		indicator.m_normalizedValue = 1.f - indicator.m_normalizedValue;
	}
}

bool ChartAxisData::isInverted() const {
    return m_axisMinimumValue > m_axisMaximumValue;
}

ChartAxisType ChartAxisData::type() const {
	return m_type;
}

std::vector<ChartAxisIndicator> const& ChartAxisData::indicators() const {
	return m_indicators;
}

float ChartAxisData::minimum() const {
	return m_axisMinimumValue;
}
float ChartAxisData::maximum() const {
	return m_axisMaximumValue;
}

float ChartAxisData::normalizedIndicatorValue(float input) const {
	return (input - minimum()) / (maximum() - minimum());
}

void ChartAxisData::normalizeIndicatorValues() {
	float const fZeroOffset = minimum();
	float const fInvDelta   = 1.f / (maximum() - fZeroOffset);
	for(ChartAxisIndicator& val: m_indicators) {
		val.m_normalizedValue = ((val.m_value - fZeroOffset) * fInvDelta) + fZeroOffset;
	}
}

MapChart3D::MapChart3D()
	: m_isValid(false),
      m_surfaceRevision(0),
      m_indicatorRevision(0),
      m_labelsRevision(0) {}

bool MapChart3D::isValid() const {
	return m_isValid;
}

size_t MapChart3D::surfaceRevision() const {
    return m_surfaceRevision;
}

size_t MapChart3D::indicatorsRevision() const {
    return m_indicatorRevision;
}

size_t MapChart3D::labelsRevision() const {
    return m_labelsRevision;
}

void MapChart3D::setAxisData(Axis3D axis, ChartAxisData data) {
	if(axis >= AXIS_COUNT)
		return;
	m_axes[axis] = std::move(data);
	normalizeAxisPoints(axis);
    ++m_surfaceRevision;
    ++m_indicatorRevision;
    ++m_labelsRevision;
}

void MapChart3D::offsetY(std::vector<Chart3DSearchResult>& points, float offset, ClampType clamp) {
	float const fZeroOffset = m_axes[AXIS_Y].minimum();
	float const fInvDelta   = 1.f / (m_axes[AXIS_Y].maximum() - m_axes[AXIS_Y].minimum());

	float const fLowest  = std::min(m_axes[AXIS_Y].minimum(), m_axes[AXIS_Y].maximum());
	float const fHighest = std::max(m_axes[AXIS_Y].minimum(), m_axes[AXIS_Y].maximum());

	if(clamp == CT_CLAMP_OFFSET) {
		for(auto it = points.begin(); it != points.end(); ++it) {
			if(it->m_index >= m_values[AXIS_Y].size() || it->m_weight < std::numeric_limits<float>::min())
				continue;

			float const expectedRealOffset = offset * it->m_weight;

			// What's the highest point we can reach, with that weight scale?
			if(offset > 0.f && m_values[AXIS_Y][it->m_index] + expectedRealOffset > fHighest) {
				offset = (fHighest - m_values[AXIS_Y][it->m_index]) / it->m_weight;
			}
			else if(offset < 0.f && m_values[AXIS_Y][it->m_index] + expectedRealOffset < fLowest) {
				offset = (fLowest - m_values[AXIS_Y][it->m_index]) / it->m_weight;
			}

			// Offset can't be applied at all; Stop early.
			if(std::abs(offset) < std::numeric_limits<float>::min())
				return;
		}
	}

	for(auto it = points.begin(); it != points.end(); ++it) {
		if(it->m_index >= m_values[AXIS_Y].size())
			continue;

		float newValue = m_values[AXIS_Y][it->m_index] + (offset * it->m_weight);
		if(clamp == CT_CLAMP_VALUE)
			newValue = std::clamp(newValue, m_axes[AXIS_Y].minimum(), m_axes[AXIS_Y].maximum());

		m_values[AXIS_Y][it->m_index] = newValue;
		it->m_value                   = newValue;

		float& val = m_normalized_values[AXIS_Y][it->m_index];
		val        = (newValue - fZeroOffset) * fInvDelta;
	}
    ++m_surfaceRevision;
}

ChartAxisData const& MapChart3D::axisData(Axis3D axis) const {
	if(axis < AXIS_COUNT)
		return m_axes[axis];

	static ChartAxisData invalid;
	return invalid;
}

void MapChart3D::setChartPoints(std::vector<float> x_input_positions, std::vector<float> z_input_positions, std::vector<float> y_values) {
	if(y_values.size() != (x_input_positions.size() * z_input_positions.size()))
		return;

	m_values[AXIS_X] = std::move(x_input_positions);
	m_values[AXIS_Y] = std::move(y_values);
	m_values[AXIS_Z] = std::move(z_input_positions);

	normalizeAxisPoints(AXIS_X);
	normalizeAxisPoints(AXIS_Y);
	normalizeAxisPoints(AXIS_Z);

	m_isValid = true;
    ++m_surfaceRevision;
}

std::vector<float> const& MapChart3D::valuesForAxis(Axis3D axis) const {
	if(axis < AXIS_COUNT)
		return m_values[axis];

	static std::vector<float> invalid;
	return invalid;
}
std::vector<float> const& MapChart3D::normalizedValuesForAxis(Axis3D axis) const {
	if(axis < AXIS_COUNT)
		return m_normalized_values[axis];

	static std::vector<float> invalid;
	return invalid;
}

QVector3D MapChart3D::meshCoordinateToAxisCoordinate(QVector3D const& meshCoordinate) const {
	if(!isValid())
		return QVector3D();

	return QVector3D(m_axes[AXIS_X].denormalizeValue(meshCoordinate.x()), m_axes[AXIS_Y].denormalizeValue(meshCoordinate.y()), m_axes[AXIS_Z].denormalizeValue(meshCoordinate.z()));
}

QVector2D MapChart3D::meshCoordinateToAxisCoordinate(QVector2D const& meshCoordinate) const {
	if(!isValid())
		return QVector2D();

	return QVector2D(m_axes[AXIS_X].denormalizeValue(meshCoordinate.x()), m_axes[AXIS_Z].denormalizeValue(meshCoordinate.y()));
}

QVector3D MapChart3D::axisCoordinateToMeshCoordinate(QVector3D const& axisCoordinate) const {
	if(!isValid())
		return QVector3D();

	return QVector3D(m_axes[AXIS_X].normalizeValue(axisCoordinate.x()), m_axes[AXIS_Y].normalizeValue(axisCoordinate.y()), m_axes[AXIS_Z].normalizeValue(axisCoordinate.z()));
}

QVector2D MapChart3D::axisCoordinateToMeshCoordinate(QVector2D const& axisCoordinate) const {
	if(!isValid())
		return QVector2D();

	return QVector2D(m_axes[AXIS_X].normalizeValue(axisCoordinate.x()), m_axes[AXIS_Z].normalizeValue(axisCoordinate.y()));
}

QVector3D MapChart3D::axisDeltaCoordinateToMeshDeltaCoordinate(QVector3D const& axisCoordinate) const {
	if(!isValid())
		return QVector3D();

	return QVector3D(m_axes[AXIS_X].normalizeDelta(axisCoordinate.x()), m_axes[AXIS_Y].normalizeDelta(axisCoordinate.y()), m_axes[AXIS_Z].normalizeDelta(axisCoordinate.z()));
}

QVector2D MapChart3D::axisDeltaCoordinateToMeshDeltaCoordinate(QVector2D const& axisCoordinate) const {
	if(!isValid())
		return QVector2D();

	return QVector2D(m_axes[AXIS_X].normalizeDelta(axisCoordinate.x()), m_axes[AXIS_Z].normalizeDelta(axisCoordinate.y()));
}

QVector3D MapChart3D::getValueFromAxisCoordinate(QVector2D const& axisCoordinate) const {
	if(!isValid())
		return QVector3D();

	std::vector<Chart3DSearchResult> result = searchNearestPointsToAxisCoordinate(axisCoordinate);

	float sumY = 0.f;

	for(auto it = result.begin(); it != result.end(); ++it) {
		sumY += (it->m_value * it->m_weight);
	}

	return QVector3D(axisCoordinate.x(), sumY, axisCoordinate.y());
}

QVector3D MapChart3D::getValueFromSearchResult(std::vector<Chart3DSearchResult> const& searchResult) const {
	QVector3D sum = QVector3D(0.f, 0.f, 0.f);

	for(auto it = searchResult.begin(); it != searchResult.end(); ++it) {
		sum += QVector3D(it->m_coordinate.x(), it->m_value, it->m_coordinate.y()) * it->m_weight;
	}

	return sum;
}

std::vector<Chart3DSearchResult> MapChart3D::searchNearestPointsToAxisCoordinate(QVector2D const& axisCoordinate) const {
	std::vector<Chart3DSearchResult> result;

	if(!isValid())
		return result;

	int leftX_index    = -1;
	int rightX_index   = -1;
	float leftX_weight = 0.f;

	for(std::size_t iX = 0; iX < m_values[AXIS_X].size(); ++iX) {
		if(m_values[AXIS_X][iX] == axisCoordinate.x()) {
			leftX_index  = static_cast<int>(iX);
			leftX_weight = 1.f;
			break;
		}
		else if(m_values[AXIS_X][iX] > axisCoordinate.x()) {
			if(iX == 0) {
				leftX_index  = static_cast<int>(iX);
				leftX_weight = 1.f;
				break;
			}
			else {
				leftX_index  = static_cast<int>(iX - 1);
				rightX_index = static_cast<int>(iX);

				float range      = m_values[AXIS_X][iX] - m_values[AXIS_X][iX - 1];
				float posInRange = axisCoordinate.x() - m_values[AXIS_X][iX - 1];

				leftX_weight = 1.f - (static_cast<float>(posInRange) / static_cast<float>(range));
				break;
			}
		}
	}

	if(leftX_index < 0) {
		leftX_index  = static_cast<int>(m_values[AXIS_X].size() - 1);
		leftX_weight = 1.f;
	}

	int leftZ_index    = -1;
	int rightZ_index   = -1;
	float leftZ_weight = 0.f;

	for(std::size_t iZ = 1; iZ < m_values[AXIS_Z].size(); ++iZ) {
		if(m_values[AXIS_Z][iZ] == axisCoordinate.y()) {
			leftZ_index  = static_cast<int>(iZ);
			leftZ_weight = 1.f;
			break;
		}
		else if(m_values[AXIS_Z][iZ] > axisCoordinate.y()) {
			if(iZ == 0) {
				leftZ_index  = static_cast<int>(iZ);
				leftZ_weight = 1.f;
				break;
			}
			else {
				leftZ_index  = static_cast<int>(iZ - 1);
				rightZ_index = static_cast<int>(iZ);

				float range      = m_values[AXIS_Z][iZ] - m_values[AXIS_Z][iZ - 1];
				float posInRange = axisCoordinate.y() - m_values[AXIS_Z][iZ - 1];

				leftZ_weight = 1.f - (static_cast<float>(posInRange) / static_cast<float>(range));
				break;
			}
		}
	}

	if(leftZ_index < 0) {
		leftZ_index  = static_cast<int>(m_values[AXIS_Z].size() - 1);
		leftZ_weight = 1.f;
	}

	// Calculate matrix

	float rightX_weight = 1.f - leftX_weight;
	float rightZ_weight = 1.f - leftZ_weight;

	if(axisData(AXIS_X).type() == CHAXIS_ENUMERATED) {
		if(leftX_weight < rightX_weight)
			std::swap(leftX_index, rightX_index);

		leftX_weight  = 1.f;
		rightX_weight = 0.f;
		rightX_index  = -1;
	}

	if(axisData(AXIS_Z).type() == CHAXIS_ENUMERATED) {
		if(leftZ_weight < rightZ_weight)
			std::swap(leftZ_index, rightZ_index);

		leftZ_weight  = 1.f;
		rightZ_weight = 0.f;
		rightZ_index  = -1;
	}

	// Top-Left
	{
		Chart3DSearchResult& xLow_zLow = result.emplace_back();

		xLow_zLow.m_coordinate = QVector2D(m_values[AXIS_X][leftX_index], m_values[AXIS_Z][leftZ_index]);
		xLow_zLow.m_index      = (leftX_index + (leftZ_index * m_values[AXIS_X].size()));
		xLow_zLow.m_value      = m_values[AXIS_Y][xLow_zLow.m_index];
		xLow_zLow.m_weight     = leftX_weight * leftZ_weight;
	}

	// Top-Right (if exists)
	if(rightZ_index >= 0) {
		Chart3DSearchResult& xLow_zHigh = result.emplace_back();

		xLow_zHigh.m_coordinate = QVector2D(m_values[AXIS_X][leftX_index], m_values[AXIS_Z][rightZ_index]);
		xLow_zHigh.m_index      = (leftX_index + (rightZ_index * m_values[AXIS_X].size()));
		xLow_zHigh.m_value      = m_values[AXIS_Y][xLow_zHigh.m_index];
		xLow_zHigh.m_weight     = leftX_weight * rightZ_weight;
	}

	// Bottom-Left (if exists)
	if(rightX_index >= 0) {
		Chart3DSearchResult& xHigh_zLow = result.emplace_back();

		xHigh_zLow.m_coordinate = QVector2D(m_values[AXIS_X][rightX_index], m_values[AXIS_Z][leftZ_index]);
		xHigh_zLow.m_index      = (rightX_index + (leftZ_index * m_values[AXIS_X].size()));
		xHigh_zLow.m_value      = m_values[AXIS_Y][xHigh_zLow.m_index];
		xHigh_zLow.m_weight     = rightX_weight * leftZ_weight;
	}

	// Bottom-Right (if exists)
	if(rightX_index >= 0 && rightZ_index >= 0) {
		Chart3DSearchResult& xHigh_zHigh = result.emplace_back();

		xHigh_zHigh.m_coordinate = QVector2D(m_values[AXIS_X][rightX_index], m_values[AXIS_Z][rightZ_index]);
		xHigh_zHigh.m_index      = (rightX_index + (rightZ_index * m_values[AXIS_X].size()));
		xHigh_zHigh.m_value      = m_values[AXIS_Y][xHigh_zHigh.m_index];
		xHigh_zHigh.m_weight     = rightX_weight * rightZ_weight;
	}

	// Sort results by weight
	std::sort(result.begin(), result.end(), [](Chart3DSearchResult const& a, Chart3DSearchResult const& b) -> bool {
		return a.m_weight > b.m_weight;
	});

	return std::move(result);
}

std::vector<Chart3DSearchResult> MapChart3D::searchNearestPointsToAxisCoordinate(QVector2D const& axisCoordinate, QVector2D const& radius) const {
	std::vector<Chart3DSearchResult> result;

	if(!isValid() || radius.x() < std::numeric_limits<float>::min() || radius.y() < std::numeric_limits<float>::min())
		return result;

	auto xBeginIt = std::lower_bound(m_values[AXIS_X].begin(), m_values[AXIS_X].end(), axisCoordinate.x() - radius.x());
	auto xEndIt   = std::upper_bound(xBeginIt, m_values[AXIS_X].end(), axisCoordinate.x() + radius.x());
	auto zBeginIt = std::lower_bound(m_values[AXIS_Z].begin(), m_values[AXIS_Z].end(), axisCoordinate.y() - radius.y());
	auto zEndIt   = std::upper_bound(zBeginIt, m_values[AXIS_Z].end(), axisCoordinate.y() + radius.y());

	if(xBeginIt == m_values[AXIS_X].end())
		return result;
	if(zBeginIt == m_values[AXIS_Z].end())
		return result;

	// xEndIt / yEndIt can be end: it's okay.

	result.reserve(std::distance(xBeginIt, xEndIt) * std::distance(zBeginIt, zEndIt));

	QVector2D const inverseRadius = QVector2D(1.f, 1.f) / radius;
	std::ptrdiff_t const xIxStart = std::distance(m_values[AXIS_X].begin(), xBeginIt);
	std::ptrdiff_t zIx            = std::distance(m_values[AXIS_Z].begin(), zBeginIt);
	float totalWeight             = 0.f;

	for(auto zIt = zBeginIt; zIt != zEndIt; ++zIt, ++zIx) {
		float const zD     = ((*zIt) - axisCoordinate.y()) * inverseRadius.y();
		std::ptrdiff_t xIx = xIxStart;

		for(auto xIt = xBeginIt; xIt != xEndIt; ++xIt, ++xIx) {
			float const xD = ((*xIt) - axisCoordinate.x()) * inverseRadius.x();

			float d2 = (xD * xD) + (zD * zD);

			if(d2 <= 1.f) {
				float const fWeight = qSqrt(1.f - d2);

				if(fWeight > std::numeric_limits<float>::min()) {
					Chart3DSearchResult& newResult = result.emplace_back();
					newResult.m_weight             = fWeight;
					newResult.m_coordinate         = QVector2D(*xIt, *zIt);
					newResult.m_index              = (zIx * m_values[AXIS_X].size()) + xIx;
					newResult.m_value              = m_values[AXIS_Y][newResult.m_index];
					totalWeight += newResult.m_weight;
				}
			}
		}
	}

	if(totalWeight > 0.f) {
		float const inverseWeight = 1.f / totalWeight;
		for(auto it = result.begin(); it != result.end(); ++it) {
			it->m_weight *= inverseWeight;
		}

		// Sort results by weight
		std::sort(result.begin(), result.end(), [](Chart3DSearchResult const& a, Chart3DSearchResult const& b) -> bool {
			return a.m_weight > b.m_weight;
		});
	}
	else {
		result.clear();
	}

	// Attempt to return a meaningful set of samples...
	if(result.empty()) {
		return searchNearestPointsToAxisCoordinate(axisCoordinate);
	}

	return std::move(result);
}

void MapChart3D::normalizeAxisPoints(Axis3D axis) {
	if(axis >= AXIS_COUNT)
		return;

	m_normalized_values[axis] = m_values[axis];

	float const fZeroOffset = m_axes[axis].minimum();
	float const fInvDelta   = 1.f / (m_axes[axis].maximum() - m_axes[axis].minimum());
	for(float& val: m_normalized_values[axis]) {
		val = (val - fZeroOffset) * fInvDelta;
	}
}

}
