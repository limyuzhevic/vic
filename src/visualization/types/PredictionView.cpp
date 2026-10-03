#include "PredictionView.hpp"

namespace nlm {

struct PredictionView::Impl {
    std::string name;
    bool enabled;
    std::string predictionType;
    bool confidenceVisualizationEnabled;
    bool errorVisualizationEnabled;
    float confidenceThreshold;
    float errorThreshold;
    bool probabilityHeatmapEnabled;
    bool predictionTrajectoryEnabled;
    float trajectorySmoothness;
    std::pair<Timestamp, Timestamp> timeWindow;
    bool uncertaintyVisualizationEnabled;
    float uncertaintyThreshold;
    
    Impl() : name("PredictionView"), enabled(true), predictionType("confidence"),
             confidenceVisualizationEnabled(true), errorVisualizationEnabled(true),
             confidenceThreshold(0.7f), errorThreshold(0.3f),
             probabilityHeatmapEnabled(true), predictionTrajectoryEnabled(true),
             trajectorySmoothness(0.1f), timeWindow({0.0, 100.0}),
             uncertaintyVisualizationEnabled(true), uncertaintyThreshold(0.5f) {}
};

PredictionView::PredictionView() : pImpl(std::make_unique<Impl>()) {}
PredictionView::~PredictionView() = default;

const std::string& PredictionView::getName() const {
    return pImpl->name;
}

void PredictionView::setName(const std::string& name) {
    pImpl->name = name;
}

void PredictionView::enable(bool enabled) {
    pImpl->enabled = enabled;
}

bool PredictionView::isEnabled() const {
    return pImpl->enabled;
}

void PredictionView::setPredictionType(const std::string& type) {
    pImpl->predictionType = type;
}

const std::string& PredictionView::getPredictionType() const {
    return pImpl->predictionType;
}

void PredictionView::enableConfidenceVisualization(bool enabled) {
    pImpl->confidenceVisualizationEnabled = enabled;
}

bool PredictionView::isConfidenceVisualizationEnabled() const {
    return pImpl->confidenceVisualizationEnabled;
}

void PredictionView::enableErrorVisualization(bool enabled) {
    pImpl->errorVisualizationEnabled = enabled;
}

bool PredictionView::isErrorVisualizationEnabled() const {
    return pImpl->errorVisualizationEnabled;
}

void PredictionView::setConfidenceThreshold(float threshold) {
    pImpl->confidenceThreshold = threshold;
}

float PredictionView::getConfidenceThreshold() const {
    return pImpl->confidenceThreshold;
}

void PredictionView::setErrorThreshold(float threshold) {
    pImpl->errorThreshold = threshold;
}

float PredictionView::getErrorThreshold() const {
    return pImpl->errorThreshold;
}

void PredictionView::enableProbabilityHeatmap(bool enabled) {
    pImpl->probabilityHeatmapEnabled = enabled;
}

bool PredictionView::isProbabilityHeatmapEnabled() const {
    return pImpl->probabilityHeatmapEnabled;
}

void PredictionView::enablePredictionTrajectory(bool enabled) {
    pImpl->predictionTrajectoryEnabled = enabled;
}

bool PredictionView::isPredictionTrajectoryEnabled() const {
    return pImpl->predictionTrajectoryEnabled;
}

void PredictionView::setTrajectorySmoothness(float smoothness) {
    pImpl->trajectorySmoothness = smoothness;
}

float PredictionView::getTrajectorySmoothness() const {
    return pImpl->trajectorySmoothness;
}

void PredictionView::setTimeWindow(Timestamp start, Timestamp end) {
    pImpl->timeWindow = {start, end};
}

std::pair<Timestamp, Timestamp> PredictionView::getTimeWindow() const {
    return pImpl->timeWindow;
}

void PredictionView::enableUncertaintyVisualization(bool enabled) {
    pImpl->uncertaintyVisualizationEnabled = enabled;
}

bool PredictionView::isUncertaintyVisualizationEnabled() const {
    return pImpl->uncertaintyVisualizationEnabled;
}

void PredictionView::setUncertaintyThreshold(float threshold) {
    pImpl->uncertaintyThreshold = threshold;
}

float PredictionView::getUncertaintyThreshold() const {
    return pImpl->uncertaintyThreshold;
}

void PredictionView::reset() {
    setName("PredictionView");
    enable(true);
    setPredictionType("confidence");
    enableConfidenceVisualization(true);
    enableErrorVisualization(true);
    setConfidenceThreshold(0.7f);
    setErrorThreshold(0.3f);
    enableProbabilityHeatmap(true);
    enablePredictionTrajectory(true);
    setTrajectorySmoothness(0.1f);
    setTimeWindow(0.0, 100.0);
    enableUncertaintyVisualization(true);
    setUncertaintyThreshold(0.5f);
}

} // namespace nlm