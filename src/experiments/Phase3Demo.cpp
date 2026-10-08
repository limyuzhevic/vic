#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo {
public:
    Phase3Demo() {}
    ~Phase3Demo() {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset world
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            float episodeReward = 0.0f;
            
            // Run steps
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // 2. Inject sensory input into brain
                agentBrain.processSensoryInput(percept);
                
                // 3. Simulate brain (multiple steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(neuralStep);
                }
                
                // 4. Decode motor command from brain activity
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // 5. Apply motor command to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // 6. Apply reward modulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // 7. Update world
                world.update(0.01);  // 10ms timestep
                
                // 8. Update development
                agentBrain.updateDevelopment(0.01);
                
                // Track metrics
                episodeReward += result.reward;
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();
                
                // Count actions
                switch (cmd) {
                    case MotorCommand::MoveForward: metrics.forwardCount++; break;
                    case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                    case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                    case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                    case MotorCommand::Interact: metrics.interactCount++; break;
                    default: metrics.waitCount++; break;
                }
                
                // Print periodic updates
                if (step % 50 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << motorCommandToString(cmd)
                              << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                              << ", novelty=" << agentBrain.getNoveltyLevel()
                              << ", energy=" << world.getAgentBody().energy
                              << "\n";
                }
            }
            
            // Record final metrics
            metrics.finalNovelty = agentBrain.getNoveltyLevel();
            metrics.finalCuriosity = agentBrain.getCuriosityLevel();
            metrics.finalEnergy = world.getAgentBody().energy;
            metrics.predictionError = agentBrain.getPredictionError();
            metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount();  // Use as proxy
            metrics.developmentalStage = stageToString(agentBrain.getDevelopmentalStage());
            metrics.totalSynapses = brain->getTotalSynapseCount();
            metrics.averageFiringRate = brain->getAverageFiringRate();
            
            allMetrics.push_back(metrics);
            
            std::cout << "--- Episode " << episode << " ended ---\n";
            std::cout << "  Steps: " << metrics.steps << "\n";
            std::cout << "  Total Reward: " << metrics.totalReward << "\n";
            std::cout << "  Final Energy: " << metrics.finalEnergy << "\n";
            std::cout << "  Synapses: " << metrics.totalSynapses << "\n";
            std::cout << "  Action counts: fwd=" << metrics.forwardCount 
                      << ", bwd=" << metrics.backwardCount
                      << ", left=" << metrics.turnLeftCount
                      << ", right=" << metrics.turnRightCount
                      << ", interact=" << metrics.interactCount
                      << ", wait=" << metrics.waitCount
                      << "\n\n";
        }
        
        // Summary
        std::cout << "===========================================\n";
        std::cout << "SUMMARY\n";
        std::cout << "===========================================\n";
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):\n";
            std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << "\n";
            std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << "\n";
            std::cout << "  Exploration (wait): " << first.waitCount << " -> " << last.waitCount << "\n";
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            std::cout << "\nBehavioral change:\n";
            if (rewardChange > 0) {
                std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)\n";
            } else if (rewardChange < -5) {
                std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)\n";
            }
            
            if (std::abs(actionChange) > 10) {
                std::cout << "  Forward actions changed by " << actionChange << "\n";
            }
            
            // Synapse change
            if (last.totalSynapses != first.totalSynapses) {
                std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << "\n";
            }
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
    }
    
