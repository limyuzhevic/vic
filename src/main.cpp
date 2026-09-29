// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "src/core/Config/Config.hpp"
#include "src/core/Random/Random.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"
#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/environment/Environment.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include "test_improvements.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <stdexcept>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char** argv) {
    try {
        printBanner();
        
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
        NLM_LOG_INFO("Implementing:");
        NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
        NLM_LOG_INFO("  - Event-driven spike propagation with delays");
        NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
        NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
        NLM_LOG_INFO("");
        
        // Parse command line arguments
        std::vector<std::string> args;
        std::string configFile = "configs/default.cfg";
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else {
                args.push_back(arg);
            }
        }
        
        // Create configuration with improved loading
        auto config = std::make_shared<Config>();
        
        if (!config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Using default configuration.");
        } else {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        }
        
        // Apply command line arguments
        if (!args.empty()) {
            config->loadFromArgs(args.size(), const_cast<char**>(args.data()));
        }
        
        // Apply default values for Phase 2
        config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config.set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config.set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config.set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Log configuration summary
        nlm::test::logConfigurationSummary(*config);
        
        // Initialize simulation clock
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        // Initialize brain
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!nlm::test::validateBrainInitialization(brain)) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run all improved tests
        int result = nlm::test::runAllTestsImproved(brain);
        
        if (result == 0) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== All Tests Completed Successfully ===");
        } else {
            NLM_LOG_ERROR("");
            NLM_LOG_ERROR("=== Tests Completed with Errors ===");
        }
        
        return result;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Fatal exception: ") + e.what());
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown fatal exception occurred.");
        return 1;
    }
}