// Refactored Phase 3 Demo using BaseExperiment Framework
// Demonstrates elimination of code duplication

#include "BaseExperimentFramework.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class Phase3Demo : public BaseDemo {
public:
    Phase3Demo() : BaseDemo() {}
    virtual ~Phase3Demo() {}
    
    // Override the base class runExperiment method
    void runExperiment(int numEpisodes, int stepsPerEpisode) override {
        std::cout << "=== NLM Phase 3: World Interaction Demo ===\n\n";
        
        // Use metrics collection from base class
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset all systems
            world->reset();
            agentBrain->reset();
            brain->reset();
            
            // Create episode metrics using base class structure
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            float episodeReward = 0.0f;
            
            // Run steps
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world->getSensoryPercept();
                
                // 2. Inject sensory input into brain
                agentBrain->processSensoryInput(percept);
                
                // 3. Simulate brain (multiple steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(neuralStep);
                }
                
                // 4. Decode motor command from brain activity
                MotorCommand cmd = agentBrain->decodeMotorCommand();
                
                // 5. Apply motor command to world
                ActionResult result = world->applyMotorCommand(cmd, world->getSimulationTime());
                
                // 6. Apply reward modulation
                agentBrain->applyRewardModulation(result.reward, 0.0f);
                
                // 7. Update world
                world->update(0.01);  // 10ms timestep
                
                // 8. Update development
                agentBrain->updateDevelopment(0.01);
                
                // Track metrics using base class structure
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
                              << ", neuromod=" << agentBrain->getNeuromodulationLevel()
                              << ", novelty=" << agentBrain->getNoveltyLevel()
                              << ", energy=" << world->getAgentBody().energy
                              << "\n";
                }
            }
            
            // Record final metrics using base class structure
            metrics.finalNovelty = agentBrain->getNoveltyLevel();
            metrics.finalCuriosity = agentBrain->getCuriosityLevel();
            metrics.finalEnergy = world->getAgentBody().energy;
            metrics.predictionError = agentBrain->getPredictionError();
            metrics.developmentalAge = agentBrain->getBrain()->getTotalSynapseCount();  // Use as proxy
            metrics.developmentalStage = stageToString(agentBrain->getDevelopmentalStage());
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
        
        // Print summary using base class functionality
        printSummary();
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
    }
};

} // namespace nlm

// Main entry point for refactored Phase 3 demo
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
    demo.runDemo(numEpisodes, stepsPerEpisode);
    
    return 0;
}