private:
    void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    }
    
    void configureBrain(std::shared_ptr<Config> config) {
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42u);
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
    }
    
    void runAdvancedFeatures(int numEpisodes = 10, int stepsPerEpisode = 500) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 ADVANCED FEATURES DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration with advanced settings
        auto config = std::make_shared<Config>();
        configureAdvancedEnvironment(config);
        configureAdvancedBrain(config);
        
        // Create world with enhanced features
        SimpleWorld world;
        world.configure(30.0f, 30.0f, 24, 24);
        world.setRandomSeed(42);
        world.setMaxEnergy(200.0f);
        
        // Create brain with larger size for advanced learning
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure advanced agent brain features
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        agentBrain.enablePrediction(true);  // NEW: Enable prediction system
        agentBrain.enableAdvancedPlanning(true);  // NEW: Enable advanced planning
        
        std::cout << "Advanced setup complete:\n";
        std::cout << "  Brain neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Brain synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Development stage: " << stageToString(agentBrain.getDevelopmentalStage()) << "\n";
        std::cout << "  Prediction enabled: " << (agentBrain.isPredictionEnabled() ? "Yes" : "No") << "\n";
        std::cout << "  Planning enabled: " << (agentBrain.isPlanningEnabled() ? "Yes" : "No") << "\n\n";
        
        // Metrics tracking with advanced metrics
        std::vector<AdvancedExperimentMetrics> allMetrics;
        
        // Run episodes with advanced analysis
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Advanced Episode " << episode << " started ---\n";
            
            // Reset world and brain
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            AdvancedExperimentMetrics metrics;
            metrics.episode = episode;
            
            float episodeReward = 0.0f;
            float explorationBonus = 0.0f;
            
            // Run steps with advanced tracking
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // 2. Inject sensory input into brain with prediction
                agentBrain.processSensoryInput(percept);
                
                // 3. Simulate brain with multiple neural steps
                for (int neuralStep = 0; neuralStep < 15; ++neuralStep) {
                    brain->step(neuralStep);
                }
                
                // 4. Decode motor command with planning
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // 5. Apply motor command to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // 6. Apply enhanced reward modulation with novelty
                agentBrain.applyRewardModulation(result.reward, agentBrain.getPredictionError() * 0.1f);
                
                // 7. Update curiosity and exploration bonus
                float curiosity = agentBrain.getCuriosityLevel();
                explorationBonus += curiosity * 0.01f;
                
                // 8. Update development with age
                agentBrain.updateDevelopment(0.01);
                
                // 9. Get prediction and planning insights
                float predictionError = agentBrain.getPredictionError();
                float planningScore = agentBrain.getPlanningScore();
                
                // Track advanced metrics
                episodeReward += result.reward;
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.explorationBonus += explorationBonus;
                metrics.predictionErrorSum += predictionError;
                metrics.planningScoreSum += planningScore;
                metrics.totalSpikes += brain->getFiringNeuronCount();
                
                // Enhanced action counting
                switch (cmd) {
                    case MotorCommand::MoveForward: metrics.forwardCount++; break;
                    case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                    case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                    case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                    case MotorCommand::Interact: metrics.interactCount++; break;
                    case MotorCommand::Eat: metrics.eatCount++; break;
                    case MotorCommand::Drink: metrics.drinkCount++; break;
                    default: metrics.waitCount++; break;
                }
                
                // Print advanced periodic updates
                if (step % 100 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << motorCommandToString(cmd)
                              << ", novelty=" << agentBrain.getNoveltyLevel()
                              << ", curiosity=" << curiosity
                              << ", prediction_error=" << predictionError
                              << ", planning_score=" << planningScore
                              << ", energy=" << world.getAgentBody().energy
                              << ", curiosity_bonus=" << explorationBonus
                              << "\n";
                }
            }
            
            // Record final advanced metrics
            metrics.finalNovelty = agentBrain.getNoveltyLevel();
            metrics.finalCuriosity = agentBrain.getCuriosityLevel();
            metrics.finalEnergy = world.getAgentBody().energy;
            metrics.predictionError = agentBrain.getPredictionError();
            metrics.planningScore = agentBrain.getPlanningScore();
            metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount();
            metrics.developmentalStage = stageToString(agentBrain.getDevelopmentalStage());
            metrics.totalSynapses = brain->getTotalSynapseCount();
            metrics.averageFiringRate = brain->getAverageFiringRate();
            metrics.totalExplorationBonus = explorationBonus;
            
            // Calculate advanced averages
            metrics.averagePredictionError = metrics.predictionErrorSum / metrics.steps;
            metrics.averagePlanningScore = metrics.planningScoreSum / metrics.steps;
            
            allMetrics.push_back(metrics);
            
            std::cout << "--- Advanced Episode " << episode << " ended ---\n";
            std::cout << "  Steps: " << metrics.steps << "\n";
            std::cout << "  Total Reward: " << metrics.totalReward << "\n";
            std::cout << "  Exploration Bonus: " << metrics.totalExplorationBonus << "\n";
            std::cout << "  Final Energy: " << metrics.finalEnergy << "\n";
            std::cout << "  Synapses: " << metrics.totalSynapses << "\n";
            std::cout << "  Prediction Error: " << metrics.averagePredictionError << "\n";
            std::cout << "  Planning Score: " << metrics.averagePlanningScore << "\n";
            std::cout << "  Action counts: fwd=" << metrics.forwardCount 
                      << ", bwd=" << metrics.backwardCount
                      << ", left=" << metrics.turnLeftCount
                      << ", right=" << metrics.turnRightCount
                      << ", interact=" << metrics.interactCount
                      << ", eat=" << metrics.eatCount
                      << ", drink=" << metrics.drinkCount
                      << ", wait=" << metrics.waitCount
                      << "\n\n";
        }
        
        // Advanced summary with trend analysis
        std::cout << "===========================================\n";
        std::cout << "ADVANCED SUMMARY\n";
        std::cout << "===========================================\n";
        
        if (allMetrics.size() >= 3) {
            // Compare early, middle, and late episodes
            const auto& early = allMetrics.front();
            const auto& middle = allMetrics[allMetrics.size() / 2];
            const auto& late = allMetrics.back();
            
            std::cout << "Early vs Middle vs Late episode comparison:\n";
            std::cout << "  Reward: " << early.totalReward << " -> " << middle.totalReward << " -> " << late.totalReward << "\n";
            std::cout << "  Exploration: " << early.totalExplorationBonus << " -> " << middle.totalExplorationBonus << " -> " << late.totalExplorationBonus << "\n";
            std::cout << "  Prediction Error: " << early.averagePredictionError << " -> " << middle.averagePredictionError << " -> " << late.averagePredictionError << "\n";
            std::cout << "  Planning Score: " << early.averagePlanningScore << " -> " << middle.averagePlanningScore << " -> " << late.averagePlanningScore << "\n";
            
            // Calculate learning curves
            std::cout << "\nLearning Analysis:\n";
            float rewardTrend = (late.totalReward - early.totalReward) / early.totalReward * 100.0f;
            float explorationTrend = (late.totalExplorationBonus - early.totalExplorationBonus) / early.totalExplorationBonus * 100.0f;
            float predictionTrend = (late.averagePredictionError - early.averagePredictionError) / early.averagePredictionError * 100.0f;
            
            std::cout << "  Reward growth: " << (rewardTrend > 0 ? "+" : "") << rewardTrend << "%\n";
            std::cout << "  Exploration growth: " << (explorationTrend > 0 ? "+" : "") << explorationTrend << "%\n";
            std::cout << "  Prediction improvement: " << (predictionTrend < 0 ? "+" : "") << -predictionTrend << "%\n";
            
            // Determine learning stage
            std::cout << "\nLearning Stage Analysis:\n";
            if (late.developmentalStage == "Adult" || late.developmentalStage == "Aging") {
                std::cout << "  Stage: MATURE - System has reached stable adult behavior\n";
            } else if (late.developmentalStage == "Maturation") {
                std::cout << "  Stage: DEVELOPING - System is approaching maturity\n";
            } else {
                std::cout << "  Stage: DEVELOPING - System still in early development\n";
            }
        }
        
        std::cout << "\nAdvanced Phase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and learned using prediction and planning.\n";
    }
    
    private:
    void configureAdvancedEnvironment(std::shared_ptr<Config> config) {
        configureEnvironment(config); // Base configuration
        // Add advanced environment settings
        config->set("environment_width", 30);
        config->set("environment_height", 30);
        config->set("vision_width", 24);
        config->set("vision_height", 24);
        config->set("max_energy", 200.0f);
        config->set("energy_decay_rate", 0.001f);
    }
    
    void configureAdvancedBrain(std::shared_ptr<Config> config) {
        configureBrain(config); // Base configuration
        // Add advanced brain settings
        config->set("neuron_count", 1500);  // Larger brain for advanced learning
        config->set("region_count", 4);     // More regions for specialization
        config->set("connection_probability", 0.15f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 30.0f);
        config->set("synaptogenesis_rate", 0.0002f);
        config->set("pruning_rate", 0.00002f);
        config->set("development_critical_period_multiplier", 1.5f);
        config->set("development_plasticity_decay_rate", 0.01f);
    }

} // namespace nlm

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    
    if (argc > 1) {
        numEpisodes = std::atoi(argv[1]);
    }
    if (argc > 2) {
        stepsPerEpisode = std::atoi(argv[2]);
    }
    
    nlm::Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
    
    return 0;
}
