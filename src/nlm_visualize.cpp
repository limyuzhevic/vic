// NLM (熙然) - Neural Learning Machine
// Visualization tool for brain state and neural activity
// Provides real-time visualization and analysis of NLM brain states

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <thread>
#include <algorithm>
#include <cmath>
#include <memory>

using namespace nlm;

void printVisualizationHelp() {
    std::cout << "NLM Visualization Tool - Brain State Visualization" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_visualize [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
    std::cout << "  --version           Show version information" << std::endl;
    std::cout << "  --mode <type>       Visualization mode: static, real-time, replay, analysis" << std::endl;
    std::cout << "  --steps <value>     Number of steps to visualize" << std::endl;
    std::cout << "  --duration <value>  Duration in seconds (for real-time mode)" << std::endl;
    std::cout << "  --file <file>       Load brain state from file" << std::endl;
    std::cout << "  --save <file>       Save visualization output" << std::endl;
    std::cout << "  --format <type>     Output format: text, ascii, html, json" << std::endl;
    std::cout << "  --quality <level>   Quality level: low, medium, high" << std::endl;
    std::cout << "  --interactive       Enable interactive mode" << std::endl;
    std::cout << "  --stats             Show statistics only" << std::endl;
    std::cout << "  --help-advanced     Show advanced options" << std::endl;
    std::cout << std::endl;
    std::cout << "Visualization Modes:" << std::endl;
    std::cout << "  static   - Static visualization of brain state" << std::endl;
    std::cout << "  real-time - Real-time visualization during simulation" << std::endl;
    std::cout << "  replay   - Replay and visualize saved simulation" << std::endl;
    std::cout << "  analysis - Statistical analysis and visualization" << std::endl;
    std::cout << std::endl;
    std::cout << "Output Formats:" << std::endl;
    std::cout << "  text     - Human-readable text output" << std::endl;
    std::cout << "  ascii    - ASCII art visualization" << std::endl;
    std::cout << "  html     - HTML visualization for web viewing" << std::endl;
    std::cout << "  json     - JSON data for programmatic analysis" << std::endl;
    std::cout << std::endl;
    std::cout << "Quality Levels:" << std::endl;
    std::cout << "  low      - Fast, minimal output" << std::endl;
    std::cout << "  medium   - Balanced detail and performance" << std::endl;
    std::cout << "  high     - Maximum detail and accuracy" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_visualize --mode static --file brain.brn --format html" << std::endl;
    std::cout << "  nlm_visualize --mode real-time --duration 30 --quality high" << std::endl;
    std::cout << "  nlm_visualize --mode analysis --stats --format json --save analysis.json" << std::endl;
}

void printVersion() {
    std::cout << "NLM Visualization Tool 0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
}

// Visualization data structures
struct NeuronVisualization {
    NeuronId id;
    NeuronType type;
    float potential;
    bool active;
    float firingRate;
    float connectivity;
    
    std::string toASCII() const {
        std::stringstream ss;
        char activity = active ? '●' : '.';
        char typeChar;
        switch (type) {
            case NeuronType::Excitatory: typeChar = 'E'; break;
            case NeuronType::Inhibitory: typeChar = 'I'; break;
            case NeuronType::Sensory: typeChar = 'S'; break;
            case NeuronType::Motor: typeChar = 'M'; break;
            default: typeChar = '?'; break;
        }
        
        ss << "[ID:" << std::setw(4) << id << "] "
           << typeChar << " " << activity
           << " Pot:" << std::fixed << std::setprecision(2) << potential
           << " FR:" << firingRate;
        
        return ss.str();
    }
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"id\":" << id << ",\"type\":" << static_cast<int>(type) 
           << ",\"potential\":" << potential << ",\"active\":" << (active ? "true" : "false")
           << ",\"firingRate\":" << firingRate << ",\"connectivity\":" << connectivity << "}";
        return ss.str();
    }
};

struct SynapseVisualization {
    SynapseId id;
    NeuronId preNeuron;
    NeuronId postNeuron;
    float weight;
    bool active;
    float delay;
    
    std::string toASCII() const {
        std::stringstream ss;
        ss << "SynID:" << std::setw(6) << id 
           << " [" << preNeuron << "→" << postNeuron << "] "
           << "W:" << std::fixed << std::setprecision(3) << weight
           << " " << (active ? "ACT" : "INACT");
        return ss.str();
    }
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"id\":" << id << ",\"pre\":" << preNeuron 
           << ",\"post\":" << postNeuron << ",\"weight\":" << weight
           << ",\"active\":" << (active ? "true" : "false") 
           << ",\"delay\":" << delay << "}";
        return ss.str();
    }
};

struct MemoryVisualization {
    std::string name;
    size_t capacity;
    size_t used;
    float utilization;
    std::vector<std::string> sampleContents;
    
    std::string toASCII() const {
        std::stringstream ss;
        ss << "Memory: " << std::left << std::setw(20) << name
           << "[" << std::setw(3) << used << "/" << capacity << "] "
           << std::fixed << std::setprecision(1) << (utilization * 100) << "%";
        return ss.str();
    }
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"name\":\"" << name << "\",\"capacity\":" << capacity
           << ",\"used\":" << used << ",\"utilization\":" << utilization
           << ",\"sampleContents\":[";
        for (size_t i = 0; i < sampleContents.size(); ++i) {
            if (i > 0) ss << ",";
            ss << "\"" << sampleContents[i] << "\"";
        }
        ss << "]}";
        return ss.str();
    }
};

