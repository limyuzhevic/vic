#include "VisualizationSuite.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

// MonitoringDashboard implementation
void MonitoringDashboard::addMetric(const std::string& name, const std::string& description, double initialValue) {
    metrics_[name] = std::make_pair(initialValue, description);
}

void MonitoringDashboard::updateMetric(const std::string& name, double value) {
    if (metrics_.find(name) != metrics_.end()) {
        metrics_[name].first = value;
    }
}

double MonitoringDashboard::getMetric(const std::string& name) const {
    if (metrics_.find(name) != metrics_.end()) {
        return metrics_.at(name).first;
    }
    return 0.0;
}

void MonitoringDashboard::addMemoryComponent(const std::string& name, size_t initialMemory) {
    memoryComponents_[name] = initialMemory;
}

void MonitoringDashboard::updateMemoryComponent(const std::string& name, size_t memory) {
    if (memoryComponents_.find(name) != memoryComponents_.end()) {
        memoryComponents_[name] = memory;
    }
}

void MonitoringDashboard::addNeuron(const std::string& name, size_t neuronId, double activity) {
    neurons_[name][neuronId] = activity;
}

void MonitoringDashboard::updateNeuron(const std::string& name, size_t neuronId, double activity) {
    if (neurons_.find(name) != neurons_.end() && neurons_[name].find(neuronId) != neurons_[name].end()) {
        neurons_[name][neuronId] = activity;
    }
}

void MonitoringDashboard::addSpike(const std::string& name, size_t neuronId, uint64_t spikeCount) {
    spikes_[name][neuronId] = spikeCount;
}

void MonitoringDashboard::updateSpike(const std::string& name, size_t neuronId, uint64_t spikeCount) {
    if (spikes_.find(name) != spikes_.end() && spikes_[name].find(neuronId) != spikes_[name].end()) {
        spikes_[name][neuronId] = spikeCount;
    }
}

void MonitoringDashboard::setBrainStatus(const std::string& status) {
    brainStatus_ = status;
}

void MonitoringDashboard::setStateInfo(const std::string& key, const std::string& value) {
    stateInfo_[key] = value;
}

std::string MonitoringDashboard::getFormattedMetrics() const {
    std::ostringstream oss;
    oss << "=== System Metrics ===\n";
    for (const auto& pair : metrics_) {
        oss << "  " << pair.first << " (" << pair.second.second << "):" 
            << " " << std::fixed << std::setprecision(2) << pair.second.first << "\n";
    }
    return oss.str();
}

std::string MonitoringDashboard::getMemoryUsageReport() const {
    std::ostringstream oss;
    oss << "=== Memory Usage ===\n";
    for (const auto& pair : memoryComponents_) {
        oss << "  " << pair.first << ": " << pair.second << " bytes\n";
    }
    return oss.str();
}

std::string MonitoringDashboard::getSpikeActivityReport() const {
    std::ostringstream oss;
    oss << "=== Spike Activity ===\n";
    for (const auto& pair : spikes_) {
        oss << "  " << pair.first << ":\n";
        for (const auto& spikePair : pair.second) {
            oss << "    Neuron " << spikePair.first << ": " << spikePair.second << " spikes\n";
        }
    }
    return oss.str();
}

void MonitoringDashboard::render() {
    if (!enabled_) return;
    
    std::cout << getFormattedMetrics() << std::endl;
    std::cout << getMemoryUsageReport() << std::endl;
    std::cout << getSpikeActivityReport() << std::endl;
}

std::string MonitoringDashboard::getOutput(const OutputFormat& format) const {
    switch (format) {
        case OutputFormat::TEXT:
            return generateTextOutput();
        case OutputFormat::JSON:
            return generateJSONOutput();
        case OutputFormat::CSV:
            return generateCSVOutput();
        default:
            return generateTextOutput();
    }
}

bool MonitoringDashboard::saveToFile(const std::string& filename, const OutputFormat& format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << getOutput(format);
    file.close();
    return true;
}

