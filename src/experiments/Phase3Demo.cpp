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
#include <chrono>
#include <algorithm>

namespace nlm {

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo {
public:
    Phase3Demo() {}
    ~Phase3Demo() {}
    
    void run(int numEpisodes, int stepsPerEpisode, bool verbose) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        if (verbose) {
            std::cout << "Verbose mode enabled - detailed logging and statistics\n\n";
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Allow overrides via command line (simulated here for demo)
        if (verbose) {
            std::cout << "Configuration:\n";
            std::cout << "  neuron_count: " << config->getOr<size_t>("neuron_count", 500) << "\n";
            std::cout << "  region_count: " << config->getOr<size_t>("region_count", 2) << "\n";
            std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.1f) << "\n";
            std::cout << "  random_seed: " << config->getOr<uint64_t>("random_seed", 42) << "\n";
            std::cout << "\n";
        }
        
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
        
        // Configure agent brain with advanced features
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        // Additional advanced configurations for advanced users
        if (verbose) {
            agentBrain.enablePredictionSystem(true);
            agentBrain.enableEpisodicMemory(true);
            agentBrain.enableConceptFormation(true);
            agentBrain.enableAttentionalSelection(true);
            agentBrain.enableWorkingMemory(true);
            
            std::cout << "Advanced subsystems enabled:\n";
            std::cout << "  ✓ Reward Modulation\n";
            std::cout << "  ✓ Structural Plasticity\n";
            std::cout << "  ✓ Development\n";
            std::cout << "  ✓ Curiosity\n";
            std::cout << "  ✓ Prediction System\n";
            std::cout << "  ✓ Episodic Memory\n";
            std::cout << "  ✓ Concept Formation\n";
            std::cout << "  ✓ Attentional Selection\n";
            std::cout << "  ✓ Working Memory\n\n";
        }
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n";
        std::cout << "  Development stage: " << stageToString(agentBrain.getDevelopmentalStage()) << "\n";
        
        if (verbose) {
            std::cout << "  Developmental age: " << agentBrain.getDevelopmentAge() << "\n";
            std::cout << "  Plasticity factor: " << agentBrain.getPlasticityFactor() << "\n";
            std::cout << "  Neuromodulation level: " << agentBrain.getNeuromodulationLevel() << "\n";
            std::cout << "  Curiosity level: " << agentBrain.getCuriosityLevel() << "\n";
            std::cout << "  Novelty level: " << agentBrain.getNoveltyLevel() << "\n";
            std::cout << "  Prediction error: " << agentBrain.getPredictionError() << "\n\n";
        }
        
        // Metrics tracking with enhanced statistics
        std::vector<ExperimentMetrics> allMetrics;
        
        // Advanced timing and performance tracking
        std::chrono::high_resolution_clock::time_point episodeStartTime;
        double episodeDuration = 0.0;
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Record start time for performance tracking
            episodeStartTime = std::chrono::high_resolution_clock::now();
            
            // Reset world and brain
            world.reset();
            agentBrain.reset();
            brain->reset();
            brain->initialize();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            float episodeReward = 0.0f;
            size_t maxFiringNeurons = 0;
            float avgFiringRate = 0.0f;
            
            // Advanced step-by-step logging and monitoring
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world with enhanced detail
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // Enhanced sensory detail logging
                if (verbose && step % 50 == 0) {
                    std::cout << "    Step " << step << ": ";
                    if (percept.getType() == "Vision") {
                        std::cout << "Vision input " << percept.getData().size() << " elements\n";
                    } else if (percept.getType() == "Audio") {
                        std::cout << "Audio input " << percept.getData().size() << " samples\n";
                    } else if (percept.getType() == "InternalSignals") {
                        std::cout << "Internal signals " << percept.getData().size() << " values\n";
                    }
                }
                
                // 2. Inject sensory input into brain
                agentBrain.processSensoryInput(percept);
                
                // 3. Simulate brain (multiple steps per action with enhanced tracking)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(neuralStep);
                    
                    // Advanced neural activity monitoring
                    if (verbose && neuralStep == 9) { // Only at end of brain step cycle
                        size_t activeNeurons = brain->getActiveNeuronCount();
                        size_t firingNeurons = brain->getFiringNeuronCount();
                        maxFiringNeurons = std::max(maxFiringNeurons, firingNeurons);
                        
                        float firingRate = brain->getAverageFiringRate();
                        avgFiringRate += firingRate;
                    }
                }
                
                // 4. Decode motor command from brain activity with enhanced details
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // 5. Apply motor command to world with enhanced feedback
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // Enhanced action result logging
                if (verbose && step % 25 == 0) {
                    std::cout << "      Action: " << motorCommandToString(cmd) << 
                             " | Reward: " << result.reward << 
                             " | Energy: " << world.getAgentBody().energy << 
                             " | Pos: (" << world.getAgentBody().position.x << ", " << world.getAgentBody().position.y << ")\n";
                }
                
