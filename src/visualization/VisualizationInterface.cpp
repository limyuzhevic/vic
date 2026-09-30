#include "VisualizationInterface.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::string outputDirectory;
    std::string defaultBackend;
    
    Impl() : active(false), updateRate(30.0), outputDirectory("./visualizations"), defaultBackend("plotly") {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize(const std::map<std::string, std::string>& options) {
    // Parse initialization options
    for (const auto& option : options) {
        if (option.first == "active") {
            pImpl->active = (option.second == "true" || option.second == "1" || option.second == "yes");
        } else if (option.first == "update_rate") {
            pImpl->updateRate = std::stod(option.second);
        } else if (option.first == "output_directory") {
            pImpl->outputDirectory = option.second;
        } else if (option.first == "default_backend") {
            pImpl->defaultBackend = option.second;
        }
    }
    
    // Create output directory
    std::ofstream test_file(pImpl->outputDirectory + "/.test");
    if (test_file.good()) {
        test_file.close();
        std::remove((pImpl->outputDirectory + "/.test").c_str());
    } else {
        // Try to create directory (simplified)
        pImpl->outputDirectory = "./visualizations";
    }
    
    pImpl->active = true;
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    // Update visualization data
    // In a real implementation, this would update network state visualizations
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Render visualization
    // In a real implementation, this would render to screen or file
}

void VisualizationInterface::close() {
    pImpl->active = false;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::setOutputDirectory(const std::string& directory) {
    pImpl->outputDirectory = directory;
}

std::string VisualizationInterface::getOutputDirectory() const {
    return pImpl->outputDirectory;
}

void VisualizationInterface::setDefaultBackend(const std::string& backendName) {
    pImpl->defaultBackend = backendName;
}

std::string VisualizationInterface::getDefaultBackend() const {
    return pImpl->defaultBackend;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::string filename = pImpl->outputDirectory + "/network_" + 
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count()) + ".html";
    
    // Create network visualization
    createNetworkVisualization(filename, brain, pImpl->defaultBackend);
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::string filename = pImpl->outputDirectory + "/activity_" + 
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count()) + ".html";
    
    // Create activity visualization
    createActivityVisualization(filename, brain, pImpl->defaultBackend);
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active) return;
    
    std::string filename = pImpl->outputDirectory + "/spikes_" + 
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count()) + ".html";
    
    // Create spike raster visualization
    createSpikeVisualization(filename, spikes, pImpl->defaultBackend);
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::string filename = pImpl->outputDirectory + "/weights_" + 
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count()) + ".html";
    
    // Create weight matrix visualization
    createWeightVisualization(filename, brain, pImpl->defaultBackend);
}

// Enhanced visualization creation functions
bool VisualizationInterface::createNetworkVisualization(const std::string& filename, 
    const Brain& brain, const std::string& backend) {
    try {
        std::vector<NetworkSnapshot> snapshots = extractNetworkSnapshots(brain);
        if (snapshots.empty()) return false;
        
        // Use the global visualization manager
        auto& vizManager = getGlobalVisualizationManager();
        
        // Create network graph
        bool success = vizManager.createNetworkGraph(filename + ".graph.html", snapshots[0]);
        if (!success) {
            // Fallback to basic visualization
            success = createBasicNetworkVisualization(filename, snapshots[0]);
        }
        
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Error creating network visualization: " << e.what() << std::endl;
        return false;
    }
}

bool VisualizationInterface::createActivityVisualization(const std::string& filename,
    const Brain& brain, const std::string& backend) {
    try {
        std::vector<NetworkSnapshot> snapshots = extractNetworkSnapshots(brain);
        if (snapshots.size() < 2) return false;
        
        auto& vizManager = getGlobalVisualizationManager();
        
        // Create time series plots
        std::vector<TimeSeriesData> series;
        
        // Total spikes over time
        std::vector<double> timestamps, spikeCounts;
        for (const auto& snapshot : snapshots) {
            timestamps.push_back(snapshot.timestamp);
            spikeCounts.push_back(static_cast<double>(snapshot.totalSpikes));
        }
        series.emplace_back(timestamps, spikeCounts, "Total Spikes");
        
        // Active neurons over time
        std::vector<double> activeCounts;
        for (const auto& snapshot : snapshots) {
            activeCounts.push_back(static_cast<double>(snapshot.activeNeurons));
        }
        series.emplace_back(timestamps, activeCounts, "Active Neurons");
        
        // Average firing rate over time
        std::vector<double> firingRates;
        for (const auto& snapshot : snapshots) {
            firingRates.push_back(snapshot.averageFiringRate);
        }
        series.emplace_back(timestamps, firingRates, "Average Firing Rate");
        
        // Create comprehensive dashboard
        std::string dashboardFilename = filename + ".dashboard.html";
        bool success = vizManager.createComprehensiveDashboard(dashboardFilename, snapshots, 
            createSimulationMetrics(snapshots), backend);
        
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Error creating activity visualization: " << e.what() << std::endl;
        return false;
    }
}

