#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <csignal>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Forward declarations
class VisualizationManager;
class MonitoringDashboard;

// Data structures for visualization
struct NeuronData {
    NeuronId id;
    float membranePotential;
    float threshold;
    float firingRate;
    bool isFiring;
    float lastSpikeTime;
    NeuronType type;
    
    NeuronData(NeuronId neuronId) : id(neuronId), membranePotential(0.0f), 
                                   threshold(0.0f), firingRate(0.0f), 
                                   isFiring(false), lastSpikeTime(-1.0f), 
                                   type(NeuronType::Internal) {}
};

struct SynapseData {
    SynapseId id;
    NeuronId source;
    NeuronId destination;
    float weight;
    float delay;
    bool isExcitatory;
    float efficacy;
    
    SynapseData(SynapseId synId, NeuronId src, NeuronId dst) 
        : id(synId), source(src), destination(dst), weight(0.0f), 
          delay(0), isExcitatory(true), efficacy(0.0f) {}
};

struct RegionData {
    RegionId id;
    std::string name;
    std::vector<NeuronData> neurons;
    std::vector<SynapseData> synapses;
    float avgFiringRate;
    float avgSynapticWeight;
    
    RegionData(RegionId regionId) : id(regionId), avgFiringRate(0.0f), avgSynapticWeight(0.0f) {}
};

struct TimeSeriesData {
    std::string name;
    std::vector<double> timestamps;
    std::vector<double> values;
    std::string unit;
    
    TimeSeriesData(const std::string& seriesName, const std::string& seriesUnit = "")
        : name(seriesName), unit(seriesUnit) {}
    
    void addDataPoint(double timestamp, double value) {
        timestamps.push_back(timestamp);
        values.push_back(value);
    }
};

struct BrainSnapshot {
    uint64_t timestamp;
    uint64_t simulationStep;
    std::vector<RegionData> regions;
    std::vector<TimeSeriesData> timeSeries;
    
    // Neural metrics
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    size_t activeNeuronCount;
    size_t firingNeuronCount;
    float totalSpikeCount;
    float averageFiringRate;
    float excitationInhibitionRatio;
    
    // Memory metrics
    size_t workingMemoryTraces;
    size_t episodicMemoryEpisodes;
    float episodicMemoryCapacity;
    
    // Neuromodulation metrics
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    
    // Development metrics
    DevelopmentalStage developmentalStage;
    double developmentalAge;
    float plasticityModifier;
    
    BrainSnapshot() : timestamp(0), simulationStep(0), totalNeuronCount(0),
                     totalSynapseCount(0), activeNeuronCount(0), firingNeuronCount(0),
                     totalSpikeCount(0.0f), averageFiringRate(0.0f), 
                     excitationInhibitionRatio(0.0f), workingMemoryTraces(0),
                     episodicMemoryEpisodes(0), episodicMemoryCapacity(0.0f),
                     dopamineLevel(0.0f), curiosityLevel(0.0f), noveltyLevel(0.0f),
                     predictionError(0.0f), developmentalAge(0.0), plasticityModifier(1.0f) {}
};

// Visualization data collector
class VisualizationDataCollector {
public:
    VisualizationDataCollector(std::shared_ptr<Brain> brain) : brain_(brain), running_(false) {
        signal(SIGINT, handleSignal);
        signal(SIGTERM, handleSignal);
    }
    
    ~VisualizationDataCollector() {
        running_ = false;
        if (collectThread_.joinable()) {
            collectThread_.join();
        }
    }
    
    void startCollecting(double intervalSeconds = 0.5) {
        running_ = true;
        collectThread_ = std::thread(&VisualizationDataCollector::collectLoop, this, intervalSeconds);
    }
    
    void stopCollecting() {
        running_ = false;
        if (collectThread_.joinable()) {
            collectThread_.join();
        }
    }
    
    BrainSnapshot getCurrentSnapshot() {
        std::lock_guard<std::mutex> lock(dataMutex_);
        return currentSnapshot_;
    }
    
    std::vector<TimeSeriesData> getTimeSeriesData() {
        std::lock_guard<std::mutex> lock(dataMutex_);
        return timeSeries_;
    }
    
    void addTimeSeriesObserver(const std::string& name, const std::string& unit = "") {
        std::lock_guard<std::mutex> lock(dataMutex_);
        timeSeries_.emplace_back(name, unit);
    }
    
