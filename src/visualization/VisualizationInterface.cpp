#include "VisualizationInterface.hpp"
#include <memory>

namespace nlm {

// Forward declaration of EnhancedVisualization
class EnhancedVisualization;

struct VisualizationInterface::Impl {
    std::unique_ptr<EnhancedVisualization> enhancedImpl;
    
    Impl() : enhancedImpl(std::make_unique<EnhancedVisualization>()) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    return pImpl->enhancedImpl->initialize(VisualizationConfig{});
}

void VisualizationInterface::update() {
    pImpl->enhancedImpl->update();
}

void VisualizationInterface::render() {
    pImpl->enhancedImpl->render();
}

void VisualizationInterface::close() {
    pImpl->enhancedImpl->close();
}

bool VisualizationInterface::isActive() const {
    return pImpl->enhancedImpl->isActive();
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->enhancedImpl->setUpdateRate(hz);
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->enhancedImpl->getUpdateRate();
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    pImpl->enhancedImpl->visualizeNetwork(brain);
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    pImpl->enhancedImpl->visualizeActivity(brain);
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    pImpl->enhancedImpl->visualizeSpikes(spikes);
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    pImpl->enhancedImpl->visualizeWeights(brain);
}

// Additional debugging and visualization methods (delegated to enhanced implementation)
void VisualizationInterface::visualizeNeuralActivityHeatMap(const Brain& brain) {
    pImpl->enhancedImpl->visualizeNeuralActivityHeatMap(brain);
}

void VisualizationInterface::visualizeSpikeTrains(const Brain& brain, size_t neuronsToShow) {
    pImpl->enhancedImpl->visualizeSpikeTrains(brain, neuronsToShow);
}

void VisualizationInterface::visualizeActivityPatterns(const Brain& brain) {
    pImpl->enhancedImpl->visualizeActivityPatterns(brain);
}

void VisualizationInterface::visualizeSynapticConnectivityGraph(const Brain& brain) {
    pImpl->enhancedImpl->visualizeSynapticConnectivityGraph(brain);
}

void VisualizationInterface::visualizeWeightMatrix(const Brain& brain) {
    pImpl->enhancedImpl->visualizeWeightMatrix(brain);
}

void VisualizationInterface::visualizePlasticityDynamics(const Brain& brain) {
    pImpl->enhancedImpl->visualizePlasticityDynamics(brain);
}

void VisualizationInterface::visualizeStructuralConnectivity(const Brain& brain) {
    pImpl->enhancedImpl->visualizeStructuralConnectivity(brain);
}

void VisualizationInterface::visualizeWorkingMemory(const Brain& brain) {
    pImpl->enhancedImpl->visualizeWorkingMemory(brain);
}

void VisualizationInterface::visualizeEpisodicMemory(const Brain& brain) {
    pImpl->enhancedImpl->visualizeEpisodicMemory(brain);
}

void VisualizationInterface::visualizeAssociativeMemory(const Brain& brain) {
    pImpl->enhancedImpl->visualizeAssociativeMemory(brain);
}

void VisualizationInterface::visualizeMemoryConsolidation(const Brain& brain) {
    pImpl->enhancedImpl->visualizeMemoryConsolidation(brain);
}

void VisualizationInterface::visualizeDopamineSignals(const Brain& brain) {
    pImpl->enhancedImpl->visualizeDopamineSignals(brain);
}

void VisualizationInterface::visualizeCuriositySignals(const Brain& brain) {
    pImpl->enhancedImpl->visualizeCuriositySignals(brain);
}

void VisualizationInterface::visualizeNoveltySignals(const Brain& brain) {
    pImpl->enhancedImpl->visualizeNoveltySignals(brain);
}

void VisualizationInterface::visualizeNeuromodulationNetwork(const Brain& brain) {
    pImpl->enhancedImpl->visualizeNeuromodulationNetwork(brain);
}

void VisualizationInterface::monitorBrainState(const Brain& brain) {
    pImpl->enhancedImpl->monitorBrainState(brain);
}

void VisualizationInterface::trackDevelopmentStages(const Brain& brain) {
    pImpl->enhancedImpl->trackDevelopmentStages(brain);
}

void VisualizationInterface::visualizePredictionSystem(const Brain& brain) {
    pImpl->enhancedImpl->visualizePredictionSystem(brain);
}

void VisualizationInterface::visualizeCognitiveFunctions(const Brain& brain) {
    pImpl->enhancedImpl->visualizeCognitiveFunctions(brain);
}

void VisualizationInterface::profilePerformance(const Brain& brain) {
    pImpl->enhancedImpl->profilePerformance(brain);
}

void VisualizationInterface::detectMemoryLeaks(const Brain& brain) {
    pImpl->enhancedImpl->detectMemoryLeaks(brain);
}

void VisualizationInterface::analyzePlasticityEfficiency(const Brain& brain) {
    pImpl->enhancedImpl->analyzePlasticityEfficiency(brain);
}

void VisualizationInterface::optimizeRendering(const Brain& brain) {
    pImpl->enhancedImpl->optimizeRendering(brain);
}

void VisualizationInterface::logEvent(const std::string& eventType, const std::string& description,
                                     float value, const std::vector<std::string>& tags) {
    pImpl->enhancedImpl->logEvent(eventType, description, value, tags);
}

void VisualizationInterface::visualizeEventTimeline(const Brain& brain) {
    pImpl->enhancedImpl->visualizeEventTimeline(brain);
}

void VisualizationInterface::analyzeEventPatterns(const Brain& brain) {
    pImpl->enhancedImpl->analyzeEventPatterns(brain);
}

void VisualizationInterface::integrateWithExperiments(const Brain& brain) {
    pImpl->enhancedImpl->integrateWithExperiments(brain);
}

void VisualizationInterface::generateExperimentReports(const Brain& brain) {
    pImpl->enhancedImpl->generateExperimentReports(brain);
}

void VisualizationInterface::createRealTimeDashboards(const Brain& brain) {
    pImpl->enhancedImpl->createRealTimeDashboards(brain);
}

void VisualizationInterface::exportToAscii(const std::string& filename, const Brain& brain) {
    pImpl->enhancedImpl->exportToAscii(filename, brain);
}

void VisualizationInterface::exportToJson(const std::string& filename, const Brain& brain) {
    pImpl->enhancedImpl->exportToJson(filename, brain);
}

void VisualizationInterface::exportToCsv(const std::string& filename, const Brain& brain) {
    pImpl->enhancedImpl->exportToCsv(filename, brain);
}

void VisualizationInterface::exportToSvg(const std::string& filename, const Brain& brain) {
    pImpl->enhancedImpl->exportToSvg(filename, brain);
}

void VisualizationInterface::exportToBinary(const std::string& filename, const Brain& brain) {
    pImpl->enhancedImpl->exportToBinary(filename, brain);
}

void VisualizationInterface::batchProcess(const std::vector<std::shared_ptr<Brain>>& brains) {
    pImpl->enhancedImpl->batchProcess(brains);
}

void VisualizationInterface::processLargeScaleSimulation(const Brain& brain, size_t steps) {
    pImpl->enhancedImpl->processLargeScaleSimulation(brain, steps);
}

void VisualizationInterface::streamVisualization(const Brain& brain) {
    pImpl->enhancedImpl->streamVisualization(brain);
}

void VisualizationInterface::registerPlugin(std::function<void(const Brain&)> plugin) {
    pImpl->enhancedImpl->registerPlugin(plugin);
}

void VisualizationInterface::unregisterPlugin(std::function<void(const Brain&)> plugin) {
    pImpl->enhancedImpl->unregisterPlugin(plugin);
}

void VisualizationInterface::runPlugins(const Brain& brain) {
    pImpl->enhancedImpl->runPlugins(brain);
}

void VisualizationInterface::updateConfig(const VisualizationConfig& config) {
    pImpl->enhancedImpl->updateConfig(config);
}

const VisualizationConfig& VisualizationInterface::getConfig() const {
    return pImpl->enhancedImpl->getConfig();
}

void VisualizationInterface::clearHistory() {
    pImpl->enhancedImpl->clearHistory();
}

void VisualizationInterface::setHistorySize(size_t size) {
    pImpl->enhancedImpl->setHistorySize(size);
}

size_t VisualizationInterface::getHistorySize() const {
    return pImpl->enhancedImpl->getHistorySize();
}

void VisualizationInterface::startRealTimeMonitoring() {
    pImpl->enhancedImpl->startRealTimeMonitoring();
}

void VisualizationInterface::stopRealTimeMonitoring() {
    pImpl->enhancedImpl->stopRealTimeMonitoring();
}

bool VisualizationInterface::isRealTimeMonitoringActive() const {
    return pImpl->enhancedImpl->isRealTimeMonitoringActive();
}

// Additional debugging methods
void VisualizationInterface::analyzeNeuralDynamics(const Brain& brain) {
    pImpl->enhancedImpl->analyzeNeuralDynamics(brain);
}

void VisualizationInterface::analyzeMemoryPatterns(const Brain& brain) {
    pImpl->enhancedImpl->analyzeMemoryPatterns(brain);
}

void VisualizationInterface::analyzePlasticityTrajectories(const Brain& brain) {
    pImpl->enhancedImpl->analyzePlasticityTrajectories(brain);
}

void VisualizationInterface::analyzeDevelopmentTrajectories(const Brain& brain) {
    pImpl->enhancedImpl->analyzeDevelopmentTrajectories(brain);
}

void VisualizationInterface::analyzePerformanceBottlenecks(const Brain& brain) {
    pImpl->enhancedImpl->analyzePerformanceBottlenecks(brain);
}

void VisualizationInterface::performParameterSensitivityAnalysis(
    const Brain& brain, const std::map<std::string, float>& parameters) {
    pImpl->enhancedImpl->performParameterSensitivityAnalysis(brain, parameters);
}

void VisualizationInterface::analyzeConfigImpact(const VisualizationConfig& config, const Brain& brain) {
    pImpl->enhancedImpl->analyzeConfigImpact(config, brain);
}

void VisualizationInterface::integrateWithExperimentRunner(
    const std::shared_ptr<ExperimentRunner>& runner) {
    pImpl->enhancedImpl->integrateWithExperimentRunner(runner);
}

void VisualizationInterface::integrateWithMetricsSystem(const std::shared_ptr<Metrics>& metrics) {
    pImpl->enhancedImpl->integrateWithMetricsSystem(metrics);
}

} // namespace nlm
