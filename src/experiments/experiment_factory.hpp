#pragma once

#include "../agent/AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include "../experiments/Experiment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <chrono>

namespace nlm {

/**
 * Factory methods for creating experiment components
 */
namespace ExperimentFactory {
    /**
     * Create an experiment configuration
     */
    struct ExperimentConfig {
        std::string name;
        uint64_t seed{42};
        std::unordered_map<std::string, std::string> configValues;
        size_t neuronCount{500};
        size_t regionCount{2};
        float connectionProbability{0.1f};
        float simulationTimestep{0.001f};
        std::string outputFile;
        
        ExperimentConfig() = default;
    };
    
    /**
     * Create a brain with the given configuration
     */
    inline std::shared_ptr<Brain> createBrain(const ExperimentConfig& config) {
        auto cfg = std::make_shared<Config>();
        
        // Set basic configuration
        cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount));
        cfg->set("region_count", static_cast<int64_t>(config.regionCount));
        cfg->set("connection_probability", config.connectionProbability);
        cfg->set("random_seed", static_cast<int64_t>(config.seed));
        cfg->set("simulation_timestep", config.simulationTimestep);
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        cfg->set("synaptogenesis_rate", 0.0001f);
        cfg->set("pruning_rate", 0.00001f);
        
        // Set additional config values
        for (const auto& pair : config.configValues) {
            cfg->set(pair.first, pair.second);
        }
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }
    
    /**
     * Create a world with standard configuration
     */
    inline std::unique_ptr<SimpleWorld> createWorld() {
        auto world = std::make_unique<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->setRandomSeed(42);
        world->reset();
        return world;
    }
    
    /**
     * Create an agent brain interface
     */
    inline std::unique_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain, SimpleWorld& world) {
        auto agentBrain = std::make_unique<AgentBrain>(brain);
        agentBrain->initialize(world);
        
        // Enable common features
        agentBrain->enableRewardModulation(true);
        agentBrain->enableCuriosity(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        
        return agentBrain;
    }
    
    /**
     * Create an experiment runner
     */
    class ExperimentRunner {
    public:
        ExperimentRunner() = default;
        ~ExperimentRunner() = default;
        
        /**
         * Run an experiment with the given configuration
         */
        void runExperiment(const ExperimentConfig& config) {
            // Create components
            auto brain = createBrain(config);
            auto world = createWorld();
            auto agentBrain = createAgentBrain(brain, *world);
            
            // Run the experiment
            runSimulation(*agentBrain, *world, config);
            
            // Cleanup will happen automatically
        }
        
        /**
         * Run multiple trials with different seeds
         */
        void runMultipleTrials(size_t numTrials, const ExperimentConfig& baseConfig) {
            std::vector<ExperimentResult> results;
            
            for (size_t i = 0; i < numTrials; ++i) {
                ExperimentConfig trialConfig = baseConfig;
                trialConfig.seed = 42 + i;  // Different seed for each trial
                
                try {
                    auto result = runSingleTrial(trialConfig);
                    results.push_back(result);
                } catch (const std::exception& e) {
                    // Log error but continue with other trials
                    std::cerr << "Trial " << i << " failed: " << e.what() << std::endl;
                }
            }
            
            // Compute statistics
            computeStatistics(results);
            
            // Save results if configured
            if (!baseConfig.outputFile.empty()) {
                saveResults(results, baseConfig.outputFile);
            }
        }
        
    private:
        struct ExperimentResult {
            uint64_t seed;
            double duration;
            float totalReward;
            float avgFiringRate;
            size_t totalSynapses;
            std::string error;
            
            bool success() const { return error.empty(); }
        };
        
        /**
         * Run a single trial
         */
        ExperimentResult runSingleTrial(const ExperimentConfig& config) {
            ExperimentResult result;
            result.seed = config.seed;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            try {
                // Create components
                auto brain = createBrain(config);
                auto world = createWorld();
                auto agentBrain = createAgentBrain(brain, *world);
                
                // Run simulation
                runSimulation(*agentBrain, *world, config);
                
                // Calculate metrics
                result.totalReward = calculateTotalReward(*agentBrain, *world);
                result.avgFiringRate = brain->getAverageFiringRate();
                result.totalSynapses = brain->getTotalSynapseCount();
                
            } catch (const std::exception& e) {
                result.error = e.what();
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
            
            return result;
        }
        
        /**
         * Run the actual simulation
         */
        void runSimulation(AgentBrain& agentBrain, SimpleWorld& world, const ExperimentConfig& config) {
            uint64_t brainStepCounter = 0;
            float totalReward = 0.0f;
            
            for (size_t step = 0; step < config.stepsPerEpisode; ++step) {
                // Run simulation step
                ActionResult actionResult = runSimulationStep(agentBrain, world, brainStepCounter);
                
                // Update metrics
                totalReward += actionResult.reward;
                
                // Log progress if verbose
                if (config.verbose && step % 100 == 0) {
                    std::cout << "Step " << step << ": reward=" << actionResult.reward
                              << ", total=" << totalReward << std::endl;
                }
            }
        }
        
        /**
         * Calculate total reward from simulation
         */
        float calculateTotalReward(AgentBrain& agentBrain, SimpleWorld& world) {
            // This would typically accumulate reward from the agent's history
            // For now, return a placeholder
            return 0.0f;
        }
        
        /**
         * Compute statistics across trials
         */
        void computeStatistics(const std::vector<ExperimentResult>& results) {
            if (results.empty()) return;
            
            // Calculate mean and standard deviation for each metric
            double totalDuration = 0.0;
            double totalReward = 0.0;
            double totalFiringRate = 0.0;
            double totalSynapses = 0.0;
            
            size_t successCount = 0;
            
            for (const auto& result : results) {
                if (result.success()) {
                    totalDuration += result.duration;
                    totalReward += result.totalReward;
                    totalFiringRate += result.avgFiringRate;
                    totalSynapses += result.totalSynapses;
                    successCount++;
                }
            }
            
            if (successCount > 0) {
                std::cout << "Statistics across " << successCount << " successful trials:" << std::endl;
                std::cout << "  Average duration: " << (totalDuration / successCount) << "s" << std::endl;
                std::cout << "  Average reward: " << (totalReward / successCount) << std::endl;
                std::cout << "  Average firing rate: " << (totalFiringRate / successCount) << std::endl;
                std::cout << "  Average synapses: " << (totalSynapses / successCount) << std::endl;
            }
        }
        
        /**
         * Save results to file
         */
        void saveResults(const std::vector<ExperimentResult>& results, const std::string& filename) {
            // Implementation for saving results to file
            // Could be JSON, CSV, or other format
        }
    };
}

} // namespace nlm
