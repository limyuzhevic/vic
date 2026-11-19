// NLM (熙然) - Neural Learning Machine
// Simple Demo - Beginner-Friendly Version
//
// This is the easiest way to get started with NLM. It creates a brain,
// runs it once, and shows basic statistics. Perfect for beginners!

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"

#include <iostream>
#include <iomanip>

using namespace nlm;

void printSimpleBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Simple Demo - Getting Started                            ║
    ║                                                               ║
    ║     The easiest way to try NLM!                              ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char** argv) {
    printSimpleBanner();
    
    std::cout << "Initializing NLM Simple Brain Demo...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Simple Demo - Beginner Edition ===");
    std::cout << "Creating a simple brain with 500 neurons..." << std::endl;
    
    // Create brain with default configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Configure for simple demo - smaller brain for faster startup
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Simple demo size
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run a simple demonstration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simple Brain Demonstration ===");
    std::cout << "Running brain for 100 simulation steps..." << std::endl;
    
    // Store initial state
    size_t initialNeurons = brain->getTotalNeuronCount();
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Run simulation
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        
        // Show progress every 25 steps
        if (step % 25 == 0) {
            std::cout << "  Step " << step << ": "
                     << brain->getFiringNeuronCount() << " neurons firing, "
                     << (brain->getTotalSpikeCount() - initialSpikes) << " total spikes" << std::endl;
        }
    }
    
    size_t finalNeurons = brain->getTotalNeuronCount();
    size_t finalSpikes = brain->getTotalSpikeCount();
    
    // Show final statistics
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Demo Results ===");
    std::cout << "Brain Demo Complete!" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Summary Statistics:" << std::endl;
    std::cout << "  • Total neurons created: " << initialNeurons << std::endl;
    std::cout << "  • Total neurons active: " << finalNeurons << std::endl;
    std::cout << "  • Total spikes generated: " << finalSpikes << std::endl;
    std::cout << "  • Average firing rate: " << std::fixed << std::setprecision(2)
              << (finalSpikes / (finalNeurons ? finalNeurons : 1)) << " spikes/neuron" << std::endl;
    std::cout << "  • Simulation steps: 100" << std::endl;
    
    // Show development progress
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Development Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
    std::cout << "Development stage: " << brain->getDevelopmentalStage() << std::endl;
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simple Demo Complete ===");
    std::cout << "" << std::endl;
    std::cout << "Congratulations! Your brain has successfully "
              << finalSpikes << " simulated steps of thinking!" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "What you just experienced:" << std::endl;
    std::cout << "  ✓ Created a virtual brain with 500 neurons" << std::endl;
    std::cout << "  ✓ Ran 100 steps of neural simulation" << std::endl;
    std::cout << "  ✓ Generated " << finalSpikes << " neural spikes" << std::endl;
    std::cout << "  ✓ Demonstrated brain activity" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Next steps to explore:" << std::endl;
    std::cout << "  • Try ./nlm_phase3_demo for world interaction" << std::endl;
    std::cout << "  • Try ./nlm_phase4_demo for memory and prediction" << std::endl;
    std::cout << "  • Try ./nlm_phase6_demo for full integration" << std::endl;
    std::cout << "  • Use 'pip install .' to try the Python API" << std::endl;
    std::cout << "" << std::endl;
    
    return 0;
}
