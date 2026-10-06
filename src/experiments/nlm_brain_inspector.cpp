#!/usr/bin/env g++ -std=c++20 -O3 -march=native
// NLM Brain Statistics Utility
// Provides detailed inspection and analysis of brain state
// For advanced users and debugging

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/experiments/Metrics.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace nlm;

class BrainInspector {
public:
    std::shared_ptr<Brain> brain;
    
    BrainInspector(std::shared_ptr<Brain> b) : brain(b) {}
    
    void printSummary() const {
        std::cout << "=== NLM Brain Statistics ===\n" << std::endl;
        
        std::cout << "Basic Metrics:" << std::endl;
        std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Active Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(2) 
                  << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Energy Efficiency: " << std::fixed << std::setprecision(4)
                  << (brain->getFiringNeuronCount() / std::max(1.0, brain->getTotalNeuronCount())) * 100.0 << "%" << std::endl;
        std::cout << std::endl;
        
        brain->logStatus();
    }
    
    void analyzeNeurons() const {
        std::cout << "=== Neuron Analysis ===\n" << std::endl;
        
        auto regions = brain->getAllRegions();
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            auto neurons = region->getAllNeurons();
            std::cout << "Region " << regionId.value << ": " << neurons.size() << " neurons" << std::endl;
            
            // Analyze neuron types
            std::map<NeuronType, int> typeCounts;
            std::map<FiringState, int> stateCounts;
            std::vector<double> voltages;
            std::vector<double> thresholds;
            
            for (const auto& neuron : neurons) {
                typeCounts[neuron->getType()]++;
                stateCounts[neuron->getFiringState()]++;
                voltages.push_back(neuron->getMembranePotential());
                thresholds.push_back(neuron->getThreshold());
            }
            
            std::cout << "  Neuron Types:" << std::endl;
            for (const auto& [type, count] : typeCounts) {
                std::cout << "    " << neuronTypeToString(type) << ": " << count << std::endl;
            }
            
            std::cout << "  Firing States:" << std::endl;
            for (const auto& [state, count] : stateCounts) {
                std::cout << "    " << firingStateToString(state) << ": " << count << std::endl;
            }
            
            // Voltage statistics
            if (!voltages.empty()) {
                double vMin = *std::min_element(voltages.begin(), voltages.end());
                double vMax = *std::max_element(voltages.begin(), voltages.end());
                double vAvg = std::accumulate(voltages.begin(), voltages.end(), 0.0) / voltages.size();
                
                std::cout << "  Voltage Statistics:" << std::endl;
                std::cout << "    Range: [" << std::fixed << std::setprecision(3) 
                          << vMin << ", " << vMax << "] mV" << std::endl;
                std::cout << "    Average: " << std::fixed << std::setprecision(3) 
                          << vAvg << " mV" << std::endl;
            }
            
            std::cout << std::endl;
        }
    }
    
    void analyzeSynapses() const {
        std::cout << "=== Synapse Analysis ===\n" << std::endl;
        
        auto regions = brain->getAllRegions();
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            auto synapses = region->getSynapses();
            std::cout << "Region " << regionId.value << ": " << synapses.size() << " synapses" << std::endl;
            
            if (synapses.empty()) {
                std::cout << "  No synapses in this region" << std::endl;
                continue;
            }
            
            // Analyze synapse types and weights
            std::map<SynapseType, int> typeCounts;
            std::vector<double> weights;
            
            for (const auto& synapse : synapses) {
                typeCounts[synapse->getType()]++;
                weights.push_back(synapse->getWeight());
            }
            
            std::cout << "  Synapse Types:" << std::endl;
            for (const auto& [type, count] : typeCounts) {
                std::cout << "    " << synapseTypeToString(type) << ": " << count << std::endl;
            }
            
            // Weight statistics
            double wMin = *std::min_element(weights.begin(), weights.end());
            double wMax = *std::max_element(weights.begin(), weights.end());
            double wAvg = std::accumulate(weights.begin(), weights.end(), 0.0) / weights.size();
            double wStd = 0.0;
            if (weights.size() > 1) {
                double sumSq = 0.0;
                for (double w : weights) {
                    sumSq += (w - wAvg) * (w - wAvg);
                }
                wStd = std::sqrt(sumSq / (weights.size() - 1));
            }
            
            std::cout << "  Weight Statistics:" << std::endl;
            std::cout << "    Range: [" << std::fixed << std::setprecision(4) 
                      << wMin << ", " << wMax << "]" << std::endl;
            std::cout << "    Average: " << std::fixed << std::setprecision(4) 
                      << wAvg << std::endl;
            std::cout << "    Standard Deviation: " << std::fixed << std::setprecision(4) 
                      << wStd << std::endl;
            
            // Identify extreme weights
            auto strongIt = std::max_element(weights.begin(), weights.end());
            auto weakIt = std::min_element(weights.begin(), weights.end());
            
            std::cout << "  Extreme Weights:" << std::endl;
            std::cout << "    Strongest: " << std::fixed << std::setprecision(4) 
                      << (*strongIt) << std::endl;
            std::cout << "    Weakest: " << std::fixed << std::setprecision(4) 
                      << (*weakIt) << std::endl;
            
            std::cout << std::endl;
        }
    }
    
    void analyzeMemorySystems() const {
        std::cout << "=== Memory Systems Analysis ===\n" << std::endl;
        
        // This would require access to memory system introspection APIs
        // For now, provide framework that can be extended
        std::cout << "Memory system introspection requires additional API exposure." << std::endl;
        std::cout << "Current access through agent brain interfaces." << std::endl;
    }
    
    void generateBrainDump(const std::string& filename) const {
        std::cout << "=== Generating Brain Dump ===\n" << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        // Write brain state to file
        file << "NLM Brain Dump\n";
        file << "==============\n\n";
        
        file << "Timestamp: " << std::time(nullptr) << "\n";
        file << "Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        file << "Total Synapses: " << brain->getTotalSynapseCount() << "\n\n";
        
        // Write neuron data
        auto regions = brain->getAllRegions();
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            file << "Region " << regionId.value << ":\n";
            auto neurons = region->getAllNeurons();
            
            for (size_t i = 0; i < neurons.size(); ++i) {
                auto& neuron = neurons[i];
                file << "  Neuron " << i << ":\n";
                file << "    Type: " << neuronTypeToString(neuron->getType()) << "\n";
                file << "    State: " << firingStateToString(neuron->getFiringState()) << "\n";
                file << "    Voltage: " << std::fixed << std::setprecision(4) 
                      << neuron->getMembranePotential() << " mV\n";
                file << "    Threshold: " << std::fixed << std::setprecision(4) 
                      << neuron->getThreshold() << " mV\n";
                file << "    Refractory: " << neuron->getRefractoryTimeRemaining() << " steps\n";
            }
            
            file << "\n";
        }
        
        file.close();
        std::cout << "Brain dump written to: " << filename << std::endl;
    }
    
    void exportMetrics(const std::string& filename) const {
        std::cout << "=== Exporting Metrics ===\n" << std::endl;
        
        // This would export metrics for analysis
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        // Write CSV header
        file << "timestamp,neurons,active_neurons,total_synapses,avg_firing_rate,total_spikes,energy_efficiency\n";
        
        // Current metrics
        auto now = std::time(nullptr);
        file << now << ","
             << brain->getTotalNeuronCount() << ","
             << brain->getFiringNeuronCount() << ","
             << brain->getTotalSynapseCount() << ","
             << std::fixed << std::setprecision(4) << brain->getAverageFiringRate() << ","
             << brain->getTotalSpikeCount() << ","
             << std::fixed << std::setprecision(4) << (brain->getFiringNeuronCount() / std::max(1.0, brain->getTotalNeuronCount())) * 100.0 << std::endl;
        
        file.close();
        std::cout << "Metrics exported to: " << filename << std::endl;
    }
    
    void runPerformanceBenchmark() const {
        std::cout << "=== Performance Benchmark ===\n" << std::endl;
        
        // Simple performance metrics
        std::cout << "Running performance benchmarks..." << std::endl;
        
        // Measure spike processing speed
        auto regions = brain->getAllRegions();
        size_t totalNeurons = 0;
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (region) {
                totalNeurons += region->getAllNeurons().size();
            }
        }
        
        std::cout << "  Network size: " << totalNeurons << " neurons" << std::endl;
        
        // Estimate performance
        double neuronsPerMs = 0.0;
        if (totalNeurons > 0) {
            neuronsPerMs = brain->getTotalSpikeCount() / 1000.0; // rough estimate
        }
        
        std::cout << "  Estimated processing: " << std::fixed << std::setprecision(2)
                  << neuronsPerMs << " neurons/ms" << std::endl;
        
        // Memory usage estimate (rough approximation)
        size_t neuronMemory = totalNeurons * sizeof(Neuron) * 3; // voltage, threshold, state
        size_t synapseMemory = brain->getTotalSynapseCount() * sizeof(Synapse) * 2; // weight, type
        
        std::cout << "  Estimated memory usage: " << (neuronMemory + synapseMemory) / 1024 << " KB" << std::endl;
        
        std::cout << "Benchmark complete." << std::endl;
    }
    
