/// @file
/// @brief Definitions for Chart Axis types, indicators, and 3D map chart data.

#ifndef A3DCHART_H
#define A3DCHART_H

#include "common.h"
#include <QFont>

namespace A3D {

uint16_t const MaxIndicators = 100;

/// @brief Types of axis scaling modes.
/// @details Determines how axis values are interpreted and rendered.
enum ChartAxisType {
	CHAXIS_LINEAR_INTERPOLATED, ///< Linear axis with continuous, interpolated values.
	CHAXIS_ENUMERATED           ///< Discrete axis with enumerated categories.
};

/// @brief Types of axis indicators.
enum ChartAxisIndicatorType {
	CHAXIND_MAJOR_INDICATOR, ///< Major tick or label on the axis.
	CHAXIND_MINOR_INDICATOR, ///< Minor tick on the axis.
	CHAXIND_COUNT            ///< Number of indicator types.
};

/// @struct ChartAxisIndicatorStyle
/// @brief Styling information for a chart axis indicator.
/// @details Specifies colors, font, and size for axis indicator markers and labels.
struct ChartAxisIndicatorStyle {
	/// @brief Construct a new style with given parameters.
	/// @param[in] indicatorColor Color of the axis indicator line or marker.
	/// @param[in] labelColor Color of the indicator label text.
	/// @param[in] labelFont Font used to render the label.
	/// @param[in] labelSize Scale factor for the label size.
	ChartAxisIndicatorStyle(QColor indicatorColor = Qt::white, QColor labelColor = Qt::white, QFont labelFont = QFont(QString(), 64), float labelSize = 1.f);
	ChartAxisIndicatorStyle(ChartAxisIndicatorStyle const&)            = default;
	ChartAxisIndicatorStyle(ChartAxisIndicatorStyle&&)                 = default;
	ChartAxisIndicatorStyle& operator=(ChartAxisIndicatorStyle const&) = default;
	ChartAxisIndicatorStyle& operator=(ChartAxisIndicatorStyle&&)      = default;
    bool operator==(ChartAxisIndicatorStyle const&) const;
    bool operator!=(ChartAxisIndicatorStyle const&) const;

	QColor m_indicatorColor; ///< Color of the indicator marker.
	QColor m_labelColor;     ///< Color of the indicator label text.
	QFont m_labelFont;       ///< Font used for the indicator label.
	float m_labelSize;       ///< Scale factor for the size of the label.
};

/// @struct ChartAxisIndicator
/// @brief Represents a single axis indicator (tick) and its label.
struct ChartAxisIndicator {
	/// @brief Construct a new indicator with given parameters.
	/// @param[in] type Type of the indicator (major or minor).
	/// @param[in] value Actual axis value corresponding to this indicator.
	/// @param[in] normalizedValue Value normalized to [0,1] along the axis.
	/// @param[in] label Text label for this indicator.
	/// @param[in] style Styling information (color, font, size).
	ChartAxisIndicator(
		ChartAxisIndicatorType type = CHAXIND_MAJOR_INDICATOR, float value = 0.f, float normalizedValue = 0.f, QString label = QString(),
		ChartAxisIndicatorStyle style = ChartAxisIndicatorStyle()
	);
	ChartAxisIndicator(ChartAxisIndicator const&)            = default;
	ChartAxisIndicator(ChartAxisIndicator&&)                 = default;
	ChartAxisIndicator& operator=(ChartAxisIndicator const&) = default;
	ChartAxisIndicator& operator=(ChartAxisIndicator&&)      = default;

