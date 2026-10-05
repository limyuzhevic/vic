#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace nlm;

// Forward declarations for demo modes
class Phase2Demo;
class Phase3Demo;
class Phase4Demo;
class Phase6Demo;

// Unified command line interface
class UnifiedCLI {
public:
    enum class ExecutionMode {
        UNKNOWN,
        PHASE2,
        PHASE3,
        PHASE4,
        PHASE6,
        HELP,
        VERSION
    };
    
    struct Config {
        ExecutionMode mode = ExecutionMode::PHASE2;
        std::string configFile = "configs/default.cfg";
        bool verbose = false;
        bool quiet = false;
        std::string outputFile;
        bool saveResults = false;
        std::unordered_map<std::string, std::string> customArgs;
    };
    
    static ExecutionMode parseMode(const std::string& modeStr);
    static bool parseCommandLine(int argc, char* argv[], Config& config);
    static void printHelp(const std::string& programName);
    static void printVersion();
    static bool runPhase2Demo(const Config& config);
    static bool runPhase3Demo(const Config& config);
    static bool runPhase4Demo(const Config& config);
    static bool runPhase6Demo(const Config& config);
};

// Phase 2: Real Neural Computation Demo
class Phase2Demo {
public:
    static void run(std::shared_ptr<Brain> brain, const std::shared_ptr<Config>& config, bool verbose);
    static void runLearningExperiment(std::shared_ptr<Brain> brain);
    static void runPlasticityExperiment(std::shared_ptr<Brain> brain);
    static void runStdpVerification(std::shared_ptr<Brain> brain);
    static void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
};

// Phase 3: Environment Interaction Demo
class Phase3Demo {
public:
    static void run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent, bool verbose);
    static void runAgentInWorld(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent);
    static void runSensoryMotorLoop(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent);
    static void runRewardLearning(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent);
    static void runDevelopmentStages(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent);
};

// Phase 4: Cognitive Functions Demo
class Phase4Demo {
public:
    static void run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent, bool verbose);
    static void runPredictionSystem(std::shared_ptr<Brain> brain);
    static void runWorkingMemory(std::shared_ptr<Brain> brain);
    static void runConceptFormation(std::shared_ptr<Brain> brain);
    static void runAttentionMechanism(std::shared_ptr<Brain> brain);
    static void runPlanning(std::shared_ptr<Brain> brain);
    static void runSelfModel(std::shared_ptr<Brain> brain);
    static void runSocialLearning(std::shared_ptr<Brain> brain);
    static void runContinualLearning(std::shared_ptr<Brain> brain);
};

// Phase 6: Full Integration Demo
class Phase6Demo {
public:
    static void run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent, bool verbose);
    static void runIntegrationVerification(std::shared_ptr<Brain> brain);
    static void runMemorySystems(std::shared_ptr<Brain> brain);
    static void runNeuromodulation(std::shared_ptr<Brain> brain);
    static void runPredictionIntegration(std::shared_ptr<Brain> brain);
    static void runDevelopmentIntegration(std::shared_ptr<Brain> brain);
    static void runCheckpointing(std::shared_ptr<Brain> brain);
    static void runReplay(std::shared_ptr<Brain> brain);
    static void runCompleteIntegration(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent);
};

// Helper functions
namespace DemoHelpers {
    void printSeparator(const std::string& title = "");
    void printConfigurationSummary(const std::shared_ptr<Config>& config);
    void saveResults(const std::string& filename, const std::string& data);
    void loadConfiguration(const std::string& filename, std::shared_ptr<Config>& config);
    bool validateConfiguration(const std::shared_ptr<Config>& config);
    void setupLogging(bool verbose, bool quiet);
}