    void updateTimeSeries(const std::string& name, double timestamp, double value) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        for (auto& series : timeSeries_) {
            if (series.name == name) {
                series.addDataPoint(timestamp, value);
                break;
            }
        }
    }
    
    static void handleSignal(int signum) {
        std::cout << "\nReceived signal " << signum << ", stopping data collection..." << std::endl;
    }
    
private:
    void collectLoop(double interval) {
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (running_) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Create snapshot
            BrainSnapshot snapshot;
            snapshot.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            snapshot.simulationStep = brain_->getTotalSpikeCount(); // Use spike count as proxy for steps
            
            // Collect neural data
            collectNeuralData(snapshot);
            
            // Collect memory data
            collectMemoryData(snapshot);
            
            // Collect neuromodulation data
            collectNeuromodulationData(snapshot);
            
            // Collect development data
            collectDevelopmentData(snapshot);
            
            // Update time series
            updateTimeSeriesData(snapshot);
            
            // Store snapshot
            {
                std::lock_guard<std::mutex> lock(dataMutex_);
                currentSnapshot_ = snapshot;
            }
            
            // Sleep until next interval
            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            auto sleepTime = static_cast<int>(interval * 1000) - elapsed;
            
            if (sleepTime > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            }
        }
    }
    
    void collectNeuralData(BrainSnapshot& snapshot) {
        snapshot.totalNeuronCount = brain_->getTotalNeuronCount();
        snapshot.totalSynapseCount = brain_->getTotalSynapseCount();
        snapshot.activeNeuronCount = brain_->getActiveNeuronCount();
        snapshot.firingNeuronCount = brain_->getFiringNeuronCount();
        snapshot.totalSpikeCount = brain_->getTotalSpikeCount();
        snapshot.averageFiringRate = brain_->getAverageFiringRate();
        snapshot.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
        
        // Collect region data
        for (const auto& region : brain_->getRegions()) {
            RegionData regionData(region->getId());
            regionData.name = region->getName();
            
            // Collect neuron data
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                NeuronData neuronData(neuron->getId());
                const auto& state = neuron->getState();
                
                neuronData.membranePotential = state.membranePotential;
                neuronData.threshold = state.threshold;
                neuronData.firingRate = neuron->getFiringRate();
                neuronData.isFiring = neuron->isFiring();
                neuronData.lastSpikeTime = neuron->getLastSpikeTime();
                neuronData.type = neuron->getType();
                
                regionData.neurons.push_back(neuronData);
            }
            
            // Collect synapse data
            auto synapses = region->getSynapses();
            for (auto& synapse : synapses) {
                SynapseData synapseData(synapse->getId(), synapse->getSourceNeuron(), synapse->getDestinationNeuron());
                synapseData.weight = synapse->getWeight();
                synapseData.delay = synapse->getDelay();
                synapseData.isExcitatory = synapse->isExcitatory();
                synapseData.efficacy = synapse->getEfficacy();
                
                regionData.synapses.push_back(synapseData);
            }
            
            // Calculate region statistics
            if (!regionData.neurons.empty()) {
                float totalFiring = 0.0f;
                for (const auto& neuron : regionData.neurons) {
                    totalFiring += neuron.firingRate;
                }
                regionData.avgFiringRate = totalFiring / regionData.neurons.size();
            }
            
            if (!regionData.synapses.empty()) {
                float totalWeight = 0.0f;
                for (const auto& synapse : regionData.synapses) {
                    totalWeight += std::abs(synapse.weight);
                }
                regionData.avgSynapticWeight = totalWeight / regionData.synapses.size();
            }
            
            snapshot.regions.push_back(regionData);
        }
    }
    
    void collectMemoryData(BrainSnapshot& snapshot) {
        // Working memory
        auto* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            snapshot.workingMemoryTraces = workingMemory->getActiveTraces();
        }
        
        // Episodic memory
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (episodicMemory) {
            snapshot.episodicMemoryEpisodes = episodicMemory->getEpisodeCount();
            snapshot.episodicMemoryCapacity = episodicMemory->getCapacity();
        }
        
        // Associative memory
        auto* associativeMemory = brain_->getAssociativeMemory();
        if (associativeMemory) {
            // Get statistics from associative memory
            snapshot.workingMemoryTraces += associativeMemory->getActiveAssociations();
        }
    }
    
    void collectNeuromodulationData(BrainSnapshot& snapshot) {
        // Dopamine
        auto* dopamine = brain_->getDopamine();
        if (dopamine) {
            snapshot.dopamineLevel = dopamine->getLevel();
        }
        
        // Curiosity
        auto* curiosity = brain_->getCuriosity();
        if (curiosity) {
            snapshot.curiosityLevel = curiosity->getLevel();
        }
        
        // Novelty
        auto* novelty = brain_->getNovelty();
        if (novelty) {
            snapshot.noveltyLevel = novelty->getLevel();
        }
        
        // Prediction error
        auto* predictionError = brain_->getPredictionErrorSignal();
        if (predictionError) {
            snapshot.predictionError = predictionError->getError();
        }
    }
    
    void collectDevelopmentData(BrainSnapshot& snapshot) {
        snapshot.developmentalStage = brain_->getDevelopmentalStage();
        snapshot.developmentalAge = brain_->getConfig()->getOr<double>("developmental_age", 0.0);
        snapshot.plasticityModifier = 1.0f; // This would come from the brain implementation
    }
    
    void updateTimeSeriesData(BrainSnapshot& snapshot) {
        double currentTime = snapshot.timestamp;
        
        // Update neural time series
        updateTimeSeries("Total Spike Count", currentTime, static_cast<double>(snapshot.totalSpikeCount));
        updateTimeSeries("Average Firing Rate", currentTime, snapshot.averageFiringRate);
        updateTimeSeries("Excitation-Inhibition Ratio", currentTime, snapshot.excitationInhibitionRatio);
        updateTimeSeries("Active Neuron Count", currentTime, static_cast<double>(snapshot.activeNeuronCount));
        updateTimeSeries("Firing Neuron Count", currentTime, static_cast<double>(snapshot.firingNeuronCount));
        
        // Update memory time series
        updateTimeSeries("Working Memory Traces", currentTime, static_cast<double>(snapshot.workingMemoryTraces));
        updateTimeSeries("Episodic Memory Episodes", currentTime, static_cast<double>(snapshot.episodicMemoryEpisodes));
        
        // Update neuromodulation time series
        updateTimeSeries("Dopamine Level", currentTime, snapshot.dopamineLevel);
        updateTimeSeries("Curiosity Level", currentTime, snapshot.curiosityLevel);
        updateTimeSeries("Novelty Level", currentTime, snapshot.noveltyLevel);
        updateTimeSeries("Prediction Error", currentTime, snapshot.predictionError);
        
        // Update development time series
        updateTimeSeries("Developmental Age", currentTime, snapshot.developmentalAge);
        updateTimeSeries("Plasticity Modifier", currentTime, snapshot.plasticityModifier);
    }
    
    void updateTimeSeries(const std::string& name, double timestamp, double value) {
        for (auto& series : timeSeries_) {
            if (series.name == name) {
                series.addDataPoint(timestamp, value);
                break;
            }
        }
    }
    
    std::shared_ptr<Brain> brain_;
    std::thread collectThread_;
    std::mutex dataMutex_;
    std::atomic<bool> running_;
    BrainSnapshot currentSnapshot_;
    std::vector<TimeSeriesData> timeSeries_;
};