	ChartAxisIndicatorType m_type;   ///< Type of this indicator (major or minor).
	float m_value;                   ///< Value of the indicator on the axis.
	float m_normalizedValue;         ///< Normalized position along the axis [0,1].
    QString m_label;                 ///< Label text for the indicator.
	ChartAxisIndicatorStyle m_style; ///< Style applied to this indicator.
};

/// @struct ChartAxisStyledEnumeratedIndicatorData
/// @brief Label and style for enumerated axis indicators.
struct ChartAxisStyledEnumeratedIndicatorData {
	/// @brief Construct with label and style.
	/// @param[in] label Text for the enumeration entry.
	/// @param[in] style Style for the enumeration indicator.
	ChartAxisStyledEnumeratedIndicatorData(QString label = QString(), ChartAxisIndicatorStyle style = ChartAxisIndicatorStyle());
	ChartAxisStyledEnumeratedIndicatorData(ChartAxisStyledEnumeratedIndicatorData const&)            = default;
	ChartAxisStyledEnumeratedIndicatorData(ChartAxisStyledEnumeratedIndicatorData&&)                 = default;
	ChartAxisStyledEnumeratedIndicatorData& operator=(ChartAxisStyledEnumeratedIndicatorData const&) = default;
	ChartAxisStyledEnumeratedIndicatorData& operator=(ChartAxisStyledEnumeratedIndicatorData&&)      = default;

	QString m_label;                 ///< Label text for the enumeration entry.
	ChartAxisIndicatorStyle m_style; ///< Style for the indicator at this enumeration.
};

/// @brief Manages axis properties and indicator points for chart rendering.
class ChartAxisData {
public:
	/// @brief Default constructor initializing an empty axis.
	ChartAxisData();
	ChartAxisData(ChartAxisData const&)            = default;
	ChartAxisData(ChartAxisData&&)                 = default;
	ChartAxisData& operator=(ChartAxisData const&) = default;
	ChartAxisData& operator=(ChartAxisData&&)      = default;

	/// @brief Set the name of the axis.
	/// @param[in] name String to identify the axis.
	void setName(QString name);

	/// @brief Get the name of the axis.
	/// @return Current axis name.
	QString name() const;

	/// @brief Set the default style for newly added indicators.
	/// @param[in] style Style applied to any new indicator.
	void setDefaultIndicatorStyle(ChartAxisIndicatorStyle style);

	/// @brief Retrieve the default indicator style.
	/// @return Style used for new indicators.
	ChartAxisIndicatorStyle defaultIndicatorStyle() const;

	/// @brief Remove all indicators from the axis.
	void removeAllIndicators();

	/// @brief Remove indicators of a specific type.
	/// @param[in] indicatorType Type of indicators to remove.
	void removeIndicators(ChartAxisIndicatorType indicatorType);

	/// @brief Set linear interpolated indicators explicitly.
	/// @details Changes axis type to CHAXIS_LINEAR_INTERPOLATED.
	/// @param[in] points Vector of pre-computed indicators.
	void setIndicators(std::vector<ChartAxisIndicator> points);

	/// @brief Set enumerated indicators from string list.
	/// @details Changes axis type to CHAXIS_ENUMERATED and updates min/max.
	/// @param[in] enumeratedPoints List of labels for each category.
	/// @param[in] indicatorType Type (major/minor) for all entries.
	void setIndicators(QStringList enumeratedPoints, ChartAxisIndicatorType indicatorType = CHAXIND_MAJOR_INDICATOR);

	/// @brief Set enumerated indicators with individual styles.
	/// @details Changes axis type to CHAXIS_ENUMERATED and updates min/max.
	/// @param[in] styledEnumeratedPoints Vector of label and style pairs.
	/// @param[in] indicatorType Type (major/minor) for all entries.
	void setIndicators(std::vector<ChartAxisStyledEnumeratedIndicatorData> styledEnumeratedPoints, ChartAxisIndicatorType indicatorType = CHAXIND_MAJOR_INDICATOR);

	/// @brief Add interpolated indicators by explicit values.
	/// @details Only valid for linear interpolated axes.
	/// @param[in] points Numeric values for new indicators.
	/// @param[in] toStringPrecision Number of decimals for string labels.
	/// @param[in] indicatorType Type (major/minor) for these indicators.
	void addIndicators(std::vector<float> const& points, int toStringPrecision = 3, ChartAxisIndicatorType indicatorType = CHAXIND_MAJOR_INDICATOR);

