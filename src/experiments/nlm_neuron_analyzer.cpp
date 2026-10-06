#!/usr/bin/env g++ -std=c++20 -O3 -march=native
// NLM Neuron Analyzer Utility
// Specialized tool for detailed neuron analysis and simulation
// Useful for research and debugging

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/memory/WorkingMemory.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>

using namespace nlm;

class NeuronAnalyzer {
public:
    std::shared_ptr<Brain> brain;
    
    NeuronAnalyzer(std::shared_ptr<Brain> b) : brain(b) {}
    
    void analyzeVoltageDynamics() const {
        std::cout << "=== Voltage Dynamics Analysis ===\n" << std::endl;
        
        auto regions = brain->getAllRegions();
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            auto neurons = region->getAllNeurons();
            std::cout << "Region " << regionId.value << ": " << neurons.size() << " neurons" << std::endl;
            
            std::vector<double> voltages;
            for (const auto& neuron : neurons) {
                voltages.push_back(neuron->getMembranePotential());
            }
            
            if (voltages.empty()) {
                std::cout << "  No voltage data available" << std::endl;
                continue;
            }
            
            // Calculate statistics
            double minV = *std::min_element(voltages.begin(), voltages.end());
            double maxV = *std::max_element(voltages.begin(), voltages.end());
            double meanV = std::accumulate(voltages.begin(), voltages.end(), 0.0) / voltages.size();
            
            double variance = 0.0;
            for (double v : voltages) {
                variance += (v - meanV) * (v - meanV);
            }
            variance /= voltages.size();
            double stdDev = std::sqrt(variance);
            
            // Calculate threshold crossing statistics
            int aboveThreshold = 0;
            int belowThreshold = 0;
            double avgThreshold = 0.0;
            for (const auto& neuron : neurons) {
                double threshold = neuron->getThreshold();
                avgThreshold += threshold;
                if (neuron->getMembranePotential() >= threshold) {
                    aboveThreshold++;
                } else {
                    belowThreshold++;
                }
            }
            avgThreshold /= neurons.size();
            
            std::cout << "  Voltage Statistics:" << std::endl;
            std::cout << "    Min: " << std::fixed << std::setprecision(4) << minV << " mV" << std::endl;
            std::cout << "    Max: " << std::fixed << std::setprecision(4) << maxV << " mV" << std::endl;
            std::cout << "    Mean: " << std::fixed << std::setprecision(4) << meanV << " mV" << std::endl;
            std::cout << "    Std Dev: " << std::fixed << std::setprecision(4) << stdDev << " mV" << std::endl;
            std::cout << "    Threshold Crossing: " << aboveThreshold << " above, " 
                      << belowThreshold << " below" << std::endl;
            std::cout << "    Average Threshold: " << std::fixed << std::setprecision(4) 
                      << avgThreshold << " mV" << std::endl;
            
            // Identify neurons near threshold
            int nearThreshold = 0;
            for (const auto& neuron : neurons) {
                double v = neuron->getMembranePotential();
                double threshold = neuron->getThreshold();
                if (std::abs(v - threshold) < 2.0) {  // Within 2mV
                    nearThreshold++;
                }
            }
            std::cout << "    Near threshold (<2mV difference): " << nearThreshold << std::endl;
            
            std::cout << std::endl;
        }
    }
    
    void analyzeSynapticDynamics() const {
        std::cout << "=== Synaptic Dynamics Analysis ===\n" << std::endl;
        
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
            
            // Analyze synaptic weights and types
            std::map<SynapseType, std::vector<double>> typeWeights;
            std::vector<double> allWeights;
            
            for (const auto& synapse : synapses) {
                typeWeights[synapse->getType()].push_back(synapse->getWeight());
                allWeights.push_back(synapse->getWeight());
            }
            
            // Calculate statistics for each synapse type
            std::cout << "  Synapse Type Statistics:" << std::endl;
            for (const auto& [type, weights] : typeWeights) {
                if (weights.empty()) continue;
                
                double minW = *std::min_element(weights.begin(), weights.end());
                double maxW = *std::max_element(weights.begin(), weights.end());
                double meanW = std::accumulate(weights.begin(), weights.end(), 0.0) / weights.size();
                
                std::cout << "    " << synapseTypeToString(type) << ": " << weights.size() << " synapses" << std::endl;
                std::cout << "      Weight range: [" << std::fixed << std::setprecision(4) 
                          << minW << ", " << maxW << "]" << std::endl;
                std::cout << "      Mean weight: " << std::fixed << std::setprecision(4) 
                          << meanW << std::endl;
            }
            
            // Analyze weight distribution
            double wMin = *std::min_element(allWeights.begin(), allWeights.end());
            double wMax = *std::max_element(allWeights.begin(), allWeights.end());
            double wMean = std::accumulate(allWeights.begin(), allWeights.end(), 0.0) / allWeights.size();
            
            double wVariance = 0.0;
            for (double w : allWeights) {
                wVariance += (w - wMean) * (w - wMean);
            }
            wVariance /= allWeights.size();
            double wStd = std::sqrt(wVariance);
            
            // Calculate synaptic strength ratio
            double strongWeight = 0.0, weakWeight = 0.0;
            for (double w : allWeights) {
                if (w > 0.5) strongWeight += w;
                if (w < 0.2) weakWeight += w;
            }
            
            std::cout << "  Overall Statistics:" << std::endl;
            std::cout << "    Total synapses: " << allWeights.size() << std::endl;
            std::cout << "    Weight range: [" << std::fixed << std::setprecision(4) 
                      << wMin << ", " << wMax << "]" << std::endl;
            std::cout << "    Mean weight: " << std::fixed << std::setprecision(4) 
                      << wMean << std::endl;
            std::cout << "    Weight standard deviation: " << std::fixed << std::setprecision(4) 
                      << wStd << std::endl;
            std::cout << "    Strong synapses (>0.5): " << strongWeight << std::endl;
            std::cout << "    Weak synapses (<0.2): " << weakWeight << std::endl;
            
            std::cout << std::endl;
        }
    }
    
    void analyzeNeuromodulation() const {
        std::cout << "=== Neuromodulation Analysis ===\n" << std::endl;
        
        // This would require access to neuromodulator system state
        std::cout << "Neuromodulation analysis requires integration with neuromodulator APIs." << std::endl;
        std::cout << "Current access through agent brain interfaces or neuromodulator components." << std::endl;
        
        // For now, provide framework for extension
        std::cout << "Neuromodulation metrics available via:" << std::endl;
        std::cout << "  - AgentBrain interface: getDopamineLevel(), getCuriosityLevel(), etc." << std::endl;
        std::cout << "  - Neuromodulator component classes" << std::endl;
    }
    
    void analyzeMemoryUtilization() const {
        std::cout << "=== Memory Utilization Analysis ===\n" << std::endl;
        
        // Check for working memory
        auto workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "Working Memory:" << std::endl;
            std::cout << "  Capacity: " << workingMemory->getMaxItems() << " items" << std::endl;
            std::cout << "  Current items: " << workingMemory->getCurrentItemCount() << std::endl;
            std::cout << "  Competition level: " << workingMemory->getCompetitionStrength() << std::endl;
        }
        
        // Check for prediction system
        auto predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            std::cout << "Prediction System:" << std::endl;
            std::cout << "  Model complexity: " << predictionSystem->getModelComplexity() << std::endl;
            std::cout << "  Prediction error: " << predictionSystem->getPredictionError() << std::endl;
            std::cout << "  Confidence level: " << predictionSystem->getConfidenceLevel() << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    void generateAnalysisReport(const std::string& filename) const {
        std::cout << "=== Generating Analysis Report ===\n" << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "NLM Neuron Analysis Report\n";
        file << "==========================\n\n";
        
        file << "Timestamp: " << std::time(nullptr) << "\n";
        file << "Analysis Type: Comprehensive Neuron Analysis\n\n";
        
        // Write analysis results
        file << "--- Voltage Dynamics ---\n";
        auto regions = brain->getAllRegions();
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            auto neurons = region->getAllNeurons();
            file << "Region " << regionId.value << ": " << neurons.size() << " neurons\n";
            
            std::vector<double> voltages;
            for (const auto& neuron : neurons) {
                voltages.push_back(neuron->getMembranePotential());
            }
            
            if (!voltages.empty()) {
                double minV = *std::min_element(voltages.begin(), voltages.end());
                double maxV = *std::max_element(voltages.begin(), voltages.end());
                file << "  Voltage range: [" << std::fixed << std::setprecision(4) 
                     << minV << ", " << maxV << "] mV\n";
            }
        }
        
        file << "\n--- Synaptic Analysis ---\n";
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            auto synapses = region->getSynapses();
            if (synapses.empty()) continue;
            
            file << "Region " << regionId.value << ": " << synapses.size() << " synapses\n";
            
            std::vector<double> weights;
            for (const auto& synapse : synapses) {
                weights.push_back(synapse->getWeight());
            }
            
            double minW = *std::min_element(weights.begin(), weights.end());
            double maxW = *std::max_element(weights.begin(), weights.end());
            double meanW = std::accumulate(weights.begin(), weights.end(), 0.0) / weights.size();
            
            file << "  Weight stats: min=" << std::fixed << std::setprecision(4) << minW
                 << " max=" << std::fixed << std::setprecision(4) << maxW
                 << " mean=" << std::fixed << std::setprecision(4) << meanW << "\n";
        }
        
        file.close();
        std::cout << "Analysis report written to: " << filename << std::endl;
    }
    
