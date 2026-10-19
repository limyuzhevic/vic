#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"

using namespace nlm;

class NLMCommandLineInterface {
public:
    struct ConfigOptions {
        std::string mode = "auto"; // "auto", "phase3", "phase4", "phase5", "phase6", "demo"
        int trials = 10;
        int steps = 200;
        int neurons = 500;
        int regions = 2;
        float connectionProb = 0.1f;
        uint64_t randomSeed = 42u;
        float timestep = 0.001f;
        bool enableLogging = true;
        bool enableCheckpointing = true;
        bool enableReplay = true;
        bool enableDevelopment = true;
        std::string logLevel = "info";
        std::string configFile = "";
    };

    NLMCommandLineInterface() = default;
    ~NLMCommandLineInterface() = default;

    bool parseArguments(int argc, char* argv[], ConfigOptions& opts) {
        std::map<std::string, std::string> args;
        for (int i = 1; i < argc; i += 2) {
            if (i + 1 < argc && argv[i][0] == '-') {
                args[std::string(argv[i])] = std::string(argv[i + 1]);
            } else if (argv[i][0] == '-') {
                args[std::string(argv[i])] = "1";
            }
        }

        // Parse mode
        if (args.count("--mode")) opts.mode = args["--mode"];
        if (args.count("-m")) opts.mode = args["-m"];

        // Parse numeric values
        if (args.count("--trials")) opts.trials = std::stoi(args["--trials"]);
        if (args.count("-t")) opts.trials = std::stoi(args["-t"]);

        if (args.count("--steps")) opts.steps = std::stoi(args["--steps"]);
        if (args.count("--episodes")) opts.steps = std::stoi(args["--episodes"]);
        if (args.count("-s")) opts.steps = std::stoi(args["-s"]);

        if (args.count("--neurons")) opts.neurons = std::stoi(args["--neurons"]);
        if (args.count("--region")) opts.regions = std::stoi(args["--region"]);

        if (args.count("--conn-prob")) opts.connectionProb = std::stof(args["--conn-prob"]);
        if (args.count("--random-seed")) opts.randomSeed = std::stoull(args["--random-seed"]);
        if (args.count("--timestep")) opts.timestep = std::stof(args["--timestep"]);

        // Parse flags
        if (args.count("--enable-logging") || args.count("-l")) opts.enableLogging = true;
        if (args.count("--disable-logging")) opts.enableLogging = false;

        if (args.count("--enable-checkpointing")) opts.enableCheckpointing = true;
        if (args.count("--disable-checkpointing")) opts.enableCheckpointing = false;

        if (args.count("--enable-replay")) opts.enableReplay = true;
        if (args.count("--disable-replay")) opts.enableReplay = false;

        if (args.count("--enable-development")) opts.enableDevelopment = true;
        if (args.count("--disable-development")) opts.enableDevelopment = false;

        // Parse logging level
        if (args.count("--log-level")) opts.logLevel = args["--log-level"];

        // Parse config file
        if (args.count("--config-file")) opts.configFile = args["--config-file"];

        // Show help
        if (args.count("--help") || args.count("-h")) {
            printHelp();
            return false;
        }

        return true;
    }

    void printHelp() {
        std::cout << "=== NLM Command Line Interface ===\n\n";
        std::cout << "Usage: nlm [OPTIONS]\n\n";
        std::cout << "Modes:\n";
        std::cout << "  --mode <auto|phase3|phase4|phase5|phase6|demo>  Run specific mode (default: auto)\n";
        std::cout << "  -m <mode>                                        Short mode selector\n\n";
        std::cout << "Configuration:\n";
        std::cout << "  --neurons <N>            Number of neurons (default: 500)\n";
        std::cout << "  --region <R>             Number of regions (default: 2)\n";
        std::cout << "  --conn-prob <P>          Connection probability (default: 0.1)\n";
        std::cout << "  --random-seed <S>        Random seed (default: 42)\n";
        std::cout << "  --timestep <T>           Simulation timestep (default: 0.001)\n\n";
        std::cout << "Experiment parameters:\n";
        std::cout << "  --trials <T>             Number of trials/episodes (default: 10)\n";
        std::cout << "  --steps <S>              Steps per episode (default: 200)\n";
        std::cout << "  --episodes <E>           Alias for --steps\n\n";
        std::cout << "Features:\n";
        std::cout << "  --enable-logging, -l      Enable logging\n";
        std::cout << "  --disable-logging         Disable logging\n";
        std::cout << "  --enable-checkpointing    Enable checkpointing (Phase 6+)\n";
        std::cout << "  --disable-checkpointing   Disable checkpointing\n";
        std::cout << "  --enable-replay           Enable memory replay\n";
        std::cout << "  --disable-replay          Disable replay\n";
        std::cout << "  --enable-development      Enable developmental processes\n";
        std::cout << "  --disable-development     Disable developmental processes\n\n";
        std::cout << "Logging:\n";
        std::cout << "  --log-level <L>           Logging level: debug, info, warn, error (default: info)\n\n";
        std::cout << "Advanced options:\n";
        std::cout << "  --config-file <F>         Configuration file path\n\n";
        std::cout << "Examples:\n";
        std::cout << "  nlm --mode phase3 --trials 5 --steps 100\n";
        std::cout << "  nlm -m phase4 -t 20 -s 500\n";
        std::cout << "  nlm --mode demo --enable-checkpointing --enable-replay\n";
        std::cout << "  nlm --help                                             Show this help\n";
    }

