#!/usr/bin/env g++
// NLM Analyze Tool
// Deep analysis of simulation results

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <unordered_map>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

using namespace nlm;

struct AnalysisResult {
    std::string analysisType;
    double analysisScore;
    std::string interpretation;
    std::map<std::string, double> metrics;
    std::vector<std::string> insights;
};

class NLMAnalyze {
private:
    std::vector<AnalysisResult> analysisResults;
    std::shared_ptr<Config> config;
    bool generateReport;
    bool saveAnalysis;
    std::string outputFile;
    bool showVisualizations;
    
public:
    NLMAnalyze() {
        config = std::make_shared<Config>();
        config->loadFromFile("configs/default.cfg");
        generateReport = false;
        saveAnalysis = false;
        outputFile = "analysis_report.txt";
        showVisualizations = false;
    }
    
    void analyzeBrain(std::shared_ptr<Brain> brain, const std::string& analysisType = "comprehensive") {
        std::cout << "=== Analyzing NLM Brain ===" << std::endl;
        std::cout << "Analysis Type: " << analysisType << std::endl;
        
        AnalysisResult result;
        result.analysisType = analysisType;
        
        if (analysisType == "comprehensive") {
            analyzeComprehensive(brain, result);
        } else if (analysisType == "neural") {
            analyzeNeuralDynamics(brain, result);
        } else if (analysisType == "plasticity") {
            analyzePlasticity(brain, result);
        } else if (analysisType == "memory") {
            analyzeMemory(brain, result);
        } else if (analysisType == "performance") {
            analyzePerformance(brain, result);
        } else if (analysisType == "learning") {
            analyzeLearning(brain, result);
        }
        
        analysisResults.push_back(result);
        
        if (generateReport) {
            generateAnalysisReport(result);
        }
        
        if (saveAnalysis) {
            saveAnalysisResult(result);
        }
    }
    
    void analyzeComprehensive(std::shared_ptr<Brain> brain, AnalysisResult& result) {
        std::cout << "Running comprehensive analysis..." << std::endl;
        
        // Neural analysis
        double neuralScore = analyzeNeuralDynamics(brain, result);
        result.analysisScore += neuralScore;
        
        // Plasticity analysis
        double plasticityScore = analyzePlasticity(brain, result);
        result.analysisScore += plasticityScore;
        
        // Memory analysis
        double memoryScore = analyzeMemory(brain, result);
        result.analysisScore += memoryScore;
        
        // Learning analysis
        double learningScore = analyzeLearning(brain, result);
        result.analysisScore += learningScore;
        
        // Performance analysis
        double performanceScore = analyzePerformance(brain, result);
        result.analysisScore += performanceScore;
        
        result.interpretation = generateInterpretation(result);
        
        std::cout << "Comprehensive analysis score: " << result.analysisScore << "/100" << std::endl;
    }
    
    void analyzeNeuralDynamics(std::shared_ptr<Brain> brain, AnalysisResult& result) {
        std::cout << "Analyzing neural dynamics..." << std::endl;
        
        double score = 0.0;
        
        // Spike analysis
        size_t totalSpikes = brain->getTotalSpikeCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        
        // Healthy spike range: 100-1000 spikes for typical simulation
        double spikeRatio = (totalSpikes / 1000.0);
        if (spikeRatio > 0.0 && spikeRatio <= 1.0) {
            score += spikeRatio * 30.0;
        }
        
        // Firing rate analysis
        double firingRatio = (firingNeurons / (double)brain->getTotalNeuronCount()) * 100.0;
        if (firingRatio > 0.0 && firingRatio <= 10.0) {
            score += firingRatio * 10.0;
        }
        
        // Connectivity analysis
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            size_t neuronCount = region->getAllNeurons().size();
            size_t synapseCount = region->getSynapses().size();
            
            if (neuronCount > 0) {
                double connectionDensity = (synapseCount / (double)neuronCount);
                if (connectionDensity >= 0.1 && connectionDensity <= 2.0) {
                    score += 40.0;
                }
            }
        }
        
