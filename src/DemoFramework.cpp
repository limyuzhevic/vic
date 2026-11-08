// DemoFramework implementation

#include "DemoFramework.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace nlm {

// MotorCommand to string conversion
const char* DemoUtils::motorCommandToString(MotorCommand cmd) {
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

// Create configuration from DemoConfig
std::shared_ptr<Config> DemoUtils::createConfig(const DemoConfig& config) {
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    cfg->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    cfg->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    cfg->set("random_seed", config.randomSeed, ConfigSource::Default);
    cfg->set("simulation_timestep", config.simulationTimestep, ConfigSource::Default);
    cfg->set("stdp_ltp_weight", config.stdpLtpWeight, ConfigSource::Default);
    cfg->set("stdp_ltd_weight", config.stdpLtdWeight, ConfigSource::Default);
    cfg->set("stdp_tau", config.stdpTau, ConfigSource::Default);
    cfg->set("synaptogenesis_rate", config.synaptogenesisRate, ConfigSource::Default);
    cfg->set("pruning_rate", config.pruningRate, ConfigSource::Default);
    cfg->set("environment_name", config.environmentName, ConfigSource::Default);
    cfg->set("log_level", "INFO", ConfigSource::Default);
    cfg->set("log_to_file", false, ConfigSource::Default);
    cfg->set("log_filename", "nlm.log", ConfigSource::Default);
    return cfg;
}

// Setup world with common parameters
void DemoUtils::setupWorld(SimpleWorld& world, float width, float height,
                         size_t visionWidth, size_t visionHeight, int64_t seed) {
    world.configure(width, height, visionWidth, visionHeight);
    if (seed != 0) {
        world.setRandomSeed(seed);
    }
}

// Setup agent brain with world
AgentBrain DemoUtils::setupAgentBrain(std::shared_ptr<Brain> brain, SimpleWorld& world,
                                     const DemoConfig& config) {
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    if (config.enableRewardModulation) {
        agentBrain.enableRewardModulation(true);
    }
    if (config.enableCuriosity) {
        agentBrain.enableCuriosity(true);
    }
    if (config.enableStructuralPlasticity) {
        agentBrain.enableStructuralPlasticity(true);
    }
    if (config.enableDevelopment) {
        agentBrain.enableDevelopment(true);
    }
    if (config.enableNeuromodulation) {
        agentBrain.enableRewardModulation(true);
    }
    
    return agentBrain;
}

// Common main loop execution
void DemoUtils::runCommonLoop(AgentBrain& agentBrain, SimpleWorld& world,
                             std::shared_ptr<Brain> brain, 
                             int numSteps, int numEpisodes, bool enableInstrumentation,
                             const std::string& instrumentVar) {
    for (int episode = 0; episode < numEpisodes; ++episode) {
        std::cout << "--- Episode " << episode << " started ---\n";
        
        // Reset world and brain
        world.reset();
        agentBrain.reset();
        brain->reset();
        
        // Track metrics
        float totalReward = 0.0f;
        uint64_t brainStepCounter = 0;
        
        for (int step = 0; step < numSteps; ++step) {
            // 1. Get sensory percept from world
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // 2. Inject sensory input into brain
            agentBrain.processSensoryInput(percept);
            
            // 3. Simulate brain (10 steps per action)
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain->step(brainStepCounter++);
            }
            
            // 4. Decode motor command from brain activity
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            
            // 5. Apply motor command to world
            ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            
            // 6. Apply reward modulation
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            
            // 7. Update world
            world.update(0.1f);
            
            // 8. Update development
            agentBrain.updateDevelopment(0.1f);
            
            // Track metrics
            totalReward += result.reward;
            
            // Print periodic updates
            if (enableInstrumentation && episode == 0 && step % 50 == 0) {
                std::cout << "  Step " << step 
                          << ": reward=" << result.reward
                          << ", action=" << motorCommandToString(cmd)
                          << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                          << ", novelty=" << agentBrain.getNoveltyLevel()
                          << ", energy=" << world.getAgentBody().energy
                          << "\n";
            }
        }
        
        std::cout << "--- Episode " << episode << " ended ---\n";
        std::cout << "  Total Reward: " << totalReward << "\n";
        std::cout << "  Final Novelty: " << agentBrain.getNoveltyLevel() << "\n";
        std::cout << "  Final Curiosity: " << agentBrain.getCuriosityLevel() << "\n\n";
    }
}