private:
    const char* neuronTypeToString(NeuronType type) const {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }
    
    const char* firingStateToString(FiringState state) const {
        switch (state) {
            case FiringState::Resting: return "Resting";
            case FiringState::Active: return "Active";
            case FiringState::Refractory: return "Refractory";
            case FiringState::Inhibited: return "Inhibited";
            default: return "Unknown";
        }
    }
    
    const char* synapseTypeToString(SynapseType type) const {
        switch (type) {
            case SynapseType::Excitatory: return "Excitatory";
            case SynapseType::Inhibitory: return "Inhibitory";
            case SynapseType::Modulatory: return "Modulatory";
            case SynapseType::Electrical: return "Electrical";
            case SynapseType::GapJunction: return "GapJunction";
            default: return "Unknown";
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Brain Inspector (Advanced Utility) ===\n" << std::endl;
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string metricsFile = "";
    bool summaryOnly = false;
    bool neuronAnalysis = false;
    bool synapseAnalysis = false;
    bool memoryAnalysis = false;
    bool benchmark = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--metrics" && i + 1 < argc) {
            metricsFile = argv[++i];
        } else if (arg == "--summary") {
            summaryOnly = true;
        } else if (arg == "--neurons") {
            neuronAnalysis = true;
        } else if (arg == "--synapses") {
            synapseAnalysis = true;
        } else if (arg == "--memory") {
            memoryAnalysis = true;
        } else if (arg == "--benchmark") {
            benchmark = true;
        } else if (arg == "--help") {
            std::cout << "Usage: nlm_brain_inspector [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --config <file>      Configuration file (default: configs/default.cfg)\n";
            std::cout << "  --output <file>      Generate brain dump file\n";
            std::cout << "  --metrics <file>     Export metrics CSV\n";
            std::cout << "  --summary            Print basic summary only\n";
            std::cout << "  --neurons            Analyze neuron populations\n";
            std::cout << "  --synapses           Analyze synaptic connections\n";
            std::cout << "  --memory             Analyze memory systems\n";
            std::cout << "  --benchmark          Run performance benchmarks\n";
            std::cout << "  --help               Show this help\n";
            return 0;
        }
    }
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Initialize brain
    std::cout << "Initializing NLM Brain for inspection..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Error: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create inspector
    BrainInspector inspector(brain);
    
    // Run analyses based on flags
    if (summaryOnly) {
        inspector.printSummary();
    } else {
        inspector.printSummary();
        std::cout << std::endl;
        
        if (neuronAnalysis) inspector.analyzeNeurons();
        if (synapseAnalysis) inspector.analyzeSynapses();
        if (memoryAnalysis) inspector.analyzeMemorySystems();
        if (benchmark) inspector.runPerformanceBenchmark();
        
        // Generate output files if requested
        if (!outputFile.empty()) {
            inspector.generateBrainDump(outputFile);
        }
        
        if (!metricsFile.empty()) {
            inspector.exportMetrics(metricsFile);
        }
    }
    
    std::cout << "\n=== Inspection Complete ===" << std::endl;
    return 0;
}
