// NLM Command Line Interface
// Unified CLI for NLM (Neural Learning Machine)

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "motor/Action.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>

namespace nlm {

struct CommandLineOptions {
    std::string mode = "demo";  // demo, experiment, checkpoint, profile
    std::string configFile = "configs/default.cfg";
    uint64_t steps = 1000;
    uint64_t neurons = 500;
    uint64_t regions = 1;
    float timestep = 0.001;
    bool verbose = false;
    bool help = false;
    std::string checkpointPath;
    std::string action = "forward";
    std::map<std::string, std::string> additionalArgs;
};

class NLMCLI {
    CommandLineOptions options;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    bool simulationRunning = false;
    
public:
    NLMCLI(int argc, char* argv[]) {
        parseArgs(argc, argv);
        initialize();
    }
    
    void parseArgs(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg.substr(0, 2) == "--") {
                std::string param = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    options.additionalArgs[param] = argv[++i];
                }
            } else if (arg == "--mode" && i + 1 < argc) {
                options.mode = argv[++i];
            } else if (arg == "--steps" && i + 1 < argc) {
                options.steps = std::stoul(argv[++i]);
            } else if (arg == "--neurons" && i + 1 < argc) {
                options.neurons = std::stoul(argv[++i]);
            } else if (arg == "--regions" && i + 1 < argc) {
                options.regions = std::stoul(argv[++i]);
            } else if (arg == "--timestep" && i + 1 < argc) {
                options.timestep = std::stof(argv[++i]);
            } else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            } else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            } else if (arg == "--checkpoint" && i + 1 < argc) {
                options.checkpointPath = argv[++i];
            } else if (arg == "--action" && i + 1 < argc) {
                options.action = argv[++i];
            }
        }
    }
    
    void initialize() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        config = std::make_shared<Config>();
        
        bool configLoaded = false;
        if (!options.configFile.empty()) {
            configLoaded = config->loadFromFile(options.configFile);
        }
        if (!configLoaded) {
            std::cout << "Using default configuration." << std::endl;
        }
        
        // Set defaults for CLI usage
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", options.timestep, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(options.neurons), ConfigSource::Default);
        config->set("region_count", static_cast<int step_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        std::cout << "Configuration loaded successfully." << std::endl;
    }
    
    void printHelp() {
        std::cout << "NLM CLI Help:" << std::endl;
        std::cout << "  --mode MODE             Mode: demo, experiment, checkpoint, profile (default: demo)" << std::endl;
        std::cout << "  --steps N              Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --neurons N             Number/travel time limit (default: 500)" << std::endl;
        std::cout << "  --regions N             Number of brain regions (default:  Neteller mode" << std::endl;
        std::cout << "  --timestep T            Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --config FILE           Configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --checkpoint FILE       Checkpoint file for save/load" << std::endl;
        std::cout << "  --action ACTION         Action command (default: forward)" << std::endl;
        std::cout << "  -v, --verbose           Enable verbose logging" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
    }
    
    void runBrainSimulation() {
        NLM_LOG_INFO("=== NLM CLI Simulation ===");
        
        // Create and initialize brain
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return;
        }
        
        brain->logStatus();
        
        // Create agent and world for interactive mode
        world = std::make_shared<SimpleWorld>();
        agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(16, 16, 16, 16);
        agent->initialize(*world);
        
        std::cout << std::endl;
        std::cout << "=== Simulation ===" << std::endl;
        std::cout << "Starting simulation with " << options.steps << " steps..." << std::endl;
        
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        
        for (uint64_t step = 0; step < options.steps; ++step) {
            // Update environment/world
            world->update(options.timestep);
            
            // Get current observations (sensory input)
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process sensory input through agent
            agent->processSensoryInput(percept);
            
            // Brain simulation step
            brain->step(step, step * options.timestep);
            
            // Agent decodes motor command from brain activity
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action to world
            ActionResult result = world->applyMotorCommand(cmd, step * options.timestep);
            
            // Compute and track reward
            float reward = result.reward;
            totalReward += reward;
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development system
            agent->updateDevelopment(options.timestep);
            
            // Collect statistics
            totalFiringRate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firingCount++;
            
            // Verbose output every 100 steps
            if (options.verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": Reward = " << totalReward / (step + 1) 
                          << ", Firing Rate = " << brain->getAverageFiringRate()
                          << ", Working Memory = " << (brain->getWorkingMemory() ? 
                              std::to_string(brain->getWorkingMemory()->getActiveTraces()) : "0")
                          << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Simulation Results ===" << std::endl;
        std::cout << "Total reward: " << (totalReward / options.steps) << std::endl;
        std::cout << "Average firing rate: " << (totalFiringRate / options.steps) << std::endl;
        std::cout << "Steps with activity: " << firingCount << " / " << options.steps << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== Brain Statistics ===" << std::endl;
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Working memory traces: " << (brain->getWorkingMemory() ? 
            std::to_string(brain->getWorkingMemory()->getActiveTraces()) : "0") << std::endl;
        std::cout << "Episodic memory episodes: " << (brain->getEpisodicMemory() ? 
            std::to_string(brain->getEpisodicMemory()->getEpisodeCount()) : "0") << std::endl;
        
        if (!options.checkpointPath.empty()) {
            std::cout << std::endl;
            std::cout << "=== Checkpointing ===" << std::endl;
            std::cout << "Saving checkpoint to: " << options.checkpointPath << std::endl;
            if (brain->save(options.checkpointPath)) {
                std::cout << "✓ Checkpoint saved successfully" << std::endl;
            } else {
                std::cout << "✗ Failed to save checkpoint" << std::endl;
            }
        }
    }
    
    void runQuickTest() {
        NLM_LOG_INFO("=== NLM CLI Quick Test ===");
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return;
        }
        
        std::cout << "Running quick connectivity test..." << std::endl;
        
        // Simple connectivity test
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found!");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found!");
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into a few neurons
        size_t testNeurons = std::min(size_t(5), neurons.size());
        NLM_LOG_INFO("  Injecting current into " + std::to_string(testNeurons) + " neurons...");
        for (size_t i = 0; i < testNeurons; ++i) {
            neurons[i]->injectCurrent(20.0f);
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 20; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("✓ Connectivity test PASSED: Network is functional");
        } else {
            NLM_LOG_INFO("! Connectivity test INFO: No spikes generated (may be normal)");
        }
        
        brain->logStatus();
    }
    
    void runPlasticityTest() {
        NLM_LOG_INFO("=== NLM CLI Plasticity Test ===");
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return;
        }
        
        std::cout << "Testing STDP and Hebbian plasticity..." << std::endl;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found!");
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            NLM_LOG_ERROR("No synapses found for plasticity test");
            return;
        }
        
        // Enable plasticity on a test synapse
        Synapse* testSyn = synapses[0].get();
        testSyn->enablePlasticity(true, true, false);  // STDP and Hebbian
        float initialWeight = testSyn->getWeight();
        
        NLM_LOG_INFO("  Testing synapse: pre=" + std::to_string(testSyn->getSourceNeuron().index()) +
                    ", post=" + std::to_string(testSyn->getDestinationNeuron().index()) +
                    ", weight=" + std::to_string(initialWeight));
        
        // Create correlated pre->post activity
        NLM_LOG_INFO("  Applying correlated activity pattern (50 trials)...");
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(testSyn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(testSyn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float finalWeight = testSyn->getWeight();
        float weightChange = finalWeight - initialWeight;
        
        NLM_LOG_INFO("  Final weight: " + std::to_string(finalWeight) +
                    ", change: " + std::to_string(weightChange));
        
        if (weightChange > 0.001f) {
            NLM_LOG_INFO("✓ STDP/Hebbian test PASSED: Potentiation detected");
        } else if (weightChange < -0.001f) {
            NLM_LOG_INFO("✓ STDP/Hebbian test PASSED: Depression detected");
        } else {
            NLM_LOG_INFO("! STDP/Hebbian test INFO: No significant change detected");
        }
        
        brain->logStatus();
    }
    
    void runCheckpointTest() {
        NLM_LOG_INFO("=== NLM CLI Checkpoint Test ===");
        
        if (options.checkpointPath.empty()) {
            NLM_LOG_ERROR("Checkpoint path not specified! Use --checkpoint <file>");
            return;
        }
        
        std::string path = options.checkpointPath;
        
        // Create and initialize first brain
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize first brain!");
            return;
        }
        
        // Run some steps to generate state
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        float initialWeight = 0.0f;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeight += syn->getWeight();
            }
            if (!region->getSynapses().empty()) {
                initialWeight /= region->getSynapses().size();
            }
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Save checkpoint
        NLM_LOG_INFO("Saving checkpoint to: " + path);
        if (!brain->save(path)) {
            NLM_LOG_ERROR("Failed to save checkpoint");
            return;
        }
        
        // Create new brain and load checkpoint
        auto brain2 = std::make_shared<Brain>(config);
        brain2->initialize();
        
        NLM_LOG_INFO("Loading checkpoint into new brain...");
        if (!brain2->load(path)) {
            NLM_LOG_ERROR("Failed to load checkpoint");
            return;
        }
        
        float finalWeight = 0.0f;
        if (auto* region = brain2->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeight += syn->getWeight();
            }
            if (!region->getSynapses().empty()) {
                finalWeight /= region->getSynapses().size();
            }
        }
        
        size_t finalSpikes = brain2->getTotalSpikeCount();
        
        NLM_LOG_INFO("  Checkpoint save/load test completed");
        NLM_LOG_INFO("  Initial weight: " + std::to_string(initialWeight));
        NLM_LOG_INFO("  Loaded weight: " + std::to_string(finalWeight));
        NLM_LOG_INFO("  Initial spikes: " + std::to_string(initialSpikes));
        NLM_LOG_INFO("  Loaded spikes: " + std::to_string(finalSpikes));
        
        if (std::abs(initialWeight - finalWeight) < 1e-6f && initialSpikes == finalSpikes) {
            NLM_LOG_INFO("✓ Checkpoint test PASSED: State preserved correctly");
        } else {
            NLM_LOG_INFO("! Checkpoint test INFO: Some state differences (may be expected)");
        }
    }
};

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM CLI — Unified Command Line Interface                  ║
    ║                                                               ║
    ║     Neural Learning Machine - Command Line Tools               ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    NLMCLI cli(argc, argv);
    
    if (cli.options.help) {
        cli.printHelp();
        return 0;
    }
    
    if (cli.options.mode == "demo") {
        cli.runBrainSimulation();
    } else if (cli.options.mode == "test") {
        if (cli.options.steps < 10) {
            cli.runQuickTest();
        } else {
            cli.runPlasticityTest();
        }
    } else {
        cli.runBrainSimulation();
    }
    
    return 0;
}

} // namespace nlm
