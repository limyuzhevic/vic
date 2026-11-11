#include "DemoFramework.hpp"
#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <utility>

using namespace nlm;

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo : public DemoFramework {
public:
    Phase3Demo() : DemoFramework(
        "NLM PHASE 3 - WORLD INTERACTION DEMO",
        "Shows: experience -> neural activity -> action -> consequence -> learning"
    ) {}
    
    bool initialize(const DemoOptions& options) override {
        try {
            // Setup logging
            setupLogging(options.verbose);
            
            // Setup configuration
            setupConfiguration(options);
            
            // Apply Phase 3 specific configuration
            applyPhaseConfig(config_, 3);
            
            // Create and setup the world
            auto world = std::make_shared<SimpleWorld>();
            world->configure(options.maxNeurons / 100, options.maxNeurons / 100, 16, 16);
            world->setRandomSeed(options.seed);
            setupWorld(world);
            
            // Setup brain
            setupBrain(config_);
            
            // Setup agent brain interface
            auto agent = std::make_shared<AgentBrain>(brain_);
            setupAgent(agent, world_);
            
            // Configure agent brain features
            agent_->enableRewardModulation(true);
            agent_->enableStructuralPlasticity(true);
            agent_->enableDevelopment(true);
            agent_->enableCuriosity(true);
            
            // Record initial state
            recordMetric("initial_neurons", brain_->getTotalNeuronCount());
            recordMetric("initial_synapses", brain_->getTotalSynapseCount());
            
            NLM_LOG_INFO("Initial brain state:");
            NLM_LOG_INFO("  Neurons: " + std::to_string(brain_->getTotalNeuronCount()));
            NLM_LOG_INFO("  Synapses: " + std::to_string(brain_->getTotalSynapseCount()));
            NLM_LOG_INFO("  Sensory input size: " + std::to_string(agent_->getSensoryInputSize()));
            NLM_LOG_INFO("  Motor output size: " + std::to_string(agent_->getMotorOutputSize()));
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool runDemo(const DemoOptions& options) override {
        // Run episodes
        for (int episode = 0; episode < options.numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset environment
            world_->reset();
            agent_->reset();
            brain_->reset();
            
            // Run steps
            for (int step = 0; step < options.stepsPerEpisode; ++step) {
                float progress = (step / static_cast<float>(options.stepsPerEpisode));
                printProgress(step, options.stepsPerEpisode, progress);
                
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world_->getSensoryPercept();
                
                // 2. Inject sensory input into brain
                agent_->processSensoryInput(percept);
                
                // 3. Simulate brain (10 steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain_->step(0);
                }
                
                // 4. Decode motor command from brain activity
                MotorCommand cmd = agent_->decodeMotorCommand();
                
                // 5. Apply motor command to world
                ActionResult result = world_->applyMotorCommand(cmd, world_->getSimulationTime());
                
                // 6. Apply reward modulation
                agent_->applyRewardModulation(result.reward, 0.0f);
                
                // Track metrics
                float episodeReward = result.reward;
                recordMetric("episode_reward", episodeReward);
                recordMetric("total_spikes", brain_->getTotalSpikeCount());
                
                // 7. Update development
                agent_->updateDevelopment(0.1);
                
                // Print periodic updates
                if (step % 50 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << motorCommandToString(cmd)
                              << ", novelty=" << agent_->getNoveltyLevel()
                              << ", energy=" << world_->getAgentBody().energy
                              << "\n";
                }
            }
        }
        
        return true;
    }
    
    void printResults(const DemoOptions& options) const override {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "SUMMARY" << "\n";
        std::cout << std::string(60, '=') << "\n\n";
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
        
        // Print metrics summary
        printMetricsSummary();
        
        // Print elapsed time
        std::cout << "\nTotal execution time: " << getElapsedTime() << "s\n";
    }
    
    // Helper methods
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
};

} // namespace nlm

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    nlm::Phase3Demo demo;
    return demo.run(argc, argv);
}