bool VisualizationInterface::createSpikeVisualization(const std::string& filename,
    const std::vector<SpikeEvent>& spikes, const std::string& backend) {
    try {
        auto& vizManager = getGlobalVisualizationManager();
        
        // Convert spikes to time series data
        std::vector<TimeSeriesData> series;
        
        // Create raster plot data
        std::vector<std::pair<double, double>> spikePoints;
        double maxTime = 0.0;
        
        for (const auto& spike : spikes) {
            spikePoints.emplace_back(spike.time, static_cast<double>(spike.neuronId));
            maxTime = std::max(maxTime, spike.time);
        }
        
        // Create scatter plot
        std::string scatterFilename = filename + ".scatter.html";
        bool success = vizManager.createScatterPlot(scatterFilename, spikePoints, "Time", "Neuron ID");
        
        if (success) {
            // Create histogram of spike times
            std::vector<double> timestamps;
            for (const auto& spike : spikes) {
                timestamps.push_back(spike.time);
            }
            
            TimeSeriesData spikeHistogram(
                std::vector<double>(timestamps.size(), 0.0), // Placeholder
                std::vector<double>(timestamps.size(), 0.0), // Placeholder
                "Spike Histogram"
            );
            
            // This would be implemented with actual histogram data
            // For now, just create a basic plot
            std::string histogramFilename = filename + ".histogram.html";
            vizManager.createTimeSeriesPlot(histogramFilename, {spikeHistogram}, backend);
        }
        
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Error creating spike visualization: " << e.what() << std::endl;
        return false;
    }
}

bool VisualizationInterface::createWeightVisualization(const std::string& filename,
    const Brain& brain, const std::string& backend) {
    try {
        auto& vizManager = getGlobalVisualizationManager();
        
        // Extract weight matrix data (simplified)
        std::vector<std::vector<double>> weightData;
        
        // This would involve iterating through brain regions and neurons
        // For now, create a placeholder visualization
        
        std::string heatmapFilename = filename + ".heatmap.html";
        bool success = vizManager.createHeatmap(heatmapFilename, weightData);
        
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Error creating weight visualization: " << e.what() << std::endl;
        return false;
    }
}

bool VisualizationInterface::createBasicNetworkVisualization(const std::string& filename,
    const NetworkSnapshot& snapshot) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head><title>Network Visualization</title></head>" << std::endl;
    file << "<body>" << std::endl;
    file << "    <h1>NLM Network Visualization</h1>" << std::endl;
    file << "    <p>Snapshot at time: " << snapshot.timestamp << "s</p>" << std::endl;
    file << "    <p>Total spikes: " << snapshot.totalSpikes << "</p>" << std::endl;
    file << "    <p>Active neurons: " << snapshot.activeNeurons << "</p>" << std::endl;
    file << "    <p>Average firing rate: " << snapshot.averageFiringRate << " Hz</p>" << std::endl;
    file << "    <p>Average membrane potential: " << snapshot.averageMembranePotential << " mV</p>" << std::endl;
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    return true;
}

// Helper functions
std::vector<NetworkSnapshot> VisualizationInterface::extractNetworkSnapshots(const Brain& brain) {
    std::vector<NetworkSnapshot> snapshots;
    
    // This would extract actual network state from brain
    // For now, return empty vector
    
    return snapshots;
}

SimulationMetrics VisualizationInterface::createSimulationMetrics(const std::vector<NetworkSnapshot>& snapshots) {
    SimulationMetrics metrics;
    
    if (snapshots.empty()) return metrics;
    
    double totalTime = 0.0;
    size_t totalSpikes = 0;
    size_t maxActiveNeurons = 0;
    
    for (const auto& snapshot : snapshots) {
        totalTime = std::max(totalTime, snapshot.timestamp);
        totalSpikes += snapshot.totalSpikes;
        maxActiveNeurons = std::max(maxActiveNeurons, snapshot.activeNeurons);
    }
    
    metrics.totalSimulationTime = totalTime;
    metrics.totalSteps = snapshots.size();
    metrics.totalSpikes = totalSpikes;
    metrics.maxActiveNeurons = maxActiveNeurons;
    metrics.averageFiringRate = (totalTime > 0.0) ? static_cast<double>(totalSpikes) / totalTime : 0.0;
    
    return metrics;
}

} // namespace nlm