// Web-based monitoring dashboard
class WebDashboard {
public:
    WebDashboard(int port = 8080, const std::string& host = "0.0.0.0")
        : port_(port), host_(host), running_(false) {
        signal(SIGINT, handleSignal);
        signal(SIGTERM, handleSignal);
    }
    
    ~WebDashboard() {
        running_ = false;
        if (serverThread_.joinable()) {
            serverThread_.join();
        }
    }
    
    void start(VisualizationDataCollector& dataCollector) {
        dataCollector_ = &dataCollector;
        
        // Start web server thread
        serverThread_ = std::thread(&WebDashboard::runServer, this);
        
        running_ = true;
        
        std::cout << "=== NLM Web Dashboard ===" << std::endl;
        std::cout << "Starting dashboard on " << host_ << ":" << port_ << std::endl;
        std::cout << "Access the dashboard at: http://" << host_ << ":" << port_ << std::endl;
        std::cout << "Press Ctrl+C to stop" << std::endl;
    }
    
    void stop() {
        running_ = false;
        if (serverThread_.joinable()) {
            serverThread_.join();
        }
    }
    
    static void handleSignal(int signum) {
        std::cout << "\nReceived signal " << signum << ", stopping dashboard..." << std::endl;
    }
    
private:
    void runServer() {
        // Note: This is a simplified placeholder for a real web server
        // In a real implementation, you would use a web framework like:
        // - Boost.Beast
        // - cpprestsdk
        // - libwebsockets
        // - Or a JavaScript-based solution with WebSockets
        
        std::cout << "Web server would start here in a real implementation" << std::endl;
        std::cout << "The dashboard would provide:
        
        - Real-time neural activity visualization
        - 3D brain model with neurons and synapses
        - Time series charts for all metrics
        - Memory system visualizations
        - Neuromodulation activity displays
        - Development stage tracking
        - Interactive controls for simulation control
        - Data export and analysis tools
        
        " << std::endl;
        
        // For this demonstration, we'll just simulate the web server
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            
            std::cout << "." << std::flush;
            
            // Simulate dashboard updates
            if (dataCollector_) {
                auto snapshot = dataCollector_->getCurrentSnapshot();
                auto timeSeries = dataCollector_->getTimeSeriesData();
                
                std::cout << "\rUpdating dashboard... Neurons: " << snapshot.totalNeuronCount 
                          << " Spikes: " << snapshot.totalSpikeCount 
                          << " Memory: " << snapshot.workingMemoryTraces << " traces" 
                          << std::flush;
            }
        }
        
