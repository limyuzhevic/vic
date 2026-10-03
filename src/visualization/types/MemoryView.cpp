#include "MemoryView.hpp"

namespace nlm {

struct MemoryView::Impl {
    std::string name;
    bool enabled;
    std::string memoryType;
    bool heatmapEnabled;
    bool traceVisualizationEnabled;
    float traceOpacity;
    bool correlationMatrixEnabled;
    float correlationThreshold;
    bool patternDetectionEnabled;
    float patternSensitivity;
    std::pair<Timestamp, Timestamp> timeWindow;
    bool memoryReplayEnabled;
    float replaySpeed;
    bool memoryReconstructionEnabled;
    
    Impl() : name("MemoryView"), enabled(true), memoryType("working"),
             heatmapEnabled(true), traceVisualizationEnabled(true),
             traceOpacity(0.7f), correlationMatrixEnabled(false),
             correlationThreshold(0.8f), patternDetectionEnabled(false),
             patternSensitivity(0.5f), timeWindow({0.0, 100.0}),
             memoryReplayEnabled(false), replaySpeed(1.0f),
             memoryReconstructionEnabled(false) {}
};

MemoryView::MemoryView() : pImpl(std::make_unique<Impl>()) {}
MemoryView::~MemoryView() = default;

const std::string& MemoryView::getName() const {
    return pImpl->name;
}

void MemoryView::setName(const std::string& name) {
    pImpl->name = name;
}

void MemoryView::enable(bool enabled) {
    pImpl->enabled = enabled;
}

bool MemoryView::isEnabled() const {
    return pImpl->enabled;
}

void MemoryView::setMemoryType(const std::string& type) {
    pImpl->memoryType = type;
}

const std::string& MemoryView::getMemoryType() const {
    return pImpl->memoryType;
}

void MemoryView::enableHeatmap(bool enabled) {
    pImpl->heatmapEnabled = enabled;
}

bool MemoryView::isHeatmapEnabled() const {
    return pImpl->heatmapEnabled;
}

void MemoryView::enableTraceVisualization(bool enabled) {
    pImpl->traceVisualizationEnabled = enabled;
}

bool MemoryView::isTraceVisualizationEnabled() const {
    return pImpl->traceVisualizationEnabled;
}

void MemoryView::setTraceOpacity(float opacity) {
    pImpl->traceOpacity = opacity;
}

float MemoryView::getTraceOpacity() const {
    return pImpl->traceOpacity;
}

void MemoryView::enableCorrelationMatrix(bool enabled) {
    pImpl->correlationMatrixEnabled = enabled;
}

bool MemoryView::isCorrelationMatrixEnabled() const {
    return pImpl->correlationMatrixEnabled;
}

void MemoryView::setCorrelationThreshold(float threshold) {
    pImpl->correlationThreshold = threshold;
}

float MemoryView::getCorrelationThreshold() const {
    return pImpl->correlationThreshold;
}

void MemoryView::enablePatternDetection(bool enabled) {
    pImpl->patternDetectionEnabled = enabled;
}

bool MemoryView::isPatternDetectionEnabled() const {
    return pImpl->patternDetectionEnabled;
}

void MemoryView::setPatternSensitivity(float sensitivity) {
    pImpl->patternSensitivity = sensitivity;
}

float MemoryView::getPatternSensitivity() const {
    return pImpl->patternSensitivity;
}

void MemoryView::setTimeWindow(Timestamp start, Timestamp end) {
    pImpl->timeWindow = {start, end};
}

std::pair<Timestamp, Timestamp> MemoryView::getTimeWindow() const {
    return pImpl->timeWindow;
}

void MemoryView::enableMemoryReplay(bool enabled) {
    pImpl->memoryReplayEnabled = enabled;
}

bool MemoryView::isMemoryReplayEnabled() const {
    return pImpl->memoryReplayEnabled;
}

void MemoryView::setReplaySpeed(float speed) {
    pImpl->replaySpeed = speed;
}

float MemoryView::getReplaySpeed() const {
    return pImpl->replaySpeed;
}

void MemoryView::enableMemoryReconstruction(bool enabled) {
    pImpl->memoryReconstructionEnabled = enabled;
}

bool MemoryView::isMemoryReconstructionEnabled() const {
    return pImpl->memoryReconstructionEnabled;
}

void MemoryView::reset() {
    setName("MemoryView");
    enable(true);
    setMemoryType("working");
    enableHeatmap(true);
    enableTraceVisualization(true);
    setTraceOpacity(0.7f);
    enableCorrelationMatrix(false);
    setCorrelationThreshold(0.8f);
    enablePatternDetection(false);
    setPatternSensitivity(0.5f);
    setTimeWindow(0.0, 100.0);
    enableMemoryReplay(false);
    setReplaySpeed(1.0f);
    enableMemoryReconstruction(false);
}

} // namespace nlm