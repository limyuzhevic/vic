#include "ConsoleRenderer.hpp"
#include <iomanip>
#include <sstream>

namespace nlm {

struct ConsoleRenderer::Impl {
    std::ostream* outputStream;
    bool colorsEnabled;
    bool verboseEnabled;
    std::string outputFormat;
    bool lineWrappingEnabled;
    int lineWidth;
    bool progressIndicatorsEnabled;
    std::string progressIndicatorStyle;
    bool autoResizeEnabled;
    int consoleWidth;
    int consoleHeight;
    
    Impl() : outputStream(nullptr), colorsEnabled(true), verboseEnabled(false),
             outputFormat("text"), lineWrappingEnabled(true), lineWidth(80),
             progressIndicatorsEnabled(true), progressIndicatorStyle("bar"),
             autoResizeEnabled(true), consoleWidth(80), consoleHeight(24) {
        outputStream = &std::cout;
    }
};

ConsoleRenderer::ConsoleRenderer() : pImpl(std::make_unique<Impl>()) {}
ConsoleRenderer::~ConsoleRenderer() = default;

bool ConsoleRenderer::initialize() {
    // Initialize console rendering
    return true;
}

void ConsoleRenderer::shutdown() {
    // Shutdown console rendering
}

void ConsoleRenderer::update(double deltaTime) {
    // Update console rendering
}

bool ConsoleRenderer::render(const std::map<std::string, VisualizationData>& data) {
    if (pImpl->outputStream) {
        if (pImpl->verboseEnabled) {
            // Render all data types
            for (const auto& pair : data) {
                *pImpl->outputStream << "=== " << pair.first << " ===" << std::endl;
                if (pair.first == "network") {
                    renderNetwork(pair.second.network);
                } else if (pair.first == "activity") {
                    renderActivity(pair.second.activity);
                } else if (pair.first == "spikes") {
                    renderSpikes(pair.second.spikes);
                } else if (pair.first == "weights") {
                    renderWeights(pair.second.weights);
                } else if (pair.first == "timeSeries") {
                    renderTimeSeries(pair.second.timeSeries);
                } else if (pair.first == "memory") {
                    renderMemory(pair.second.memory);
                } else if (pair.first == "prediction") {
                    renderPrediction(pair.second.prediction);
                } else if (pair.first == "simulation") {
                    renderSimulation(pair.second.simulation);
                } else if (pair.first == "performance") {
                    renderPerformance(pair.second.performance);
                }
            }
        } else {
            // Render summary
            if (data.find("simulation") != data.end()) {
                renderSimulation(data.at("simulation"));
            }
            if (data.find("performance") != data.end()) {
                renderPerformance(data.at("performance"));
            }
        }
        *pImpl->outputStream << std::endl;
    }
    return true;
}

bool ConsoleRenderer::exportToConsole(const std::string& filepath, const std::map<std::string, VisualizationData>& data) {
    std::ofstream file(filepath);
    if (file.is_open()) {
        setOutputStream(file);
        render(data);
        setOutputStream(std::cout);
        return true;
    }
    return false;
}

void ConsoleRenderer::setOutputStream(std::ostream& stream) {
    pImpl->outputStream = &stream;
}

std::ostream& ConsoleRenderer::getOutputStream() {
    return *pImpl->outputStream;
}

void ConsoleRenderer::enableColors(bool enabled) {
    pImpl->colorsEnabled = enabled;
}

bool ConsoleRenderer::areColorsEnabled() const {
    return pImpl->colorsEnabled;
}

void ConsoleRenderer::enableVerbose(bool enabled) {
    pImpl->verboseEnabled = enabled;
}

bool ConsoleRenderer::isVerboseEnabled() const {
    return pImpl->verboseEnabled;
}

void ConsoleRenderer::setOutputFormat(const std::string& format) {
    pImpl->outputFormat = format;
}

const std::string& ConsoleRenderer::getOutputFormat() const {
    return pImpl->outputFormat;
}

void ConsoleRenderer::enableLineWrapping(bool enabled) {
    pImpl->lineWrappingEnabled = enabled;
}

bool ConsoleRenderer::isLineWrappingEnabled() const {
    return pImpl->lineWrappingEnabled;
}

void ConsoleRenderer::setLineWidth(int width) {
    pImpl->lineWidth = width;
}

int ConsoleRenderer::getLineWidth() const {
    return pImpl->lineWidth;
}

void ConsoleRenderer::enableProgressIndicators(bool enabled) {
    pImpl->progressIndicatorsEnabled = enabled;
}

bool ConsoleRenderer::areProgressIndicatorsEnabled() const {
    return pImpl->progressIndicatorsEnabled;
}

void ConsoleRenderer::setProgressIndicatorStyle(const std::string& style) {
    pImpl->progressIndicatorStyle = style;
}

const std::string& ConsoleRenderer::getProgressIndicatorStyle() const {
    return pImpl->progressIndicatorStyle;
}

void ConsoleRenderer::renderNetwork(const VisualizationData::Network& network) {
    if (pImpl->verboseEnabled) {
        *pImpl->outputStream << "Network:".bold_green() << std::endl;
        *pImpl->outputStream << "  Neurons: " << network.neurons.size() << std::endl;
        *pImpl->outputStream << "  Connections: " << network.connections.size() << std::endl;
        *pImpl->outputStream << "  Regions: " << std::set<int>(network.regionIds.begin(), network.regionIds.end()).size() << std::endl;
    }
}

void ConsoleRenderer::renderActivity(const VisualizationData::Activity& activity) {
    if (pImpl->verboseEnabled) {
        *pImpl->outputStream << "Activity:".bold_green() << std::endl;
        *pImpl->outputStream << "  Neurons with activity: " << activity.neuronIds.size() << std::endl;
        *pImpl->outputStream << "  Heatmap size: " << activity.heatmaps.size() << " x " << (activity.heatmaps.empty() ? 0 : activity.heatmaps[0].size()) << std::endl;
    }
}

void ConsoleRenderer::renderSpikes(const VisualizationData::Spikes& spikes) {
    if (pImpl->verboseEnabled) {
        *pImpl->outputStream << "Spikes:".bold_green() << std::endl;
        *pImpl->outputStream << "  Spike events: " << spikes.spikeEvents.size() << std::endl;
        *pImpl->outputStream << "  Neuron spike history entries: " << spikes.neuronSpikeHistory.size() << std::endl;
    }
}

void ConsoleRenderer::renderWeights(const VisualizationData::Weights& weights) {
    if (pImpl->verboseEnabled) {
        *pImpl->outputStream << "Weights:".bold_green() << std::endl;
        *pImpl->outputStream << "  Weight matrix entries: " << weights.weightMatrix.size() << std::endl;
        *pImpl->outputStream << "  Adjacency matrix size: " << weights.adjacencyMatrix.size() << " x " << (weights.adjacencyMatrix.empty() ? 0 : weights.adjacencyMatrix[0].size()) << std::endl;
    }
}

void ConsoleRenderer::renderTimeSeries(const VisualizationData::TimeSeries& timeSeries) {
    if (pImpl->verboseEnabled) {
        *pImpl->outputStream << "Time Series:".bold_green() << std::endl;
        *pImpl->outputStream << "  Time points: " << timeSeries.timestamps.size() << std::endl;
        *pImpl->outputStream << "  Neuron activity entries: " << timeSeries.neuronActivity.size() << std::endl;
        *pImpl->outputStream << "  Region activity entries: " << timeSeries.regionActivity.size() << std::endl;
        *pImpl->outputStream << "  Population activity entries: " << timeSeries.populationActivity.size() << std::endl;
    }
}

void ConsoleRenderer::renderMemory(const VisualizationData::Memory& memory) {
    if (pImpl->verboseEnabled) {
        *pImpl->outputStream << "Memory:".bold_green() << std::endl;
        *pImpl->outputStream << "  Memory entries: " << memory.memoryEntries.size() << std::endl;
        *pImpl->outputStream << "  Memory heatmaps: " << memory.memoryHeatmaps.size() << " x " << (memory.memoryHeatmaps.empty() ? 0 : memory.memoryHeatmaps[0].size()) << std::endl;
    }
}

void ConsoleRenderer::renderPrediction(const VisualizationData::Prediction& prediction) {
    if (pImpl->verboseEnabled) {
        *pImpl->outputStream << "Prediction:".bold_green() << std::endl;
        *pImpl->outputStream << "  Confidence maps: " << prediction.confidenceMaps.size() << " x " << (prediction.confidenceMaps.empty() ? 0 : prediction.confidenceMaps[0].size()) << std::endl;
        *pImpl->outputStream << "  Error maps: " << prediction.errorMaps.size() << " x " << (prediction.errorMaps.empty() ? 0 : prediction.errorMaps[0].size()) << std::endl;
        *pImpl->outputStream << "  Prediction probabilities: " << prediction.predictionProbabilities.size() << " x " << (prediction.predictionProbabilities.empty() ? 0 : prediction.predictionProbabilities[0].size()) << std::endl;
    }
}

void ConsoleRenderer::renderSimulation(const VisualizationData::Simulation& simulation) {
    *pImpl->outputStream << "Simulation: Step " << simulation.currentStep << ", Time " << simulation.currentTime << std::endl;
    *pImpl->outputStream << "  Total spikes: " << simulation.totalSpikeCount << ", Active neurons: " << simulation.activeNeuronCount << std::endl;
}

void ConsoleRenderer::renderPerformance(const VisualizationData::Performance& performance) {
    *pImpl->outputStream << "Performance: Frame time " << performance.frameTime << "ms, Render time " << performance.renderTime << "ms" << std::endl;
    *pImpl->outputStream << "  Vertices: " << performance.vertexCount << ", Triangles: " << performance.triangleCount << std::endl;
    *pImpl->outputStream << "  Memory usage: " << performance.memoryUsage << "MB, CPU usage: " << performance.cpuUsage << "%" << std::endl;
}

void ConsoleRenderer::clear() {
    if (pImpl->outputStream) {
        *pImpl->outputStream << "\033[2J\033[H";  // Clear screen (ANSI escape codes)
    }
}

std::pair<int, int> ConsoleRenderer::getConsoleSize() const {
    return {pImpl->consoleWidth, pImpl->consoleHeight};
}

void ConsoleRenderer::setConsoleSize(int width, int height) {
    pImpl->consoleWidth = width;
    pImpl->consoleHeight = height;
}

void ConsoleRenderer::enableAutoResize(bool enabled) {
    pImpl->autoResizeEnabled = enabled;
}

bool ConsoleRenderer::isAutoResizeEnabled() const {
    return pImpl->autoResizeEnabled;
}

} // namespace nlm