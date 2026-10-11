#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// Convenience functions for common neural network setup and testing

// Default brain configuration helper
class BrainConfigurator {
public:
    static std::shared_ptr<Config> createStandardConfig() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }
    
    static std::shared_ptr<Config> createHighPerformanceConfig() {
        auto config = createStandardConfig();
        config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.02f, ConfigSource::Default);
        return config;
    }
    
    static std::shared_ptr<Config> createSparseConfig() {
        auto config = createStandardConfig();
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.008f, ConfigSource::Default);
        return config;
    }
    
    static std::shared_ptr<Config> createPlasticityConfig() {
        auto config = createStandardConfig();
        config->set("stdp_ltp_weight", 0.05f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.03f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.002f, ConfigSource::Default);
        config->set("pruning_rate", 0.0005f, ConfigSource::Default);
        return config;
    }
};

// Quick test functions for power users
class QuickTestSuite {
public:
    // Test basic neural connectivity
    static bool testBasicConnectivity(std::shared_ptr<Brain> brain) {
        if (!brain || !brain->initialize()) {
            return false;
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return false;
        
        // Inject current
        for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run steps
        for (SimulationStep step = 0; step < 20; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Check for spikes
        return brain->getTotalSpikeCount() > 0;
    }
    
    // Test learning capability
    static bool testLearningCapability(std::shared_ptr<Brain> brain) {
        if (!brain || !brain->initialize()) {
            return false;
        }
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Record initial weights
        std::vector<float> initialWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        // Apply learning input
        for (SimulationStep step = 0; step < 100; ++step) {
            // Inject sensory input
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        // Check if weights changed
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                float newWeight = syn->getWeight();
                // Check if any weight changed significantly
                for (float initial : initialWeights) {
                    if (std::abs(newWeight - initial) > 0.01f) {
                        return true;
                    }
                }
            }
        }
        
        return false;
    }
    
    // Test STDP functionality
    static bool testStdp(std::shared_ptr<Brain> brain) {
        if (!brain || !brain->initialize()) {
            return false;
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 2) return false;
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 2) return false;
        
        // Record initial weights
        float initialWeight1 = synapses[0]->getWeight();
        float initialWeight2 = synapses[1]->getWeight();
        
        // Enable STDP only
        synapses[0]->enablePlasticity(false, true, false);
        synapses[1]->enablePlasticity(false, true, false);
        
        // Create correlated activity
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            // Fire pre then post
            neurons[0]->injectCurrent(60.0f);
            brain->step(0, 0.0);
            
            neurons[1]->injectCurrent(60.0f);
            brain->step(1, 0.001);
        }
        
        // Check weight changes
        float newWeight1 = synapses[0]->getWeight();
        float newWeight2 = synapses[1]->getWeight();
        
        return (std::abs(newWeight1 - initialWeight1) > 0.001f || 
                std::abs(newWeight2 - initialWeight2) > 0.001f);
    }
    
    // Test structural plasticity
    static bool testStructuralPlasticity(std::shared_ptr<Brain> brain) {
        if (!brain || !brain->initialize()) {
            return false;
        }
        
        size_t initialSynapses = brain->getTotalSynapseCount();
        
        // Enable structural plasticity
        if (auto* plasticity = brain->getStructuralPlasticity()) {
            plasticity->enable(true);
        }
        
        // Run for many steps to allow structural changes
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Inject activity to stimulate structural changes
            brain->injectCurrentToNeurons(NeuronType::Internal, 10.0f);
            brain->step(step, step * 0.001);
        }
        
        size_t finalSynapses = brain->getTotalSynapseCount();
        
        // Check if synapse count changed (indicating structural plasticity)
        return finalSynapses != initialSynapses;
    }
};

// Experiment setup convenience functions
class ExperimentSetup {
public:
    // Create standard experiment suite
    static std::vector<std::shared_ptr<Experiment>> createStandardSuite(ExperimentRunner& runner) {
        std::vector<std::shared_ptr<Experiment>> experiments;
        
        experiments.push_back(runner.createExperiment("Connectivity Test"));
        experiments.push_back(runner.createExperiment("Learning Test"));
        experiments.push_back(runner.createExperiment("Plasticity Test"));
        experiments.push_back(runner.createExperiment("STDP Test"));
        
        return experiments;
    }
    