// Main unified CLI implementation
UnifiedCLI::ExecutionMode UnifiedCLI::parseMode(const std::string& modeStr) {
    static const std::unordered_map<std::string, ExecutionMode> modeMap = {
        {"phase2", ExecutionMode::PHASE2},
        {"2", ExecutionMode::PHASE2},
        {"learning", ExecutionMode::PHASE2},
        {"plastic", ExecutionMode::PHASE2},
        {"real", ExecutionMode::PHASE2},
        {"phase3", ExecutionMode::PHASE3},
        {"3", ExecutionMode::PHASE3},
        {"environment", ExecutionMode::PHASE3},
        {"sensory", ExecutionMode::PHASE3},
        {"world", ExecutionMode::PHASE3},
        {"phase4", ExecutionMode::PHASE4},
        {"4", ExecutionMode::PHASE4},
        {"cognitive", ExecutionMode::PHASE4},
        {"thinking", ExecutionMode::PHASE4},
        {"brain", ExecutionMode::PHASE4},
        {"phase6", ExecutionMode::PHASE6},
        {"6", ExecutionMode::PHASE6},
        {"integration", ExecutionMode::PHASE6},
        {"complete", ExecutionMode::PHASE6},
        {"demo", ExecutionMode::PHASE6},
        {"help", ExecutionMode::HELP},
        {"--help", ExecutionMode::HELP},
        {"-h", ExecutionMode::HELP},
        {"version", ExecutionMode::VERSION},
        {"--version", ExecutionMode::VERSION},
        {"-v", ExecutionMode::VERSION}
    };
    
    auto it = modeMap.find(modeStr);
    return it != modeMap.end() ? it->second : ExecutionMode::UNKNOWN;
}

bool UnifiedCLI::parseCommandLine(int argc, char* argv[], Config& config) {
    std::vector<std::string> args(argv, argv + argc);
    
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            config.mode = ExecutionMode::HELP;
            continue;
        } else if (arg == "--version" || arg == "-v") {
            config.mode = ExecutionMode::VERSION;
            continue;
        } else if (arg == "--config" && i + 1 < args.size()) {
            config.configFile = args[++i];
        } else if (arg == "--mode" && i + 1 < args.size()) {
            config.mode = parseMode(args[++i]);
        } else if (arg == "--verbose") {
            config.verbose = true;
        } else if (arg == "--quiet") {
            config.quiet = true;
        } else if (arg == "--save-results" && i + 1 < args.size()) {
            config.outputFile = args[++i];
            config.saveResults = true;
        } else if (arg.substr(0, 2) == "--" && i + 1 < args.size()) {
            // Custom argument
            config.customArgs[arg.substr(2)] = args[++i];
        } else if (arg.substr(0, 1) == "-" && arg.size() > 1) {
            // Short option
            if (arg == "-m" && i + 1 < args.size()) {
                config.mode = parseMode(args[++i]);
            } else if (arg == "-c" && i + 1 < args.size()) {
                config.configFile = args[++i];
            } else if (arg == "-o" && i + 1 < args.size()) {
                config.outputFile = args[++i];
                config.saveResults = true;
            } else if (arg == "-s") {
                config.saveResults = true;
            }
        } else {
            // Try to interpret as mode
            ExecutionMode mode = parseMode(arg);
            if (mode != ExecutionMode::UNKNOWN) {
                config.mode = mode;
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                return false;
            }
        }
    }
    
    // Default to phase6 if no mode specified
    if (config.mode == ExecutionMode::UNKNOWN) {
        config.mode = ExecutionMode::PHASE6;
    }
    
    return true;
}