        std::cout << "\nWeb server stopped." << std::endl;
    }
    
    int port_;
    std::string host_;
    std::atomic<bool> running_;
    std::thread serverThread_;
    VisualizationDataCollector* dataCollector_;
};

// Command-line visualization tools
class CommandLineVisualizationTools {
public:
    CommandLineVisualizationTools(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Generate ASCII art visualization of the brain
    void generateASCIIDiagram() {
        std::cout << "=== NLM Brain Diagram ===" << std::endl;
        std::cout << "This would generate an ASCII art representation of the brain" << std::endl;
        std::cout << "showing regions, neurons, and connections." << std::endl;
        std::cout << std::endl;
        
        for (const auto& region : brain_->getRegions()) {
            std::cout << "Region " << region->getId().index() << ": " << region->getName() << std::endl;
            std::cout << "  Neurons: " << region->getTotalNeuronCount() << std::endl;
            std::cout << "  Synapses: " << region->getSynapseCount() << std::endl;
            std::cout << "  Avg Firing Rate: " << std::fixed << std::setprecision(3) 
                      << region->getAverageFiringRate() << " Hz" << std::endl;
            std::cout << std::endl;
        }
    }
    
    // Generate text-based time series plots
    void generateTimeSeriesPlots(const std::vector<TimeSeriesData>& timeSeries, 
                                 int width = 80, int height = 24) {
        std::cout << "=== Time Series Plots ===" << std::endl;
        
        for (const auto& series : timeSeries) {
            std::cout << std::endl;
            std::cout << "Series: " << series.name << " (" << series.unit << ")" << std::endl;
            
            if (series.timestamps.empty()) {
                std::cout << "  No data available" << std::endl;
                continue;
            }
            
            // Simple line plot
            double minValue = *std::min_element(series.values.begin(), series.values.end());
            double maxValue = *std::max_element(series.values.begin(), series.values.end());
            
            if (maxValue - minValue < 0.001) {
                maxValue = minValue + 1.0; // Avoid division by zero
            }
            
            std::cout << "  Range: " << minValue << " - " << maxValue << std::endl;
            std::cout << "  Points: " << series.timestamps.size() << std::endl;
            
            // Simple ASCII plot
            std::cout << "  Plot:" << std::endl;
            for (int y = height - 1; y >= 0; --y) {
                double threshold = minValue + (maxValue - minValue) * (static_cast<double>(y) / height);
                std::cout << "    |";
                
                for (size_t x = 0; x < series.timestamps.size() && x < width; ++x) {
                    if (series.values[x] >= threshold) {
                        std::cout << "*";
                    } else {
                        std::cout << " ";
                    }
                }
                
                std::cout << std::endl;
            }
        }
    }
    
    // Generate brain statistics report
    void generateBrainStatsReport() {
        std::cout << "=== NLM Brain Statistics Report ===" << std::endl;
        std::cout << "Generated: " << std::chrono::system_clock::now() << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Neural Statistics ===" << std::endl;
        std::cout << "Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
        std::cout << "Firing Neurons: " << brain_->getFiringNeuronCount() << std::endl;
        std::cout << "Total Spike Count: " << brain_->getTotalSpikeCount() << std::endl;
        std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(3) 
                  << brain_->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "Excitation-Inhibition Ratio: " << std::fixed << std::setprecision(3) 
                  << brain_->getExcitationInhibitionRatio() << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Regional Statistics ===" << std::endl;
        for (const auto& region : brain_->getRegions()) {
            std::cout << "Region " << region->getId().index() << ": " << region->getName() << std::endl;
            std::cout << "  Neurons: " << region->getTotalNeuronCount() << std::endl;
            std::cout << "  Populations: " << region->getPopulationCount() << std::endl;
            std::cout << "  Synapses: " << region->getSynapseCount() << std::endl;
            std::cout << "  Active Neurons: " << region->getActiveNeuronCount() << std::endl;
            std::cout << "  Firing Neurons: " << region->getFiringNeuronCount() << std::endl;
            std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) 
                      << region->getAverageFiringRate() << " Hz" << std::endl;
            std::cout << "  Average Synaptic Weight: " << std::fixed << std::setprecision(3) 
                      << region->getAverageSynapticWeight() << std::endl;
            std::cout << "  Synaptic Density: " << std::fixed << std::setprecision(3) 
                      << region->getSynapticDensity() << std::endl;
            std::cout << std::endl;
        }
    }
    
