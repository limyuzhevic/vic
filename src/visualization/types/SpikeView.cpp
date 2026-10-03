#include "SpikeView.hpp"

namespace nlm {

struct SpikeView::Impl {
    std::string name;
    bool enabled;
    std::string rasterPlotType;
    bool spikeTimeLinesEnabled;
    bool spikeMarkersEnabled;
    float markerSize;
    std::array<float, 4> markerColor;
    float lineWidth;
    bool clusteringEnabled;
    int clusterCount;
    float firingRateThreshold;
    bool burstDetectionEnabled;
    float burstSensitivity;
    std::pair<Timestamp, Timestamp> timeWindow;
    bool autoScaleEnabled;
    bool spikeInterpolationEnabled;
    
    Impl() : name("SpikeView"), enabled(true), rasterPlotType("raster"),
             spikeTimeLinesEnabled(true), spikeMarkersEnabled(true), markerSize(2.0f),
             markerColor({1.0f, 0.0f, 0.0f, 1.0f}), lineWidth(1.0f), clusteringEnabled(false),
             clusterCount(5), firingRateThreshold(10.0f), burstDetectionEnabled(false),
             burstSensitivity(0.5f), timeWindow({0.0, 100.0}), autoScaleEnabled(true),
             spikeInterpolationEnabled(false) {}
};

SpikeView::SpikeView() : pImpl(std::make_unique<Impl>()) {}
SpikeView::~SpikeView() = default;

const std::string& SpikeView::getName() const {
    return pImpl->name;
}

void SpikeView::setName(const std::string& name) {
    pImpl->name = name;
}

void SpikeView::enable(bool enabled) {
    pImpl->enabled = enabled;
}

bool SpikeView::isEnabled() const {
    return pImpl->enabled;
}

void SpikeView::setRasterPlotType(const std::string& type) {
    pImpl->rasterPlotType = type;
}

const std::string& SpikeView::getRasterPlotType() const {
    return pImpl->rasterPlotType;
}

void SpikeView::enableSpikeTimeLines(bool enabled) {
    pImpl->spikeTimeLinesEnabled = enabled;
}

bool SpikeView::areSpikeTimeLinesEnabled() const {
    return pImpl->spikeTimeLinesEnabled;
}

void SpikeView::enableSpikeMarkers(bool enabled) {
    pImpl->spikeMarkersEnabled = enabled;
}

bool SpikeView::areSpikeMarkersEnabled() const {
    return pImpl->spikeMarkersEnabled;
}

void SpikeView::setMarkerSize(float size) {
    pImpl->markerSize = size;
}

float SpikeView::getMarkerSize() const {
    return pImpl->markerSize;
}

void SpikeView::setMarkerColor(float r, float g, float b, float a = 1.0f) {
    pImpl->markerColor = {r, g, b, a};
}

std::array<float, 4> SpikeView::getMarkerColor() const {
    return pImpl->markerColor;
}

void SpikeView::setLineWidth(float width) {
    pImpl->lineWidth = width;
}

float SpikeView::getLineWidth() const {
    return pImpl->lineWidth;
}

void SpikeView::enableClustering(bool enabled) {
    pImpl->clusteringEnabled = enabled;
}

bool SpikeView::isClusteringEnabled() const {
    return pImpl->clusteringEnabled;
}

void SpikeView::setClusterCount(int count) {
    pImpl->clusterCount = count;
}

int SpikeView::getClusterCount() const {
    return pImpl->clusterCount;
}

void SpikeView::setFiringRateThreshold(float threshold) {
    pImpl->firingRateThreshold = threshold;
}

float SpikeView::getFiringRateThreshold() const {
    return pImpl->firingRateThreshold;
}

void SpikeView::enableBurstDetection(bool enabled) {
    pImpl->burstDetectionEnabled = enabled;
}

bool SpikeView::isBurstDetectionEnabled() const {
    return pImpl->burstDetectionEnabled;
}

void SpikeView::setBurstSensitivity(float sensitivity) {
    pImpl->burstSensitivity = sensitivity;
}

float SpikeView::getBurstSensitivity() const {
    return pImpl->burstSensitivity;
}

void SpikeView::setTimeWindow(Timestamp start, Timestamp end) {
    pImpl->timeWindow = {start, end};
}

std::pair<Timestamp, Timestamp> SpikeView::getTimeWindow() const {
    return pImpl->timeWindow;
}

void SpikeView::enableAutoScale(bool enabled) {
    pImpl->autoScaleEnabled = enabled;
}

bool SpikeView::isAutoScaleEnabled() const {
    return pImpl->autoScaleEnabled;
}

void SpikeView::enableSpikeInterpolation(bool enabled) {
    pImpl->spikeInterpolationEnabled = enabled;
}

bool SpikeView::isSpikeInterpolationEnabled() const {
    return pImpl->spikeInterpolationEnabled;
}

void SpikeView::reset() {
    setName("SpikeView");
    enable(true);
    setRasterPlotType("raster");
    enableSpikeTimeLines(true);
    enableSpikeMarkers(true);
    setMarkerSize(2.0f);
    setMarkerColor(1.0f, 0.0f, 0.0f, 1.0f);
    setLineWidth(1.0f);
    enableClustering(false);
    setClusterCount(5);
    setFiringRateThreshold(10.0f);
    enableBurstDetection(false);
    setBurstSensitivity(0.5f);
    setTimeWindow(0.0, 100.0);
    enableAutoScale(true);
    enableSpikeInterpolation(false);
}

} // namespace nlm