void MonitoringDashboard::updateFromData(const VisualizationData& data) {
    // Update metrics from time series data
    for (const auto& pair : data.timeSeries) {
        std::string metricName = "metric_" + std::to_string(pair.first.time_since_epoch().count());
        addMetric(metricName, "Time series data", pair.second);
    }
    
    // Update memory components
    for (const auto& memPair : data.memoryUsage) {
        updateMemoryComponent(memPair.first, memPair.second);
    }
    
    // Update neuron activity
    for (const auto& neuPair : data.neuronActivity) {
        updateNeuron("neuron_activity", neuPair.first, neuPair.second);
    }
    
    // Update spike activity
    for (const auto& spikePair : data.spikeActivity) {
        updateSpike("spike_activity", spikePair.first, static_cast<uint64_t>(spikePair.second));
    }
}

std::string MonitoringDashboard::generateTextOutput() const {
    std::ostringstream oss;
    oss << "=== Monitoring Dashboard ===\n";
    oss << "Brain Status: " << brainStatus_ << "\n\n";
    oss << getFormattedMetrics() << "\n";
    oss << getMemoryUsageReport() << "\n";
    oss << getSpikeActivityReport() << "\n";
    
    if (!stateInfo_.empty()) {
        oss << "=== State Information ===\n";
        for (const auto& pair : stateInfo_) {
            oss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    return oss.str();
}

std::string MonitoringDashboard::generateJSONOutput() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"brainStatus\": \"" << brainStatus_ << "\",\n";
    oss << "  \"metrics\": [";
    
    bool first = true;
    for (const auto& pair : metrics_) {
        if (!first) oss << ", ";
        oss << "{\"name\": \"" << pair.first << "\", \"value\": " << pair.second.first << ", \"description\": \"" << pair.second.second << "\"}";
        first = false;
    }
    oss << "],\n";
    
    oss << "  \"memoryComponents\": [";
    first = true;
    for (const auto& pair : memoryComponents_) {
        if (!first) oss << ", ";
        oss << "{\"name\": \"" << pair.first << "\", \"memory\": " << pair.second << "}";
        first = false;
    }
    oss << "],\n";
    
    oss << "  \"stateInfo\": [";
    first = true;
    for (const auto& pair : stateInfo_) {
        if (!first) oss << ", ";
        oss << "{\"key\": \"" << pair.first << "\", \"value\": \"" << pair.second << "\"}";
        first = false;
    }
    oss << "]\n";
    
    oss << "}\n";
    return oss.str();
}

std::string MonitoringDashboard::generateCSVOutput() const {
    std::ostringstream oss;
    oss << "Metric,Value,Description\n";
    
    for (const auto& pair : metrics_) {
        oss << pair.first << "," << pair.second.first << "," << pair.second.second << "\n";
    }
    
    oss << "\nMemory Components\n";
    oss << "Component,Memory\n";
    for (const auto& pair : memoryComponents_) {
        oss << pair.first << "," << pair.second << "\n";
    }
    
    return oss.str();
}

// NeuralNetworkVisualizer implementation
void NeuralNetworkVisualizer::addRegion(const std::string& name, const std::string& type, double x, double y, double radius) {
    regions_.push_back({name, type, x, y, radius});
}

void NeuralNetworkVisualizer::addConnection(size_t sourceNeuron, size_t targetNeuron, double weight) {
    connections_.push_back(std::make_pair(sourceNeuron, targetNeuron));
    connectionWeights_.push_back(weight);
}

void NeuralNetworkVisualizer::updateNeuronActivity(size_t neuronId, double activity) {
    neuronActivities_[neuronId] = activity;
}

void NeuralNetworkVisualizer::updateConnectionActivity(size_t sourceNeuron, size_t targetNeuron, double weight) {
    for (size_t i = 0; i < connections_.size(); ++i) {
        if (connections_[i].first == sourceNeuron && connections_[i].second == targetNeuron) {
            connectionActivities_[connections_[i]] = weight;
            break;
        }
    }
}

std::string NeuralNetworkVisualizer::getNetworkLayout() const {
    std::ostringstream oss;
    oss << "=== Neural Network Layout ===\n";
    oss << "Layout Type: " << layout_ << "\n";
    oss << "Neuron Radius: " << neuronRadius_ << "\n";
    oss << "Connection Threshold: " << connectionThreshold_ << "\n\n";
    
    oss << "Regions:\n";
    for (const auto& region : regions_) {
        oss << "  " << region.name << " (" << region.type << ") at (" 
            << region.x << ", " << region.y << ") radius " << region.radius << "\n";
    }
    
    oss << "\nConnections: " << connections_.size() << "\n";
    return oss.str();
}

std::string NeuralNetworkVisualizer::getConnectionMatrix() const {
    std::ostringstream oss;
    oss << "=== Connection Matrix ===\n";
    
    if (connections_.empty()) {
        oss << "No connections available.\n";
        return oss.str();
    }
    
    // Find max neuron ID to size the matrix
    size_t maxNeuronId = 0;
    for (const auto& conn : connections_) {
        maxNeuronId = std::max(maxNeuronId, std::max(conn.first, conn.second));
    }
    
    oss << "Matrix size: " << (maxNeuronId + 1) << " x " << (maxNeuronId + 1) << "\n\n";
    
    for (size_t i = 0; i <= maxNeuronId; ++i) {
        oss << "Neuron " << i << ": ";
        bool first = true;
        for (size_t j = 0; j <= maxNeuronId; ++j) {
            double weight = 0.0;
            for (size_t k = 0; k < connections_.size(); ++k) {
                if (connections_[k].first == i && connections_[k].second == j) {
                    weight = connectionWeights_[k];
                    break;
                }
            }
            if (weight >= connectionThreshold_) {
                if (!first) oss << " ";
                oss << "*";
                first = false;
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string NeuralNetworkVisualizer::getActivityHeatmap() const {
    std::ostringstream oss;
    oss << "=== Activity Heatmap ===\n";
    
    if (neuronActivities_.empty()) {
        oss << "No neuron activity data available.\n";
        return oss.str();
    }
    
    size_t maxActivity = 0;
    for (const auto& pair : neuronActivities_) {
        if (pair.second > maxActivity) {
            maxActivity = pair.second;
        }
    }
    
    oss << "Maximum activity: " << maxActivity << "\n\n";
    
    for (const auto& pair : neuronActivities_) {
        size_t neuronId = pair.first;
        double activity = pair.second;
        
        oss << "Neuron " << std::setw(3) << neuronId << ": ";
        size_t barLength = static_cast<size_t>(activity * 50.0 / maxActivity);
        for (size_t i = 0; i < barLength; ++i) {
            oss << "#";
        }
        oss << " (" << activity << ")\n";
    }
    
    return oss.str();
}

void NeuralNetworkVisualizer::render() {
    if (!enabled_) return;
    
    std::cout << getNetworkLayout() << std::endl;
    std::cout << getConnectionMatrix() << std::endl;
    std::cout << getActivityHeatmap() << std::endl;
}

std::string NeuralNetworkVisualizer::getOutput(const OutputFormat& format) const {
    switch (format) {
        case OutputFormat::TEXT:
            return generateTextOutput();
        case OutputFormat::JSON:
            return generateJSONOutput();
        case OutputFormat::SVG:
            return generateSVGOutput();
        default:
            return generateTextOutput();
    }
}

bool NeuralNetworkVisualizer::saveToFile(const std::string& filename, const OutputFormat& format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << getOutput(format);
    file.close();
    return true;
}

void NeuralNetworkVisualizer::updateFromData(const VisualizationData& data) {
    // Update neurons from activity data
    for (const auto& neuPair : data.neuronActivity) {
        updateNeuronActivity(neuPair.first, neuPair.second);
    }
    
    // Update connections from network data
    for (size_t i = 0; i < data.connections.size(); ++i) {
        addConnection(data.connections[i].first, data.connections[i].second);
        if (i < data.connectionWeights.size()) {
            connectionWeights_[i] = data.connectionWeights[i];
        }
    }
}

std::string NeuralNetworkVisualizer::generateTextOutput() const {
    return getNetworkLayout() + "\n" + getConnectionMatrix() + "\n" + getActivityHeatmap();
}

std::string NeuralNetworkVisualizer::generateJSONOutput() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"layoutType\": \"" << layout_ << "\",\n";
    oss << "  \"neuronRadius\": " << neuronRadius_ << ",\n";
    oss << "  \"connectionThreshold\": " << connectionThreshold_ << ",\n";
    
    oss << "  \"regions\": [";
    bool first = true;
    for (const auto& region : regions_) {
        if (!first) oss << ", ";
        oss << "{\"name\": \"" << region.name << "\", \"type\": \"" << region.type << "\", \"x\": " << region.x 
            << ", \"y\": " << region.y << ", \"radius\": " << region.radius << "}";
        first = false;
    }
    oss << "],\n";
    
    oss << "  \"connections\": [";
    first = true;
    for (size_t i = 0; i < connections_.size(); ++i) {
        if (!first) oss << ", ";
        oss << "{\"source\": " << connections_[i].first << ", \"target\": " << connections_[i].second 
            << ", \"weight\": " << connectionWeights_[i] << "}";
        first = false;
    }
    oss << "],\n";
    
    oss << "  \"neuronActivities\": [";
    first = true;
    for (const auto& pair : neuronActivities_) {
        if (!first) oss << ", ";
        oss << "{\"neuronId\": " << pair.first << ", \"activity\": " << pair.second << "}";
        first = false;
    }
    oss << "]\n";
    
    oss << "}\n";
    return oss.str();
}

std::string NeuralNetworkVisualizer::generateSVGOutput() const {
    std::ostringstream oss;
    oss << "<svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // Draw connections
    oss << "  <!-- Connections -->\n";
    for (size_t i = 0; i < connections_.size(); ++i) {
        // In a real implementation, would calculate coordinates based on regions
        oss << "  <line x1=\"" << connections_[i].first * 10 << "\" y1=\"" << connections_[i].first * 10 
            << "\" x2=\"" << connections_[i].second * 10 << "\" y2=\"" << connections_[i].second * 10 
            << "\" stroke=\"#ccc\" stroke-width=\"\" << " << (connectionWeights_[i] > connectionThreshold_ ? "2\"" : "1\"") 
            << "/>\n";
    }
    
    // Draw neurons as circles
    oss << "  <!-- Neurons -->\n";
    for (size_t i = 0; i < connections_.size(); ++i) {
        size_t neuronId = connections_[i].first;
        double activity = 1.0;
        // Find activity for this neuron
        for (const auto& pair : neuronActivities_) {
            if (pair.first == neuronId) {
                activity = pair.second;
                break;
            }
        }
        double colorIntensity = activity * 255;
        oss << "  <circle cx=\"" << neuronId * 10 << "\" cy=\"" << neuronId * 10 
            << "\" r=\"" << neuronRadius_ << "\" fill=\"rgb(0, " << static_cast<int>(colorIntensity) 
            << ", 0)\" stroke=\"#000\"/>\n";
    }
    
    oss << "</svg>\n";
    return oss.str();
}

// PerformanceMetricsViz implementation (simplified for brevity)
void PerformanceMetricsViz::render() {
    if (!enabled_) return;
    
    std::cout << getTimeSeriesPlot("All Metrics") << std::endl;
}

std::string PerformanceMetricsViz::getOutput(const OutputFormat& format) const {
    switch (format) {
        case OutputFormat::TEXT:
            return generateTextPlot();
        case OutputFormat::JSON:
            return generateJSONOutput();
        case OutputFormat::CSV:
            return generateCSVOutput();
        default:
            return generateTextPlot();
    }
}

bool PerformanceMetricsViz::saveToFile(const std::string& filename, const OutputFormat& format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << getOutput(format);
    file.close();
    return true;
}

void PerformanceMetricsViz::updateFromData(const VisualizationData& data) {
    // Update time series from performance metrics
    for (const auto& metricPair : data.performanceMetrics) {
        addTimeSeries(metricPair.first, std::vector<std::pair<double, double>>{std::make_pair(
            static_cast<double>(data.timestamp.time_since_epoch().count() / 1000000.0), 
            metricPair.second
        )});
    }
}

std::string PerformanceMetricsViz::generateTextPlot() const {
    std::ostringstream oss;
    oss << "=== Performance Metrics Visualization ===\n";
    oss << "Time Range: [" << timeRangeStart_ << ", " << timeRangeEnd_ << "]\n\n";
    
    for (const auto& pair : timeSeries_) {
        oss << "Metric: " << pair.first << "\n";
        oss << "Data points: " << pair.second.size() << "\n";
        for (const auto& dataPair : pair.second) {
            oss << "  Time: " << dataPair.first << ", Value: " << dataPair.second << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

// Other visualization implementations would follow similar patterns...

// VisualizationContainer implementation
void VisualizationContainer::addComponent(std::unique_ptr<IVizComponent> component) {
    if (component) {
        components_[component->getName()] = std::move(component);
    }
}

void VisualizationContainer::removeComponent(const std::string& name) {
    components_.erase(name);
}

IVizComponent* VisualizationContainer::getComponent(const std::string& name) {
    auto it = components_.find(name);
    if (it != components_.end()) {
        return it->second.get();
    }
    return nullptr;
}

const IVizComponent* VisualizationContainer::getComponent(const std::string& name) const {
    auto it = components_.find(name);
    if (it != components_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void VisualizationContainer::clear() {
    components_.clear();
}

void VisualizationContainer::enableAll() {
    for (auto& pair : components_) {
        pair.second->setEnabled(true);
    }
}

void VisualizationContainer::disableAll() {
    for (auto& pair : components_) {
        pair.second->setEnabled(false);
    }
}

void VisualizationContainer::toggleComponent(const std::string& name) {
    auto it = components_.find(name);
    if (it != components_.end()) {
        it->second->setEnabled(!it->second->isEnabled());
    }
}

void VisualizationContainer::update(const VisualizationData& data) {
    for (auto& pair : components_) {
        pair.second->update(data);
    }
}

void VisualizationContainer::renderAll() {
    for (auto& pair : components_) {
        pair.second->render();
    }
}

std::string VisualizationContainer::generateCombinedOutput(const OutputFormat& format) const {
    std::ostringstream oss;
    oss << "=== Combined Visualization Output ===\n";
    
    for (const auto& pair : components_) {
        oss << "\n--- " << pair.first << " ---\n";
        oss << pair.second->getOutput(format) << "\n";
    }
    
    return oss.str();
}

bool VisualizationContainer::saveAllToFile(const std::string& prefix, const OutputFormat& format) const {
    for (const auto& pair : components_) {
        std::string filename = prefix + "_" + pair.first + "." + getFormatExtension(format);
        if (!pair.second->saveToFile(filename, format)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> VisualizationContainer::getComponentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : components_) {
        names.push_back(pair.first);
    }
    return names;
}

std::string VisualizationContainer::getFormatExtension(const OutputFormat& format) const {
    switch (format) {
        case OutputFormat::TEXT: return "txt";
        case OutputFormat::JSON: return "json";
        case OutputFormat::CSV: return "csv";
        case OutputFormat::PNG: return "png";
        case OutputFormat::SVG: return "svg";
        case OutputFormat::HTML: return "html";
        default: return "txt";
    }
}

// VisualizationFactory implementation
std::unique_ptr<MonitoringDashboard> VisualizationFactory::createMonitoringDashboard() {
    return std::make_unique<MonitoringDashboard>();
}

std::unique_ptr<NeuralNetworkVisualizer> VisualizationFactory::createNeuralNetworkVisualizer() {
    return std::make_unique<NeuralNetworkVisualizer>();
}

std::unique_ptr<PerformanceMetricsViz> VisualizationFactory::createPerformanceMetricsViz() {
    return std::make_unique<PerformanceMetricsViz>();
}

std::unique_ptr<SpikeActivityViz> VisualizationFactory::createSpikeActivityViz() {
    return std::make_unique<SpikeActivityViz>();
}

std::unique_ptr<ConfigStateViz> VisualizationFactory::createConfigStateViz() {
    return std::make_unique<ConfigStateViz>();
}

std::unique_ptr<VisualizationContainer> VisualizationFactory::createDefaultVisualizationContainer() {
    auto container = std::make_unique<VisualizationContainer>();
    
    container->addComponent(createMonitoringDashboard());
    container->addComponent(createNeuralNetworkVisualizer());
    container->addComponent(createPerformanceMetricsViz());
    container->addComponent(createSpikeActivityViz());
    container->addComponent(createConfigStateViz());
    
    return container;
}

std::unique_ptr<VisualizationContainer> VisualizationFactory::createCustomVisualization(
    const std::vector<std::string>& componentNames) {
    auto container = std::make_unique<VisualizationContainer>();
    
    for (const auto& name : componentNames) {
        if (name == "monitoring_dashboard") {
            container->addComponent(createMonitoringDashboard());
        } else if (name == "neural_network") {
            container->addComponent(createNeuralNetworkVisualizer());
        } else if (name == "performance_metrics") {
            container->addComponent(createPerformanceMetricsViz());
        } else if (name == "spike_activity") {
            container->addComponent(createSpikeActivityViz());
        } else if (name == "config_state") {
            container->addComponent(createConfigStateViz());
        }
    }
    
    return container;
}

std::unique_ptr<VisualizationContainer> VisualizationFactory::loadFromConfig(
    const std::string& configFile) {
    // In a real implementation, would parse the configuration file
    // For now, return a default container
    return createDefaultVisualizationContainer();
}

bool VisualizationFactory::saveConfig(
    const std::string& configFile,
    const VisualizationContainer& container) {
    std::ofstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    // In a real implementation, would serialize the container configuration
    file << "Visualization Configuration\n";
    file << "Components: " << container.getComponentCount() << "\n";
    
    file.close();
    return true;
}

// Convenience functions implementation
void Visualization::initializeDefaultVisualization() {
    globalContainer.clear();
    
    // Add default components
    globalContainer.addComponent(VisualizationFactory::createMonitoringDashboard());
    globalContainer.addComponent(VisualizationFactory::createNeuralNetworkVisualizer());
    globalContainer.addComponent(VisualizationFactory::createPerformanceMetricsViz());
    globalContainer.addComponent(VisualizationFactory::createSpikeActivityViz());
    globalContainer.addComponent(VisualizationFactory::createConfigStateViz());
}

void Visualization::updateGlobalVisualization() {
    // In a real implementation, would collect data from the actual system
    VisualizationData data;
    globalContainer.update(data);
}

std::string Visualization::generateVisualizationReport() {
    return globalContainer.generateCombinedOutput(OutputFormat::TEXT);
}

void Visualization::saveAllVisualizations(const std::string& directory) {
    for (const auto& name : globalContainer.getComponentNames()) {
        std::string filename = directory + "/viz_" + name + ".txt";
        // In a real implementation, would save to file
        // For now, just log
        std::cout << "Would save visualization " << name << " to " << filename << std::endl;
    }
}

IVizComponent* Visualization::getComponent(const std::string& name) {
    return globalContainer.getComponent(name);
}

void Visualization::addComponent(std::unique_ptr<IVizComponent> component) {
    globalContainer.addComponent(std::move(component));
}

} // namespace nlm