    // Export brain data to various formats
    void exportBrainData(const std::string& filename, const std::string& format = "json") {
        std::cout << "=== Exporting Brain Data ===" << std::endl;
        std::cout << "Format: " << format << std::endl;
        std::cout << "Filename: " << filename << std::endl;
        std::cout << "This would export brain data in " << format << " format." << std::endl;
        std::cout << "Data would include neuron states, synaptic connections, and metrics." << std::endl;
    }
    
    // Save visualization to file
    void saveVisualization(const std::string& filename, const std::string& format = "png") {
        std::cout << "=== Saving Visualization ===" << std::endl;
        std::cout << "Format: " << format << std::endl;
        std::cout << "Filename: " << filename << std::endl;
        std::cout << "This would save the current visualization to " << format << " format." << std::endl;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm

// Convenience functions for visualization and monitoring
inline void startNLMDashboard(std::shared_ptr<Brain> brain, int port = 8080) {
    std::cout << "=== Starting NLM Visualization Dashboard ===" << std::endl;
    
    // Create data collector
    nlm::VisualizationDataCollector dataCollector(brain);
    
    // Start data collection
    dataCollector.startCollecting(0.5); // Collect every 0.5 seconds
    
    // Add default time series
    dataCollector.addTimeSeriesObserver("Total Spike Count");
    dataCollector.addTimeSeriesObserver("Average Firing Rate");
    dataCollector.addTimeSeriesObserver("Active Neuron Count");
    dataCollector.addTimeSeriesObserver("Working Memory Traces");
    dataCollector.addTimeSeriesObserver("Dopamine Level");
    
    // Create web dashboard
    nlm::WebDashboard dashboard(port);
    
    // Start dashboard
    dashboard.start(dataCollector);
    
    // Create CLI visualization tools
    nlm::CommandLineVisualizationTools cliTools(brain);
    
    // Display initial statistics
    cliTools.generateBrainStatsReport();
    
    // Main loop
    std::cout << "Dashboard is running. Press Ctrl+C to stop." << std::endl;
    
    // Wait for dashboard to stop
    dashboard.stop();
    dataCollector.stopCollecting();
    
    std::cout << "=== Dashboard Stopped ===" << std::endl;
}

inline void generateNLMBrainDiagram(std::shared_ptr<Brain> brain) {
    nlm::CommandLineVisualizationTools tools(brain);
    tools.generateASCIIDiagram();
}

inline void exportNLMBrainData(std::shared_ptr<Brain> brain, const std::string& filename, 
                              const std::string& format = "json") {
    nlm::CommandLineVisualizationTools tools(brain);
    tools.exportBrainData(filename, format);
}

inline void saveNLMBrainVisualization(std::shared_ptr<Brain> brain, const std::string& filename, 
                                     const std::string& format = "png") {
    nlm::CommandLineVisualizationTools tools(brain);
    tools.saveVisualization(filename, format);
}