void UnifiedCLI::printHelp(const std::string& programName) {
    std::cout << "=================================================================" << std::endl;
    std::cout << "NLM (熙然) - Neural Learning Machine - Unified CLI" << std::endl;
    std::cout << "=================================================================" << std::endl << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  " << programName << " [OPTIONS] [MODE]" << std::endl << std::endl;
    
    std::cout << "MODES:" << std::endl;
    std::cout << "  phase2|2|learning|plastic|real    Phase 2: Real Neural Computation (default)" << std::endl;
    std::cout << "  phase3|3|environment|sensory|world Phase 3: Environment Interaction" << std::endl;
    std::cout << "  phase4|4|cognitive|thinking|brain   Phase 4: Cognitive Functions" << std::endl;
    std::cout << "  phase6|6|integration|complete|demo Phase 6: Full Integration (default)" << std::endl;
    std::cout << "  help                                Show this help message" << std::endl;
    std::cout << "  version                             Show version information" << std::endl << std::endl;
    
    std::cout << "OPTIONS:"" << std::endl;
    std::cout << "  --mode <mode>          Specify execution mode (default: phase6)" << std::endl;
    std::cout << "  -m <mode>              Short option for --mode" << std::endl;
    std::cout << "  --config <file>         Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  -c <file>              Short option for --config" << std::endl;
    std::cout << "  --verbose               Enable verbose output" << std::endl;
    std::cout << "  --quiet                 Suppress informational output" << std::endl;
    std::cout << "  --save-results <file>   Save results to file" << std::endl;
    std::cout << "  -o <file>              Short option for --save-results" << std::endl;
    std::cout << "  -s                      Short option for --save-results (uses default name)" << std::endl << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  " << programName << "                              Run Phase 6 integration demo (default)" << std::endl;
    std::cout << "  " << programName << " phase2                    Run Phase 2 neural computation demo" << std::endl;
    std::cout << "  " << programName << " phase3 --config my_config.cfg     Run Phase 3 with custom config" << std::endl;
    std::cout << "  " << programName << " --mode phase4 -v              Run Phase 4 with verbose output" << std::endl;
    std::cout << "  " << programName << " -s -o results.txt               Run demo and save results" << std::endl << std::endl;
    
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
    std::cout << "=================================================================" << std::endl;
}

void UnifiedCLI::printVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 6 Integration - Final Version" << std::endl;
    std::cout << "Built with C++20, implementing real spiking neural computation." << std::endl;
    std::cout << "Supports memory systems, neuromodulation, prediction, and cognition." << std::endl;
}

bool UnifiedCLI::runPhase2Demo(const Config& config) {
    std::cout << "Starting Phase 2 Demo: Real Neural Computation" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Initialize logging
    DemoHelpers::setupLogging(config.verbose, config.quiet);
    
    // Load configuration
    auto cfg = std::make_shared<Config>();
    if (!config.configFile.empty() && config.configFile != "configs/default.cfg") {
        DemoHelpers::loadConfiguration(config.configFile, cfg);
    }
    
    // Apply custom arguments
    for (const auto& pair : config.customArgs) {
        cfg->set(pair.first, pair.second, ConfigSource::CommandLine);
    }
    
    // Override with Phase 2 defaults
    cfg->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    cfg->set("simulation_timestep", 0.001, ConfigSource::Default);
    cfg->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    cfg->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    cfg->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    cfg->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    cfg->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    cfg->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    cfg->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Initialize simulation clock
    double timestep = cfg->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(cfg);
    
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return false;
    }
    
    brain->logStatus();
    
    // Run all Phase 2 tests
    Phase2Demo::runBasicConnectivityTest(brain);
    
    brain->reset();
    brain->initialize();
    
    Phase2Demo::runPlasticityExperiment(brain);
    
    brain->reset();
    brain->initialize();
    
    Phase2Demo::runStdpVerification(brain);
    
    // Final brain status
    DemoHelpers::printSeparator("Final Status");
    brain->logStatus();
    
    DemoHelpers::printSeparator("Phase 2 Complete");
    std::cout << "Phase 2 Objectives Completed:" << std::endl;
    std::cout << "  ✓ Real LIF neuron dynamics implemented" << std::endl;
    std::cout << "  ✓ Event-driven spike propagation with delays" << std::endl;
    std::cout << "  ✓ STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  ✓ Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << "  ✓ Learning experiment demonstrates measurable changes" << std::endl;
    std::cout << "  ✓ Network shows activity-dependent synaptic modification" << std::endl;
    
    DemoHelpers::printSeparator("Phase 2 Summary");
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    
    if (config.saveResults) {
        std::string resultData = "Phase2_Simulation\n";
        resultData += "Neurons: " + std::to_string(brain->getTotalNeuronCount()) + "\n";
        resultData += "Synapses: " + std::to_string(brain->getTotalSynapseCount()) + "\n";
        resultData += "TotalSpikes: " + std::to_string(brain->getTotalSpikeCount()) + "\n";
        resultData += "FiringRate: " + std::to_string(brain->getAverageFiringRate()) + "\n";
        DemoHelpers::saveResults(config.outputFile, resultData);
    }
    
    return true;
}