    // Create advanced experiment suite
    static std::vector<std::shared_ptr<Experiment>> createAdvancedSuite(ExperimentRunner& runner) {
        std::vector<std::shared_ptr<Experiment>> experiments;
        
        experiments.push_back(runner.createExperiment("High Performance Learning"));
        experiments.push_back(runner.createExperiment("Sparse Network Exploration"));
        experiments.push_back(runner.createExperiment("Strong Plasticity"));
        experiments.push_back(runner.createExperiment("Continual Learning"));
        experiments.push_back(runner.createExperiment("Damage Recovery"));
        
        return experiments;
    }
    
    // Create quick test suite
    static std::vector<std::shared_ptr<Experiment>> createQuickSuite(ExperimentRunner& runner) {
        std::vector<std::shared_ptr<Experiment>> experiments;
        
        experiments.push_back(runner.createExperiment("Quick Connectivity"));
        experiments.push_back(runner.createExperiment("Quick Learning"));
        experiments.push_back(runner.createExperiment("Quick Plasticity"));
        
        return experiments;
    }
};

// Configuration builder for advanced users
class AdvancedConfigBuilder {
public:
    struct NeuralNetworkConfig {
        std::string name;
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
        float stdpLtpWeight;
        float stdpLtdWeight;
        float synaptogenesisRate;
        float pruningRate;
        bool enableMultithreading;
        size_t threadCount;
        
        NeuralNetworkConfig(const std::string& n = "Default")
            : name(n), neuronCount(500), regionCount(2), connectionProbability(0.15f),
              stdpLtpWeight(0.02f), stdpLtdWeight(0.015f), synaptogenesisRate(0.0001f),
              pruningRate(0.00001f), enableMultithreading(false), threadCount(0) {}
    };
    
    static std::shared_ptr<Config> buildConfig(const NeuralNetworkConfig& config) {
        auto result = std::make_shared<Config>();
        
        result->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        result->set("simulation_timestep", 0.001, ConfigSource::Default);
        result->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        result->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
        result->set("connection_probability", config.connectionProbability, ConfigSource::Default);
        result->set("stdp_ltp_weight", config.stdpLtpWeight, ConfigSource::Default);
        result->set("stdp_ltd_weight", config.stdpLtdWeight, ConfigSource::Default);
        result->set("stdp_tau", 20.0f, ConfigSource::Default);
        result->set("synaptogenesis_rate", config.synaptogenesisRate, ConfigSource::Default);
        result->set("pruning_rate", config.pruningRate, ConfigSource::Default);
        
        if (config.enableMultithreading) {
            result->set("thread_count", static_cast<int64_t>(config.threadCount), ConfigSource::Default);
        }
        
        return result;
    }
    
    // Predefined configurations
    static std::shared_ptr<Config> createResearchConfig() {
        NeuralNetworkConfig config("Research");
        config.neuronCount = 10000;
        config.regionCount = 4;
        config.connectionProbability = 0.2f;
        config.stdpLtpWeight = 0.03f;
        config.stdpLtdWeight = 0.02f;
        config.synaptogenesisRate = 0.001f;
        config.pruningRate = 0.0005f;
        config.enableMultithreading = true;
        config.threadCount = 4;
        return buildConfig(config);
    }
    
    static std::shared_ptr<Config> createProductionConfig() {
        NeuralNetworkConfig config("Production");
        config.neuronCount = 50000;
        config.regionCount = 8;
        config.connectionProbability = 0.25f;
        config.stdpLtpWeight = 0.01f;
        config.stdpLtdWeight = 0.008f;
        config.synaptogenesisRate = 0.0005f;
        config.pruningRate = 0.0001f;
        config.enableMultithreading = true;
        config.threadCount = 8;
        return buildConfig(config);
    }
    
    static std::shared_ptr<Config> createEmbeddedConfig() {
        NeuralNetworkConfig config("Embedded");
        config.neuronCount = 100;
        config.regionCount = 1;
        config.connectionProbability = 0.05f;
        config.stdpLtpWeight = 0.005f;
        config.stdpLtdWeight = 0.004f;
        config.synaptogenesisRate = 0.00005f;
        config.pruningRate = 0.00001f;
        config.enableMultithreading = false;
        return buildConfig(config);
    }
};