    void loadConfigFromFile(const std::string& filename, std::shared_ptr<Config> config) {
        if (filename.empty()) return;
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Warning: Could not open config file: " << filename << std::endl;
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            // Simple config parsing: key = value
            size_t pos = line.find('=');
            if (pos != std::string::npos && pos > 0) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                // Remove whitespace
                key.erase(0, key.find_first_not_of(" \t\n\r"));
                key.erase(key.find_last_not_of(" \t\n\r") + 1);
                value.erase(0, value.find_first_not_of(" \t\n\r"));
                value.erase(value.find_last_not_of(" \t\n\r") + 1);
                
                if (key == "neuron_count") config->set("neuron_count", std::stoull(value));
                else if (key == "region_count") config->set("region_count", std::stoull(value));
                else if (key == "connection_probability") config->set("connection_probability", std::stof(value));
                else if (key == "random_seed") config->set("random_seed", std::stoull(value));
                else if (key == "simulation_timestep") config->set("simulation_timestep", std::stof(value));
                else if (key == "stdp_ltp_weight") config->set("stdp_ltp_weight", std::stof(value));
                else if (key == "stdp_ltd_weight") config->set("stdp_ltd_weight", std::stof(value));
            }
        }
    }

    void printSystemInfo() {
        std::cout << "=== NLM System Information ===\n";
        std::cout << "Version: NLM Phase 6 Integration\n";
        std::cout << "Build Configuration: Debug\n";
        std::cout << "Memory: Optimized\n";
        std::cout << "Features:\n";
        std::cout << "  - Neural dynamics simulation (LIF neurons)\n";
        std::cout << "  - Multiple memory systems (working, episodic, associative)\n";
        std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)\n";
        std::cout << "  - Prediction and planning systems\n";
        std::cout << "  - Cognitive mechanisms (attention, concept formation)\n";
        std::cout << "  - Developmental plasticity\n";
        std::cout << "  - Checkpointing and memory replay\n\n";
    }

    void runMode(const ConfigOptions& opts) {
        auto config = std::make_shared<Config>();
        
        // Load from file if specified
        loadConfigFromFile(opts.configFile, config);
        
        // Override with command line values
        config->set("neuron_count", static_cast<size_t>(opts.neurons));
        config->set("region_count", static_cast<size_t>(opts.regions));
        config->set("connection_probability", opts.connectionProb);
        config->set("random_seed", opts.randomSeed);
        config->set("simulation_timestep", opts.timestep);
        config->set("enable_checkpointing", opts.enableCheckpointing);
        config->set("enable_replay", opts.enableReplay);
        config->set("enable_development", opts.enableDevelopment);
        
        // Set up logging
        if (opts.enableLogging) {
            Logger::getInstance().setLevel(stringToLogLevel(opts.logLevel));
            Logger::getInstance().setFile("nlm_output.log");
        } else {
            Logger::getInstance().setLevel(Logger::Level::None);
        }

        std::cout << "=== NLM Mode: " << opts.mode << " ===\n";
        
        if (opts.mode == "phase3" || opts.mode == "auto") {
            std::cout << "Running Phase 3 - World Interaction Demo\n\n";
            runPhase3Demo(config, opts);
        } else if (opts.mode == "phase4" || opts.mode == "auto") {
            std::cout << "Running Phase 4 - Emerging Cognition\n\n";
            runPhase4Demo(config, opts);
        } else if (opts.mode == "phase5" || opts.mode == "auto") {
            std::cout << "Running Phase 5 - Advanced Learning\n\n";
            runPhase5Demo(config, opts);
        } else if (opts.mode == "phase6" || opts.mode == "auto") {
            std::cout << "Running Phase 6 - Integration Demo\n\n";
            runPhase6Demo(config, opts);
        } else if (opts.mode == "demo") {
            std::cout << "Running Comprehensive Demo\n\n";
            runComprehensiveDemo(config, opts);
        } else {
            std::cerr << "Unknown mode: " << opts.mode << std::endl;
            std::cout << "Available modes: auto, phase3, phase4, phase5, phase6, demo\n";
            return;
        }
    }

