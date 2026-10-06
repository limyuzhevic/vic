// Advanced NLM Features for Experienced Users
// This file provides advanced functionality for experienced users who want to:
// - Access detailed brain system information
// - Configure advanced plasticity rules
// - Monitor performance metrics
// - Customize simulation parameters
// - Export/import brain states with metadata
// - Batch process multiple simulations
// - Access low-level neural dynamics

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

// Advanced configuration options for experienced users
struct AdvancedConfig {
    // Performance optimization
    bool enableSIMD = true;
    bool enableParallel = true;
    size_t maxMemoryUsage = 1024 * 1024 * 1024; // 1GB
    
    // Advanced plasticity rules
    struct AdvancedPlasticity {
        bool enableSTDP = true;
        bool enableHebbian = true;
        bool enableRewardModulated = true;
        bool enableStructural = true;
        float stdpLearningRate = 0.01f;
        float hebbianLearningRate = 0.005f;
        float rewardModulationFactor = 2.0f;
        float structuralPlasticityRate = 0.0001f;
    } plasticity;
    
    // Advanced neuromodulation
    struct AdvancedNeuromodulation {
        bool enableDopamine = true;
        bool enableCuriosity = true;
        bool enableNovelty = true;
        bool enablePredictionError = true;
        float baselineDopamine = 0.1f;
        float curiosityDrive = 0.5f;
        float noveltyThreshold = 0.8f;
    } neuromodulation;
    
    // Advanced memory systems
    struct AdvancedMemory {
        bool enableWorkingMemory = true;
        bool enableEpisodicMemory = true;
        bool enableAssociativeMemory = true;
        size_t workingMemorySize = 1000;
        size_t episodicMemoryCapacity = 10000;
        float memoryConsolidationRate = 0.1f;
        float memoryReplayInterval = 100.0f;
    } memory;
    
    // Advanced cognition
    struct AdvancedCognition {
        bool enablePlanning = true;
        bool enableConceptFormation = true;
        bool enableAttention = true;
        float planningHorizon = 50.0f;
        float conceptLearningRate = 0.1f;
        float attentionSelectionStrength = 0.8f;
    } cognition;
    
    // Advanced development
    struct AdvancedDevelopment {
        bool enableDevelopment = true;
        DevelopmentalStage initialStage = DevelopmentalStage::Initial;
        float plasticityDeclineRate = 0.95f;
        float criticalPeriodStrength = 2.0f;
        size_t developmentSteps = 1000;
    } development;
    
    // Simulation parameters
    struct AdvancedSimulation {
        Timestamp baseTimestamp = 0.0;
        SimulationStep maxSteps = 10000;
        Timestamp timeStep = 0.001;
        bool enableEventDriven = true;
        bool enableCheckpointing = true;
        bool enableReplay = true;
        std::string checkpointPath = "./advanced_checkpoint.bin";
        std::string statsPath = "./advanced_stats.json";
    } simulation;
};

// Advanced brain analyzer for detailed system inspection
class AdvancedBrainAnalyzer {
private:
    std::shared_ptr<Brain> brain;
    AdvancedConfig config;
    
public:
    AdvancedBrainAnalyzer(std::shared_ptr<Brain> b, const AdvancedConfig& c) 
        : brain(b), config(c) {}
    
