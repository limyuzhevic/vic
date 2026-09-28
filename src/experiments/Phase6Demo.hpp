#pragma once

#include "Phase6IntegratedExperiment.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>

namespace nlm {

// Phase 6: Final Integration Demo
// Demonstrates the complete integrated brain system
// Shows all Phase 6 integration achievements working together

class Phase6Demo {
public:
    Phase6Demo() {}
    ~Phase6Demo() {}
    
    // Main demonstration with multiple integration test scenarios
    void run() {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 6: FINAL INTEGRATION DEMO\n";
        std::cout << "===========================================\n\n";
        
        std::cout << "This demonstration tests the complete integration of all brain systems:\n";
        std::cout << "✓ Memory Systems (Working, Episodic, Associative)\n";
        std::cout << "✓ Neuromodulation (Dopamine, Curiosity, Novelty)\n";
        std::cout << "✓ Prediction Systems (Temporal and Action-Consequence)\n";
        std::cout << "✓ Cognition Systems (Planning, Concepts, Attention)\n";
        std::cout << "✓ Development System (Maturation and Plasticity)\n";
        std::cout << "✓ Replay and Consolidation Systems\n";
        std::cout << "✓ All systems connected in unified brain loop\n\n";
        
        // Run comprehensive integration tests
        runComprehensiveIntegrationTest();
        
        std::cout << "===========================================\n";
        std::cout << "PHASE 6 INTEGRATION COMPLETE\n";
        std::cout << "===========================================\n";
    }
    
    // Run a focused integration test with metrics collection
    void runIntegrationTest(const std::string& testName, 
                          int numEpisodes = 10, 
                          int stepsPerEpisode = 100) {
        std::cout << "\n--- " << testName << " ---\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureForPhase6(config);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!\n";
            return;
        }
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Enable all integration features
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        // Metrics tracking
        IntegrationMetrics metrics;
        metrics.testName = testName;
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            // Reset all systems for new episode
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            float episodeReward = 0.0f;
            
            // Run steps with full integration
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // Step 1: Update world
                world.update(0.01);
                
                // Step 2: Get sensory input
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // Step 3: Process sensory input
                agentBrain.processSensoryInput(percept);
                
                // Step 4: Run brain simulation
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(neuralStep);
                }
                
                // Step 5: Decode action
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // Step 6: Apply action to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // Step 7: Apply reward modulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // Step 8: Update development
                agentBrain.updateDevelopment(0.01);
                
                // Track metrics
                episodeReward += result.reward;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();
                
                // Collect system states
                metrics.neuromodulationLevels.push_back(agentBrain.getNeuromodulationLevel());
                metrics.curiosityLevels.push_back(agentBrain.getCuriosityLevel());
                metrics.noveltyLevels.push_back(agentBrain.getNoveltyLevel());
                metrics.predictionErrors.push_back(agentBrain.getPredictionError());
                
                // Track action distributions
                switch (cmd) {
                    case MotorCommand::MoveForward: metrics.actionCounts["MoveForward"]++; break;
                    case MotorCommand::MoveBackward: metrics.actionCounts["MoveBackward"]++; break;
                    case MotorCommand::TurnLeft: metrics.actionCounts["TurnLeft"]++; break;
                    case MotorCommand::TurnRight: metrics.actionCounts["TurnRight"]++; break;
                    case MotorCommand::LookLeft: metrics.actionCounts["LookLeft"]++; break;
                    case MotorCommand::LookRight: metrics.actionCounts["LookRight"]++; break;
                    case MotorCommand::Interact: metrics.actionCounts["Interact"]++; break;
                    default: metrics.actionCounts["Wait"]++; break;
                }
                
                // Periodically checkpoint brain state
                if (step % 50 == 0 && step > 0) {
                    brain->save("./checkpoints/phase6_checkpoint_" + std::to_string(step) + ".nlm");
                }
            }
            
            // Record episode metrics
            metrics.episodeRewards.push_back(episodeReward);
            metrics.finalEnergy.push_back(world.getAgentBody().energy);
            metrics.finalNovelty.push_back(agentBrain.getNoveltyLevel());
            metrics.finalCuriosity.push_back(agentBrain.getCuriosityLevel());
            metrics.finalPredictionError.push_back(agentBrain.getPredictionError());
            
            // Print episode summary
            if (episode % 5 == 0) {
                std::cout << "  Episode " << episode << ": reward=" << episodeReward 
                          << ", energy=" << world.getAgentBody().energy
                          << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                          << ", curiosity=" << agentBrain.getCuriosityLevel()
                          << ", novelty=" << agentBrain.getNoveltyLevel()
                          << "\n";
            }
        }
        
        // Calculate statistics
        calculateMetrics(metrics);
        
        // Print detailed results
        printMetrics(metrics);
    }
    
    // Run comprehensive integration showing all achievements
    void runComprehensiveIntegrationTest() {
        std::cout << "COMPREHENSIVE INTEGRATION TEST\n";
        std::cout << "Testing all Phase 6 integration features:\n\n";
        
        // Create integrated experiment
        Phase6IntegratedExperiment experiment;
        
        // Create configuration with all features enabled
        auto config = std::make_shared<Config>();
        configureForPhase6(config);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!\n";
            return;
        }
        
        // Run the integrated experiment
        std::cout << "Running Phase 6 integrated experiment...\n";
        Phase6Results results = experiment.run(brain.get(), 5, 20);
        
        // Display results
        printIntegratedResults(results);
    }
    