	/// @brief Add equidistant indicators by count.
	/// @details Only valid for linear interpolated axes.
	/// @param[in] from Start value of the axis range.
	/// @param[in] to End value of the axis range.
	/// @param[in] indicatorCount Number of indicators to place.
	/// @param[in] toStringPrecision Decimals in labels.
	/// @param[in] indicatorType Type (major/minor).
	void addEquidistantIndicatorsByIndicatorCount(
		float from, float to, size_t indicatorCount, int toStringPrecision = 3, ChartAxisIndicatorType indicatorType = CHAXIND_MAJOR_INDICATOR
	);

	/// @brief Add equidistant indicators by step size.
	/// @details Only valid for linear interpolated axes.
	/// @param[in] from Start value of the axis range.
	/// @param[in] to End value of the axis range.
	/// @param[in] stepSize Interval between adjacent indicators.
	/// @param[in] toStringPrecision Decimals in labels.
	/// @param[in] indicatorType Type (major/minor).
	void addEquidistantIndicatorsByStepSize(float from, float to, float stepSize, int toStringPrecision = 3, ChartAxisIndicatorType indicatorType = CHAXIND_MAJOR_INDICATOR);

	/// @brief Set custom minimum and maximum values for linear axis.
	/// @details Only valid when axis is linear interpolated.
	/// @param[in] minimum Lower bound of the axis.
	/// @param[in] maximum Upper bound of the axis.
	void setMinMax(float minimum, float maximum);

	/// @brief Get label for an enumerated entry by index.
	/// @details Only valid when axis is enumerated.
	/// @param[in] index Index of the enumeration.
	/// @return Label string at the given index.
	QString getEnumerationName(std::size_t index) const;

	/// @brief Count of enumeration entries.
	/// @details Only valid when axis is enumerated.
	/// @return Number of categories.
	std::size_t getEnumerationsCount() const;

	/// @brief Convert normalized [0,1] value to actual axis value.
	/// @param[in] normalizedValue Value between 0 and 1.
	/// @return Corresponding axis value or enumeration mapping.
	float denormalizeValue(float normalizedValue) const;

	/// @brief Convert axis value to normalized [0,1] value.
	/// @param[in] denormalizedValue Value between minimum() and maximum() while keeping eventual value inversions.
	/// @return Corresponding normalized value or enumeration index.
	float normalizeValue(float denormalizedValue) const;

	/// @brief Convert axis value to normalized [0,1] value.
	/// @param[in] denormalizedValue Value between minimum() and maximum().
	/// @return Corresponding normalized value or enumeration index.
	float normalizeDelta(float denormalizedDelta) const;

	/// @brief Invert the axis direction.
	void invert();

    bool isInverted() const;

	/// @brief Get the current axis type.
	/// @return The axis type.
	ChartAxisType type() const;

	/// @brief Access all indicators on the axis.
	/// @return Vector of indicator descriptors.
	std::vector<ChartAxisIndicator> const& indicators() const;

	/// @brief Get the minimum value of the axis range.
	/// @return Lower bound for linear axis.
	float minimum() const;

	/// @brief Get the maximum value of the axis range.
	/// @return Upper bound for linear axis.
	float maximum() const;

private:
	/// @brief Converts a raw axis value to a normalized [0, 1] range based on the current minimum and maximum.
	/// @details Values below the minimum will map to < 0, and values above the maximum will map to > 1.
	/// @param[in] input The raw axis value to normalize.
	/// @return The value normalized to the [0, 1] range (or beyond if out of bounds).
	float normalizedIndicatorValue(float input) const;

	/// @brief Recompute normalized values for all indicators.
	void normalizeIndicatorValues();

	QString m_name;                               ///< Name of the axis.
	ChartAxisType m_type;                         ///< Current axis type.
	ChartAxisIndicatorStyle m_defaultStyle;       ///< Default style for new indicators.
	std::vector<ChartAxisIndicator> m_indicators; ///< List of axis indicators.

