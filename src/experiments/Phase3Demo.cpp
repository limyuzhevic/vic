#include "Phase3Demo.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <utility>

namespace nlm {

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo : public DemoBase {
public:
    Phase3Demo() : DemoBase("Phase 3 - World Interaction") {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) override {
        printHeader();
        
        // Initialize components
        if (!initialize()) {
            std::cout << "Failed to initialize Phase 3 demo!\n";
            return;
        }
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain->getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain->getMotorOutputSize() << "\n\n";
        
        // Configure agent brain
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        
        // Metrics tracking
        std::vector<DemoMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset world
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;

            // Phase 5 navigation/interact metrics: visited 1x1 cells,
            // interact outcomes from the world's own result message
            // (consumed_resource / hit_hazard / no_object — not a label
            // given to the brain), and forward/backward oscillation
            // (a Fwd step immediately followed by Bwd, or vice versa).
            std::set<std::pair<int, int>> visitedCells;
            int resourcesConsumed = 0, hazardsHit = 0, emptyInteracts = 0;
            int tooSoonCount = 0;
            int oscillations = 0;
            MotorCommand prevMove = MotorCommand::Wait;
            bool hasPrevMove = false;

            float episodeReward = 0.0f;
            // Monotonic brain-step counter for this episode. The old code
            // reused neuralStep 0..9 for every action, so delayed spikes
            // scheduled for step 10+ could never be delivered and
            // accumulated forever in the spike system.
            uint64_t brainStepCounter = 0;

            // Convert old ExperimentMetrics to DemoMetrics
            DemoMetrics demoMetrics;
            demoMetrics.episode = episode;
            demoMetrics.steps = metrics.steps;
            demoMetrics.totalReward = metrics.totalReward;
            demoMetrics.totalSpikes = metrics.totalSpikes;
            demoMetrics.finalNovelty = metrics.finalNovelty;
            demoMetrics.finalCuriosity = metrics.finalCuriosity;
            demoMetrics.finalEnergy = metrics.finalEnergy;
            demoMetrics.predictionError = metrics.predictionError;
            demoMetrics.developmentalAge = metrics.developmentalAge;
            demoMetrics.developmentalStage = metrics.developmentalStage;
            demoMetrics.totalSynapses = metrics.totalSynapses;
            demoMetrics.averageFiringRate = metrics.averageFiringRate;
            demoMetrics.forwardCount = metrics.forwardCount;
            demoMetrics.backwardCount = metrics.backwardCount;
            demoMetrics.turnLeftCount = metrics.turnLeftCount;
            demoMetrics.turnRightCount = metrics.turnRightCount;
            demoMetrics.interactCount = metrics.interactCount;
            demoMetrics.waitCount = metrics.waitCount;
            demoMetrics.lookLeftCount = metrics.lookLeftCount;
            demoMetrics.lookRightCount = metrics.lookRightCount;
            
            // Phase 5 metrics
            demoMetrics.visitedCells = static_cast<uint64_t>(visitedCells.size());
            demoMetrics.resourcesConsumed = resourcesConsumed;
            demoMetrics.hazardsHit = hazardsHit;
            demoMetrics.emptyInteracts = emptyInteracts;
            demoMetrics.oscillations = oscillations;
            demoMetrics.tooSoonCount = tooSoonCount;
            
            // Set additional metrics (these would need to be gathered from agentBrain)
            demoMetrics.reflexBonusCount = 0; // TODO: Implement
            demoMetrics.reflexSuppressCount = 0; // TODO: Implement
            demoMetrics.wtaPulses = 0; // TODO: Implement
            demoMetrics.plannerOverrideCount = 0; // TODO: Implement
            demoMetrics.plannerAgreementCount = 0; // TODO: Implement
            demoMetrics.plannerDecisionCount = 0; // TODO: Implement
            demoMetrics.conceptBiasCount = 0; // TODO: Implement
            demoMetrics.selfBiasCount = 0; // TODO: Implement
            demoMetrics.socialBiasCount = 0; // TODO: Implement
            demoMetrics.conceptCount = 0; // TODO: Implement
            demoMetrics.selfModelConfidence = 0.0f; // TODO: Implement
            demoMetrics.wmTraces = 0; // TODO: Implement
            
            allMetrics.push_back(demoMetrics);
            
            printEpisodeMetrics(demoMetrics, episode);
        }
        
        // Print demo summary
        printSummary(allMetrics);
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
    }
    
private:
    void configureEnvironment(std::shared_ptr<Config> config) override {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    }
    
    void configureBrain(std::shared_ptr<Config> config) override {
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
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
                      << ", lookL=" << metrics.lookLeftCount
                      << ", lookR=" << metrics.lookRightCount
                      << ", avgFiringRate=" << metrics.averageFiringRate
                      << "\n";
            std::cout << "  Phase5 metrics: visitedCells=" << visitedCells.size()
                      << ", resources=" << resourcesConsumed
                      << ", hazards=" << hazardsHit
                      << ", emptyInteracts=" << emptyInteracts
                      << ", oscillations=" << oscillations
                      << ", finalNovelty=" << metrics.finalNovelty
                      << ", finalCuriosity=" << metrics.finalCuriosity
                      << ", tooSoon=" << tooSoonCount
                      << ", executed=" << (stepsPerEpisode - tooSoonCount)
                      << ", reflexBonus=" << agentBrain.getReflexBonusCount()
                      << ", reflexSuppress=" << agentBrain.getReflexSuppressCount()
                      << ", wtaPulses=" << agentBrain.getWtaPulseCount()
                      << ", plannerOverride=" << agentBrain.getPlannerOverrideCount()
                      << ", plannerAgree=" << agentBrain.getPlannerAgreementCount()
                      << "/" << agentBrain.getPlannerDecisionCount()
                      << ", conceptBias=" << agentBrain.getConceptBiasCount()
                      << ", selfBias=" << agentBrain.getSelfBiasCount()
                      << ", socialBias=" << agentBrain.getSocialBiasCount()
                      << ", concepts=" << agentBrain.getConceptCount()
                      << ", selfConf=" << agentBrain.getSelfModelConfidence()
                      << ", wmTraces=" << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0)
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
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
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