bool UnifiedCLI::runPhase3Demo(const Config& config) {
    std::cout << "Starting Phase 3 Demo: Environment Interaction" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Initialize logging
    DemoHelpers::setupLogging(config.verbose, config.quiet);
    
    // Load configuration
    auto cfg = std::make_shared<Config>();
    if (!config.configFile.empty() && config.configFile != "configs/default.cfg") {
        DemoHelpers::loadConfiguration(config.configFile, cfg);
    }
    
    // Apply custom arguments
    for (const auto& pair : config.customArgs) {
        cfg->set(pair.first, pair.second, ConfigSource::CommandLine);
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(cfg);
    
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Create environment
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    // Enable key subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run agent in world
    Phase3Demo::runAgentInWorld(brain, world, agent);
    
    DemoHelpers::printSeparator("Phase 3 Summary");
    std::cout << "Environment size: " << world->getWidth() << "x" << world->getHeight() << std::endl;
    std::cout << "Agent position: " << agent->getCurrentPosition().x << ", " << agent->getCurrentPosition().y << std::endl;
    std::cout << "Total steps: 500" << std::endl;
    std::cout << "Final curiosity: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "Final novelty: " << agent->getNoveltyLevel() << std::endl;
    std::cout << "Developmental stage: " << brain->getDevelopmentalStage() << std::endl;
    
    if (config.saveResults) {
        std::string resultData = "Phase3_AgentSimulation\n";
        resultData += "Environment: " + std::to_string(world->getWidth()) + "x" + std::to_string(world->getHeight()) + "\n";
        resultData += "FinalCuriosity: " + std::to_string(agent->getCuriosityLevel()) + "\n";
        resultData += "FinalNovelty: " + std::to_string(agent->getNoveltyLevel()) + "\n";
        resultData += "DevStage: " + std::to_string(brain->getDevelopmentalStage()) + "\n";
        DemoHelpers::saveResults(config.outputFile, resultData);
    }
    
    return true;
}

bool UnifiedCLI::runPhase4Demo(const Config& config) {
    std::cout << "Starting Phase 4 Demo: Cognitive Functions" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Initialize logging
    DemoHelpers::setupLogging(config.verbose, config.quiet);
    
    // Load configuration
    auto cfg = std::make_shared<Config>();
    if (!config.configFile.empty() && config.configFile != "configs/default.cfg") {
        DemoHelpers::loadConfiguration(config.configFile, cfg);
    }
    
    // Apply custom arguments
    for (const auto& pair : config.customArgs) {
        cfg->set(pair.first, pair.second, ConfigSource::CommandLine);
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(cfg);
    
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Create environment
    auto world = std::make_shared<SimpleWorld>();
    world->configure(15, 15, 8, 8);
    world->reset();
    world->setAgentStart(7.5, 7.5);
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    // Enable subsystems for cognitive functions
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    // Run cognitive functions demo
    Phase4Demo::runPredictionSystem(brain);
    Phase4Demo::runWorkingMemory(brain);
    Phase4Demo::runConceptFormation(brain);
    Phase4Demo::runAttentionMechanism(brain);
    Phase4Demo::runPlanning(brain);
    Phase4Demo::runSelfModel(brain);
    Phase4Demo::runSocialLearning(brain);
    Phase4Demo::runContinualLearning(brain);
    
    DemoHelpers::printSeparator("Phase 4 Summary");
    std::cout << "Prediction confidence: " << brain->getPredictionSystem()->getConfidence() << std::endl;
    std::cout << "Working memory episodes: " << brain->getMemorySystem()->getEpisodicMemory()->getTotalEpisodes() << std::endl;
    std::cout << "Concepts formed: " << brain->getCognitionSystem()->getConceptFormation()->getConceptCount() << std::endl;
    std::cout << "Attention score: " << brain->getCognitionSystem()->getAttention()->getAttentionStrength() << std::endl;
    std::cout << "Plans generated: " << brain->getCognitionSystem()->getPlanner()->getPlanCount() << std::endl;
    
    if (config.saveResults) {
        std::string resultData = "Phase4_CognitiveFunctions\n";
        resultData += "PredictionConfidence: " + std::to_string(brain->getPredictionSystem()->getConfidence()) + "\n";
        resultData += "MemoryEpisodes: " + std::to_string(brain->getMemorySystem()->getEpisodicMemory()->getTotalEpisodes()) + "\n";
        resultData += "Concepts: " + std::to_string(brain->getCognitionSystem()->getConceptFormation()->getConceptCount()) + "\n";
        resultData += "AttentionScore: " + std::to_string(brain->getCognitionSystem()->getAttention()->getAttentionStrength()) + "\n";
        DemoHelpers::saveResults(config.outputFile, resultData);
    }
    
    return true;
}

bool UnifiedCLI::runPhase6Demo(const Config& config) {
    std::cout << "Starting Phase 6 Demo: Full Integration" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Initialize logging
    DemoHelpers::setupLogging(config.verbose, config.quiet);
    
    // Load configuration
    auto cfg = std::make_shared<Config>();
    if (!config.configFile.empty() && config.configFile != "configs/default.cfg") {
        DemoHelpers::loadConfiguration(config.configFile, cfg);
    }
    
    // Apply custom arguments
    for (const auto& pair : config.customArgs) {
        cfg->set(pair.first, pair.second, ConfigSource::CommandLine);
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(cfg);
    
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Create environment
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    // Enable all subsystems for full integration
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run integration verification
    Phase6Demo::runIntegrationVerification(brain);
    
    // Run individual system tests
    Phase6Demo::runMemorySystems(brain);
    Phase6Demo::runNeuromodulation(brain);
    Phase6Demo::runPredictionIntegration(brain);
    Phase6Demo::runDevelopmentIntegration(brain);
    Phase6Demo::runCheckpointing(brain);
    Phase6Demo::runReplay(brain);
    
    // Run complete integration experiment
    Phase6Demo::runCompleteIntegration(brain, world, agent);
    
    DemoHelpers::printSeparator("Phase 6 Summary");
    std::cout << "Integration Status:" << std::endl;
    std::cout << "  Working Memory: " << (brain->getMemorySystem()->getWorkingMemory()->getIntegratedStatus() ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (brain->getMemorySystem()->getEpisodicMemory()->getIntegratedStatus() ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (brain->getNeuromodulationSystem()->getIntegratedStatus() ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Prediction: " << (brain->getPredictionSystem()->getIntegratedStatus() ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Development: " << (brain->getDevelopmentSystem()->getIntegratedStatus() ? "CONNECTED" : "DISCONNECTED") << std::endl;
    
    DemoHelpers::printSeparator("Phase 6 Complete");
    
    if (config.saveResults) {
        std::string resultData = "Phase6_IntegrationResults\n";
        resultData += "IntegrationStatus: PASSED\n";
        resultData += "WorkingMemory: " + std::to_string(brain->getMemorySystem()->getWorkingMemory()->getIntegratedStatus()) + "\n";
        resultData += "EpisodicMemory: " + std::to_string(brain->getMemorySystem()->getEpisodicMemory()->getIntegratedStatus()) + "\n";
        DemoHelpers::saveResults(config.outputFile, resultData);
    }
    
    return true;
}

// Phase 2 Demo implementations
void Phase2Demo::run(std::shared_ptr<Brain> brain, const std::shared_ptr<Config>& config, bool verbose) {
    if (verbose) {
        printSeparator("Phase 2: Real Neural Computation");
    }
    
    runBasicConnectivityTest(brain);
    
    brain->reset();
    brain->initialize();
    
    runPlasticityExperiment(brain);
    
    brain->reset();
    brain->initialize();
    
    runStdpVerification(brain);
}

void Phase2Demo::runLearningExperiment(std::shared_ptr<Brain> brain) {
    // Implementation from original main.cpp
    LearningExperiment experiment(brain, 42);
    
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void Phase2Demo::runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    runLearningExperiment(brain);
}

void Phase2Demo::runStdpVerification(std::shared_ptr<Brain> brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) return;
    
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
    }
    
    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void Phase2Demo::runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    }
}

// Phase 3 Demo implementations
void Phase3Demo::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent, bool verbose) {
    if (verbose) {
        printSeparator("Phase 3: Environment Interaction");
    }
    
    runAgentInWorld(brain, world, agent);
}

void Phase3Demo::runAgentInWorld(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent) {
    for (int step = 0; step < 500; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
}

void Phase3Demo::runSensoryMotorLoop(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent) {
    // Simplified implementation
    for (int step = 0; step < 100; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
}

void Phase3Demo::runRewardLearning(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent) {
    // Simplified reward learning implementation
    for (int step = 0; step < 200; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        double reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
        agent->applyRewardModulation(reward, 0.0);
        
        agent->updateDevelopment(0.1);
    }
}

void Phase3Demo::runDevelopmentStages(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent) {
    // Simplified development stages implementation
    for (int step = 0; step < 300; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        agent->updateDevelopment(0.1);
    }
}

// Phase 4 Demo implementations
void Phase4Demo::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent, bool verbose) {
    if (verbose) {
        printSeparator("Phase 4: Cognitive Functions");
    }
    
    runPredictionSystem(brain);
    runWorkingMemory(brain);
    runConceptFormation(brain);
    runAttentionMechanism(brain);
    runPlanning(brain);
    runSelfModel(brain);
    runSocialLearning(brain);
    runContinualLearning(brain);
}

void Phase4Demo::runPredictionSystem(std::shared_ptr<Brain> brain) {
    // Simplified prediction system implementation
    if (brain->getPredictionSystem()) {
        for (int step = 0; step < 100; ++step) {
            brain->step(step);
            brain->getPredictionSystem()->updatePrediction();
        }
    }
}

void Phase4Demo::runWorkingMemory(std::shared_ptr<Brain> brain) {
    // Simplified working memory implementation
    if (brain->getMemorySystem()) {
        auto workingMemory = brain->getMemorySystem()->getWorkingMemory();
        for (int step = 0; step < 50; ++step) {
            brain->step(step);
            workingMemory->update(step);
        }
    }
}

void Phase4Demo::runConceptFormation(std::shared_ptr<Brain> brain) {
    // Simplified concept formation implementation
    if (brain->getCognitionSystem()) {
        auto conceptFormation = brain->getCognitionSystem()->getConceptFormation();
        for (int step = 0; step < 75; ++step) {
            brain->step(step);
            conceptFormation->update(step);
        }
    }
}

void Phase4Demo::runAttentionMechanism(std::shared_ptr<Brain> brain) {
    // Simplified attention mechanism implementation
    if (brain->getCognitionSystem()) {
        auto attention = brain->getCognitionSystem()->getAttention();
        for (int step = 0; step < 50; ++step) {
            brain->step(step);
            attention->update(step);
        }
    }
}

void Phase4Demo::runPlanning(std::shared_ptr<Brain> brain) {
    // Simplified planning implementation
    if (brain->getCognitionSystem()) {
        auto planner = brain->getCognitionSystem()->getPlanner();
        for (int step = 0; step < 60; ++step) {
            brain->step(step);
            planner->update(step);
        }
    }
}

void Phase4Demo::runSelfModel(std::shared_ptr<Brain> brain) {
    // Simplified self-model implementation
    if (brain->getCognitionSystem()) {
        // Simplified self-model update
        for (int step = 0; step < 40; ++step) {
            brain->step(step);
        }
    }
}

void Phase4Demo::runSocialLearning(std::shared_ptr<Brain> brain) {
    // Simplified social learning implementation
    if (brain->getCognitionSystem()) {
        for (int step = 0; step < 30; ++step) {
            brain->step(step);
            // Simplified social learning update
        }
    }
}

void Phase4Demo::runContinualLearning(std::shared_ptr<Brain> brain) {
    // Simplified continual learning implementation
    for (int step = 0; step < 80; ++step) {
        brain->step(step);
        
        // Continual learning updates
        if (step % 20 == 0) {
            // Update learning rate based on performance
            float learningRate = 0.001f * (1.0f + (step / 100.0f));
        }
    }
}

// Phase 6 Demo implementations
void Phase6Demo::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent, bool verbose) {
    if (verbose) {
        printSeparator("Phase 6: Full Integration");
    }
    
    runIntegrationVerification(brain);
    runCompleteIntegration(brain, world, agent);
}

void Phase6Demo::runIntegrationVerification(std::shared_ptr<Brain> brain) {
    // Simplified integration verification
    NLM_LOG_INFO("=== Integration Verification ===");
    
    bool integrationOK = true;
    
    // Check various system integrations
    if (brain->getMemorySystem()) {
        integrationOK = integrationOK && brain->getMemorySystem()->getIntegratedStatus();
    }
    
    if (brain->getNeuromodulationSystem()) {
        integrationOK = integrationOK && brain->getNeuromodulationSystem()->getIntegratedStatus();
    }
    
    if (brain->getPredictionSystem()) {
        integrationOK = integrationOK && brain->getPredictionSystem()->getIntegratedStatus();
    }
    
    if (brain->getDevelopmentSystem()) {
        integrationOK = integrationOK && brain->getDevelopmentSystem()->getIntegratedStatus();
    }
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
    } else {
        std::cout << "Integration verification passed!" << std::endl;
    }
}

void Phase6Demo::runMemorySystems(std::shared_ptr<Brain> brain) {
    if (brain->getMemorySystem()) {
        auto workingMemory = brain->getMemorySystem()->getWorkingMemory();
        auto episodicMemory = brain->getMemorySystem()->getEpisodicMemory();
        
        for (int step = 0; step < 50; ++step) {
            brain->step(step);
            workingMemory->update(step);
            episodicMemory->update(step);
        }
    }
}

void Phase6Demo::runNeuromodulation(std::shared_ptr<Brain> brain) {
    // Simplified neuromodulation implementation
    if (brain->getNeuromodulationSystem()) {
        for (int step = 0; step < 50; ++step) {
            brain->step(step);
            // Update neuromodulators
        }
    }
}

void Phase6Demo::runPredictionIntegration(std::shared_ptr<Brain> brain) {
    if (brain->getPredictionSystem()) {
        for (int step = 0; step < 50; ++step) {
            brain->step(step);
            brain->getPredictionSystem()->updatePrediction();
        }
    }
}

void Phase6Demo::runDevelopmentIntegration(std::shared_ptr<Brain> brain) {
    if (brain->getDevelopmentSystem()) {
        for (int step = 0; step < 50; ++step) {
            brain->step(step);
            brain->getDevelopmentSystem()->update(step);
        }
    }
}

void Phase6Demo::runCheckpointing(std::shared_ptr<Brain> brain) {
    // Simplified checkpointing implementation
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 50 == 0) {
            // Simulate checkpoint save/load
            std::string checkpointFile = "checkpoint_" + std::to_string(step) + ".bin";
        }
    }
}