// Results analyzer for power users
class ResultsAnalyzer {
public:
    struct AnalysisResults {
        double averagePerformance;
        double performanceStdDev;
        double convergenceRate;
        double learningEfficiency;
        double stabilityScore;
        size_t totalExperiments;
        size_t successfulExperiments;
        
        AnalysisResults() : averagePerformance(0), performanceStdDev(0), 
                           convergenceRate(0), learningEfficiency(0),
                           stabilityScore(0), totalExperiments(0),
                           successfulExperiments(0) {}
    };
    
    static AnalysisResults analyzeExperimentResults(
        const std::vector<ExperimentResult>& results) {
        
        AnalysisResults analysis;
        analysis.totalExperiments = results.size();
        analysis.successfulExperiments = 0;
        
        std::vector<double> performances;
        
        for (const auto& result : results) {
            if (result.success) {
                analysis.successfulExperiments++;
                
                // Extract performance metrics
                if (result.metrics.find("total_reward") != result.metrics.end()) {
                    performances.push_back(result.metrics.at("total_reward"));
                }
                
                if (result.metrics.find("firing_rate") != result.metrics.end()) {
                    // Use firing rate as performance indicator
                    performances.push_back(result.metrics.at("firing_rate") * 10.0); // Scale for comparison
                }
                
                if (result.metrics.find("weight_change") != result.metrics.end()) {
                    // Learning efficiency based on weight change
                    double learningEff = result.metrics.at("weight_change") > 0 ? 
                        result.metrics.at("weight_change") : 0.0;
                    analysis.learningEfficiency += learningEff;
                }
                
                if (result.metrics.find("weight_stability_cv") != result.metrics.end()) {
                    // Stability score (inverse of coefficient of variation)
                    double stability = 1.0 / (1.0 + result.metrics.at("weight_stability_cv"));
                    analysis.stabilityScore += stability;
                }
            }
        }
        
        analysis.totalExperiments = results.size();
        
        if (!performances.empty()) {
            analysis.averagePerformance = std::accumulate(performances.begin(), performances.end(), 0.0) / performances.size();
            
            // Calculate standard deviation
            double sqSum = 0.0;
            for (double p : performances) {
                double diff = p - analysis.averagePerformance;
                sqSum += diff * diff;
            }
            analysis.performanceStdDev = std::sqrt(sqSum / performances.size());
            
            // Normalize convergence and stability
            if (analysis.successfulExperiments > 0) {
                analysis.convergenceRate = analysis.learningEfficiency / analysis.successfulExperiments;
                analysis.stabilityScore /= analysis.successfulExperiments;
            }
        }
        
        return analysis;
    }
    
    static std::string generateAnalysisReport(const AnalysisResults& analysis) {
        std::ostringstream report;
        report << "=== Experiment Analysis Report ===\n\n";
        report << "Overall Performance:\n";
        report << "  Average Performance: " << std::fixed << std::setprecision(4) << analysis.averagePerformance << "\n";
        report << "  Performance Std Dev: " << std::fixed << std::setprecision(4) << analysis.performanceStdDev << "\n";
        report << "  Learning Efficiency: " << std::fixed << std::setprecision(4) << analysis.learningEfficiency << "\n";
        report << "  Stability Score: " << std::fixed << std::setprecision(4) << analysis.stabilityScore << "\n\n";
        
        report << "Experiment Summary:\n";
        report << "  Total Experiments: " << analysis.totalExperiments << "\n";
        report << "  Successful Experiments: " << analysis.successfulExperiments << "\n";
        report << "  Success Rate: " << std::fixed << std::setprecision(2) 
               << (analysis.totalExperiments > 0 ? 
                     (static_cast<double>(analysis.successfulExperiments) / analysis.totalExperiments * 100.0) : 0.0) << "%\n\n";
        
        report << "Statistical Significance: ";
        if (analysis.totalExperiments >= 30) {
            report << "High\n";
        } else if (analysis.totalExperiments >= 10) {
            report << "Medium\n";
        } else {
            report << "Low\n";
        }
        
        return report.str();
    }
};