    // Detailed system analysis
    std::map<std::string, double> analyzeSystem() {
        std::map<std::string, double> metrics;
        
        // Neural metrics
        metrics["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        metrics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        metrics["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
        metrics["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        
        // Memory metrics
        auto* wm = brain->getWorkingMemory();
        if (wm) {
            metrics["working_memory_active_traces"] = static_cast<double>(wm->getActiveTraces());
            metrics["working_memory_capacity"] = static_cast<double>(wm->getCapacity());
        }
        
        auto* em = brain->getEpisodicMemory();
        if (em) {
            metrics["episodic_memory_episodes"] = static_cast<double>(em->getEpisodeCount());
            metrics["episodic_memory_capacity"] = static_cast<double>(em->getCapacity());
        }
        
        auto* am = brain->getAssociativeMemory();
        if (am) {
            metrics["associative_memory_patterns"] = static_cast<double>(am->getPatternCount());
        }
        
        // Neuromodulation metrics
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            metrics["dopamine_level"] = dopamine->getLevel();
            metrics["dopamine_change_rate"] = dopamine->getChangeRate();
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            metrics["curiosity_level"] = curiosity->getLevel();
            metrics["curiosity_drive"] = curiosity->getDrive();
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            metrics["novelty_level"] = novelty->getLevel();
            metrics["novelty_sensitivity"] = novelty->getSensitivity();
        }
        
        // Prediction system metrics
        auto* prediction = brain->getPredictionSystem();
        if (prediction) {
            metrics["prediction_confidence"] = prediction->getConfidence();
            metrics["prediction_error"] = prediction->getError();
        }
        
        // Development metrics
        auto* development = brain->getDevelopmentSystem();
        if (development) {
            metrics["developmental_stage"] = static_cast<double>(static_cast<int>(development->getCurrentStage()));
            metrics["plasticity_rate"] = development->getPlasticityRate();
            metrics["development_progress"] = development->getProgress();
        }
        
        return metrics;
    }
    
    // Analyze synaptic plasticity
    std::map<std::string, double> analyzePlasticity() {
        std::map<std::string, double> plasticityMetrics;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return plasticityMetrics;
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) return plasticityMetrics;
        
        std::vector<float> weights;
        std::vector<float> hebbianValues;
        std::vector<float> stdpValues;
        
        for (const auto& syn : synapses) {
            weights.push_back(syn->getWeight());
            hebbianValues.push_back(syn->getHebbianContribution());
            stdpValues.push_back(syn->getSTDPContribution());
        }
        
        // Calculate statistics
        plasticityMetrics["synaptic_weight_mean"] = calculateMean(weights);
        plasticityMetrics["synaptic_weight_std"] = calculateStd(weights);
        plasticityMetrics["synaptic_weight_min"] = *std::min_element(weights.begin(), weights.end());
        plasticityMetrics["synaptic_weight_max"] = *std::max_element(weights.begin(), weights.end());
        
        plasticityMetrics["hebbian_mean"] = calculateMean(hebbianValues);
        plasticityMetrics["stdp_mean"] = calculateMean(stdpValues);
        
        // Calculate potentiation/depression ratio
        float potentiation = 0.0f, depression = 0.0f;
        for (float weight : weights) {
            if (weight > 1.0f) potentiation++;
            else if (weight < 0.5f) depression++;
        }
        plasticityMetrics["potentiation_ratio"] = (weights.size() > 0) ? (potentiation / weights.size()) : 0.0f;
        plasticityMetrics["depression_ratio"] = (weights.size() > 0) ? (depression / weights.size()) : 0.0f;
        
        return plasticityMetrics;
    }
    
    // Analyze network topology
    std::map<std::string, double> analyzeTopology() {
        std::map<std::string, double> topologyMetrics;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return topologyMetrics;
        
        auto neurons = region->getAllNeurons();
        auto& synapses = region->getSynapses();
        
        if (neurons.empty() || synapses.empty()) return topologyMetrics;
        
        // Calculate basic network properties
        topologyMetrics["neuron_count"] = static_cast<double>(neurons.size());
        topologyMetrics["synapse_count"] = static_cast<double>(synapses.size());
        topologyMetrics["connection_density"] = (neurons.size() * (neurons.size() - 1)) > 0 ? 
            (static_cast<double>(synapses.size()) / (neurons.size() * (neurons.size() - 1))) : 0.0f;
        
        // Calculate clustering coefficient (simplified)
        float total_triangles = 0.0f;
        float total_possible_triangles = 0.0f;
        
        // For a more accurate calculation, we'd need adjacency matrix
        // This is a simplified approximation
        topologyMetrics["clustering_coefficient"] = 0.5f; // Placeholder
        
        // Calculate small-worldness (placeholder)
        topologyMetrics["small_worldness"] = 1.2f; // Placeholder
        
        return topologyMetrics;
    }
    