	float m_axisMinimumValue; ///< Lower bound for linear axis.
	float m_axisMaximumValue; ///< Upper bound for linear axis.
};

/// @struct Chart3DSearchResult
/// @brief Result of a point search on a 3D axis.
struct Chart3DSearchResult {
	std::size_t m_index;    ///< Index of the data point.
	QVector2D m_coordinate; ///< Axis coordinate of the data point.
	float m_value;          ///< Raw data value.
	float m_weight;         ///< Weight of this point in relation to the returned dataset.
};

/// @enum ClampType
/// @brief Determines the rule for value clamping
enum ClampType {
	CT_NONE = 0, ///< Perform no clamping checks

	/// @brief Clamp the values to ensure that no sample goes past the axis' min/max range.
	///
	/// Useful to "splat" all values towards an end point.
	CT_CLAMP_VALUE,

	/// @brief Clamp the offset to ensure that no sample goes past the axis' min/max range.
	///
	/// Useful to vertically translate all values without the "splatting" effect.
	CT_CLAMP_OFFSET,
};

/// @brief 3D chart representation with axes and mesh coordinates (think Heightmap).
class MapChart3D {
public:
	/// @brief Construct an empty, invalid 3D map chart.
	MapChart3D();
	MapChart3D(MapChart3D const&)            = default;
	MapChart3D(MapChart3D&&)                 = default;
	MapChart3D& operator=(MapChart3D const&) = default;
	MapChart3D& operator=(MapChart3D&&)      = default;

	/// @brief Check if chart data has been initialized.
	/// @return True if valid data is set, false otherwise.
	bool isValid() const;

    /// @brief Returns the revision ID of the mesh.
	///
	/// The revision ID is automatically increased when the map is changed.
	/// By looking at the revision ID, an Entity can tell if it needs to be refreshed.
	/// @return The revision ID of this chart.
    size_t surfaceRevision() const;

    /// @brief Returns the revision ID of this indicators.
    ///
    /// The revision ID is automatically increased when the map is changed.
    /// By looking at the revision ID, an Entity can tell if it needs to be refreshed.
    /// @return The revision ID of this chart.
    size_t indicatorsRevision() const;

    /// @brief Returns the revision ID of this labels.
    ///
    /// The revision ID is automatically increased when the map is changed.
    /// By looking at the revision ID, an Entity can tell if it needs to be refreshed.
    /// @return The revision ID of this chart.
    size_t labelsRevision() const;

	/// @brief Set axis metadata and indicators.
	/// @param[in] axis Identifier for the 3D axis.
	/// @param[in] data Axis data and indicators.
	void setAxisData(Axis3D axis, ChartAxisData data);

	/// @brief Adds an offset to the Y axis on selected values, with a weight.
	/// @param[in] points The list of points, possibly returned by a search function.
	/// @param[in] offset The Y offset to apply to those points, in a weighted fashion.
	/// @param[in] clampType The clamping algorithm to apply.
	void offsetY(std::vector<Chart3DSearchResult>& points, float offset, ClampType clampType);

	/// @brief Retrieve axis data for given axis.
	/// @param[in] axis Identifier for the 3D axis.
	/// @return Const reference to axis data.
	ChartAxisData const& axisData(Axis3D axis) const;

	/// @brief Set the 3D point values for X, Z positions and Y heights.
	/// @details y_values size must equal x*z, row-major by X then Z.
	/// @param[in] x_input_positions Vector of X axis-coordinates.
	/// @param[in] z_input_positions Vector of Z axis-coordinates.
	/// @param[in] y_values Height values in a flattened grid.
	void setChartPoints(std::vector<float> x_input_positions, std::vector<float> z_input_positions, std::vector<float> y_values);

	/// @brief Values for a given axis before normalization.
	/// @param[in] axis Identifier for the 3D axis.
	/// @return Vector of raw values.
	std::vector<float> const& valuesForAxis(Axis3D axis) const;

	/// @brief Values for a given axis after normalization to [0,1].
	/// @param[in] axis Identifier for the 3D axis.
	/// @return Vector of normalized values.
	std::vector<float> const& normalizedValuesForAxis(Axis3D axis) const;