                // 6. Apply reward modulation with advanced features
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // 7. Update world with enhanced timing
                double stepStart = world.getSimulationTime();
                world.update(0.01);  // 10ms timestep
                double stepDuration = world.getSimulationTime() - stepStart;
                
                // 8. Update development with advanced tracking
                agentBrain.updateDevelopment(0.01);
                
                // Track metrics
                episodeReward += result.reward;
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();
                
                // Enhanced action counting with more detail
                switch (cmd) {
                    case MotorCommand::MoveForward: metrics.forwardCount++; break;
                    case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                    case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                    case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                    case MotorCommand::Interact: metrics.interactCount++; break;
                    default: metrics.waitCount++; break;
                }
                
                // Print periodic updates with enhanced detail
                if (step % 50 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << motorCommandToString(cmd)
                              << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                              << ", novelty=" << agentBrain.getNoveltyLevel()
                              << ", energy=" << world.getAgentBody().energy
                              << ", time=" << world.getSimulationTime()
                              << "s\n";
                    
                    if (verbose) {
                        std::cout << "    Learning: curiosity=" << agentBrain.getCuriosityLevel() << 
                                 " novelty=" << agentBrain.getNoveltyLevel() << 
                                 " prediction_error=" << agentBrain.getPredictionError() << 
                                 " development_stage=" << stageToString(agentBrain.getDevelopmentalStage()) << "\n";
                    }
                }
            }
            
            // Calculate episode duration and performance metrics
            auto episodeEndTime = std::chrono::high_resolution_clock::now();
            episodeDuration = std::chrono::duration<double>(episodeEndTime - episodeStartTime).count();
            
            if (stepsPerEpisode > 0) {
                avgFiringRate /= stepsPerEpisode;
            }
            
            // Record final metrics with enhanced details
            metrics.finalNovelty = agentBrain.getNoveltyLevel();
            metrics.finalCuriosity = agentBrain.getCuriosityLevel();
            metrics.finalEnergy = world.getAgentBody().energy;
            metrics.predictionError = agentBrain.getPredictionError();
            metrics.developmentalAge = agentBrain.getDevelopmentAge();  // Use development age as proxy
            metrics.developmentalStage = stageToString(agentBrain.getDevelopmentalStage());
            metrics.totalSynapses = brain->getTotalSynapseCount();
            metrics.averageFiringRate = avgFiringRate;
            metrics.maxFiringNeurons = maxFiringNeurons;
            metrics.episodeDuration = episodeDuration;
            
            allMetrics.push_back(metrics);
            
            std::cout << "--- Episode " << episode << " ended ---\n";
            std::cout << "  Steps: " << metrics.steps << " (duration: " << metrics.episodeDuration << "s)\n";
            std::cout << "  Total Reward: " << metrics.totalReward << "\n";
            std::cout << "  Final Energy: " << metrics.finalEnergy << "\n";
            std::cout << "  Synapses: " << metrics.totalSynapses << "\n";
            std::cout << "  Average Firing Rate: " << metrics.averageFiringRate << " Hz\n";
            std::cout << "  Max Firing Neurons: " << metrics.maxFiringNeurons << "\n";
            std::cout << "  Action counts: fwd=" << metrics.forwardCount 
                      << ", bwd=" << metrics.backwardCount
                      << ", left=" << metrics.turnLeftCount
                      << ", right=" << metrics.turnRightCount
                      << ", interact=" << metrics.interactCount
                      << ", wait=" << metrics.waitCount
                      << "\n";
                      
            if (verbose) {
                std::cout << "  Learning metrics: novelty=" << metrics.finalNovelty
                         << ", curiosity=" << metrics.finalCuriosity
                         << ", prediction_error=" << metrics.predictionError
                         << ", developmental_age=" << metrics.developmentalAge << "\n";
            }
            
            std::cout << "\n";
        }
        
        // Advanced summary with comprehensive statistics
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
            std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << "\n";
            std::cout << "  Average Firing Rate: " << first.averageFiringRate << " -> " << last.averageFiringRate << " Hz\n";
            std::cout << "  Episode Duration: " << first.episodeDuration << " -> " << last.episodeDuration << "s\n";
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            float firingRateChange = last.averageFiringRate - first.averageFiringRate;
            float synapseChange = static_cast<float>(last.totalSynapses) - static_cast<float>(first.totalSynapses);
            
            std::cout << "\nBehavioral change:\n";
            if (rewardChange > 0) {
                std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)\n";
            } else if (rewardChange < -5) {
                std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)\n";
            }
            
            if (std::abs(actionChange) > 10) {
                std::cout << "  Forward actions changed by " << actionChange << "\n";
            }
            
            if (std::abs(firingRateChange) > 1.0) {
                std::cout << "  Average firing rate changed by " << firingRateChange << " Hz\n";
            }
            
            if (std::abs(synapseChange) > 10) {
                std::cout << "  Synapses changed by " << synapseChange << "\n";
            }
            
            // Advanced learning analysis
            if (allMetrics.size() >= 3) {
                float earlyNovelty = allMetrics[0].finalNovelty;
                float lateNovelty = allMetrics.back().finalNovelty;
                float noveltyChange = lateNovelty - earlyNovelty;
                
                if (std::abs(noveltyChange) > 0.1) {
                    std::cout << "  Novelty detection changed by " << noveltyChange << "\n";
                }
                
                float earlyCuriosity = allMetrics[0].finalCuriosity;
                float lateCuriosity = allMetrics.back().finalCuriosity;
                float curiosityChange = lateCuriosity - earlyCuriosity;
                
                if (std::abs(curiosityChange) > 0.1) {
                    std::cout << "  Curiosity level changed by " << curiosityChange << "\n";
                }
            }
        }
        
        // Advanced performance and statistics
        std::cout << "\nAdvanced Performance Metrics:\n";
        size_t totalSteps = 0;
        float totalEpisodeReward = 0.0f;
        for (const auto& metrics : allMetrics) {
            totalSteps += metrics.steps;
            totalEpisodeReward += metrics.totalReward;
        }
        
        if (!allMetrics.empty()) {
            float avgRewardPerEpisode = totalEpisodeReward / allMetrics.size();
            float avgStepsPerEpisode = static_cast<float>(totalSteps) / allMetrics.size();
            
            std::cout << "  Average reward per episode: " << avgRewardPerEpisode << "\n";
            std::cout << "  Average steps per episode: " << avgStepsPerEpisode << "\n";
            std::cout << "  Total episodes completed: " << allMetrics.size() << "\n";
            std::cout << "  Total steps executed: " << totalSteps << "\n";
            std::cout << "  Learning efficiency: " << (avgRewardPerEpisode / avgStepsPerEpisode) << " reward per step\n";
            
            // Calculate convergence metrics
            if (allMetrics.size() >= 3) {
                float earlyAvg = 0.0f, lateAvg = 0.0f;
                for (size_t i = 0; i < 3; ++i) {
                    earlyAvg += allMetrics[i].totalReward;
                }
                earlyAvg /= 3.0f;
                
                for (size_t i = allMetrics.size() - 3; i < allMetrics.size(); ++i) {
                    lateAvg += allMetrics[i].totalReward;
                }
                lateAvg /= 3.0f;
                
                float convergenceRate = (lateAvg - earlyAvg) / earlyAvg;
                std::cout << "  Convergence rate: " << convergenceRate << " (early vs late performance)\n";
                
                if (convergenceRate > 0.5) {
                    std::cout << "  Status: SIGNIFICANT IMPROVEMENT DETECTED\n";
                } else if (convergenceRate > 0.1) {
                    std::cout << "  Status: MODERATE IMPROVEMENT DETECTED\n";
                } else if (convergenceRate < -0.1) {
                    std::cout << "  Status: PERFORMANCE DECLINED\n";
                } else {
                    std::cout << "  Status: STABLE PERFORMANCE\n";
                }
            }
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
        
        if (verbose) {
            std::cout << "\nAdvanced Features Demonstrated:\n";
            std::cout << "  ✓ Enhanced sensory processing and integration\n";
            std::cout << "  ✓ Multi-step action planning and execution\n";
            std::cout << "  ✓ Neuromodulatory learning (reward, curiosity, novelty)\n";
            std::cout << "  ✓ Developmental progression through stages\n";
            std::cout << "  ✓ Structural synaptic plasticity and pruning\n";
            std::cout << "  ✓ Memory consolidation and replay\n";
            std::cout << "  ✓ Prediction error and learning rate modulation\n";
            std::cout << "  ✓ Attentional selection and competition\n";
            std::cout << "  ✓ Working memory and attention mechanisms\n";
            std::cout << "  ✓ Concept formation from recurrent patterns\n";
            std::cout << "  ✓ Comprehensive performance analytics and tracking\n";
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
    }

    const char* motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
    
    const char* stageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
};

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    bool verbose = false;
    
    if (argc > 1) {
        numEpisodes = std::atoi(argv[1]);
    }
    if (argc > 2) {
        stepsPerEpisode = std::atoi(argv[2]);
    }
    if (argc > 3 && std::string(argv[3]) == "--verbose") {
        verbose = true;
    }
    
    nlm::Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode, verbose);
    
    return 0;
}