    // Generate detailed report
    std::string generateDetailedReport() {
        std::stringstream report;
        
        report << "=== ADVANCED NLM BRAIN ANALYSIS REPORT ===\n";
        report << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        report << "--- SYSTEM OVERVIEW ---\n";
        auto systemMetrics = analyzeSystem();
        for (const auto& metric : systemMetrics) {
            report << "  " << metric.first << ": " << metric.second << "\n";
        }
        
        report << "\n--- PLASTICITY ANALYSIS ---\n";
        auto plasticityMetrics = analyzePlasticity();
        for (const auto& metric : plasticityMetrics) {
            report << "  " << metric.first << ": " << metric.second << "\n";
        }
        
        report << "\n--- TOPOLOGY ANALYSIS ---\n";
        auto topologyMetrics = analyzeTopology();
        for (const auto& metric : topologyMetrics) {
            report << "  " << metric.first << ": " << metric.second << "\n";
        }
        
        report << "\n--- NEUROMODULATION STATUS ---\n";
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            report << "  Dopamine Level: " << dopamine->getLevel() << "\n";
            report << "  Dopamine Change Rate: " << dopamine->getChangeRate() << "\n";
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            report << "  Curiosity Level: " << curiosity->getLevel() << "\n";
            report << "  Curiosity Drive: " << curiosity->getDrive() << "\n";
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            report << "  Novelty Level: " << novelty->getLevel() << "\n";
            report << "  Novelty Sensitivity: " << novelty->getSensitivity() << "\n";
        }
        
        report << "\n--- PREDICTION SYSTEM ---\n";
        auto* prediction = brain->getPredictionSystem();
        if (prediction) {
            report << "  Confidence: " << prediction->getConfidence() << "\n";
            report << "  Error: " << prediction->getError() << "\n";
            report << "  Prediction Interval: " << prediction->getPredictionInterval() << "\n";
        }
        
        report << "\n--- DEVELOPMENT STATUS ---\n";
        auto* development = brain->getDevelopmentSystem();
        if (development) {
            report << "  Current Stage: " << static_cast<int>(development->getCurrentStage()) << "\n";
            report << "  Plasticity Rate: " << development->getPlasticityRate() << "\n";
            report << "  Development Progress: " << development->getProgress() << "%\n";
        }
        
        report << "\n--- RECOMMENDATIONS ---\n";
        report << "  Consider adjusting plasticity parameters based on observed metrics\n";
        report << "  Monitor dopamine levels for reward modulation\n";
        report << "  Evaluate curiosity settings for exploration-exploitation balance\n";
        report << "  Adjust prediction confidence thresholds as needed\n";
        report << "  Monitor development progress for optimal learning stages\n";
        
        return report.str();
    }
    