	/// @brief Map from mesh-coordinates to axis-coordinates.
	/// @param[in] meshCoordinate The input mesh-coordinate to convert.
	/// @return The equivalent axis-coordinate.
	QVector2D meshCoordinateToAxisCoordinate(QVector2D const& meshCoordinate) const;

	/// @brief Map from mesh-coordinates to axis-coordinates.
	/// @param[in] meshCoordinate The input mesh-coordinate to convert.
	/// @return The equivalent axis-coordinate.
	QVector3D meshCoordinateToAxisCoordinate(QVector3D const& meshCoordinate) const;

	/// @brief Map from axis-coordinates to mesh-coordinates.
	/// @param[in] axisCoordinate The input axis-coordinate to convert.
	/// @return The equivalent mesh-coordinate.
	QVector2D axisCoordinateToMeshCoordinate(QVector2D const& axisCoordinate) const;

	/// @brief Map from axis-coordinates to mesh-coordinates.
	/// @param[in] axisCoordinate The input axis-coordinate to convert.
	/// @return The equivalent mesh-coordinate.
	QVector3D axisCoordinateToMeshCoordinate(QVector3D const& axisCoordinate) const;

	/// @brief Map from axis-delta-coordinates to mesh-delta-coordinates.
	/// @param[in] axisCoordinate The input axis-delta-coordinate to convert.
	/// @return The equivalent mesh-delta-coordinate.
	QVector2D axisDeltaCoordinateToMeshDeltaCoordinate(QVector2D const& axisCoordinate) const;

	/// @brief Map from axis-delta-coordinates to mesh-delta-coordinates.
	/// @param[in] axisCoordinate The input axis-delta-coordinate to convert.
	/// @return The equivalent mesh-delta-coordinate.
	QVector3D axisDeltaCoordinateToMeshDeltaCoordinate(QVector3D const& axisCoordinate) const;

	/// @brief Completes an axis-coordinate with the resulting Y value.
	/// @param[in] axisCoordinate 2D axis-coordinate to retrieve the Y value for.
	/// @return 3D data value at that location.
	/// @remark X/Z coordinates in the returned values are copied from X/Y of axisCoordinate.
	QVector3D getValueFromAxisCoordinate(QVector2D const& axisCoordinate) const;

	/// @brief Computes the 3D axis-coordinate from a Search Result.
	/// @param[in] searchResult array of Chart3DSearchResult
	/// @return Resulting 3D data value.
	QVector3D getValueFromSearchResult(std::vector<Chart3DSearchResult> const& searchResult) const;

	/// @brief Finds the neighboring data points to a given axis-coordinate.
	/// @param[in] axisCoordinate 2D axis-coordinate to retrieve the points for.
	/// @return A weight-sorted vector of all the points that contribute to the axisCoordinate's resulting value.
	std::vector<Chart3DSearchResult> searchNearestPointsToAxisCoordinate(QVector2D const& axisCoordinate) const;

	/// @brief Finds the neighboring data points to a given axis-coordinate, with a specific radius.
	/// @param[in] axisCoordinate 2D axis-coordinate to retrieve the points for.
	/// @param[in] radius 2D axis-coordinate radius of the circle.
	/// @return A weight-sorted vector of all the points that match this search, spherically weighted.
	std::vector<Chart3DSearchResult> searchNearestPointsToAxisCoordinate(QVector2D const& axisCoordinate, QVector2D const& radius) const;

private:
	/// @brief Normalize axis values for mesh coordinate mapping.
	/// @param[in] axis Identifier for the 3D axis.
	void normalizeAxisPoints(Axis3D axis);

	bool m_isValid;                                     ///< True if valid data is set.
	ChartAxisData m_axes[AXIS_COUNT];                   ///< Axis data for each dimension.
	std::vector<float> m_values[AXIS_COUNT];            ///< Raw input values per axis.
	std::vector<float> m_normalized_values[AXIS_COUNT]; ///< Normalized values per axis.
    size_t m_surfaceRevision;                           ///< Current map revision index.
    size_t m_indicatorRevision;
    size_t m_labelsRevision;
};

}

#endif // CHART_H
