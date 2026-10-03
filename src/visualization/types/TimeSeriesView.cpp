#include "TimeSeriesView.hpp"

namespace nlm {

struct TimeSeriesView::Impl {
    std::string name;
    bool enabled;
    std::string timeSeriesType;
    bool multipleTracesEnabled;
    std::string traceGrouping;
    float smoothingFactor;
    bool interpolationEnabled;
    std::pair<Timestamp, Timestamp> timeWindow;
    bool rangeSelectionEnabled;
    float rangeMin;
    float rangeMax;
    bool zoomEnabled;
    bool panEnabled;
    bool cursorLineEnabled;
    std::array<float, 4> cursorLineColor;
    bool statisticsOverlayEnabled;
    
    Impl() : name("TimeSeriesView"), enabled(true), timeSeriesType("line"),
             multipleTracesEnabled(true), traceGrouping("by_neuron"),
             smoothingFactor(0.1f), interpolationEnabled(false),
             timeWindow({0.0, 100.0}), rangeSelectionEnabled(false),
             rangeMin(0.0f), rangeMax(1.0f), zoomEnabled(true), panEnabled(true),
             cursorLineEnabled(true), cursorLineColor({1.0f, 0.0f, 0.0f, 1.0f}),
             statisticsOverlayEnabled(true) {}
};

TimeSeriesView::TimeSeriesView() : pImpl(std::make_unique<Impl>()) {}
TimeSeriesView::~TimeSeriesView() = default;

const std::string& TimeSeriesView::getName() const {
    return pImpl->name;
}

void TimeSeriesView::setName(const std::string& name) {
    pImpl->name = name;
}

void TimeSeriesView::enable(bool enabled) {
    pImpl->enabled = enabled;
}

bool TimeSeriesView::isEnabled() const {
    return pImpl->enabled;
}

void TimeSeriesView::setTimeSeriesType(const std::string& type) {
    pImpl->timeSeriesType = type;
}

const std::string& TimeSeriesView::getTimeSeriesType() const {
    return pImpl->timeSeriesType;
}

void TimeSeriesView::enableMultipleTraces(bool enabled) {
    pImpl->multipleTracesEnabled = enabled;
}

bool TimeSeriesView::areMultipleTracesEnabled() const {
    return pImpl->multipleTracesEnabled;
}

void TimeSeriesView::setTraceGrouping(const std::string& grouping) {
    pImpl->traceGrouping = grouping;
}

const std::string& TimeSeriesView::getTraceGrouping() const {
    return pImpl->traceGrouping;
}

void TimeSeriesView::setSmoothingFactor(float factor) {
    pImpl->smoothingFactor = factor;
}

float TimeSeriesView::getSmoothingFactor() const {
    return pImpl->smoothingFactor;
}

void TimeSeriesView::enableInterpolation(bool enabled) {
    pImpl->interpolationEnabled = enabled;
}

bool TimeSeriesView::isInterpolationEnabled() const {
    return pImpl->interpolationEnabled;
}

void TimeSeriesView::setTimeWindow(Timestamp start, Timestamp end) {
    pImpl->timeWindow = {start, end};
}

std::pair<Timestamp, Timestamp> TimeSeriesView::getTimeWindow() const {
    return pImpl->timeWindow;
}

void TimeSeriesView::enableRangeSelection(bool enabled) {
    pImpl->rangeSelectionEnabled = enabled;
}

bool TimeSeriesView::isRangeSelectionEnabled() const {
    return pImpl->rangeSelectionEnabled;
}

void TimeSeriesView::setRangeMin(float min) {
    pImpl->rangeMin = min;
}

float TimeSeriesView::getRangeMin() const {
    return pImpl->rangeMin;
}

void TimeSeriesView::setRangeMax(float max) {
    pImpl->rangeMax = max;
}

float TimeSeriesView::getRangeMax() const {
    return pImpl->rangeMax;
}

void TimeSeriesView::enableZoom(bool enabled) {
    pImpl->zoomEnabled = enabled;
}

bool TimeSeriesView::isZoomEnabled() const {
    return pImpl->zoomEnabled;
}

void TimeSeriesView::enablePan(bool enabled) {
    pImpl->panEnabled = enabled;
}

bool TimeSeriesView::isPanEnabled() const {
    return pImpl->panEnabled;
}

void TimeSeriesView::enableCursorLine(bool enabled) {
    pImpl->cursorLineEnabled = enabled;
}

bool TimeSeriesView::isCursorLineEnabled() const {
    return pImpl->cursorLineEnabled;
}

void TimeSeriesView::setCursorLineColor(float r, float g, float b, float a = 1.0f) {
    pImpl->cursorLineColor = {r, g, b, a};
}

std::array<float, 4> TimeSeriesView::getCursorLineColor() const {
    return pImpl->cursorLineColor;
}

void TimeSeriesView::enableStatisticsOverlay(bool enabled) {
    pImpl->statisticsOverlayEnabled = enabled;
}

bool TimeSeriesView::areStatisticsOverlayEnabled() const {
    return pImpl->statisticsOverlayEnabled;
}

void TimeSeriesView::reset() {
    setName("TimeSeriesView");
    enable(true);
    setTimeSeriesType("line");
    enableMultipleTraces(true);
    setTraceGrouping("by_neuron");
    setSmoothingFactor(0.1f);
    enableInterpolation(false);
    setTimeWindow(0.0, 100.0);
    enableRangeSelection(false);
    setRangeMin(0.0f);
    setRangeMax(1.0f);
    enableZoom(true);
    enablePan(true);
    enableCursorLine(true);
    setCursorLineColor(1.0f, 0.0f, 0.0f, 1.0f);
    enableStatisticsOverlay(true);
}

} // namespace nlm