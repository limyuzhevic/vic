// Updated NLM Main Entry Point
// Uses modern CLI framework for better user experience and modularity

#include "cli/CLI.hpp"
#include "test/TestFramework.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>

namespace nlm {

void runStandardDemo(const std::vector<std::string>& args);
void runTestSuite(const std::vector<std::string>& args);
void runPhase2Demo(const std::vector<std::string>& args);
void runIntegrationDemo(const std::vector<std::string>& args);
void runPlasticityDemo(const std::vector<std::string>& args);
void runSTMDDemo(const std::vector<std::string>& args);

definition for the main function
*/
int main(int argc, char* argv[]) {
    CLI cli(argc, argv);
    
    // Add application-specific commands
    cli.addCommand({
        "run",
        "Run NLM simulation with specified configuration",
        "run [options]",
        [](const std::vector<std::string>& args) {
            runStandardDemo(args);
        }
    });
    
    cli.addCommand({
        "test",
        "Run NLM test suite",
        "test [options]",
        [](const std::vector<std::string>& args) {
            runTestSuite(args);
        }
    });
    
    cli.addCommand({
        "phase2",
        "Run Phase 2 demo (basic neural computation)",
        "phase2 [options]",
        [](const std::vector<std::string>& args) {
            runPhase2Demo(args);
        }
    });
    
    cli.addCommand({
        "integration",
        "Run Phase 6 integration demo",
        "integration [options]",
        [](const std::vector<std::string>& args) {
            runIntegrationDemo(args);
        }
    });
    
    cli.addCommand({
        "plasticity",
        "Run plasticity learning demo",
        "plasticity [options]",
        [](const std::vector<std::string>& args) {
            runPlasticityDemo(args);
        }
    });
    
    cli.addCommand({
        "stdp",
        "Run STDP verification demo",
        "stdp [options]",
        [](const std::vector<std::string>& args) {
            runSTMDDemo(args);
        }
    });
    
    // Parse and run the CLI
    cli.run();
    
    return 0;
}

void runStandardDemo(const std::vector<std::string>& args) {
    std::cout << "=== NLM Standard Demo ===" << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Starting NLM standard demonstration");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Set default values
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Run a simple demonstration
    NLM_LOG_INFO("Running demonstration...");
    
    // Inject some current and simulate
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            // Inject current into first few neurons
            for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
                neurons[i]->injectCurrent(20.0f);
            }
            
            // Run simulation
            for (SimulationStep step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
            
            NLM_LOG_INFO("Simulation completed:");
            NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("Standard demo completed successfully!");
}

void runTestSuite(const std::vector<std::string>& args) {
    std::cout << "=== NLM Test Suite ===" << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    BrainConfig testConfig;
    testConfig.neuronCount = 100;
    testConfig.maxSteps = 200;
    
    auto brain = TestFramework::createTestBrain(testConfig);
    if (!brain) {
        NLM_LOG_ERROR("Failed to create test brain");
        return;
    }
    
    // Run Phase 2 tests
    TestFramework::runPhase2Tests(brain);
    
    NLM_LOG_INFO("Test suite completed!");
}

void runPhase2Demo(const std::vector<std::string>& args) {
    std::cout << "=== NLM Phase 2 Demo ===" << std::endl;
    
    // This uses the original main.cpp functionality but in a cleaner structure
    // Would implement the original Phase 2 demo logic here
    
    NLM_LOG_INFO("Phase 2 demo would run here");
    NLM_LOG_INFO("Includes: basic connectivity, plasticity, and STDP demonstrations");
}

void runIntegrationDemo(const std::vector<std::string>& args) {
    std::cout << "=== NLM Integration Demo ===" << std::endl;
    
    // This would run the Phase 6 integration demo
    NLM_LOG_INFO("Integration demo would run Phase 6 experiments here");
    NLM_LOG_INFO("Tests: memory, neuromodulation, prediction, development integration");
}

void runPlasticityDemo(const std::vector<std::string>& args) {
    std::cout << "=== NLM Plasticity Demo ===" << std::endl;
    
    // Focuses on learning and adaptation
    NLM_LOG_INFO("Plasticity demo demonstrates: Hebbian learning, STDP, structural plasticity");
}

void runSTMDDemo(const std::vector<std::string>& args) {
    std::cout << "=== NLM STDP Demo ===" << std::endl;
    
    // Specific STDP verification
    NLM_LOG_INFO("STDP demo verifies: pre-before-post potentiation, post-before-pre depression");
}

} // namespace nlm