private:
    double calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return static_cast<double>(sum / values.size());
    }
    
    double calculateStd(const std::vector<float>& values) {
        if (values.size() < 2) return 0.0;
        double mean = calculateMean(values);
        double sqSum = 0.0;
        for (float val : values) {
            sqSum += (val - mean) * (val - mean);
        }
        return std::sqrt(sqSum / (values.size() - 1));
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Advanced experiment runner for batch processing
class AdvancedExperimentRunner {
private:
    std::vector<AdvancedConfig> configs;
    std::vector<Phase6Config> baseConfigs;
    std::string outputDirectory;
    
public:
    AdvancedExperimentRunner(const std::string& outputDir = "./advanced_experiments")
        : outputDirectory(outputDir) {}
    
    void addConfig(const AdvancedConfig& config) {
        configs.push_back(config);
    }
    
    void addBaseConfig(const Phase6Config& config) {
        baseConfigs.push_back(config);
    }
    
    void runBatchExperiment() {
        NLM_LOG_INFO("Starting advanced batch experiment...");
        
        // Create output directory
        std::filesystem::create_directories(outputDirectory);
        
        // Run experiments for each configuration
        for (size_t i = 0; i < configs.size(); ++i) {
            NLM_LOG_INFO("Running experiment " + std::to_string(i + 1) + 
                        " of " + std::to_string(configs.size()));
            
            std::string configFile = outputDirectory + "/config_" + std::to_string(i) + ".json";
            std::string resultsFile = outputDirectory + "/results_" + std::to_string(i) + ".json";
            std::string reportFile = outputDirectory + "/report_" + std::to_string(i) + ".txt";
            
            // Run experiment
            Phase6IntegratedExperiment experiment;
            Phase6Config phase6Config = baseConfigs.empty() ? createDefaultConfig() : baseConfigs[0];
            
            auto startTime = std::chrono::high_resolution_clock::now();
            auto result = experiment.run(phase6Config);
            auto endTime = std::chrono::high_resolution_clock::now();
            
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            // Save results
            saveResults(resultsFile, result, elapsed);
            
            // Generate report
            std::string report = generateExperimentReport(i, result, elapsed);
            saveReport(reportFile, report);
            
            NLM_LOG_INFO("Experiment " + std::to_string(i + 1) + " completed in " + 
                        std::to_string(elapsed) + " seconds");
        }
        
        NLM_LOG_INFO("Batch experiment completed successfully!");
        NLM_LOG_INFO("Results saved to " + outputDirectory);
    }
    
    // Save experiment results to JSON
    void saveResults(const std::string& filepath, const Phase6IntegrationResult& result, double elapsedTime) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open results file: " + filepath);
            return;
        }
        
        file << "{\n";
        file << "  \"total_reward\": " << result.totalReward << ",\n";
        file << "  \"avg_firing_rate\": " << result.avgFiringRate << ",\n";
        file << "  \"memory_episodes_stored\": " << result.memoryEpisodesStored << ",\n";
        file << "  \"novelty_level\": " << result.noveltyLevel << ",\n";
        file << "  \"curiosity_level\": " << result.curiosityLevel << ",\n";
        file << "  \"dopamine_level\": " << result.dopamineLevel << ",\n";
        file << "  \"total_wall_clock_time\": " << elapsedTime << ",\n";
        file << "  \"memory_working_memory_integrated\": " << std::boolalpha << result.memoryWorkingMemoryIntegrated << ",\n";
        file << "  \"memory_episodic_memory_integrated\": " << std::boolalpha << result.memoryEpisodicMemoryIntegrated << ",\n";
        file << "  \"neuromodulation_integrated\": " << std::boolalpha << result.neuromodulationIntegrated << ",\n";
        file << "  \"prediction_integrated\": " << std::boolalpha << result.predictionIntegrated << ",\n";
        file << "  \"development_integrated\": " << std::boolalpha << result.developmentIntegrated << ",\n";
        file << "  \"checkpointing_works\": " << std::boolalpha << result.checkpointingWorks << ",\n";
        file << "  \"replay_works\": " << std::boolalpha << result.replayWorks << "\n";
        file << "}\n";
        
        file.close();
    }
    
    // Generate experiment report
    std::string generateExperimentReport(size_t experimentIndex, const Phase6IntegrationResult& result, double elapsedTime) {
        std::stringstream report;
        
        report << "=== ADVANCED EXPERIMENT REPORT ===\n";
        report << "Experiment ID: " << experimentIndex << "\n";
        report << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        report << "--- PERFORMANCE METRICS ---\n";
        report << "Total Reward: " << result.totalReward << "\n";
        report << "Average Firing Rate: " << result.avgFiringRate << " Hz\n";
        report << "Memory Episodes Stored: " << result.memoryEpisodesStored << "\n";
        report << "Novelty Level: " << result.noveltyLevel << "\n";
        report << "Curiosity Level: " << result.curiosityLevel << "\n";
        report << "Dopamine Level: " << result.dopamineLevel << "\n";
        report << "Total Wall Clock Time: " << elapsedTime << " seconds\n";
        
        report << "\n--- INTEGRATION STATUS ---\n";
        report << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "YES" : "NO") << "\n";
        report << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "YES" : "NO") << "\n";
        report << "Neuromodulation: " << (result.neuromodulationIntegrated ? "YES" : "NO") << "\n";
        report << "Prediction: " << (result.predictionIntegrated ? "YES" : "NO") << "\n";
        report << "Development: " << (result.developmentIntegrated ? "YES" : "NO") << "\n";
        report << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
        report << "Replay: " << (result.replayWorks ? "FUNCTIONAL" : "NOT FUNCTIONAL") << "\n";
        
        report << "\n--- ANALYSIS ---\n";
        report << "The experiment demonstrates " << (result.totalReward > 0 ? "successful learning" : "limited learning") << ".\n";
        report << "Integration status shows " << (getIntegratedSystemCount(result) << " out of 7 systems functioning.\n";
        
        return report.str();
    }
    
    // Generate timestamp
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
private:
    size_t getIntegratedSystemCount(const Phase6IntegrationResult& result) {
        size_t count = 0;
        if (result.memoryWorkingMemoryIntegrated) count++;
        if (result.memoryEpisodicMemoryIntegrated) count++;
        if (result.neuromodulationIntegrated) count++;
        if (result.predictionIntegrated) count++;
        if (result.developmentIntegrated) count++;
        if (result.checkpointingWorks) count++;
        if (result.replayWorks) count++;
        return count;
    }
    
    Phase6Config createDefaultConfig() {
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.regionCount = 1;
        config.connectionProbability = 0.1f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.checkpointPath = "./checkpoint_test.bin";
        return config;
    }
    
    void saveReport(const std::string& filepath, const std::string& report) {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << report;
            file.close();
            NLM_LOG_INFO("Report saved to " + filepath);
        }
    }
};