struct RegionVisualization {
    RegionId id;
    std::string name;
    size_t neuronCount;
    size_t activeNeurons;
    float avgPotential;
    float avgFiringRate;
    float excitationInhibitionRatio;
    
    std::string toASCII() const {
        std::stringstream ss;
        ss << "Region " << std::setw(2) << id << ": " << std::left << std::setw(15) << name
           << "Neurons:" << std::setw(3) << neuronCount
           << " Active:" << std::setw(3) << activeNeurons
           << " Pot:" << std::fixed << std::setprecision(2) << avgPotential
           << " FR:" << avgFiringRate
           << " E/I:" << excitationInhibitionRatio;
        return ss.str();
    }
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"id\":" << id << ",\"name\":\"" << name << "\",\"neuronCount\":" << neuronCount
           << ",\"activeNeurons\":" << activeNeurons << ",\"avgPotential\":" << avgPotential
           << ",\"avgFiringRate\":" << avgFiringRate << ",\"excitationInhibitionRatio\":" << excitationInhibitionRatio << "}";
        return ss.str();
    }
};

struct VisualizationData {
    std::string timestamp;
    SimulationStep currentStep;
    Timestamp currentTime;
    std::vector<NeuronVisualization> neurons;
    std::vector<SynapseVisualization> synapses;
    std::vector<RegionVisualization> regions;
    std::vector<MemoryVisualization> memorySystems;
    float overallFiringRate;
    float averagePotential;
    float totalConnectivity;
    std::string visualizationMode;
};

class NLMVisualizer {
public:
    NLMVisualizer() : running_(false), currentStep_(0) {}
    
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
    }
    
    void setConfig(std::shared_ptr<Config> config) {
        config_ = config;
    }
    
    void setMode(const std::string& mode) {
        mode_ = mode;
    }
    
    void setSteps(size_t steps) {
        steps_ = steps;
    }
    
    void setDuration(double duration) {
        duration_ = duration;
    }
    
    void setOutputFile(const std::string& filepath) {
        outputFile_ = filepath;
    }
    
    void setOutputFormat(const std::string& format) {
        outputFormat_ = format;
    }
    
    void setQuality(int quality) {
        quality_ = quality;
    }
    
    void setInteractive(bool interactive) {
        interactive_ = interactive;
    }
    
    void setStatsOnly(bool statsOnly) {
        statsOnly_ = statsOnly;
    }
    
    void run() {
        if (!brain_) {
            std::cerr << "Error: No brain loaded" << std::endl;
            return;
        }
        
        std::cout << "=== NLM Visualization Tool ===" << std::endl;
        std::cout << "Mode: " << mode_ << std::endl;
        std::cout << "Output Format: " << outputFormat_ << std::endl;
        std::cout << "Quality: " << quality_ << std::endl;
        std::cout << std::endl;
        
        if (mode_ == "static") {
            runStaticVisualization();
        } else if (mode_ == "real-time") {
            runRealTimeVisualization();
        } else if (mode_ == "replay") {
            runReplayVisualization();
        } else if (mode_ == "analysis") {
            runAnalysisVisualization();
        } else {
            std::cerr << "Error: Unknown visualization mode: " << mode_ << std::endl;
            return;
        }
        
        saveOutput();
    }
    