private:
    void configureForPhase6(std::shared_ptr<Config> config) {
        // Basic configuration
        config->set("neuron_count", size_t(1000));
        config->set("region_count", size_t(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", uint64_t(42));
        config->set("simulation_timestep", 0.001);
        
        // Plasticity parameters
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Memory system parameters
        config->set("working_memory_capacity", size_t(100));
        config->set("episodic_memory_max_episodes", size_t(1000));
        config->set("replay_interval", size_t(100));
        config->set("consolidation_interval", size_t(1000));
        
        // Prediction system parameters
        config->set("sequence_memory_size", size_t(10));
        config->set("prediction_horizon", size_t(5));
        
        // Development parameters
        config->set("development_speed", 1.0);
        config->set("critical_period_factor", 1.5f);
        
        // Neuromodulation parameters
        config->set("dopamine_sensitivity", 1.0f);
        config->set("curiosity_threshold", 0.3f);
        config->set("novelty_decay", 0.99f);
        
        // Sensory and motor parameters
        config->set("vision_width", 16);
        config->set("vision_height", 16);
        config->set("vision_channels", 3);
        config->set("max_energy", 100.0f);
        config->set("energy_decay_rate", 0.01f);
        
        // Integration parameters
        config->set("replay_interval", size_t(100));
        config->set("consolidation_interval", size_t(1000));
        config->set("checkpoint_interval", size_t(500));
        config->set("enable_sleep_cycles", true);
    }
    
    struct IntegrationMetrics {
        std::string testName;
        std::vector<float> episodeRewards;
        std::vector<float> finalEnergy;
        std::vector<float> neuromodulationLevels;
        std::vector<float> curiosityLevels;
        std::vector<float> noveltyLevels;
        std::vector<float> predictionErrors;
        std::map<std::string, int> actionCounts;
        float totalReward = 0.0f;
        size_t totalSpikes = 0;
    };
    
    void calculateMetrics(IntegrationMetrics& metrics) {
        // Calculate averages
        metrics.totalReward = 0.0f;
        
        float sumRewards = 0.0f;
        for (float reward : metrics.episodeRewards) {
            sumRewards += reward;
        }
        
        float avgReward = metrics.episodeRewards.empty() ? 0.0f : sumRewards / metrics.episodeRewards.size();
        
        // Calculate neuromodulation statistics
        float sumNeuromod = 0.0f;
        float maxNeuromod = -1000.0f;
        float minNeuromod = 1000.0f;
        
        for (float neuromod : metrics.neuromodulationLevels) {
            sumNeuromod += neuromod;
            maxNeuromod = std::max(maxNeuromod, neuromod);
            minNeuromod = std::min(minNeuromod, neuromod);
        }
        
        float avgNeuromod = metrics.neuromodulationLevels.empty() ? 0.0f : sumNeuromod / metrics.neuromodulationLevels.size();
        
        // Calculate curiosity statistics
        float sumCuriosity = 0.0f;
        float maxCuriosity = -1000.0f;
        
        for (float curiosity : metrics.curiosityLevels) {
            sumCuriosity += curiosity;
            maxCuriosity = std::max(maxCuriosity, curiosity);
        }
        
        float avgCuriosity = metrics.curiosityLevels.empty() ? 0.0f : sumCuriosity / metrics.curiosityLevels.size();
        
        // Calculate novelty statistics
        float sumNovelty = 0.0f;
        float maxNovelty = -1000.0f;
        
        for (float novelty : metrics.noveltyLevels) {
            sumNovelty += novelty;
            maxNovelty = std::max(maxNovelty, novelty);
        }
        
        float avgNovelty = metrics.noveltyLevels.empty() ? 0.0f : sumNovelty / metrics.noveltyLevels.size();
        
        // Calculate prediction error statistics
        float sumPredictionError = 0.0f;
        float maxPredictionError = -1000.0f;
        
        for (float error : metrics.predictionErrors) {
            sumPredictionError += error;
            maxPredictionError = std::max(maxPredictionError, error);
        }
        
        float avgPredictionError = metrics.predictionErrors.empty() ? 0.0f : sumPredictionError / metrics.predictionErrors.size();
        
        // Print summary statistics
        std::cout << "\n=== " << metrics.testName << " STATISTICS ===\n";
        std::cout << "Average reward per episode: " << std::fixed << std::setprecision(3) << avgReward << "\n";
        std::cout << "Average neuromodulation level: " << std::setprecision(3) << avgNeuromod 
                  << " (min: " << std::setprecision(3) << minNeuromod << ", max: " << std::setprecision(3) << maxNeuromod << ")\n";
        std::cout << "Average curiosity level: " << std::setprecision(3) << avgCuriosity 
                  << " (max: " << std::setprecision(3) << maxCuriosity << ")\n";
        std::cout << "Average novelty level: " << std::setprecision(3) << avgNovelty 
                  << " (max: " << std::setprecision(3) << maxNovelty << ")\n";
        std::cout << "Average prediction error: " << std::setprecision(3) << avgPredictionError 
                  << " (max: " << std::setprecision(3) << maxPredictionError << ")\n";
        
        // Print action distribution
        std::cout << "\nAction distribution:\n";
        for (const auto& pair : metrics.actionCounts) {
            std::cout << "  " << pair.first << ": " << pair.second << "\n";
        }
        
        // Calculate exploration vs exploitation
        int waitActions = metrics.actionCounts["Wait"];
        float explorationRatio = metrics.actionCounts.empty() ? 0.0f : 
                                 static_cast<float>(waitActions) / static_cast<float>(metrics.totalSpikes);
        
        std::cout << "\nBehavior analysis:\n";
        std::cout << "  Total steps: " << (metrics.totalSpikes > 0 ? metrics.totalSpikes : 0) << "\n";
        std::cout << "  Exploration ratio (wait actions): " << std::setprecision(3) << explorationRatio << "\n";
        
        if (explorationRatio > 0.3f) {
            std::cout << "  → HIGH EXPLORATION: Agent is trying many actions\n";
        } else if (explorationRatio > 0.1f) {
            std::cout << "  → MODERATE EXPLORATION: Balanced exploration/exploitation\n";
        } else {
            std::cout << "  → LOW EXPLORATION: Focused on successful actions\n";
        }
        
        // Check for development effects
        if (metrics.episodeRewards.size() >= 2) {
            float firstEpisode = metrics.episodeRewards.front();
            float lastEpisode = metrics.episodeRewards.back();
            float rewardChange = lastEpisode - firstEpisode;
            
            std::cout << "  Reward change (first to last episode): " << std::setprecision(3) << rewardChange << "\n";
            
            if (rewardChange > 5.0f) {
                std::cout << "  → SIGNIFICANT LEARNING: Reward improved greatly\n";
            } else if (rewardChange < -5.0f) {
                std::cout << "  → BEHAVIORAL DECLINE: Need intervention or more exploration\n";
            }
        }
    }
    
    void printMetrics(const IntegrationMetrics& metrics) {
        std::cout << "\n=== DETAILED METRICS FOR " << metrics.testName << " ===\n";
        
        // Print first 10 episodes as sample
        int episodesToShow = std::min(10, static_cast<int>(metrics.episodeRewards.size()));
        
        std::cout << "Episode rewards (first " << episodesToShow << " episodes):\n";
        for (int i = 0; i < episodesToShow; ++i) {
            std::cout << "  Episode " << i << ": " << std::fixed << std::setprecision(2) 
                      << metrics.episodeRewards[i] << " (energy: " 
                      << std::setprecision(2) << metrics.finalEnergy[i] << ")\n";
        }
        
        // Show system evolution over time
        if (!metrics.episodeRewards.empty()) {
            std::cout << "\nSystem evolution:\n";
            int samples = std::min(10, static_cast<int>(metrics.episodeRewards.size()));
            
            std::cout << "Episode | Reward | Energy | Neuromod | Curiosity | Novelty | PredError\n";
            std::cout << "--------|--------|--------|----------|-----------|---------|----------\n";
            
            for (int i = 0; i < samples; ++i) {
                int episode = (i * (metrics.episodeRewards.size() - 1)) / (samples - 1);
                std::cout << std::setw(7) << episode << " | "
                          << std::setw(7) << std::fixed << std::setprecision(2) << metrics.episodeRewards[episode] << " | "
                          << std::setw(7) << std::setprecision(2) << metrics.finalEnergy[episode] << " | "
                          << std::setw(10) << std::setprecision(3) << metrics.finalNovelty[episode] << " | "
                          << std::setw(10) << std::setprecision(3) << metrics.finalCuriosity[episode] << " | "
                          << std::setw(8) << std::setprecision(3) << metrics.finalPredictionError[episode] << " |\n";
            }
        }
    }
    
    void printIntegratedResults(const Phase6Results& results) {
        std::cout << "\n=== PHASE 6 INTEGRATION RESULTS ===\n";
        
        // Memory systems
        std::cout << "MEMORY SYSTEMS:\n";
        std::cout << "  Working memory capacity: " << results.workingMemoryCapacity << " units\n";
        std::cout << "  Working memory retention: " << std::setprecision(3) << results.workingMemoryRetention << "\n";
        std::cout << "  Episodic recall accuracy: " << std::setprecision(3) << results.episodicRecallAccuracy << "\n";
        std::cout << "  Episodic memory influence: " << std::setprecision(3) << results.episodicInfluence << "\n";
        
        // Prediction systems
        std::cout << "\nPREDICTION SYSTEMS:\n";
        std::cout << "  Initial prediction error: " << std::setprecision(3) << results.predictionErrorInitial << "\n";
        std::cout << "  Final prediction error: " << std::setprecision(3) << results.predictionErrorFinal << "\n";
        std::cout << "  Prediction accuracy improvement: " << std::setprecision(3) << results.predictionAccuracyImprovement << "\n";
        
        // Neuromodulation
        std::cout << "\nNEUROMODULATION:\n";
        std::cout << "  Neuromodulation stability: " << std::setprecision(3) << results.neuromodulationStability << "\n";
        std::cout << "  Curiosity optimization: " << std::setprecision(3) << results.curiosityOptimization << "\n";
        std::cout << "  Novelty detection rate: " << std::setprecision(3) << results.noveltyDetectionRate << "\n";
        
        // Cognition
        std::cout << "\nCOGNITION:\n";
        std::cout << "  Concepts formed: " << results.conceptsFormed << "\n";
        std::cout << "  Concept stability: " << std::setprecision(3) << results.conceptStability << "\n";
        std::cout << "  Planning accuracy: " << std::setprecision(3) << results.planningAccuracy << "\n";
        std::cout << "  Planning confidence: " << std::setprecision(3) << results.planningConfidence << "\n";
        
        // Integration
        std::cout << "\nINTEGRATION:\n";
        std::cout << "  Integration efficiency: " << std::setprecision(3) << results.integrationEfficiency << "\n";
        std::cout << "  System coherence: " << std::setprecision(3) << results.systemCoherence << "\n";
        std::cout << "  Emergent behavior score: " << std::setprecision(3) << results.emergentBehaviorScore << "\n";
        std::cout << "  Total reward accumulated: " << std::setprecision(3) << results.totalReward << "\n";
        
        // Success indicators
        std::cout << "\nSUCCESS INDICATORS:\n";
        
        bool memorySuccess = results.workingMemoryRetention > 0.5f && results.episodicRecallAccuracy > 0.6f;
        bool predictionSuccess = results.predictionAccuracyImprovement > 0.3f;
        bool neuromodSuccess = results.neuromodulationStability > 0.7f && results.curiosityOptimization > 0.5f;
        bool cognitionSuccess = results.conceptsFormed > 0 && results.planningAccuracy > 0.4f;
        bool integrationSuccess = results.integrationEfficiency > 0.6f && results.systemCoherence > 0.7f;
        
        std::cout << "  Memory Systems: " << (memorySuccess ? "✓ PASS" : "✗ FAIL") << "\n";
        std::cout << "  Prediction Systems: " << (predictionSuccess ? "✓ PASS" : "✗ FAIL") << "\n";
        std::cout << "  Neuromodulation: " << (neuromodSuccess ? "✓ PASS" : "✗ FAIL") << "\n";
        std::cout << "  Cognition: " << (cognitionSuccess ? "✓ PASS" : "✗ FAIL") << "\n";
        std::cout << "  Integration: " << (integrationSuccess ? "✓ PASS" : "✗ FAIL") << "\n";
        
        int successCount = (memorySuccess ? 1 : 0) + (predictionSuccess ? 1 : 0) +
                          (neuromodSuccess ? 1 : 0) + (cognitionSuccess ? 1 : 0) +
                          (integrationSuccess ? 1 : 0);
        
        std::cout << "\nOVERALL RESULT: " << successCount << "/5 systems successfully integrated\n";
        
        if (successCount >= 4) {
            std::cout << "🎉 EXCELLENT: All major brain systems are working together!\n";
        } else if (successCount >= 3) {
            std::cout << "✓ GOOD: Most brain systems are integrated successfully\n";
        } else {
            std::cout << "⚠️  NEEDS WORK: Several systems need better integration\n";
        }
    }
};

} // namespace nlm