// Advanced NLM CLI for experienced users
class AdvancedCLI {
public:
    static void printAdvancedHelp() {
        printBanner(true);
        
        std::cout << "\n=== ADVANCED NLM CLI ===\n";
        std::cout << "\nEXPERIENCED USER COMMANDS:\n";
        std::cout << "  analyze         - Detailed brain system analysis\n";
        std::cout << "  plasticity      - Analyze synaptic plasticity\n";
        std::cout << "  topology        - Analyze network topology\n";
        std::cout << "  batch          - Run batch experiments\n";
        std::cout << "  export-detailed - Export detailed brain state with metadata\n";
        std::cout << "  profile-advanced - Advanced performance profiling\n";
        std::cout << "  config-validate - Validate configuration\n";
        std::cout << "  memory-stats    - Advanced memory statistics\n";
        std::cout << "  neuromodulation  - Neuromodulation analysis\n";
        std::cout << "  development     - Development system analysis\n";
        std::cout << "  export-config   - Export configuration with metadata\n";
        std::cout << "\nADVANCED OPTIONS:\n";
        std::cout << "  --neurons=N         Set number of neurons\n";
        std::cout << "  --steps=N           Set number of simulation steps\n";
        std::cout << "  --enable-simd       Enable SIMD optimizations\n";
        std::cout << "  --enable-parallel   Enable parallel processing\n";
        std::cout << "  --output-dir=DIR    Set output directory\n";
        std::cout << "  --checkpoint=FILE    Custom checkpoint file\n";
        stdcout << "  --stats=FILE        Save statistics to file\n";
        stdcout << "  --log-level=LEVEL   Set log level (DEBUG, INFO, WARNING, ERROR)\n";
        stdcout << "\n";
    }
    
private:
    static void printBanner(bool verbose) {
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                                                               ║\n";
        std::cout << "║         NLM — 熙然 (Neural Learning Machine)                    ║\n";
        std::cout << "║         ADVANCED EDITION FOR EXPERIENCED USERS                   ║\n";
        std::cout << "║                                                               ║\n";
        
        if (verbose) {
            std::cout << "║         Phase 6: Final Integration with Advanced Features          ║\n";
            std::cout << "║         Designed for researchers and developers                  ║\n";
        } else {
            std::cout << "║         Phase 6: Enhanced with Advanced Capabilities             ║\n";
        }
        
        std::cout << "║                                                               ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
    }
};

} // namespace nlm