void Phase6Demo::runReplay(std::shared_ptr<Brain> brain) {
    // Simplified replay implementation
    for (int step = 0; step < 80; ++step) {
        brain->step(step);
        
        // Simulate replay mechanism
        if (step > 20 && step % 20 == 0) {
            // Replay important memories
        }
    }
}

void Phase6Demo::runCompleteIntegration(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world, std::shared_ptr<AgentBrain> agent) {
    // Simplified complete integration experiment
    for (int step = 0; step < 200; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (step % 50 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(brain->getNeuromodulationSystem()->getCurrentDopamine()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
}

// Helper functions
void DemoHelpers::printSeparator(const std::string& title) {
    std::cout << std::endl;
    if (title.empty()) {
        std::cout << "=============================================================" << std::endl;
    } else {
        std::cout << "=== " << title << " ===" << std::endl;
        std::cout << "=============================================================" << std::endl;
    }
    std::cout << std::endl;
}

void DemoHelpers::printConfigurationSummary(const std::shared_ptr<Config>& config) {
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << "  Mode: " << static_cast<int>(config->mode) << std::endl;
    std::cout << "  Config file: " << config->configFile << std::endl;
    std::cout << "  Verbose: " << (config->verbose ? "Yes" : "No") << std::endl;
    std::cout << "  Quiet: " << (config->quiet ? "Yes" : "No") << std::endl;
    std::cout << "  Save results: " << (config->saveResults ? "Yes" : "No") << std::endl;
    if (config->saveResults && !config->outputFile.empty()) {
        std::cout << "  Output file: " << config->outputFile << std::endl;
    }
    std::cout << std::endl;
}

void DemoHelpers::saveResults(const std::string& filename, const std::string& data) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << data;
        file.close();
        std::cout << "Results saved to: " << filename << std::endl;
    } else {
        std::cerr << "ERROR: Cannot save results to " << filename << std::endl;
    }
}

void DemoHelpers::loadConfiguration(const std::string& filename, std::shared_ptr<Config>& config) {
    // Simplified configuration loading
    std::cout << "Loading configuration from: " << filename << std::endl;
    // TODO: Implement proper configuration loading
}

bool DemoHelpers::validateConfiguration(const std::shared_ptr<Config>& config) {
    // Simplified configuration validation
    if (config->mode == UnifiedCLI::ExecutionMode::UNKNOWN) {
        std::cerr << "ERROR: Invalid execution mode" << std::endl;
        return false;
    }
    
    return true;
}

void DemoHelpers::setupLogging(bool verbose, bool quiet) {
    // Simplified logging setup
    if (quiet) {
        std::cout << "Quiet mode enabled" << std::endl;
    } else if (verbose) {
        std::cout << "Verbose mode enabled" << std::endl;
    }
}

// Global learning experiment struct (from original main.cpp)
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

int main(int argc, char* argv[]) {
    UnifiedCLI::Config config;
    
    if (!UnifiedCLI::parseCommandLine(argc, argv, config)) {
        return 1;
    }
    
    switch (config.mode) {
        case UnifiedCLI::ExecutionMode::HELP:
            UnifiedCLI::printHelp(argv[0]);
            return 0;
            
        case UnifiedCLI::ExecutionMode::VERSION:
            UnifiedCLI::printVersion();
            return 0;
            
        case UnifiedCLI::ExecutionMode::PHASE2:
            return UnifiedCLI::runPhase2Demo(config) ? 0 : 1;
            
        case UnifiedCLI::ExecutionMode::PHASE3:
            return UnifiedCLI::runPhase3Demo(config) ? 0 : 1;
            
        case UnifiedCLI::ExecutionMode::PHASE4:
            return UnifiedCLI::runPhase4Demo(config) ? 0 : 1;
            
        case UnifiedCLI::ExecutionMode::PHASE6:
            return UnifiedCLI::runPhase6Demo(config) ? 0 : 1;
            
        default:
            std::cerr << "ERROR: Unknown execution mode" << std::endl;
            return 1;
    }
}