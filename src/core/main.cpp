#include "main.hpp"
#include "core/CLI/CLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase6Demo.hpp"
#include <iostream>
#include <memory>
#include <unordered_map>

using namespace nlm;

/**
 * NLM Phase 2 Demo Application
 */
class Phase2DemoApp : public NLMApplication {
public:
    Phase2DemoApp() : NLMApplication("nlm_phase2_demo", "0.1.0") {}
    
    int run(int argc, char** argv) override {
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("=== NLM Phase 2 Demo Application ===");
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Set default values
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", 42, ConfigSource::Default);
        
        // Parse config file from CLI
        std::string configFile = "configs/default.cfg";
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            }
        }
        
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Initialize brain
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run basic tests
        NLM_LOG_INFO("Running basic connectivity test...");
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("Running plasticity learning experiment...");
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("Running STDP verification...");
        runStdpVerification(brain);
        
        NLM_LOG_INFO("=== Phase 2 Demo Complete ===");
        return 0;
    }
    
protected:
    void registerCommands() override {
        // No subcommands for phase 2 demo
    }
    
    void registerOptions() override {
        // Add options
        getCLI().addOption("config", "Configuration file path", 'c', false);
        getCLI().addOption("neurons", "Number of neurons (overrides config)", 'n', false);
        getCLI().addOption("steps", "Maximum simulation steps", 's', false);
    }
    
private:
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
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
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        }
    }
    
    void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        NLM_LOG_INFO("  Plasticity experiment completed");
    }
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
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
        
        NLM_LOG_INFO("  After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
        }
        
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }
};

/**
 * Unified NLM Application with subcommands
 */
class NLMApp : public NLMApplication {
public:
    NLMApp() : NLMApplication("nlm", "0.1.0") {}
    
    int run(int argc, char** argv) override {
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Parse arguments
        if (!getCLI().parse(argc, argv)) {
            return 1;
        }
        
        // Execute the appropriate command
        std::string command = getCLI().getCommand();
        
        if (command == "phase2") {
            return runPhase2Demo();
        } else if (command == "phase3") {
            return runPhase3Demo();
        } else if (command == "phase4") {
            return runPhase4Demo();
        } else if (command == "phase6") {
            return runPhase6Demo();
        } else if (command == "help" || command.empty()) {
            getCLI().printHelp();
            return 0;
        } else {
            std::cerr << "Error: Unknown command: " << command << std::endl;
            getCLI().printHelp();
            return 1;
        }
    }
    
protected:
    void registerCommands() override {
        getCLI().addCommand("phase2", "Run Phase 2 Real Neural Computation demo",
                            [this](const CLI& cli) { runPhase2Demo(); });
        getCLI().addCommand("phase3", "Run Phase 3 World Interaction demo",
                            [this](const CLI& cli) { runPhase3Demo(); });
        getCLI().addCommand("phase4", "Run Phase 4 Emerging Cognition demo",
                            [this](const CLI& cli) { runPhase4Demo(); });
        getCLI().addCommand("phase6", "Run Phase 6 Integration demo",
                            [this](const CLI& cli) { runPhase6Demo(); });
        getCLI().addCommand("help", "Show help message", [](const CLI& cli) {
            cli.printHelp();
        });
    }
    
    void registerOptions() override {
        getCLI().addOption("config", "Configuration file path", 'c', false);
        getCLI().addOption("verbose", "Verbose output", 'v', false);
        getCLI().addOption("quiet", "Quiet output", 'q', false);
        getCLI().addOption("help", "Show help message", 'h', false);
        getCLI().addOption("version", "Show version information", 0, false);
    }
    
private:
    int runPhase2Demo() {
        NLM_LOG_INFO("=== Running Phase 2 Demo ===");
        Phase2DemoApp app;
        return app.run(1, nullptr); // Simplified for demo
    }
    
    int runPhase3Demo() {
        NLM_LOG_INFO("=== Running Phase 3 Demo ===");
        Phase3Demo demo;
        int numEpisodes = 5;
        int stepsPerEpisode = 200;
        
        // Parse CLI options
        if (getCLI().hasOption("neurons")) {
            numEpisodes = std::stoi(getCLI().getOption("neurons"));
        }
        if (getCLI().hasOption("steps")) {
            stepsPerEpisode = std::stoi(getCLI().getOption("steps"));
        }
        
        demo.run(numEpisodes, stepsPerEpisode);
        return 0;
    }
    
    int runPhase4Demo() {
        NLM_LOG_INFO("=== Running Phase 4 Demo ===");
        Phase4Demo demo;
        size_t numTrials = 50;
        
        if (getCLI().hasOption("trials")) {
            numTrials = std::stoi(getCLI().getOption("trials"));
        }
        
        demo.run(numTrials);
        return 0;
    }
    
    int runPhase6Demo() {
        NLM_LOG_INFO("=== Running Phase 6 Integration Demo ===");
        Phase6Demo demo;
        demo.run();
        return 0;
    }
};

int main(int argc, char** argv) {
    NLMApp app;
    return app.run(argc, argv);
}