// Batch processing utilities
class BatchProcessor {
public:
    // Process experiments in parallel
    static std::vector<ExperimentResult> processBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        const AdvancedExperimentConfig& config = AdvancedExperimentConfig()) {
        
        std::vector<ExperimentResult> results;
        
        if (config.enableParallelExecution && config.threadCount > 0) {
            // Parallel processing (simplified implementation)
            results.reserve(experiments.size());
            
            auto environment = std::make_shared<Environment>(*environmentTemplate);
            
            for (auto& exp : experiments) {
                auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
                brain->initialize();
                
                ExperimentResult result = runExperiment(exp, brain, environment, config);
                results.push_back(result);
            }
        } else {
            // Sequential processing
            results.reserve(experiments.size());
            
            auto environment = std::make_shared<Environment>(*environmentTemplate);
            
            for (auto& exp : experiments) {
                auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
                brain->initialize();
                
                ExperimentResult result = runExperiment(exp, brain, environment, config);
                results.push_back(result);
            }
        }
        
        return results;
    }
    
    // Run experiment multiple times with different seeds
    static std::vector<ExperimentResult> processWithMultipleSeeds(
        std::shared_ptr<Experiment> experiment,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        size_t numSeeds,
        const AdvancedExperimentConfig& config = AdvancedExperimentConfig()) {
        
        std::vector<ExperimentResult> results;
        results.reserve(numSeeds);
        
        auto environment = std::make_shared<Environment>(*environmentTemplate);
        
        for (size_t seed = 0; seed < numSeeds; ++seed) {
            uint64_t actualSeed = config.seed + seed;
            
            auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
            brain->initialize();
            
            auto brainConfig = brain->getConfig();
            brainConfig->set("random_seed", static_cast<int64_t>(actualSeed), ConfigSource::Default);
            
            ExperimentResult result = runExperiment(experiment, brain, environment, config);
            results.push_back(result);
        }
        
        return results;
    }
    
private:
    static ExperimentResult runExperiment(
        std::shared_ptr<Experiment> experiment,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Environment> environment,
        const AdvancedExperimentConfig& config) {
        
        ExperimentRunner runner;
        return runner.runExperiment(experiment, brain, environment, config);
    }
};

// One-shot setup function for power users
class PowerUserSetup {
public:
    // Complete experimental setup for advanced users
    static std::pair<std::vector<ExperimentResult>, ResultsAnalyzer::AnalysisResults>
    runCompleteExperimentSuite(
        ExperimentRunner& runner,
        const std::string& experimentName,
        size_t numTrials = 5,
        const AdvancedExperimentConfig& config = AdvancedExperimentConfig()) {
        
        // Create experiment suite
        auto experiments = ExperimentSetup::createAdvancedSuite(runner);
        
        // Create brain template
        auto brainConfig = AdvancedConfigBuilder::createResearchConfig();
        auto brainTemplate = std::make_shared<Brain>(brainConfig);
        brainTemplate->initialize();
        
        // Create environment template
        auto environmentTemplate = std::make_shared<Environment>();
        
        // Run experiments
        std::vector<ExperimentResult> results;
        
        for (auto& exp : experiments) {
            // Run with multiple seeds for statistical significance
            auto seedResults = runner.runWithMultipleSeeds(
                exp, brainTemplate, environmentTemplate, numTrials, config);
            
            results.insert(results.end(), seedResults.begin(), seedResults.end());
        }
        
        // Analyze results
        ResultsAnalyzer::AnalysisResults analysis = 
            ResultsAnalyzer::analyzeExperimentResults(results);
        
        return std::make_pair(results, analysis);
    }
    
    // Quick experimental setup for rapid prototyping
    static std::vector<ExperimentResult> runQuickPrototype(
        ExperimentRunner& runner,
        const std::string& prototypeName,
        const AdvancedExperimentConfig& config = AdvancedExperimentConfig()) {
        
        // Create simple experiment suite
        auto experiments = ExperimentSetup::createQuickSuite(runner);
        
        // Create minimal brain template
        auto brainConfig = AdvancedConfigBuilder::createEmbeddedConfig();
        auto brainTemplate = std::make_shared<Brain>(brainConfig);
        brainTemplate->initialize();
        
        // Create environment
        auto environmentTemplate = std::make_shared<Environment>();
        
        // Run experiments
        return runner.runBatch(experiments, brainTemplate, environmentTemplate, config);
    }
};

} // namespace nlm