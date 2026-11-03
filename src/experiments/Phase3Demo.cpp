#include "DemoFramework.hpp"
#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <utility>

namespace nlm {

class Phase3Demo : public DemoBase {
public:
    Phase3Demo() : DemoBase() {}
    ~Phase3Demo() {}
    
    std::string getDemoName() const override {
        return "PHASE 3 - WORLD INTERACTION DEMO";
    }
    
    void onSetup() override {
        std::cout << "Setting up Phase 3 demo...\n";
    }
    
    void onRun(int numEpisodes, int stepsPerEpisode) override {
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
            
            // Reset state for new episode
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            // Phase 5 navigation/interact metrics
            std::set<std::pair<int, int>> visitedCells;
            int resourcesConsumed = 0, hazardsHit = 0, emptyInteracts = 0;
            int tooSoonCount = 0;
            int oscillations = 0;
            MotorCommand prevMove = MotorCommand::Wait;
            bool hasPrevMove = false;
            
            float episodeReward = 0.0f;
            uint64_t brainStepCounter = 0;
            
            // Run steps
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // 2. Inject sensory input into brain
                agentBrain.processSensoryInput(percept);
                
                // 3. Simulate brain (multiple steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    (void)neuralStep;
                    brain->step(brainStepCounter++);
                }
                
                // 4. Decode motor command from brain activity
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // 5. Apply motor command to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // 6. Apply reward modulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // 7. Update world
                world.update(0.1);
                
                // 8. Update development
                agentBrain.updateDevelopment(0.1);
                
                // Track metrics
                episodeReward += result.reward;
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();
                
                // Phase 5 metrics tracking
                const auto& body = world.getAgentBody();
                visitedCells.insert({static_cast<int>(std::floor(body.x)),
                                    static_cast<int>(std::floor(body.y))});
                
                if (result.message == "too soon") {
                    ++tooSoonCount;
                }
                
                if (cmd == MotorCommand::Interact) {
                    if (result.message == "consumed_resource") {
                        ++resourcesConsumed;
                    } else if (result.message == "hit_hazard") {
                        ++hazardsHit;
                    } else {
                        ++emptyInteracts;
                    }
                }
                
                if (cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward) {
                    if (hasPrevMove && prevMove != cmd) {
                        ++oscillations;
                    }
                    prevMove = cmd;
                    hasPrevMove = true;
                }
                
                // Count actions
                switch (cmd) {
                    case MotorCommand::MoveForward: metrics.forwardCount++; break;
                    case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                    case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                    case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                    case MotorCommand::Interact: metrics.interactCount++; break;
                    case MotorCommand::LookLeft: metrics.lookLeftCount++; break;
                    case MotorCommand::LookRight: metrics.lookRightCount++; break;
                    default: metrics.waitCount++; break;
                }
                
                // Print periodic updates
                if (step % 50 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << DemoHelpers::motorCommandToString(cmd)
                              << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                              << ", novelty=" << agentBrain.getNoveltyLevel()
                              << ", energy=" << body.energy
                              << "\n";
                }
            }
            
            // Record final metrics
            metrics.finalNovelty = agentBrain.getNoveltyLevel();
            metrics.finalCuriosity = agentBrain.getCuriosityLevel();
            metrics.finalEnergy = world.getAgentBody().energy;
            metrics.predictionError = agentBrain.getPredictionError();
            metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount();
            metrics.developmentalStage = DemoHelpers::stageToString(agentBrain.getDevelopmentalStage());
            metrics.totalSynapses = brain->getTotalSynapseCount();
            metrics.averageFiringRate = brain->getAverageFiringRate();
            
            // Add Phase 5 metrics
            metrics.visitedCells = visitedCells.size();
            metrics.resourcesConsumed = resourcesConsumed;
            metrics.hazardsHit = hazardsHit;
            metrics.emptyInteracts = emptyInteracts;
            metrics.oscillations = oscillations;
            metrics.tooSoonCount = tooSoonCount;
            metrics.reflexBonus = agentBrain.getReflexBonusCount();
            metrics.reflexSuppress = agentBrain.getReflexSuppressCount();
            metrics.wtaPulses = agentBrain.getWtaPulseCount();
            metrics.plannerOverride = agentBrain.getPlannerOverrideCount();
            metrics.plannerAgree = agentBrain.getPlannerAgreementCount();
            metrics.plannerDecision = agentBrain.getPlannerDecisionCount();
            metrics.conceptBias = agentBrain.getConceptBiasCount();
            metrics.selfBias = agentBrain.getSelfBiasCount();
            metrics.socialBias = agentBrain.getSocialBiasCount();
            metrics.conceptCount = agentBrain.getConceptCount();
            metrics.selfConf = agentBrain.getSelfModelConfidence();
            metrics.wmTraces = brain->getWorkingMemory() ? 
                              brain->getWorkingMemory()->getActiveTraces() : 0;
            
            allMetrics.push_back(metrics);
            
            std::cout << "--- Episode " << episode << " ended ---\n";
            std::cout << "  Steps: " << metrics.steps << "\n";
            std::cout << "  Total Reward: " << metrics.totalReward << "\n";
            std::cout << "  Final Energy: " << metrics.finalEnergy << "\n";
            std::cout << "  Synapses: " << metrics.totalSynapses << "\n";
        }
        
        // Summary
        printSummary(allMetrics);
    }
    
    void onComplete() override {
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
    }
    
private:
    void printSummary(const std::vector<ExperimentMetrics>& allMetrics) {
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
            float actionChange = static_cast<float>(last.forwardCount) - 
                                static_cast<float>(first.forwardCount);
            
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
    }
    
    // ExperimentMetrics struct (defined here for simplicity)
    struct ExperimentMetrics {
        int episode = 0;
        int steps = 0;
        float totalReward = 0.0f;
        float finalNovelty = 0.0f;
        float finalCuriosity = 0.0f;
        float finalEnergy = 0.0f;
        float predictionError = 0.0f;
        uint64_t developmentalAge = 0;
        std::string developmentalStage = "";
        uint64_t totalSynapses = 0;
        float averageFiringRate = 0.0f;
        
        // Phase 5 metrics
        size_t visitedCells = 0;
        int resourcesConsumed = 0;
        int hazardsHit = 0;
        int emptyInteracts = 0;
        int oscillations = 0;
        int tooSoonCount = 0;
        int reflexBonus = 0;
        int reflexSuppress = 0;
        int wtaPulses = 0;
        int plannerOverride = 0;
        int plannerAgree = 0;
        int plannerDecision = 0;
        int conceptBias = 0;
        int selfBias = 0;
        int socialBias = 0;
        int conceptCount = 0;
        float selfConf = 0.0f;
        int wmTraces = 0;
        
        // Action counts
        int forwardCount = 0;
        int backwardCount = 0;
        int turnLeftCount = 0;
        int turnRightCount = 0;
        int interactCount = 0;
        int lookLeftCount = 0;
        int lookRightCount = 0;
        int waitCount = 0;
    };
};

} // namespace nlm

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
