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
        
        // Create configuration using helper methods
        auto config = createDefaultConfig();
        auto world = createConfiguredWorld();
        auto brain = createConfiguredBrain(config);
        auto agentBrain = createConfiguredAgentBrain(brain, world);
        
        displayInitialState(brain, agentBrain);
        
        // Run episodes using episode runner
        auto allMetrics = runAllEpisodes(world, agentBrain, brain, numEpisodes, stepsPerEpisode);
        
        // Display summary
        displaySummary(allMetrics);
    }
    
private:
    // Configuration helpers
    std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        return config;
    }
    
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
    
    // World creation
    SimpleWorld createConfiguredWorld() {
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        return world;
    }
    
    // Brain creation
    std::shared_ptr<Brain> createConfiguredBrain(std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }
    
    // Agent creation
    AgentBrain createConfiguredAgentBrain(std::shared_ptr<Brain> brain, SimpleWorld& world) {
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        return agentBrain;
    }
    
    // Display initial state
    void displayInitialState(std::shared_ptr<Brain> brain, AgentBrain& agentBrain) {
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
    }
    
    // Run all episodes
    std::vector<ExperimentMetrics> runAllEpisodes(
        SimpleWorld& world,
        AgentBrain& agentBrain,
        std::shared_ptr<Brain> brain,
        int numEpisodes,
        int stepsPerEpisode
    ) {
        std::vector<ExperimentMetrics> allMetrics;
        
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            runSingleEpisode(world, agentBrain, brain, episode, stepsPerEpisode, allMetrics);
            
            std::cout << "--- Episode " << episode << " ended ---\n";
            printEpisodeResults(allMetrics.back());
            std::cout << std::endl;
        }
        
        return allMetrics;
    }
    
    // Run a single episode
    void runSingleEpisode(
        SimpleWorld& world,
        AgentBrain& agentBrain,
        std::shared_ptr<Brain> brain,
        int episode,
        int stepsPerEpisode,
        std::vector<ExperimentMetrics>& allMetrics
    ) {
        // Reset state
        world.reset();
        agentBrain.reset();
        brain->reset();
        
        // Create metrics for this episode
        ExperimentMetrics metrics;
        metrics.episode = episode;
        
        // Run episode steps
        for (int step = 0; step < stepsPerEpisode; ++step) {
            runSingleStep(world, agentBrain, brain, step, metrics);
        }
        
        // Record final metrics
        recordFinalMetrics(agentBrain, brain, metrics);
        allMetrics.push_back(metrics);
    }
    
    // Run a single step
    void runSingleStep(
        SimpleWorld& world,
        AgentBrain& agentBrain,
        std::shared_ptr<Brain> brain,
        int step,
        ExperimentMetrics& metrics
    ) {
        // 1. Get sensory percept from world
        const SensoryPercept& percept = world.getSensoryPercept();
        
        // 2. Inject sensory input into brain
        agentBrain.processSensoryInput(percept);
        
        // 3. Simulate brain (multiple steps per action)
        simulateBrain(brain, step);
        
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
        metrics.totalReward += result.reward;
        metrics.steps++;
        metrics.totalSpikes += brain->getFiringNeuronCount();
        countAction(metrics, cmd);
        
        // Print periodic updates
        if (step % 50 == 0) {
            printStepProgress(step, result, cmd, agentBrain, world);
        }
    }
    
    // Simulate brain for multiple steps
    void simulateBrain(std::shared_ptr<Brain> brain, int step) {
        for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
            brain->step(neuralStep);
        }
    }
    
    // Count action in metrics
    void countAction(ExperimentMetrics& metrics, MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: metrics.forwardCount++; break;
            case MotorCommand::MoveBackward: metrics.backwardCount++; break;
            case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
            case MotorCommand::TurnRight: metrics.turnRightCount++; break;
            case MotorCommand::Interact: metrics.interactCount++; break;
            default: metrics.waitCount++; break;
        }
    }
    
    // Print step progress
    void printStepProgress(
        int step,
        ActionResult& result,
        MotorCommand cmd,
        AgentBrain& agentBrain,
        SimpleWorld& world
    ) {
        std::cout << "  Step " << step 
                  << ": reward=" << result.reward
                  << ", action=" << motorCommandToString(cmd)
                  << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                  << ", novelty=" << agentBrain.getNoveltyLevel()
                  << ", energy=" << world.getAgentBody().energy
                  << "\n";
    }
    
    // Record final metrics
    void recordFinalMetrics(
        AgentBrain& agentBrain,
        std::shared_ptr<Brain> brain,
        ExperimentMetrics& metrics
    ) {
        metrics.finalNovelty = agentBrain.getNoveltyLevel();
        metrics.finalCuriosity = agentBrain.getCuriosityLevel();
        metrics.finalEnergy = world.getAgentBody().energy;  // Need world reference
        metrics.predictionError = agentBrain.getPredictionError();
        metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount();  // Use as proxy
        metrics.developmentalStage = stageToString(agentBrain.getDevelopmentalStage());
        metrics.totalSynapses = brain->getTotalSynapseCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
    }
    
    // Print episode results
    void printEpisodeResults(const ExperimentMetrics& metrics) {
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
                  << "\n";
    }
    
    // Display summary
    void displaySummary(const std::vector<ExperimentMetrics>& allMetrics) {
        std::cout << "===========================================\n";
        std::cout << "SUMMARY\n";
        std::cout << "===========================================\n";
        
        if (allMetrics.size() >= 2) {
            analyzeAndDisplayBehavioralChanges(allMetrics);
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
    }
    
    // Analyze and display behavioral changes
    void analyzeAndDisplayBehavioralChanges(const std::vector<ExperimentMetrics>& allMetrics) {
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