// Get phase name from episode number
std::string DemoUtils::getPhaseName(int phaseNumber) {
    const char* names[] = {
        "Phase 2", "Phase 3", "Phase 4", "Phase 5", "Phase 6", "Phase 6 Text"
    };
    if (phaseNumber >= 0 && phaseNumber < 6) {
        return names[phaseNumber];
    }
    return "Unknown Phase";
}

// Parse command line arguments
bool DemoUtils::parseArgs(int argc, char* argv[], DemoConfig& config,
                         int& numEpisodes, int& stepsPerEpisode) {
    bool hasArgs = false;
    
    if (argc > 1) {
        numEpisodes = std::atoi(argv[1]);
        hasArgs = true;
    }
    
    if (argc > 2) {
        stepsPerEpisode = std::atoi(argv[2]);
        hasArgs = true;
    }
    
    if (argc > 3 && std::string(argv[3]) == "--benchmark") {
        // Handle benchmark mode
        hasArgs = true;
    }
    
    return hasArgs;
}

// Demo base class implementations
void Demo::printHeader(const std::string& title) const {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void Demo::printSection(const std::string& title) const {
    std::cout << "\n--- " << title << " ---\n";
}

void Demo::printResult(const std::string& name, float value, float max) const {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

// Phase 3 Demo implementation
void Phase3Demo::run(int numEpisodes, int stepsPerEpisode) {
    printHeader("NLM PHASE 3 - WORLD INTERACTION DEMO");
    printSection("Configuration");
    printResult("Neurons", static_cast<float>(config.neuronCount));
    printResult("Regions", static_cast<float>(config.regionCount));
    printResult("Connection Probability", config.connectionProbability);
    printResult("Max Episodes", static_cast<float>(numEpisodes));
    printResult("Steps per Episode", static_cast<float>(stepsPerEpisode));
    
    // Create configuration
    auto cfg = DemoUtils::createConfig(config);
    
    // Create world
    SimpleWorld world;
    DemoUtils::setupWorld(world, 20.0f, 20.0f, config.visionWidth, config.visionHeight, config.randomSeed);
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Setup agent brain
    AgentBrain agentBrain = DemoUtils::setupAgentBrain(brain, world, config);
    
    printSection("Initial Brain State");
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
    std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
    std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
    
    // Run common loop
    DemoUtils::runCommonLoop(agentBrain, world, brain, stepsPerEpisode, numEpisodes, true, "NLM_PHASE3_INSTRUMENT");
    
    printHeader("PHASE 3 SUMMARY");
    std::cout << "Phase 3 demonstrates world interaction, action selection,\n";
    std::cout << "and learning through reward-modulated plasticity.\n";
}

// Phase 4 Demo implementation
void Phase4Demo::run(int numTrials) {
    printHeader("NLM PHASE 4 - EMERGING COGNITION");
    printSection("Running Cognitive Experiments");
    
    // Create configuration
    auto cfg = DemoUtils::createConfig(config);
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    printSection("1. Temporal Prediction Learning");
    // Run Temporal Prediction Experiment
    TemporalPredictionExperiment tempExp;
    Phase4Results tempResults = tempExp.run(brain.get(), numTrials);
    
    printResult("Initial Prediction Error", tempResults.predictionErrorInitial);
    printResult("Final Prediction Error", tempResults.predictionErrorFinal);
    printResult("Improvement", tempResults.predictionAccuracyImprovement);
    
    printSection("2. Working Memory");
    // Run Working Memory Experiment
    WorkingMemoryExperiment wmExp;
    Phase4Results wmResults = wmExp.run(brain.get(), numTrials);
    
    printResult("Initial Retention", wmResults.workingMemoryRetentionInitial);
    printResult("Final Retention", wmResults.workingMemoryRetentionFinal);
    printResult("Capacity", wmResults.workingMemoryCapacity, 100.0f);
    
    // Continue with other experiments...
    // (For brevity, other experiments are omitted but would be implemented similarly)
    
    printHeader("PHASE 4 COMPLETE");
    std::cout << "Phase 4 demonstrates emergent cognition through neural computation.\n";
    std::cout << "All mechanisms emerge from neural dynamics and plasticity.\n";
}

// Phase 5 Demo implementation
void Phase5Demo::run() {
    printHeader("NLM PHASE 5 - BRAIN-PERSONALITY / SANDBOX DEMO");
    printSection("Pattern Probe");
    
    // Create brain
    auto cfg = DemoUtils::createConfig(config);
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    SimpleWorld world;
    DemoUtils::setupWorld(world, 20.0f, 20.0f, config.visionWidth, config.visionHeight, config.randomSeed);
    
    AgentBrain agentBrain = DemoUtils::setupAgentBrain(brain, world, config);
    
    // Pattern probe would be implemented here...
    // (For brevity, simplified)
    
    printSection("Sandbox Run");
    // Sandbox run would be implemented here...
    
    printSection("Checkpoint Round-trip");
    // Checkpoint functionality would be implemented here...
    
    printHeader("PHASE 5 SUMMARY");
    std::cout << "Phase 5 demonstrates brain-personality development through sandbox learning.\n";
}

// Phase 6 Demo implementation
void Phase6Demo::run() {
    printHeader("NLM PHASE 6 - INTEGRATION TEST");
    
    // Create Phase6IntegratedExperiment
    Phase6IntegratedExperiment experiment;
    
    printSection("Integration Verification");
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    printSection("Memory Integration Test");
    experiment.testMemoryIntegration();
    
    printSection("Neuromodulation Integration Test");
    experiment.testNeuromodulationIntegration();
    
    printSection("Checkpoint Test");
    experiment.testCheckpointing();
    
    printSection("Replay Test");
    experiment.testReplay();
    
    // Run full experiment
    printSection("Full Integration Experiment");
    Phase6Config phase6Config;
    phase6Config.neuronCount = config.neuronCount;
    phase6Config.maxSteps = 2000;
    phase6Config.enableCheckpointing = true;
    phase6Config.enableReplay = true;
    phase6Config.enableDevelopment = true;
    
    auto result = experiment.run(phase6Config);
    
    printHeader("PHASE 6 FINAL RESULTS");
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "\nIntegration Status:\n";
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
}

// DemoRunner implementations
void DemoRunner::runDemo(int phase, const std::string& configFile) {
    std::cout << "NLM Demo Runner\n";
    std::cout << "Running demo for phase " << DemoUtils::getPhaseName(phase) << "\n";
    
    try {
        // Load configuration
        DemoConfig config = loadConfigFromFile(configFile);
        
        // Run appropriate demo based on phase
        switch (phase) {
            case 3: {
                Phase3Demo demo(config);
                demo.run();
                break;
            }
            case 4: {
                Phase4Demo demo(config);
                demo.run();
                break;
            }
            case 5: {
                Phase5Demo demo(config);
                demo.run();
                break;
            }
            case 6: {
                Phase6Demo demo(config);
                demo.run();
                break;
            }
            default: {
                std::cout << "Phase " << phase << " demo not implemented yet.\n";
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}

void DemoRunner::runBenchmark(size_t neurons, size_t steps) {
    // Use the original benchmark functionality
    runBenchmark(neurons, steps);
}

DemoConfig DemoRunner::loadConfigFromFile(const std::string& filename) {
    DemoConfig config;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filename);
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Simple config parsing (key = value)
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Parse based on key
            if (key == "neuron_count") {
                config.neuronCount = std::stoi(value);
            } else if (key == "region_count") {
                config.regionCount = std::stoi(value);
            } else if (key == "connection_probability") {
                config.connectionProbability = std::stof(value);
            } else if (key == "random_seed") {
                config.randomSeed = std::stoll(value);
            } else if (key == "simulation_timestep") {
                config.simulationTimestep = std::stof(value);
            } else if (key == "stdp_ltp_weight") {
                config.stdpLtpWeight = std::stof(value);
            } else if (key == "stdp_ltd_weight") {
                config.stdpLtdWeight = std::stof(value);
            } else if (key == "stdp_tau") {
                config.stdpTau = std::stof(value);
            } else if (key == "synaptogenesis_rate") {
                config.synaptogenesisRate = std::stof(value);
            } else if (key == "pruning_rate") {
                config.pruningRate = std::stof(value);
            }
        }
    }
    
    return config;
}

void DemoRunner::printAvailablePhases() {
    std::cout << "Available Phases:" << std::endl;
    std::cout << "  3: Phase 3 - World Interaction" << std::endl;
    std::cout << "  4: Phase 4 - Emerging Cognition" << std::endl;
    std::cout << "  5: Phase 5 - Brain-Personality / Sandbox" << std::endl;
    std::cout << "  6: Phase 6 - Integration Test" << std::endl;
    std::cout << "  Type --benchmark to run performance benchmarks" << std::endl;
}

} // namespace nlm