private:
    void runStaticVisualization() {
        std::cout << "--- Static Visualization ---" << std::endl;
        
        // Collect visualization data
        VisualizationData data;
        data.currentStep = 0;
        data.currentTime = 0.0;
        data.timestamp = getCurrentTimestamp();
        data.visualizationMode = "static";
        
        collectVisualizationData(data);
        
        // Display data based on quality and format
        if (outputFormat_ == "ascii") {
            displayASCIIVisualization(data);
        } else if (outputFormat_ == "json") {
            displayJSONVisualization(data);
        } else {
            displayTextVisualization(data);
        }
        
        showStatistics(data);
    }
    
    void runRealTimeVisualization() {
        std::cout << "--- Real-time Visualization ---" << std::endl;
        std::cout << "Running for " << duration_ << " seconds..." << std::endl;
        std::cout << "Press Ctrl+C to stop early." << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        size_t frameCount = 0;
        
        running_ = true;
        while (running_ && (duration_ <= 0 || 
            std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count() < duration_)) {
            
            VisualizationData data;
            data.currentStep = currentStep_;
            data.currentTime = currentStep_ * config_->getOr<double>("simulation_timestep", 0.001);
            data.timestamp = getCurrentTimestamp();
            data.visualizationMode = "real-time";
            
            collectVisualizationData(data);
            
            displayRealTimeVisualization(data);
            
            // Advance simulation
            if (statsOnly_) {
                // For stats-only mode, don't advance steps
                break;
            }
            
            brain_->step(currentStep_, currentStep_ * config_->getOr<double>("simulation_timestep", 0.001));
            currentStep_++;
            
            frameCount++;
            
            // Control frame rate
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        running_ = false;
        std::cout << std::endl;
        std::cout << "Visualization completed. Total frames: " << frameCount << std::endl;
    }
    
    void runReplayVisualization() {
        std::cout << "--- Replay Visualization ---" << std::endl;
        
        // In a real implementation, this would load a saved brain state
        // For now, we'll just run the simulation in analysis mode
        if (steps_ == 0) {
            steps_ = 1000;
        }
        
        runAnalysisVisualization();
    }
    
    void runAnalysisVisualization() {
        std::cout << "--- Analysis Visualization ---" << std::endl;
        
        if (steps_ == 0) {
            steps_ = 1000;
        }
        
        VisualizationData data;
        data.currentStep = static_cast<SimulationStep>(steps_);
        data.currentTime = steps_ * config_->getOr<double>("simulation_timestep", 0.001);
        data.timestamp = getCurrentTimestamp();
        data.visualizationMode = "analysis";
        
        collectVisualizationData(data);
        
        if (outputFormat_ == "ascii") {
            displayASCIIDetailedVisualization(data);
        } else if (outputFormat_ == "json") {
            displayJSONDetailedVisualization(data);
        } else {
            displayTextDetailedVisualization(data);
        }
        
        showDetailedStatistics(data);
    }
    
    void collectVisualizationData(VisualizationData& data) {
        if (!brain_) return;
        
        // Collect neuron data
        size_t totalNeurons = brain_->getTotalNeuronCount();
        data.neurons.reserve(totalNeurons);
        
        for (const auto& regionId : brain_->getRegionIds()) {
            if (auto* region = brain_->getRegion(regionId)) {
                RegionVisualization regionViz;
                regionViz.id = regionId;
                regionViz.name = region->getName();
                regionViz.neuronCount = region->getNeuronCount();
                regionViz.activeNeurons = region->getActiveNeuronCount();
                
                // Calculate average potential and firing rate
                float totalPot = 0.0f, totalFiring = 0.0f;
                size_t neuronCount = 0;
                
                for (const auto& neuron : region->getAllNeurons()) {
                    if (neuron) {
                        totalPot += neuron->getMembranePotential();
                        totalFiring += neuron->getFiringRate();
                        neuronCount++;
                        
                        // Add to neuron visualization list
                        NeuronVisualization nv;
                        nv.id = neuron->getId();
                        nv.type = neuron->getType();
                        nv.potential = neuron->getMembranePotential();
                        nv.active = neuron->isActive();
                        nv.firingRate = neuron->getFiringRate();
                        nv.connectivity = 0.0f; // Simplified - would calculate actual connectivity
                        data.neurons.push_back(nv);
                    }
                }
                
                if (neuronCount > 0) {
                    regionViz.avgPotential = totalPot / neuronCount;
                    regionViz.avgFiringRate = totalFiring / neuronCount;
                    regionViz.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
                }
                
                data.regions.push_back(regionViz);
            }
        }
        
        // Collect synapse data (sample for performance)
        if (quality_ >= 2) { // Only collect full synapse data for high quality
            for (const auto& regionId : brain_->getRegionIds()) {
                if (auto* region = brain_->getRegion(regionId)) {
                    for (const auto& synapse : region->getSynapses()) {
                        if (synapse) {
                            SynapseVisualization sv;
                            sv.id = synapse->getId();
                            sv.preNeuron = synapse->getPreNeuronId();
                            sv.postNeuron = synapse->getPostNeuronId();
                            sv.weight = synapse->getWeight();
                            sv.active = synapse->isActive();
                            sv.delay = synapse->getDelay();
                            data.synapses.push_back(sv);
                        }
                    }
                }
            }
        } else {
            // Sample synapses for lower quality
            size_t synapseCount = 0;
            for (const auto& regionId : brain_->getRegionIds()) {
                if (auto* region = brain_->getRegion(regionId)) {
                    for (const auto& synapse : region->getSynapses()) {
                        if (synapse && ++synapseCount <= 100) { // Limit to 100 synapses
                            SynapseVisualization sv;
                            sv.id = synapse->getId();
                            sv.preNeuron = synapse->getPreNeuronId();
                            sv.postNeuron = synapse->getPostNeuronId();
                            sv.weight = synapse->getWeight();
                            sv.active = synapse->isActive();
                            sv.delay = synapse->getDelay();
                            data.synapses.push_back(sv);
                        }
                    }
                }
            }
        }
        
        // Collect memory system data
        if (auto* workingMem = brain_->getWorkingMemory()) {
            MemoryVisualization memViz;
            memViz.name = "Working Memory";
            memViz.capacity = workingMem->getCapacity();
            memViz.used = workingMem->getActiveTraceCount();
            memViz.utilization = static_cast<float>(memViz.used) / memViz.capacity;
            memViz.sampleContents = {"trace1", "trace2", "trace3"}; // Simplified
            data.memorySystems.push_back(memViz);
        }
        
        if (auto* episodicMem = brain_->getEpisodicMemory()) {
            MemoryVisualization memViz;
            memViz.name = "Episodic Memory";
            memViz.capacity = episodicMem->getMaxEpisodes();
            memViz.used = episodicMem->getEpisodeCount();
            memViz.utilization = static_cast<float>(memViz.used) / memViz.capacity;
            memViz.sampleContents = {"episode1", "episode2", "episode3"}; // Simplified
            data.memorySystems.push_back(memViz);
        }
        
        if (auto* assocMem = brain_->getAssociativeMemory()) {
            MemoryVisualization memViz;
            memViz.name = "Associative Memory";
            memViz.capacity = 5000; // Simplified
            memViz.used = assocMem->getAssociationCount();
            memViz.utilization = static_cast<float>(memViz.used) / memViz.capacity;
            memViz.sampleContents = {"pattern1", "pattern2", "pattern3"}; // Simplified
            data.memorySystems.push_back(memViz);
        }
        
        // Calculate overall statistics
        data.overallFiringRate = brain_->getAverageFiringRate();
        data.averagePotential = brain_->getFiringNeuronCount() > 0 ? 
            brain_->getExcitationInhibitionRatio() : 0.0f;
        
        double totalConn = 0.0;
        for (const auto& synapse : data.synapses) {
            totalConn += synapse.weight;
        }
        data.totalConnectivity = static_cast<float>(totalConn / data.synapses.size());
    }
    
    void displayASCIIVisualization(const VisualizationData& data) {
        std::cout << "=== NLM Brain Visualization (ASCII) ===" << std::endl;
        std::cout << "Step: " << data.currentStep << " | Time: " << data.currentTime << "s" << std::endl;
        std::cout << std::endl;
        
        // Show regions
        std::cout << "--- Regions ---" << std::endl;
        for (const auto& region : data.regions) {
            std::cout << "  " << region.toASCII() << std::endl;
        }
        std::cout << std::endl;
        
        // Show neurons (sampled for performance)
        std::cout << "--- Neurons (sample) ---" << std::endl;
        size_t neuronSample = std::min(static_cast<size_t>(10), data.neurons.size());
        for (size_t i = 0; i < neuronSample; ++i) {
            std::cout << "  " << data.neurons[i].toASCII() << std::endl;
        }
        std::cout << "  ... and " << data.neurons.size() - neuronSample << " more neurons" << std::endl;
        std::cout << std::endl;
        
        // Show synapses (sampled)
        std::cout << "--- Synapses (sample) ---" << std::endl;
        size_t synapseSample = std::min(static_cast<size_t>(10), data.synapses.size());
        for (size_t i = 0; i < synapseSample; ++i) {
            std::cout << "  " << data.synapses[i].toASCII() << std::endl;
        }
        std::cout << "  ... and " << data.synapses.size() - synapseSample << " more synapses" << std::endl;
        std::cout << std::endl;
        
        // Show memory systems
        std::cout << "--- Memory Systems ---" << std::endl;
        for (const auto& mem : data.memorySystems) {
            std::cout << "  " << mem.toASCII() << std::endl;
        }
        std::cout << std::endl;
        
        // Show overall stats
        std::cout << "--- Overall Statistics ---" << std::endl;
        std::cout << "  Total Neurons: " << data.neurons.size() << std::endl;
        std::cout << "  Total Synapses: " << data.synapses.size() << std::endl;
        std::cout << "  Average Firing Rate: " << data.overallFiringRate << std::endl;
        std::cout << "  Total Connectivity: " << data.totalConnectivity << std::endl;
    }
    
    void displayJSONVisualization(const VisualizationData& data) {
        std::cout << "{" << std::endl;
        std::cout << "  \"timestamp\": \"" << data.timestamp << "\"," << std::endl;
        std::cout << "  \"step\": " << data.currentStep << "," << std::endl;
        std::cout << "  \"time\": " << data.currentTime << "," << std::endl;
        std::cout << "  \"mode\": \"" << data.visualizationMode << "\"," << std::endl;
        std::cout << "  \"regions\": [" << std::endl;
        
        for (size_t i = 0; i < data.regions.size(); ++i) {
            std::cout << "    {" << data.regions[i].toJSON();
            if (i < data.regions.size() - 1) std::cout << "},";
            else std::cout << "}";
            std::cout << std::endl;
        }
        
        std::cout << "  ]," << std::endl;
        std::cout << "  \"neurons\": [" << std::endl;
        
        for (size_t i = 0; i < std::min(static_cast<size_t>(10), data.neurons.size()); ++i) {
            std::cout << "    {" << data.neurons[i].toJSON();
            if (i < std::min(static_cast<size_t>(10), data.neurons.size()) - 1) std::cout << "},";
            else std::cout << "}";
            std::cout << std::endl;
        }
        if (data.neurons.size() > 10) {
            std::cout << "    ... and " << data.neurons.size() - 10 << " more" << std::endl;
        }
        
        std::cout << "  ]," << std::endl;
        std::cout << "  \"synapses\": [" << std::endl;
        
        for (size_t i = 0; i < std::min(static_cast<size_t>(10), data.synapses.size()); ++i) {
            std::cout << "    {" << data.synapses[i].toJSON();
            if (i < std::min(static_cast<size_t>(10), data.synapses.size()) - 1) std::cout << "},";
            else std::cout << "}";
            std::cout << std::endl;
        }
        
        if (data.synapses.size() > 10) {
            std::cout << "    ... and " << data.synapses.size() - 10 << " more" << std::endl;
        }
        
        std::cout << "  ]," << std::endl;
        std::cout << "  \"memorySystems\": [" << std::endl;
        
        for (size_t i = 0; i < data.memorySystems.size(); ++i) {
            std::cout << "    {" << data.memorySystems[i].toJSON();
            if (i < data.memorySystems.size() - 1) std::cout << "},";
            else std::cout << "}";
            std::cout << std::endl;
        }
        
        std::cout << "  ]," << std::endl;
        std::cout << "  \"overallFiringRate\": " << data.overallFiringRate << "," << std::endl;
        std::cout << "  \"averagePotential\": " << data.averagePotential << "," << std::endl;
        std::cout << "  \"totalConnectivity\": " << data.totalConnectivity << std::endl;
        std::cout << "}" << std::endl;
    }
    
    void displayTextVisualization(const VisualizationData& data) {
        std::cout << "=== NLM Brain Visualization (Text) ===" << std::endl;
        std::cout << "Timestamp: " << data.timestamp << std::endl;
        std::cout << "Step: " << data.currentStep << std::endl;
        std::cout << "Time: " << data.currentTime << "s" << std::endl;
        std::cout << "Mode: " << data.visualizationMode << std::endl;
        std::cout << std::endl;
        
        for (const auto& region : data.regions) {
            std::cout << "Region " << region.id << " (" << region.name << "):" << std::endl;
            std::cout << "  Neurons: " << region.neuronCount 
                     << " | Active: " << region.activeNeurons
                     << " | Avg Potential: " << region.avgPotential
                     << " | Avg Firing Rate: " << region.avgFiringRate
                     << " | E/I Ratio: " << region.excitationInhibitionRatio << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Memory Systems:" << std::endl;
        for (const auto& mem : data.memorySystems) {
            std::cout << "  " << mem.name << ": " << mem.used << "/" << mem.capacity 
                     << " (" << (mem.utilization * 100) << "%)" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Overall Statistics:" << std::endl;
        std::cout << "  Total Neurons: " << data.neurons.size() << std::endl;
        std::cout << "  Total Synapses: " << data.synapses.size() << std::endl;
        std::cout << "  Average Firing Rate: " << data.overallFiringRate << std::endl;
        std::cout << "  Average Potential: " << data.averagePotential << std::endl;
        std::cout << "  Total Connectivity: " << data.totalConnectivity << std::endl;
    }
    
    void displayRealTimeVisualization(const VisualizationData& data) {
        // Clear screen and display current state
        system("clear"); // Use "cls" for Windows
        
        std::cout << "=== NLM Real-time Visualization ===" << std::endl;
        std::cout << "Step: " << data.currentStep << " | Time: " << data.currentTime << "s" << std::endl;
        std::cout << "Press Ctrl+C to stop" << std::endl;
        std::cout << std::endl;
        
        // Simple real-time display
        for (const auto& region : data.regions) {
            std::cout << "Region " << region.id << ": "
                     << region.activeNeurons << "/" << region.neuronCount 
                     << " neurons active | " << region.avgFiringRate << " avg firing rate" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Overall: " << data.overallFiringRate << " avg firing rate" << std::endl;
        
        // Display some neuron activity
        if (!data.neurons.empty()) {
            std::cout << std::endl;
            std::cout << "Sample Neuron Activity:" << std::endl;
            for (size_t i = 0; i < std::min(static_cast<size_t>(5), data.neurons.size()); ++i) {
                const auto& n = data.neurons[i];
                std::cout << "  Neuron " << n.id << ": " << (n.active ? "ACTIVE" : "inactive")
                         << " | Potential: " << n.potential << std::endl;
            }
        }
        
        // Show memory status
        std::cout << std::endl;
        std::cout << "Memory Status:" << std::endl;
        for (const auto& mem : data.memorySystems) {
            std::cout << "  " << mem.name << ": " << (mem.utilization > 0.8 ? "HIGH" : "normal")
                     << " utilization (" << (mem.utilization * 100) << "%)" << std::endl;
        }
        
        // Add a simple progress bar
        double progress = static_cast<double>(data.currentStep) / steps_;
        int barWidth = 50;
        int pos = static_cast<int>(barWidth * progress);
        
        std::cout << std::endl;
        std::cout << "Progress: [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100) << "%" << std::endl;
    }
    
    void displayASCIIDetailedVisualization(const VisualizationData& data) {
        std::cout << "=== NLM Detailed Analysis Visualization ===" << std::endl;
        std::cout << "Step: " << data.currentStep << " | Time: " << data.currentTime << "s" << std::endl;
        std::cout << std::endl;
        
        // Show region statistics
        std::cout << "--- Region Analysis ---" << std::endl;
        for (const auto& region : data.regions) {
            std::cout << "  " << region.toASCII() << std::endl;
            
            // Show neuron distribution by type if we have detailed data
            if (quality_ >= 3) {
                std::cout << "    Neuron distribution:" << std::endl;
                // Simplified - would show actual distribution
                std::cout << "      Excitatory: ~" << region.neuronCount * 0.6 << std::endl;
                std::cout << "      Inhibitory: ~" << region.neuronCount * 0.2 << std::endl;
                std::cout << "      Sensory: ~" << region.neuronCount * 0.1 << std::endl;
                std::cout << "      Motor: ~" << region.neuronCount * 0.1 << std::endl;
            }
        }
        
        // Show memory analysis
        std::cout << std::endl;
        std::cout << "--- Memory Analysis ---" << std::endl;
        for (const auto& mem : data.memorySystems) {
            std::cout << "  " << mem.toASCII() << std::endl;
            
            if (quality_ >= 3 && mem.used > 0) {
                std::cout << "    Sample contents:" << std::endl;
                for (size_t i = 0; i < std::min(static_cast<size_t>(3), mem.sampleContents.size()); ++i) {
                    std::cout << "      - " << mem.sampleContents[i] << std::endl;
                }
            }
        }
        
        // Show connectivity analysis
        if (quality_ >= 2) {
            std::cout << std::endl;
            std::cout << "--- Connectivity Analysis ---" << std::endl;
            std::cout << "  Total synapses: " << data.synapses.size() << std::endl;
            std::cout << "  Average weight: " << data.totalConnectivity / data.synapses.size() << std::endl;
            
            if (quality_ >= 3) {
                // Show synapse statistics
                int activeCount = 0, inactiveCount = 0;
                float minWeight = 1e6f, maxWeight = -1e6f;
                for (const auto& synapse : data.synapses) {
                    if (synapse.active) activeCount++;
                    else inactiveCount++;
                    minWeight = std::min(minWeight, synapse.weight);
                    maxWeight = std::max(maxWeight, synapse.weight);
                }
                
                std::cout << "    Active synapses: " << activeCount << std::endl;
                std::cout << "    Inactive synapses: " << inactiveCount << std::endl;
                std::cout << "    Weight range: " << minWeight << " - " << maxWeight << std::endl;
                std::cout << "    Average delay: " << calculateAverageDelay(data.synapses) << std::endl;
            }
        }
    }
    
    double calculateAverageDelay(const std::vector<SynapseVisualization>& synapses) {
        if (synapses.empty()) return 0.0;
        
        double totalDelay = 0.0;
        for (const auto& synapse : synapses) {
            totalDelay += synapse.delay;
        }
        
        return totalDelay / synapses.size();
    }
    
    void displayJSONDetailedVisualization(const VisualizationData& data) {
        std::cout << "{" << std::endl;
        std::cout << "  \"timestamp\": \"" << data.timestamp << "\"," << std::endl;
        std::cout << "  \"step\": " << data.currentStep << "," << std::endl;
        std::cout << "  \"time\": " << data.currentTime << "," << std::endl;
        std::cout << "  \"analysis_mode\": true," << std::endl;
        std::cout << "  \"region_analysis\": [" << std::endl;
        
        for (size_t i = 0; i < data.regions.size(); ++i) {
            std::cout << "    {" << data.regions[i].toJSON();
            if (i < data.regions.size() - 1) std::cout << "},";
            else std::cout << "}";
            std::cout << std::endl;
        }
        
        std::cout << "  ]," << std::endl;
        std::cout << "  \"memory_analysis\": [" << std::endl;
        
        for (size_t i = 0; i < data.memorySystems.size(); ++i) {
            std::cout << "    {" << data.memorySystems[i].toJSON();
            if (i < data.memorySystems.size() - 1) std::cout << "},";
            else std::cout << "}";
            std::cout << std::endl;
        }
        
        std::cout << "  ]," << std::endl;
        std::cout << "  \"connectivity_stats\": {" << std::endl;
        std::cout << "    \"total_synapses\": " << data.synapses.size() << "," << std::endl;
        std::cout << "    \"avg_weight\": " << data.totalConnectivity / (data.synapses.size() > 0 ? data.synapses.size() : 1) << "," << std::endl;
        std::cout << "    \"delay_stats\": {" << std::endl;
        double minDelay = 1e6, maxDelay = -1e6;
        double avgDelay = 0.0;
        for (const auto& synapse : data.synapses) {
            minDelay = std::min(minDelay, synapse.delay);
            maxDelay = std::max(maxDelay, synapse.delay);
            avgDelay += synapse.delay;
        }
        if (!data.synapses.empty()) avgDelay /= data.synapses.size();
        std::cout << "      \"min\": " << minDelay << "," << std::endl;
        std::cout << "      \"max\": " << maxDelay << "," << std::endl;
        std::cout << "      \"avg\": " << avgDelay << std::endl;
        std::cout << "    }" << std::endl;
        std::cout << "  }" << std::endl;
        std::cout << "}" << std::endl;
    }
    
    void displayTextDetailedVisualization(const VisualizationData& data) {
        std::cout << "=== NLM Detailed Analysis ===" << std::endl;
        std::cout << "Step: " << data.currentStep << " | Time: " << data.currentTime << "s" << std::endl;
        std::cout << "Analysis Mode: Enabled" << std::endl;
        std::cout << std::endl;
        
        // Detailed region analysis
        std::cout << "--- Detailed Region Analysis ---" << std::endl;
        for (const auto& region : data.regions) {
            std::cout << "Region " << region.id << ": " << region.name << std::endl;
            std::cout << "  Statistics:" << std::endl;
            std::cout << "    Neurons: " << region.neuronCount << std::endl;
            std::cout << "    Active Neurons: " << region.activeNeurons << std::endl;
            std::cout << "    Activity Ratio: " << (region.neuronCount > 0 ? 
                static_cast<double>(region.activeNeurons) / region.neuronCount : 0.0) << std::endl;
            std::cout << "    Average Potential: " << region.avgPotential << std::endl;
            std::cout << "    Average Firing Rate: " << region.avgFiringRate << std::endl;
            std::cout << "    Excitatory/Inhibitory Ratio: " << region.excitationInhibitionRatio << std::endl;
        }
        
        // Memory system analysis
        std::cout << std::endl;
        std::cout << "--- Memory System Analysis ---" << std::endl;
        for (const auto& mem : data.memorySystems) {
            std::cout << "Memory System: " << mem.name << std::endl;
            std::cout << "  Capacity: " << mem.capacity << std::endl;
            std::cout << "  Usage: " << mem.used << std::endl;
            std::cout << "  Utilization: " << (mem.utilization * 100) << "%" << std::endl;
            std::cout << "  Status: " << (mem.utilization > 0.8 ? "HIGH" : "NORMAL") << std::endl;
            
            if (quality_ >= 2) {
                std::cout << "  Sample Contents:" << std::endl;
                for (size_t i = 0; i < std::min(static_cast<size_t>(3), mem.sampleContents.size()); ++i) {
                    std::cout << "    - " << mem.sampleContents[i] << std::endl;
                }
            }
        }
        
        // Connectivity analysis
        if (quality_ >= 1) {
            std::cout << std::endl;
            std::cout << "--- Connectivity Analysis ---" << std::endl;
            std::cout << "Total Synapses: " << data.synapses.size() << std::endl;
            std::cout << "Average Weight: " << data.totalConnectivity << std::endl;
            
            if (quality_ >= 2) {
                // Calculate synapse statistics
                int active = 0, inactive = 0;
                float minW = 1e6, maxW = -1e6;
                for (const auto& synapse : data.synapses) {
                    if (synapse.active) active++;
                    else inactive++;
                    minW = std::min(minW, synapse.weight);
                    maxW = std::max(maxW, synapse.weight);
                }
                
                std::cout << "Active Synapses: " << active << std::endl;
                std::cout << "Inactive Synapses: " << inactive << std::endl;
                std::cout << "Weight Range: " << minW << " - " << maxW << std::endl;
                
                double avgDelay = calculateAverageDelay(data.synapses);
                std::cout << "Average Delay: " << avgDelay << std::endl;
            }
        }
        
        // Overall system health
        std::cout << std::endl;
        std::cout << "--- System Health ---" << std::endl;
        std::cout << "Overall Firing Rate: " << data.overallFiringRate << std::endl;
        std::cout << "Average Potential: " << data.averagePotential << std::endl;
        std::cout << "Memory Pressure: " << (calculateMemoryPressure(data.memorySystems) * 100) << "%" << std::endl;
        std::cout << "System Status: " << (isSystemHealthy(data) ? "HEALTHY" : "DEGRADED") << std::endl;
    }
    
    double calculateMemoryPressure(const std::vector<MemoryVisualization>& memorySystems) {
        if (memorySystems.empty()) return 0.0;
        
        double totalPressure = 0.0;
        for (const auto& mem : memorySystems) {
            totalPressure += mem.utilization;
        }
        
        return totalPressure / memorySystems.size();
    }
    
    bool isSystemHealthy(const VisualizationData& data) {
        // Simple health check based on firing rate and memory pressure
        bool healthy = true;
        
        // Check firing rate
        if (data.overallFiringRate > 100.0) { // Arbitrary threshold
            healthy = false;
        }
        
        // Check memory pressure
        double memoryPressure = calculateMemoryPressure(data.memorySystems);
        if (memoryPressure > 0.9) {
            healthy = false;
        }
        
        return healthy;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void showStatistics(const VisualizationData& data) {
        if (!statsOnly_) return;
        
        std::cout << "=== Statistics Only ===" << std::endl;
        std::cout << "Generated at: " << data.timestamp << std::endl;
        std::cout << "Step: " << data.currentStep << std::endl;
        std::cout << "Time: " << data.currentTime << "s" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Region Statistics:" << std::endl;
        for (const auto& region : data.regions) {
            std::cout << "  " << region.name << ": "
                     << region.activeNeurons << "/" << region.neuronCount 
                     << " active | " << region.avgFiringRate << " avg FR" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Memory Statistics:" << std::endl;
        for (const auto& mem : data.memorySystems) {
            std::cout << "  " << mem.name << ": " << (mem.utilization * 100) << "% utilized" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "System Statistics:" << std::endl;
        std::cout << "  Overall Firing Rate: " << data.overallFiringRate << std::endl;
        std::cout << "  Average Potential: " << data.averagePotential << std::endl;
        std::cout << "  Total Synapses: " << data.synapses.size() << std::endl;
        std::cout << "  Connectivity: " << data.totalConnectivity << std::endl;
        std::cout << "  System Healthy: " << (isSystemHealthy(data) ? "Yes" : "No") << std::endl;
    }
    
    void showDetailedStatistics(const VisualizationData& data) {
        std::cout << "=== Detailed Statistics ===" << std::endl;
        std::cout << "Generated at: " << data.timestamp << std::endl;
        std::cout << "Visualization Mode: " << data.visualizationMode << std::endl;
        std::cout << std::endl;
        
        // Detailed neuron statistics
        std::cout << "--- Neuron Statistics ---" << std::endl;
        std::cout << "Total Neurons: " << data.neurons.size() << std::endl;
        if (!data.neurons.empty()) {
            float minPot = 1e6, maxPot = -1e6, avgPot = 0.0f;
            float minFR = 1e6, maxFR = -1e6, avgFR = 0.0f;
            int activeCount = 0;
            
            for (const auto& neuron : data.neurons) {
                minPot = std::min(minPot, neuron.potential);
                maxPot = std::max(maxPot, neuron.potential);
                avgPot += neuron.potential;
                
                minFR = std::min(minFR, neuron.firingRate);
                maxFR = std::max(maxFR, neuron.firingRate);
                avgFR += neuron.firingRate;
                
                if (neuron.active) activeCount++;
            }
            
            if (data.neurons.size() > 0) {
                avgPot /= data.neurons.size();
                avgFR /= data.neurons.size();
                
                std::cout << "  Potential: " << minPot << " - " << maxPot << " (avg: " << avgPot << ")" << std::endl;
                std::cout << "  Firing Rate: " << minFR << " - " << maxFR << " (avg: " << avgFR << ")" << std::endl;
                std::cout << "  Active Neurons: " << activeCount << "/" << data.neurons.size() 
                         << " (" << (static_cast<double>(activeCount) / data.neurons.size() * 100) << "%)" << std::endl;
            }
        }
        
        // Detailed synapse statistics
        if (!data.synapses.empty()) {
            std::cout << std::endl;
            std::cout << "--- Synapse Statistics ---" << std::endl;
            
            float minWeight = 1e6, maxWeight = -1e6, avgWeight = 0.0f;
            double minDelay = 1e6, maxDelay = -1e6, avgDelay = 0.0;
            int activeSynapses = 0;
            
            for (const auto& synapse : data.synapses) {
                minWeight = std::min(minWeight, synapse.weight);
                maxWeight = std::max(maxWeight, synapse.weight);
                avgWeight += synapse.weight;
                
                minDelay = std::min(minDelay, synapse.delay);
                maxDelay = std::max(maxDelay, synapse.delay);
                avgDelay += synapse.delay;
                
                if (synapse.active) activeSynapses++;
            }
            
            size_t count = data.synapses.size();
            avgWeight /= count;
            avgDelay /= count;
            
            std::cout << "Total Synapses: " << count << std::endl;
            std::cout << "Active Synapses: " << activeSynapses << "/" << count 
                     << " (" << (static_cast<double>(activeSynapses) / count * 100) << "%)" << std::endl;
            std::cout << "Weight Range: " << minWeight << " - " << maxWeight << " (avg: " << avgWeight << ")" << std::endl;
            std::cout << "Delay Range: " << minDelay << " - " << maxDelay << " (avg: " << avgDelay << ")" << std::endl;
        }
        
        // Memory system statistics
        std::cout << std::endl;
        std::cout << "--- Memory System Statistics ---" << std::endl;
        for (const auto& mem : data.memorySystems) {
            std::cout << mem.name << ": " << mem.used << "/" << mem.capacity 
                     << " (" << (mem.utilization * 100) << "%)" << std::endl;
        }
        
        // Overall system metrics
        std::cout << std::endl;
        std::cout << "--- Overall System Metrics ---" << std::endl;
        std::cout << "Overall Firing Rate: " << data.overallFiringRate << std::endl;
        std::cout << "Average Potential: " << data.averagePotential << std::endl;
        std::cout << "Total Connectivity: " << data.totalConnectivity << std::endl;
        std::cout << "Memory Pressure: " << (calculateMemoryPressure(data.memorySystems) * 100) << "%" << std::endl;
        std::cout << "System Health: " << (isSystemHealthy(data) ? "HEALTHY" : "DEGRADED") << std::endl;
    }
    
    void saveOutput() {
        if (outputFile_.empty()) return;
        
        std::cout << "Saving visualization output to: " << outputFile_ << std::endl;
        
        // For now, we just create a simple text file
        // In a full implementation, this would generate the actual visualization files
        std::ofstream file(outputFile_);
        if (file.is_open()) {
            file << "=== NLM Visualization Output ===" << std::endl;
            file << "Mode: " << mode_ << std::endl;
            file << "Format: " << outputFormat_ << std::endl;
            file << "Quality: " << quality_ << std::endl;
            file << std::endl;
            file << "Note: This is a placeholder implementation. "
                 << "Full visualization would generate "
                 << (outputFormat_ == "ascii" ? "ASCII art" :
                     outputFormat_ == "html" ? "HTML visualization" :
                     outputFormat_ == "json" ? "JSON data" : "text output")
                 << " based on brain state." << std::endl;
            
            file.close();
            std::cout << "Output saved successfully." << std::endl;
        } else {
            std::cerr << "Error: Failed to save output to: " << outputFile_ << std::endl;
        }
    }
    
    // Signal handling for real-time mode
    static void signalHandler(int signum) {
        running_ = false;
    }
    
    bool running_;
    size_t currentStep_;
    size_t steps_;
    double duration_;
    std::string mode_;
    std::string outputFormat_;
    std::string outputFile_;
    int quality_;
    bool interactive_;
    bool statsOnly_;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
};

int main(int argc, char** argv) {
    NLMVisualizer visualizer;
    
    std::string mode = "static";
    std::string outputFormat = "text";
    std::string outputFile;
    int quality = 1;
    size_t steps = 1000;
    double duration = 30.0;
    bool interactive = false;
    bool statsOnly = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printVisualizationHelp();
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--help-advanced") {
            printVisualizationHelp();
            return 0;
        } else if (arg.substr(0, 6) == "--mode") {
            if (arg.find('=') != std::string::npos) {
                mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                mode = argv[++i];
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                steps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                steps = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 9) == "--duration") {
            if (arg.find('=') != std::string::npos) {
                duration = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                duration = std::stod(argv[++i]);
            }
        } else if (arg.substr(0, 5) == "--file") {
            if (arg.find('=') != std::string::npos) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else if (arg.substr(0, 6) == "--save") {
            if (arg.find('=') != std::string::npos) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else if (arg.substr(0, 7) == "--format") {
            if (arg.find('=') != std::string::npos) {
                outputFormat = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFormat = argv[++i];
            }
        } else if (arg.substr(0, 9) == "--quality") {
            if (arg.find('=') != std::string::npos) {
                quality = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                quality = std::stoi(argv[++i]);
            }
        } else if (arg == "--interactive") {
            interactive = true;
        } else if (arg == "--stats") {
            statsOnly = true;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printVisualizationHelp();
            return 1;
        }
    }
    
    // Validate mode
    std::vector<std::string> validModes = {"static", "real-time", "replay", "analysis"};
    if (std::find(validModes.begin(), validModes.end(), mode) == validModes.end()) {
        std::cerr << "Error: Invalid mode: " << mode << std::endl;
        printVisualizationHelp();
        return 1;
    }
    
    // Validate quality
    if (quality < 1 || quality > 3) {
        std::cerr << "Error: Quality must be 1, 2, or 3" << std::endl;
        return 1;
    }
    
    try {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warn);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Set defaults for visualization
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return 1;
        }
        
        // Set up visualizer
        visualizer.setBrain(brain);
        visualizer.setConfig(config);
        visualizer.setMode(mode);
        visualizer.setSteps(steps);
        visualizer.setDuration(duration);
        visualizer.setOutputFile(outputFile);
        visualizer.setOutputFormat(outputFormat);
        visualizer.setQuality(quality);
        visualizer.setInteractive(interactive);
        visualizer.setStatsOnly(statsOnly);
        
        // Run visualization
        visualizer.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
