#include "ActivityView.hpp"

namespace nlm {

struct ActivityView::Impl {
    std::string name;
    bool enabled;
    std::string heatmapType;
    bool colorScalingEnabled;
    std::string colorScale;
    std::string colormap;
    float threshold;
    bool normalizationEnabled;
    bool smoothingEnabled;
    float smoothingFactor;
    std::pair<Timestamp, Timestamp> timeWindow;
    bool realTimeUpdatesEnabled;
    double updateRate;
    bool spikeOverlayEnabled;
    bool weightOverlayEnabled;
    std::string neuronFilter;
    std::string regionFilter;
    float activityThreshold;
    
    Impl() : name("ActivityView"), enabled(true), heatmapType("heatmap"),
             colorScalingEnabled(true), colorScale("linear"), colormap("viridis"),
             threshold(0.5f), normalizationEnabled(true), smoothingEnabled(true),
             smoothingFactor(0.1f), timeWindow({0.0, 100.0}), realTimeUpdatesEnabled(true),
             updateRate(30.0), spikeOverlayEnabled(false), weightOverlayEnabled(false),
             neuronFilter("all"), regionFilter("all"), activityThreshold(0.1f) {}
};

ActivityView::ActivityView() : pImpl(std::make_unique<Impl>()) {}
ActivityView::~ActivityView() = default;

const std::string& ActivityView::getName() const {
    return pImpl->name;
}

void ActivityView::setName(const std::string& name) {
    pImpl->name = name;
}

void ActivityView::enable(bool enabled) {
    pImpl->enabled = enabled;
}

bool ActivityView::isEnabled() const {
    return pImpl->enabled;
}

void ActivityView::setHeatmapType(const std::string& type) {
    pImpl->heatmapType = type;
}

const std::string& ActivityView::getHeatmapType() const {
    return pImpl->heatmapType;
}

void ActivityView::enableColorScaling(bool enabled) {
    pImpl->colorScalingEnabled = enabled;
}

bool ActivityView::isColorScalingEnabled() const {
    return pImpl->colorScalingEnabled;
}

void ActivityView::setColorScale(const std::string& scale) {
    pImpl->colorScale = scale;
}

const std::string& ActivityView::getColorScale() const {
    return pImpl->colorScale;
}

void ActivityView::setColormap(const std::string& colormap) {
    pImpl->colormap = colormap;
}

const std::string& ActivityView::getColormap() const {
    return pImpl->colormap;
}

void ActivityView::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float ActivityView::getThreshold() const {
    return pImpl->threshold;
}

void ActivityView::enableNormalization(bool enabled) {
    pImpl->normalizationEnabled = enabled;
}

bool ActivityView::isNormalizationEnabled() const {
    return pImpl->normalizationEnabled;
}

void ActivityView::enableSmoothing(bool enabled) {
    pImpl->smoothingEnabled = enabled;
}

bool ActivityView::isSmoothingEnabled() const {
    return pImpl->smoothingEnabled;
}

void ActivityView::setSmoothingFactor(float factor) {
    pImpl->smoothingFactor = factor;
}

float ActivityView::getSmoothingFactor() const {
    return pImpl->smoothingFactor;
}

void ActivityView::setTimeWindow(Timestamp start, Timestamp end) {
    pImpl->timeWindow = {start, end};
}

std::pair<Timestamp, Timestamp> ActivityView::getTimeWindow() const {
    return pImpl->timeWindow;
}

void ActivityView::enableRealTimeUpdates(bool enabled) {
    pImpl->realTimeUpdatesEnabled = enabled;
}

bool ActivityView::areRealTimeUpdatesEnabled() const {
    return pImpl->realTimeUpdatesEnabled;
}

void ActivityView::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double ActivityView::getUpdateRate() const {
    return pImpl->updateRate;
}

void ActivityView::enableSpikeOverlay(bool enabled) {
    pImpl->spikeOverlayEnabled = enabled;
}

bool ActivityView::isSpikeOverlayEnabled() const {
    return pImpl->spikeOverlayEnabled;
}

void ActivityView::enableWeightOverlay(bool enabled) {
    pImpl->weightOverlayEnabled = enabled;
}

bool ActivityView::isWeightOverlayEnabled() const {
    return pImpl->weightOverlayEnabled;
}

void ActivityView::setNeuronFilter(const std::string& filter) {
    pImpl->neuronFilter = filter;
}

const std::string& ActivityView::getNeuronFilter() const {
    return pImpl->neuronFilter;
}

void ActivityView::setRegionFilter(const std::string& filter) {
    pImpl->regionFilter = filter;
}

const std::string& ActivityView::getRegionFilter() const {
    return pImpl->regionFilter;
}

void ActivityView::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = threshold;
}

float ActivityView::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

void ActivityView::reset() {
    setName("ActivityView");
    enable(true);
    setHeatmapType("heatmap");
    enableColorScaling(true);
    setColorScale("linear");
    setColormap("viridis");
    setThreshold(0.5f);
    enableNormalization(true);
    enableSmoothing(true);
    setSmoothingFactor(0.1f);
    setTimeWindow(0.0, 100.0);
    enableRealTimeUpdates(true);
    setUpdateRate(30.0);
    enableSpikeOverlay(false);
    enableWeightOverlay(false);
    setNeuronFilter("all");
    setRegionFilter("all");
    setActivityThreshold(0.1f);
}

} // namespace nlm