        result.metrics["total_spikes"] = totalSpikes;
        result.metrics["firing_neurons"] = firingNeurons;
        result.metrics["firing_rate_percent"] = firingRatio;
        result.analysisScore += score;
    }
    
    void analyzePlasticity(std::shared_ptr<Brain> brain, AnalysisResult& result) {
        std::cout << "Analyzing plasticity..." << std::endl;
        
        double score = 0.0;
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto& synapses = region->getSynapses();
            if (!synapses.empty()) {
                // Count plastic synapses
                size_t plasticSynapses = 0;
                for (const auto& syn : synapses) {
                    if (syn->hasPlasticity()) {
                        plasticSynapses++;
                    }
                }
                
                double plasticityRatio = (plasticSynapses / (double)synapses.size()) * 100.0;
                score += plasticityRatio * 0.5;
                
                // Check weight changes
                double avgWeight = getAverageSynapticWeight(brain);
                if (avgWeight > 0.0 && avgWeight < 1.0) {
                    score += 30.0;
                }
            }
        }
        
        result.metrics["plastic_synapses"] = result.metrics.count("plastic_synapses") ? result.metrics.at("plastic_synapses") : 0.0;
        result.analysisScore += score;
    }
    
    void analyzeMemory(std::shared_ptr<Brain> brain, AnalysisResult& result) {
        std::cout << "Analyzing memory systems..." << std::endl;
        
        double score = 0.0;
        
        // Check working memory
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            score += 40.0;
            result.metrics["working_memory_active"] = 1.0;
        }
        
        // Check episodic memory
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            score += 30.0;
            result.metrics["episodic_memory_active"] = 1.0;
        }
        
        // Check semantic memory
        auto* semanticMemory = brain->getSemanticMemory();
        if (semanticMemory) {
            score += 20.0;
            result.metrics["semantic_memory_active"] = 1.0;
        }
        
        result.analysisScore += score;
    }
    
    void analyzeLearning(std::shared_ptr<Brain> brain, AnalysisResult& result) {
        std::cout << "Analyzing learning dynamics..." << std::endl;
        
        double score = 0.0;
        
        // Check for learning patterns
        size_t totalSpikes = brain->getTotalSpikeCount();
        
        if (totalSpikes > 50) {
            score += 40.0;
        }
        
        // Check spike variability (indicates learning)
        auto* spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            auto mostActive = spikeSystem->getMostActiveNeurons(5);
            if (!mostActive.empty()) {
                score += 30.0;
                result.metrics["most_active_neuron_count"] = mostActive.size();
            }
        }
        
        // Check memory changes
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            score += 30.0;
        }
        
        result.analysisScore += score;
    }
    
    void analyzePerformance(std::shared_ptr<Brain> brain, AnalysisResult& result) {
        std::cout << "Analyzing performance..." << std::endl;
        
        double score = 0.0;
        
        // Time efficiency (inferred from spike generation rate)
        double spikeEfficiency = (brain->getTotalSpikeCount() / 1000.0);
        if (spikeEfficiency >= 0.1 && spikeEfficiency <= 1.0) {
            score += spikeEfficiency * 20.0;
        }
        
        // Stability (check for extreme variations)
        auto* spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            auto spikeHistory = spikeSystem->getSpikeHistory(10);
            if (!spikeHistory.empty()) {
                double stdDev = calculateStandardDeviation(spikeHistory);
                if (stdDev < 50.0) {
                    score += 30.0;
                }
            }
        }
        
        result.metrics["spike_efficiency"] = spikeEfficiency;
        result.analysisScore += score;
    }
    
    std::string generateInterpretation(const AnalysisResult& result) {
        if (result.analysisScore >= 80.0) {
            return "Excellent performance with well-integrated neural systems.";
        } else if (result.analysisScore >= 60.0) {
            return "Good performance with room for improvement in some areas.";
        } else if (result.analysisScore >= 40.0) {
            return "Adequate performance but significant optimization needed.";
        } else {
            return "Performance requires substantial improvement across multiple systems.";
        }
    }
    
    double calculateStandardDeviation(const std::vector<size_t>& data) {
        if (data.empty()) return 0.0;
        
        double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        double variance = 0.0;
        
        for (size_t value : data) {
            variance += (value - mean) * (value - mean);
        }
        
        variance /= data.size();
        return std::sqrt(variance);
    }
    
    double getAverageSynapticWeight(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().empty()) {
            return 0.0;
        }
        
        double totalWeight = 0.0;
        for (const auto& syn : region->getSynapses()) {
            totalWeight += syn->getWeight();
        }
        
        return totalWeight / region->getSynapses().size();
    }
    
    void generateAnalysisReport(const AnalysisResult& result) {
        std::cout << "\n=== Analysis Report ===" << std::endl;
        std::cout << "Analysis Type: " << result.analysisType << std::endl;
        std::cout << "Score: " << result.analysisScore << "/100" << std::endl;
        std::cout << "Interpretation: " << result.interpretation << std::endl;
        std::cout << "Key Metrics:" << std::endl;
        
        for (const auto& metric : result.metrics) {
            std::cout << "  " << metric.first << ": " << metric.second << std::endl;
        }
        
        if (!result.insights.empty()) {
            std::cout << "Key Insights:" << std::endl;
            for (const auto& insight : result.insights) {
                std::cout << "  - " << insight << std::endl;
            }
        }
    }
    
    void saveAnalysisResult(const AnalysisResult& result) {
        std::ofstream file(outputFile, std::ios::app);
        if (file.is_open()) {
            file << "=== " << result.analysisType << " Analysis ===" << std::endl;
            file << "Score: " << result.analysisScore << "/100" << std::endl;
            file << "Interpretation: " << result.interpretation << std::endl;
            file << "Timestamp: " << time(0) << std::endl;
            file << std::endl;
            
            file << "Metrics:" << std::endl;
            for (const auto& metric : result.metrics) {
                file << "  " << metric.first << ": " << metric.second << std::endl;
            }
            
            file << std::endl;
            file.close();
            
            std::cout << "Analysis saved to: " << outputFile << std::endl;
        } else {
            std::cerr << "Error: Cannot save analysis to file: " << outputFile << std::endl;
        }
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --type <type>            Analysis type (comprehensive, neural, plasticity, memory, performance, learning)" << std::endl;
        std::cout << "  --generate-report        Generate analysis report" << std::endl;
        std::cout << "  --save <file>            Save analysis results to file" << std::endl;
        std::cout << "  --visualize              Show visualizations (if supported)" << std::endl;
        std::cout << "  --config <file>          Load configuration file" << std::endl;
        std::cout << std::endl;
        std::cout << "Analysis Types:" << std::endl;
        std::cout << "  comprehensive - Complete brain analysis" << std::endl;
        std::cout << "  neural        - Neural dynamics analysis" << std::endl;
        std::cout << "  plasticity    - Plasticity mechanisms analysis" << std::endl;
        std::cout << "  memory        - Memory systems analysis" << std::endl;
        std::cout << "  performance   - Performance optimization analysis" << std::endl;
        std::cout << "  learning      - Learning dynamics analysis" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-analyze";
    std::string analysisType = "comprehensive";
    bool generateReport = false;
    bool saveResults = false;
    std::string outputFile;
    bool visualizations = false;
    std::string configFile;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMAnalyze analyzer;
            analyzer.printUsage(programName);
            return 0;
        } else if (arg == "--type") {
            if (i + 1 < argc) {
                analysisType = argv[++i];
            } else {
                std::cerr << "Error: --type requires a type argument" << std::endl;
                return 1;
            }
        } else if (arg == "--generate-report") {
            generateReport = true;
        } else if (arg == "--save") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
                saveResults = true;
            } else {
                std::cerr << "Error: --save requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--visualize") {
            visualizations = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename argument" << std::endl;
                return 1;
            }
        }
    }
    
    NLMAnalyze analyzer;
    analyzer.generateReport = generateReport;
    analyzer.saveAnalysis = saveResults;
    analyzer.outputFile = outputFile;
    analyzer.showVisualizations = visualizations;
    
    if (!configFile.empty()) {
        if (!analyzer.config->loadFromFile(configFile)) {
            std::cerr << "Error: Failed to load configuration file: " << configFile << std::endl;
            return 1;
        }
    }
    
    // Create and analyze brain
    auto config = analyzer.config;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Starting analysis of NLM brain..." << std::endl;
    analyzer.analyzeBrain(brain, analysisType);
    
    std::cout << "\nAnalysis completed!" << std::endl;
    
    return 0;
}
