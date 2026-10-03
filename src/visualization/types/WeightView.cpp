#include "WeightView.hpp"

namespace nlm {

struct WeightView::Impl {
    std::string name;
    bool enabled;
    std::string matrixType;
    bool colorMappingEnabled;
    float weightScale;
    float threshold;
    bool normalizationEnabled;
    bool smoothingEnabled;
    float smoothingFactor;
    bool adjacencyMatrixEnabled;
    bool connectivityMapEnabled;
    bool plasticityVisualizationEnabled;
    float plasticityStrength;
    std::pair<Timestamp, Timestamp> timeWindow;
    
    Impl() : name("WeightView"), enabled(true), matrixType("heatmap"),
             colorMappingEnabled(true), weightScale(1.0f), threshold(0.5f),
             normalizationEnabled(true), smoothingEnabled(true), smoothingFactor(0.1f),
             adjacencyMatrixEnabled(false), connectivityMapEnabled(false),
             plasticityVisualizationEnabled(false), plasticityStrength(1.0f),
             timeWindow({0.0, 100.0}) {}
};

WeightView::WeightView() : pImpl(std::make_unique<Impl>()) {}
WeightView::~WeightView() = default;

const std::string& WeightView::getName() const {
    return pImpl->name;
}

void WeightView::setName(const std::string& name) {
    pImpl->name = name;
}

void WeightView::enable(bool enabled) {
    pImpl->enabled = enabled;
}

bool WeightView::isEnabled() const {
    return pImpl->enabled;
}

void WeightView::setMatrixType(const std::string& type) {
    pImpl->matrixType = type;
}

const std::string& WeightView::getMatrixType() const {
    return pImpl->matrixType;
}

void WeightView::enableColorMapping(bool enabled) {
    pImpl->colorMappingEnabled = enabled;
}

bool WeightView::isColorMappingEnabled() const {
    return pImpl->colorMappingEnabled;
}

void WeightView::setWeightScale(float scale) {
    pImpl->weightScale = scale;
}

float WeightView::getWeightScale() const {
    return pImpl->weightScale;
}

void WeightView::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float WeightView::getThreshold() const {
    return pImpl->threshold;
}

void WeightView::enableNormalization(bool enabled) {
    pImpl->normalizationEnabled = enabled;
}

bool WeightView::isNormalizationEnabled() const {
    return pImpl->normalizationEnabled;
}

void WeightView::enableSmoothing(bool enabled) {
    pImpl->smoothingEnabled = enabled;
}

bool WeightView::isSmoothingEnabled() const {
    return pImpl->smoothingEnabled;
}

void WeightView::setSmoothingFactor(float factor) {
    pImpl->smoothingFactor = factor;
}

float WeightView::getSmoothingFactor() const {
    return pImpl->smoothingFactor;
}

void WeightView::enableAdjacencyMatrix(bool enabled) {
    pImpl->adjacencyMatrixEnabled = enabled;
}

bool WeightView::isAdjacencyMatrixEnabled() const {
    return pImpl->adjacencyMatrixEnabled;
}

void WeightView::enableConnectivityMap(bool enabled) {
    pImpl->connectivityMapEnabled = enabled;
}

bool WeightView::isConnectivityMapEnabled() const {
    return pImpl->connectivityMapEnabled;
}

void WeightView::enablePlasticityVisualization(bool enabled) {
    pImpl->plasticityVisualizationEnabled = enabled;
}

bool WeightView::isPlasticityVisualizationEnabled() const {
    return pImpl->plasticityVisualizationEnabled;
}

void WeightView::setPlasticityStrength(float strength) {
    pImpl->plasticityStrength = strength;
}

float WeightView::getPlasticityStrength() const {
    return pImpl->plasticityStrength;
}

void WeightView::setTimeWindow(Timestamp start, Timestamp end) {
    pImpl->timeWindow = {start, end};
}

std::pair<Timestamp, Timestamp> WeightView::getTimeWindow() const {
    return pImpl->timeWindow;
}

void WeightView::reset() {
    setName("WeightView");
    enable(true);
    setMatrixType("heatmap");
    enableColorMapping(true);
    setWeightScale(1.0f);
    setThreshold(0.5f);
    enableNormalization(true);
    enableSmoothing(true);
    setSmoothingFactor(0.1f);
    enableAdjacencyMatrix(false);
    enableConnectivityMap(false);
    enablePlasticityVisualization(false);
    setPlasticityStrength(1.0f);
    setTimeWindow(0.0, 100.0);
}

} // namespace nlm