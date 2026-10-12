#include "Phase3Experiment.hpp"
#include "ExperimentBase.hpp"
#include "ExperimentConfig.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nlm {

struct ExperimentLogger::Impl {
    std::ofstream file;
    bool isOpen;
    
    Impl(const std::string& filename) : isOpen(false) {
        file.open(filename);
        if (file.is_open()) {
            isOpen = true;
        }
    }
    
    ~Impl() {
        if (file.is_open()) {
            file.close();
        }
    }
};

ExperimentLogger::ExperimentLogger(const std::string& filename) 
    : pImpl(new Impl(filename)) {}

ExperimentLogger::~ExperimentLogger() = default;

void ExperimentLogger::log(const std::string& message) {
    if (pImpl->isOpen) {
        pImpl->file << message << std::endl;
    }
    std::cout << message << std::endl;
}

void ExperimentLogger::logPhase3Header() {
    std::string header = R"(
=======================================
NLM PHASE 3 - WORLD INTERACTION DEMO
=======================================
)";
    log(header);
}

void ExperimentLogger::logEpisodeStart(int episode) {
    log("--- Episode " + std::to_string(episode) + " started ---");
}

void ExperimentLogger::logEpisodeEnd(int episode, float totalReward, int steps, float finalNovelty) {
    std::ostringstream oss;
    oss << "--- Episode " << episode << " ended ---\n"
        << "  Steps: " << steps << "\n"
        << "  Total Reward: " << totalReward << "\n"
        << "  Final Novelty: " << finalNovelty << "\n"
        << "-------------------------------------";
    log(oss.str());
}

void ExperimentLogger::logStep(int step, float reward, float neuromod, float curiosity, 
                                float energy, const std::string& action) {
    std::ostringstream oss;
    oss << "Step " << step << ": reward=" << reward 
        << ", neuromod=" << neuromod << ", curiosity=" << curiosity
        << ", energy=" << energy << ", action=" << action;
    log(oss.str());
}

void ExperimentLogger::logDevelopment(float age, const std::string& stage) {
    log("Development: age=" + std::to_string(age) + ", stage=" + stage);
}

void ExperimentLogger::logSynapticStats(size_t totalSynapses, float avgWeight, float maxWeight) {
    std::ostringstream oss;
    oss << "Synapses: " << totalSynapses << ", avgWeight=" << avgWeight 
        << ", maxWeight=" << maxWeight;
    log(oss.str());
}

void ExperimentLogger::flush() {
    if (pImpl->isOpen) {
        pImpl->file.flush();
    }
}

// Phase3Experiment implementation
Phase3Experiment::Phase3Experiment(const std::string& name) 
    : ExperimentBase(name) {
    logger_ = std::make_unique<ExperimentLogger>("phase3_experiment.log");
}

bool Phase3Experiment::initialize() {
    setStatus("Initializing Phase 3 Experiment");
    logger_->logPhase3Header();
    
    std::cout << "Phase 3 Experiment initialized successfully.\n";
    return true;
}

bool Phase3Experiment::isRunning() const {
    return getStatus() == "Running";
}

void Phase3Experiment::cleanup() {
    setStatus("Cleaning up Phase 3 Experiment");
    logger_->flush();
    
    std::cout << "Phase 3 Experiment cleanup complete.\n";
}

bool Phase3Experiment::run() {
    return runExperiment(nullptr, 5, 200); // Default parameters
}

Phase3Results Phase3Experiment::runExperiment(Brain* brain, size_t numEpisodes, size_t stepsPerEpisode) {
    Phase3Results results;
    results.experimentName = getName();
    results.seed = getSeed();
    
    setStatus("Running Phase 3 Experiment");
    setProgress(0.0f);
    
    // Configure using new config system
    auto config = ExperimentConfig::createPhase3Config(numEpisodes, stepsPerEpisode);
    
    // Create configuration
    auto experimentConfig = std::make_shared<Config>();
    configureEnvironment(experimentConfig);
    configureBrain(experimentConfig);
    
    // Create world
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(42);
    
    // Create brain if not provided
    if (!brain) {
        brain = new Brain(experimentConfig);
        brain->initialize();
    }
    
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
    
    // Run episodes
    for (size_t episode = 0; episode < numEpisodes; ++episode) {
        logger_->logEpisodeStart(episode);
        
        // Reset world
        world.reset();
        agentBrain.reset();
        brain->reset();
        
        float episodeReward = 0.0f;
        
        // Run steps
        for (size_t step = 0; step < stepsPerEpisode; ++step) {
            // 1. Get sensory percept from world
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // 2. Inject sensory input into brain
            agentBrain.processSensoryInput(percept);
            
            // 3. Simulate brain (multiple steps per action)
            for (size_t neuralStep = 0; neuralStep < 10; ++neuralStep) {
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
            
            // Log step
            logger_->logStep(step, result.reward, agentBrain.getNeuromodulationLevel(),
                            agentBrain.getCuriosityLevel(), world.getAgentBody().energy,
                            motorCommandToString(cmd));
        }
        
        // Record final metrics
        float totalSpikes = brain->getFiringNeuronCount();
        
        // Update results
        results.totalReward += episodeReward;
        results.steps += stepsPerEpisode;
        
        // Record episodic metrics
        logger_->logEpisodeEnd(episode, episodeReward, stepsPerEpisode, 
                             agentBrain.getNoveltyLevel());
        
        // Update step data
        std::ostringstream stepData;
        stepData << "Episode " << episode << ": reward=" << episodeReward 
                 << ", steps=" << stepsPerEpisode;
        results.stepData.push_back({stepData.str(), episodeReward});
        
        // Update development metrics
        results.developmentalAge = brain->getTotalSynapseCount();
        
        float progress = static_cast<float>(episode + 1) / numEpisodes * 100.0f;
        setProgress(progress);
    }
    
    setStatus("Phase 3 Experiment completed");
    setProgress(100.0f);
    
    return results;
}

void Phase3Experiment::configureEnvironment(std::shared_ptr<Config> config) {
    config->set("environment_width", 20);
    config->set("environment_height", 20);
    config->set("vision_width", 16);
    config->set("vision_height", 16);
}

void Phase3Experiment::configureBrain(std::shared_ptr<Config> config) {
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

const char* Phase3Experiment::motorCommandToString(MotorCommand cmd) {
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

} // namespace nlm
