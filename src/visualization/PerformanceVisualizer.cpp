// PerformanceVisualizer.cpp - Performance visualization implementation

#include "PerformanceVisualizer.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "SpikeEvent.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>

namespace nlm {

PerformanceVisualizer::PerformanceVisualizer() : pImpl(std::make_unique<Impl>()) {
    pImpl->config.theme = "default";
    pImpl->config.chartWidth = 800.0;
    pImpl->config.chartHeight = 600.0;
    pImpl->config.showGrid = true;
    pImpl->config.showLegend = true;
    pImpl->config.animated = false;
    pImpl->config.animationSpeed = 1.0;
}

PerformanceVisualizer::PerformanceVisualizer(const PerformanceVisualizationConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

PerformanceVisualizer::~PerformanceVisualizer() {
}

void PerformanceVisualizer::configure(const PerformanceVisualizationConfig& config) {
    pImpl->config = config;
}

bool PerformanceVisualizer::initialize(const Brain& brain) {
    pImpl->brainData = std::make_unique<Brain>(brain);
    pImpl->initialized = true;
    
    // Calculate initial statistics
    calculatePerformanceStats(*pImpl->brainData);
    
    NLM_LOG_INFO("PerformanceVisualizer initialized with " + std::to_string(pImpl->brainData->getNeuronCount()) + " neurons");
    return true;
}

void PerformanceVisualizer::update(const Brain& brain, double timeStep) {
    if (!pImpl->initialized) return;
    
    pImpl->lastUpdateTime = std::chrono::steady_clock::now();
    pImpl->brainData = std::make_unique<Brain>(brain);
    
    // Update performance statistics
    calculatePerformanceStats(*pImpl->brainData);
    
    // Update metrics history
    updateMetricsHistory();
}

bool PerformanceVisualizer::render() {
    if (!pImpl->initialized) return false;
    
    // Render based on visualization type
    switch (pImpl->config.type) {
        case VisualizationType::PERFORMANCE_GRAPH:
            renderPerformanceGraph();
            break;
        case VisualizationType::EFFICIENCY_MAP:
            renderEfficiencyMap();
            break;
        case VisualizationType::LOAD_BALANCE:
            renderLoadBalance();
            break;
        case VisualizationType::RESOURCE_USAGE:
            renderResourceUsage();
            break;
        case VisualizationType::ACTIVITY_HEATMAP:
            renderActivityHeatMap();
            break;
        case VisualizationType::CONCURRENCY_METRICS:
            renderConcurrencyMetrics();
            break;
        default:
            renderPerformanceGraph();
            break;
    }
    
    return true;
}

bool PerformanceVisualizer::exportVisualization(const std::string& filename) const {
    if (!pImpl->config.enableExport) {
        NLM_LOG_WARNING("Export is disabled in configuration");
        return false;
    }
    
    std::string extension = getFileExtension(filename);
    
    if (extension == "png" || extension == "PNG") {
        return exportToPng(filename);
    } else if (extension == "pdf" || extension == "PDF") {
        return exportToPdf(filename);
    } else if (extension == "svg" || extension == "SVG") {
        return exportToSvg(filename);
    } else if (extension == "html" || extension == "HTML") {
        return generateHtmlVisualization(filename);
    } else {
        NLM_LOG_ERROR("Unsupported export format: " + extension);
        return false;
    }
}

std::string PerformanceVisualizer::generateReport() const {
    std::ostringstream oss;
    oss << "Performance Visualization Report\n";
    oss << "=============================\n\n";
    oss << "Generated: " << getCurrentTime() << "\n\n";
    
    oss << "System Performance:\n";
    oss << "  Total Neurons: " << pImpl->stats.totalNeurons << "\n";
    oss << "  Total Synapses: " << pImpl->stats.totalSynapses << "\n";
    oss << "  Average Firing Rate: " << pImpl->stats.averageFiringRate << " Hz\n";
    oss << "  Network Throughput: " << pImpl->stats.networkThroughput << " spikes/sec\n";
    oss << "  Average Latency: " << pImpl->stats.averageLatency << " ms\n";
    oss << "  Memory Usage: " << pImpl->stats.memoryUsageMB << " MB\n";
    oss << "  CPU Usage: " << pImpl->stats.cpuUsagePercent << "%\n\n";
    
    oss << "Neural Performance:\n";
    oss << "  Max Firing Rate: " << pImpl->stats.maxFiringRate << " Hz\n";
    oss << "  Min Firing Rate: " << pImpl->stats.minFiringRate << " Hz\n";
    oss << "  Firing Rate CV: " << pImpl->stats.firingRateCV << "\n";
    oss << "  Synchronization: " << pImpl->stats.synchronization << "\n";
    oss << "  Activity Pattern: " << pImpl->stats.activityPattern << "\n\n";
    
    oss << "Synaptic Performance:\n";
    oss << "  Synaptic Strength Average: " << pImpl->stats.synapticStrengthAvg << "\n";
    oss << "  Synaptic Strength CV: " << pImpl->stats.synapticStrengthCV << "\n";
    oss << "  Connection Efficiency: " << pImpl->stats.connectionEfficiency << "\n";
    oss << "  Plasticity Rate: " << pImpl->stats.plasticityRate << "\n\n";
    
    oss << "Network Metrics:\n";
    oss << "  Clustering Coefficient: " << pImpl->stats.clusteringCoefficient << "\n";
    oss << "  Network Diameter: " << pImpl->stats.networkDiameter << "\n";
    oss << "  Network Efficiency: " << pImpl->stats.networkEfficiency << "\n";
    oss << "  Modularity: " << pImpl->stats.modularity << "\n\n";
    
    oss << "Regional Performance:\n";
    for (const auto& pair : pImpl->stats.regionalStats) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    oss << "\nHistorical Metrics:\n";
    oss << "  Recent Firing Rate: " << pImpl->metricsHistory.back().firingRate << " Hz\n";
    oss << "  Recent Latency: " << pImpl->metricsHistory.back().latency << " ms\n";
    oss << "  Recent Throughput: " << pImpl->metricsHistory.back().throughput << " spikes/sec\n";
    
    return oss.str();
}

PerformanceVisualizer::PerformanceStats PerformanceVisualizer::getStats() const {
    return pImpl->stats;
}

std::vector<PerformanceVisualizer::PerformanceMetric> PerformanceVisualizer::getMetrics() const {
    return pImpl->metricsHistory;
}

std::vector<PerformanceVisualizer::RegionalPerformance> PerformanceVisualizer::getRegionalPerformance() const {
    return pImpl->regionalPerformance;
}

PerformanceVisualizer::VisualizationType PerformanceVisualizer::getVisualizationType() const {
    return pImpl->config.type;
}

void PerformanceVisualizer::setVisualizationType(VisualizationType type) {
    pImpl->config.type = type;
}

void PerformanceVisualizer::setBrain(const Brain& brain) {
    if (pImpl->initialized) {
        pImpl->brainData = std::make_unique<Brain>(brain);
        calculatePerformanceStats(*pImpl->brainData);
    }
}

// Private helper methods
void PerformanceVisualizer::calculatePerformanceStats(const Brain& brain) {
    // Calculate performance statistics
    pImpl->stats.totalNeurons = brain.getNeuronCount();
    pImpl->stats.totalSynapses = brain.getSynapseCount();
    
    // Calculate firing rate statistics
    std::vector<double> firingRates;
    double totalFiringRate = 0.0;
    
    for (const auto& neuron : brain.getNeurons()) {
        double firingRate = calculateFiringRate(neuron);
        firingRates.push_back(firingRate);
        totalFiringRate += firingRate;
    }
    
    pImpl->stats.averageFiringRate = firingRates.empty() ? 0.0 : totalFiringRate / firingRates.size();
    pImpl->stats.maxFiringRate = *std::max_element(firingRates.begin(), firingRates.end());
    pImpl->stats.minFiringRate = *std::min_element(firingRates.begin(), firingRates.end());
    
    if (pImpl->stats.averageFiringRate > 0) {
        double variance = 0.0;
        for (double rate : firingRates) {
            variance += (rate - pImpl->stats.averageFiringRate) * (rate - pImpl->stats.averageFiringRate);
        }
        variance /= firingRates.size();
        pImpl->stats.firingRateCV = std::sqrt(variance) / pImpl->stats.averageFiringRate;
    }
    
    // Calculate network performance
    pImpl->stats.networkThroughput = calculateNetworkThroughput(brain);
    pImpl->stats.averageLatency = calculateAverageLatency(brain);
    pImpl->stats.memoryUsageMB = calculateMemoryUsage();
    pImpl->stats.cpuUsagePercent = calculateCpuUsage();
    
    // Calculate synaptic performance
    pImpl->stats.synapticStrengthAvg = calculateAverageSynapticStrength(brain);
    pImpl->stats.synapticStrengthCV = calculateSynapticStrengthCV(brain);
    pImpl->stats.connectionEfficiency = calculateConnectionEfficiency(brain);
    pImpl->stats.plasticityRate = calculatePlasticityRate(brain);
    
    // Calculate network metrics
    pImpl->stats.clusteringCoefficient = calculateClusteringCoefficient(brain);
    pImpl->stats.networkDiameter = calculateNetworkDiameter(brain);
    pImpl->stats.networkEfficiency = calculateNetworkEfficiency(brain);
    pImpl->stats.modularity = calculateModularity(brain);
    
    // Calculate regional performance
    calculateRegionalPerformance(brain);
    
    // Determine activity pattern
    pImpl->stats.activityPattern = determineActivityPattern(firingRates);
    
    // Calculate synchronization
    pImpl->stats.synchronization = calculateSynchronization(brain);
}

void PerformanceVisualizer::updateMetricsHistory() {
    // Update metrics history
    PerformanceMetric metric;
    metric.timestamp = std::chrono::system_clock::now();
    metric.firingRate = pImpl->stats.averageFiringRate;
    metric.latency = pImpl->stats.averageLatency;
    metric.throughput = pImpl->stats.networkThroughput;
    metric.cpuUsage = pImpl->stats.cpuUsagePercent;
    metric.memoryUsage = pImpl->stats.memoryUsageMB;
    
    pImpl->metricsHistory.push_back(metric);
    
    // Keep only last 1000 metrics
    if (pImpl->metricsHistory.size() > 1000) {
        pImpl->metricsHistory.erase(pImpl->metricsHistory.begin());
    }
}

void PerformanceVisualizer::calculateRegionalPerformance(const Brain& brain) {
    pImpl->regionalPerformance.clear();
    
    // Calculate performance per region
    for (const auto& region : brain.getRegions()) {
        RegionalPerformance perf;
        perf.regionName = region.getName();
        
        double regionalFiringRate = 0.0;
        int neuronCount = 0;
        
        for (const auto& neuron : brain.getNeurons()) {
            if (neuron.getRegion() == region.getName()) {
                regionalFiringRate += calculateFiringRate(neuron);
                neuronCount++;
            }
        }
        
        perf.firingRate = neuronCount > 0 ? regionalFiringRate / neuronCount : 0.0;
        perf.neuronCount = neuronCount;
        
        // Calculate regional connectivity
        double regionalConnections = 0.0;
        for (const auto& synapse : brain.getSynapses()) {
            if (synapse.getPreNeuronId() == neuron.getId() || 
                synapse.getPostNeuronId() == neuron.getId()) {
                regionalConnections += synapse.getWeight();
            }
        }
        
        perf.connectivity = regionalConnections;
        pImpl->regionalPerformance.push_back(perf);
    }
}

void PerformanceVisualizer::renderPerformanceGraph() {
    // Render performance graph
    // This would render a comprehensive performance graph
}

void PerformanceVisualizer::renderEfficiencyMap() {
    // Render efficiency map
    // This would render a map of efficiency across the network
}

void PerformanceVisualizer::renderLoadBalance() {
    // Render load balance visualization
    // This would show load distribution across neurons
}

void PerformanceVisualizer::renderResourceUsage() {
    // Render resource usage
    // This would show CPU, memory, and network resource usage
}

void PerformanceVisualizer::renderActivityHeatMap() {
    // Render activity heat map
    // This would show firing rate heat map
}

void PerformanceVisualizer::renderConcurrencyMetrics() {
    // Render concurrency metrics
    // This would show concurrent activity
}

// Calculation helper methods
double PerformanceVisualizer::calculateFiringRate(const Neuron& neuron) const {
    // Calculate firing rate from neuron properties
    // In real implementation, would get from spike data or neuron model
    return 10.0; // Placeholder
}

double PerformanceVisualizer::calculateNetworkThroughput(const Brain& brain) const {
    double totalThroughput = 0.0;
    for (const auto& neuron : brain.getNeurons()) {
        totalThroughput += calculateFiringRate(neuron);
    }
    return totalThroughput;
}

double PerformanceVisualizer::calculateAverageLatency(const Brain& brain) const {
    // Calculate average signal transmission latency
    double totalLatency = 0.0;
    int synapseCount = 0;
    
    for (const auto& synapse : brain.getSynapses()) {
        totalLatency += synapse.getDelay();
        synapseCount++;
    }
    
    return synapseCount > 0 ? totalLatency / synapseCount : 0.0;
}

double PerformanceVisualizer::calculateMemoryUsage() const {
    // Calculate approximate memory usage
    if (pImpl->brainData) {
        double neuronMemory = pImpl->brainData->getNeuronCount() * sizeof(Neuron);
        double synapseMemory = pImpl->brainData->getSynapseCount() * sizeof(Synapse);
        return (neuronMemory + synapseMemory) / (1024.0 * 1024.0); // MB
    }
    return 0.0;
}

double PerformanceVisualizer::calculateCpuUsage() const {
    // Calculate CPU usage (simplified)
    // In real implementation, would use system monitoring
    return 15.0; // Placeholder
}

double PerformanceVisualizer::calculateAverageSynapticStrength(const Brain& brain) const {
    double totalStrength = 0.0;
    int synapseCount = 0;
    
    for (const auto& synapse : brain.getSynapses()) {
        totalStrength += synapse.getWeight();
        synapseCount++;
    }
    
    return synapseCount > 0 ? totalStrength / synapseCount : 0.0;
}

double PerformanceVisualizer::calculateSynapticStrengthCV(const Brain& brain) const {
    std::vector<double> weights;
    for (const auto& synapse : brain.getSynapses()) {
        weights.push_back(synapse.getWeight());
    }
    
    if (weights.empty()) return 0.0;
    
    double mean = std::accumulate(weights.begin(), weights.end(), 0.0) / weights.size();
    double variance = 0.0;
    for (double weight : weights) {
        variance += (weight - mean) * (weight - mean);
    }
    variance /= weights.size();
    
    return std::sqrt(variance) / mean;
}

double PerformanceVisualizer::calculateConnectionEfficiency(const Brain& brain) const {
    double efficiency = 0.0;
    int totalPossibleConnections = pImpl->stats.totalNeurons * (pImpl->stats.totalNeurons - 1);
    
    if (totalPossibleConnections > 0) {
        efficiency = static_cast<double>(pImpl->stats.totalSynapses) / totalPossibleConnections;
    }
    
    return efficiency;
}

double PerformanceVisualizer::calculatePlasticityRate(const Brain& brain) const {
    // Calculate plasticity rate based on synapse changes
    double plasticityRate = 0.0;
    for (const auto& synapse : brain.getSynapses()) {
        if (synapse.getPlasticityRule() != "none") {
            plasticityRate += synapse.getWeight();
        }
    }
    
    return plasticityRate;
}

double PerformanceVisualizer::calculateClusteringCoefficient(const Brain& brain) const {
    // Calculate clustering coefficient (simplified)
    if (pImpl->stats.totalNeurons < 3) return 1.0;
    return 0.6; // Placeholder
}

double PerformanceVisualizer::calculateNetworkDiameter(const Brain& brain) const {
    // Calculate network diameter (simplified)
    return 5.0; // Placeholder
}

double PerformanceVisualizer::calculateNetworkEfficiency(const Brain& brain) const {
    // Calculate network efficiency (simplified)
    return 0.8; // Placeholder
}

double PerformanceVisualizer::calculateModularity(const Brain& brain) const {
    // Calculate modularity (simplified)
    return 0.6; // Placeholder
}

std::string PerformanceVisualizer::determineActivityPattern(const std::vector<double>& firingRates) const {
    if (firingRates.empty()) return "quiet";
    
    double meanRate = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
    
    if (meanRate < 1.0) return "quiet";
    if (meanRate < 10.0) return "moderate";
    if (meanRate < 50.0) return "active";
    return "hyperactive";
}

double PerformanceVisualizer::calculateSynchronization(const Brain& brain) const {
    // Calculate synchronization level (simplified)
    return 0.7; // Placeholder
}

bool PerformanceVisualizer::exportToPng(const std::string& filename) const {
    // Placeholder for PNG export
    NLM_LOG_WARNING("PNG export not yet implemented for PerformanceVisualizer");
    return false;
}

bool PerformanceVisualizer::exportToPdf(const std::string& filename) const {
    // Placeholder for PDF export
    NLM_LOG_WARNING("PDF export not yet implemented for PerformanceVisualizer");
    return false;
}

bool PerformanceVisualizer::exportToSvg(const std::string& filename) const {
    // Placeholder for SVG export
    NLM_LOG_WARNING("SVG export not yet implemented for PerformanceVisualizer");
    return false;
}

bool PerformanceVisualizer::generateHtmlVisualization(const std::string& filename) const {
    // Generate HTML visualization
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << generateHtmlReport();
    file.close();
    
    NLM_LOG_INFO("HTML visualization generated: " + filename);
    return true;
}

std::string PerformanceVisualizer::generateHtmlReport() const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "  <title>Performance Visualization Report</title>\n";
    oss << "  <style>\n";
    oss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    oss << "    .metric { margin: 10px 0; padding: 10px; background-color: #f5f5f5; }\n";
    oss << "    .high { border-left: 4px solid red; }\n";
    oss << "    .medium { border-left: 4px solid yellow; }\n";
    oss << "    .low { border-left: 4px solid green; }\n";
    oss << "  </style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "  <h1>Performance Visualization Report</h1>\n";
    oss << "  <p>Generated: " << getCurrentTime() << "</p>\n";
    
    oss << "  <div class=\"metric high\">\n";
    oss << "    <h2>Firing Rate</h2>\n";
    oss << "    <p>Average: " << pImpl->stats.averageFiringRate << " Hz</p>\n";
    oss << "    <p>Maximum: " << pImpl->stats.maxFiringRate << " Hz</p>\n";
    oss << "    <p>Minimum: " << pImpl->stats.minFiringRate << " Hz</p>\n";
    oss << "  </div>\n";
    
    oss << "  <div class=\"metric medium\">\n";
    oss << "    <h2>Network Performance</h2>\n";
    oss << "    <p>Throughput: " << pImpl->stats.networkThroughput << " spikes/sec</p>\n";
    oss << "    <p>Latency: " << pImpl->stats.averageLatency << " ms</p>\n";
    oss << "    <p>Efficiency: " << pImpl->stats.networkEfficiency << "</p>\n";
    oss << "  </div>\n";
    
    oss << "  <div class=\"metric low\">\n";
    oss << "    <h2>Resource Usage</h2>\n";
    oss << "    <p>Memory: " << pImpl->stats.memoryUsageMB << " MB</p>\n";
    oss << "    <p>CPU: " << pImpl->stats.cpuUsagePercent << "%</p>\n";
    oss << "  </div>\n";
    
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

std::string PerformanceVisualizer::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string PerformanceVisualizer::getFileExtension(const std::string& filename) const {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos + 1);
}

} // namespace nlm