private:
    Logger::Level stringToLogLevel(const std::string& level) {
        if (level == "debug") return Logger::Level::Debug;
        if (level == "info") return Logger::Level::Info;
        if (level == "warn") return Logger::Level::Warn;
        if (level == "error") return Logger::Level::Error;
        return Logger::Level::Info;
    }

    void runPhase3Demo(std::shared_ptr<Config> config, const ConfigOptions& opts) {
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(opts.randomSeed);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        std::cout << "Initial state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n\n";
        
        float totalReward = 0.0f;
        int totalSteps = opts.steps * opts.trials;
        
        for (int trial = 0; trial < opts.trials; ++trial) {
            std::cout << "Trial " << trial + 1 << "/" << opts.trials << ":\n";
            
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            for (int step = 0; step < opts.steps; ++step) {
                // Get percept and process
                const SensoryPercept& percept = world.getSensoryPercept();
                agentBrain.processSensoryInput(percept);
                
                // Simulate brain
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(neuralStep);
                }
                
                // Get action
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // Apply action
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                world.update(0.01);
                agentBrain.updateDevelopment(0.01);
                
                totalReward += result.reward;
                
                if (step % 50 == 0) {
                    std::cout << "    Step " << step 
                              << ": " << motorCommandToString(cmd)
                              << " reward: " << result.reward
                              << " energy: " << world.getAgentBody().energy << "\n";
                }
            }
            
            std::cout << "  Total reward: " << totalReward << "\n\n";
        }
        
        std::cout << "Phase 3 Summary:\n";
        std::cout << "  Total trials: " << opts.trials << "\n";
        std::cout << "  Steps per trial: " << opts.steps << "\n";
        std::cout << "  Total reward: " << totalReward << "\n";
        std::cout << "  Average reward per step: " << (totalSteps > 0 ? totalReward / totalSteps : 0) << "\n";
    }

    void runPhase4Demo(std::shared_ptr<Config> config, const ConfigOptions& opts) {
        std::cout << "Phase 4 demo would run here...\n";
        std::cout << "This mode requires integration with existing Phase4Demo.cpp\n\n";
        
        // For now, just print that Phase 4 is not implemented via CLI
        std::cout << "To run Phase 4 demo, use: ./nlm_phase4_demo [trials]\n";
    }

    void runPhase5Demo(std::shared_ptr<Config> config, const ConfigOptions& opts) {
        std::cout << "Phase 5 demo would run here...\n";
        std::cout << "This mode requires integration with existing Phase5Experiment\n\n";
        
        // For now, create a simple Phase5 experiment if we have the header
        if (std::ifstream("src/experiments/Phase5Experiment.hpp")) {
            std::cout << "Phase5Experiment header found. Ready for implementation.\n";
        }
    }

    void runPhase6Demo(std::shared_ptr<Config> config, const ConfigOptions& opts) {
        Phase6Config phase6Config;
        phase6Config.neuronCount = opts.neurons;
        phase6Config.maxSteps = opts.steps;
        phase6Config.enableCheckpointing = opts.enableCheckpointing;
        phase6Config.enableReplay = opts.enableReplay;
        phase6Config.enableDevelopment = opts.enableDevelopment;
        
        Phase6IntegratedExperiment experiment;
        
        std::cout << "--- Integration Verification ---\n";
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!\n";
            return;
        }
        
        std::cout << "Integration verification passed!\n\n";
        
        // Run quick tests
        std::cout << "--- Memory Integration Test ---\n";
        experiment.testMemoryIntegration();
        std::cout << "\n--- Neuromodulation Integration Test ---\n";
        experiment.testNeuromodulationIntegration();
        std::cout << "\n--- Checkpoint Test ---\n";
        experiment.testCheckpointing();
        std::cout << "\n--- Replay Test ---\n";
        experiment.testReplay();
        
        // Run full experiment
        std::cout << "\n--- Full Integration Experiment ---\n";
        auto result = experiment.run(phase6Config);
        
        std::cout << "\n=== FINAL RESULTS ===\n";
        std::cout << "Total reward: " << result.totalReward << "\n";
        std::cout << "Avg firing rate: " << result.avgFiringRate << "\n";
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << "\n";
        std::cout << "Dopamine level: " << result.dopamineLevel << "\n";
    }

    void runComprehensiveDemo(std::shared_ptr<Config> config, const ConfigOptions& opts) {
        std::cout << "=== Comprehensive NLM Demo ===\n\n";
        
        // Phase 3 demo
        std::cout << "=== Phase 3: World Interaction ===\n";
        runPhase3Demo(config, opts);
        std::cout << "\n=== Phase 4: Emerging Cognition ===\n";
        runPhase4Demo(config, opts);
        std::cout << "\n=== Phase 6: Integration Test ===\n";
        runPhase6Demo(config, opts);
        
        std::cout << "\n=== Comprehensive Demo Complete ===\n";
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
};

int main(int argc, char* argv[]) {
    NLMCommandLineInterface nlm;
    
    NLMCommandLineInterface::ConfigOptions opts;
    
    if (!nlm.parseArguments(argc, argv, opts)) {
        return 0; // Help was printed
    }
    
    nlm.printSystemInfo();
    nlm.runMode(opts);
    
    std::cout << "\nNLM execution completed successfully!\n";
    
    return 0;
}