private:
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
    std::cout << "=== NLM Neuron Analyzer (Specialized Tool) ===\n" << std::endl;
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    bool voltageAnalysis = false;
    bool synapticAnalysis = false;
    bool neuromodAnalysis = false;
    bool memoryAnalysis = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--voltage") {
            voltageAnalysis = true;
        } else if (arg == "--synapses") {
            synapticAnalysis = true;
        } else if (arg == "--neuromod") {
            neuromodAnalysis = true;
        } else if (arg == "--memory") {
            memoryAnalysis = true;
        } else if (arg == "--help") {
            std::cout << "Usage: nlm_neuron_analyzer [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --config <file>      Configuration file (default: configs/default.cfg)\n";
            std::cout << "  --output <file>      Generate analysis report file\n";
            std::cout << "  --voltage            Analyze voltage dynamics\n";
            std::cout << "  --synapses           Analyze synaptic connections\n";
            std::cout << "  --neuromod           Analyze neuromodulation\n";
            std::cout << "  --memory             Analyze memory systems\n";
            std::cout << "  --help               Show this help\n";
            return 0;
        }
    }
    
    // Set default analysis flags if none specified
    if (!voltageAnalysis && !synapticAnalysis && !neuromodAnalysis && !memoryAnalysis) {
        voltageAnalysis = true;
        synapticAnalysis = true;
        neuromodAnalysis = true;
        memoryAnalysis = true;
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
    std::cout << "Initializing NLM Brain for analysis..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Error: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create analyzer
    NeuronAnalyzer analyzer(brain);
    
    // Run analyses based on flags
    if (voltageAnalysis) analyzer.analyzeVoltageDynamics();
    if (synapticAnalysis) analyzer.analyzeSynapticDynamics();
    if (neuromodAnalysis) analyzer.analyzeNeuromodulation();
    if (memoryAnalysis) analyzer.analyzeMemoryUtilization();
    
    // Generate output file if requested
    if (!outputFile.empty()) {
        analyzer.generateAnalysisReport(outputFile);
    }
    
    std::cout << "\n=== Analysis Complete ===" << std::endl;
    return 0;
}
