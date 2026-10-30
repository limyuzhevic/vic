// Phase 7 Demo - Advanced Visualization and Monitoring
//
// This demo demonstrates the advanced visualization and monitoring capabilities
// added in Phase 7, including real-time console visualization, performance monitoring,
// and enhanced user experience.

#include "experiments/Phase7Demo.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "visualization/NLMConsoleVisualization.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>

using namespace nlm;

// Global flag for graceful shutdown
std::atomic<bool> g_running{true};

void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << ", shutting down gracefully..." << std::endl;
    g_running = false;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 7 Advanced Demo ===" << std::endl;
    std::cout << "Real-time visualization, monitoring, and enhanced user experience" << std::endl;
    std::endl;
    
    // Setup signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    Phase7Config config;
    config.showHelp = false;
    config.exportData = false;
    config.numSteps = 1000;
    config.updateInterval = 0.1;
    config.width = 80;
    config.height = 40;
    config.enableColors = true;
    config.saveFrames = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            config.showHelp = true;
        } else if (arg == "--steps" && i + 1 < argc) {
            config.numSteps = std::stoi(argv[++i]);
        } else if (arg == "--width" && i + 1 < argc) {
            config.width = std::stoi(argv[++i]);
        } else if (arg == "--height" && i + 1 < argc) {
            config.height = std::stoi(argv[++i]);
        } else if (arg == "--export" && i + 1 < argc) {
            config.exportPath = argv[++i];
            config.exportData = true;
        } else if (arg == "--save-frames") {
            config.saveFrames = true;
        } else if (arg == "--no-colors") {
            config.enableColors = false;
        } else if (arg == "--quiet" || arg == "-q") {
            consoleLogger->setLevel(LogLevel::Warning);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }
    
    if (config.showHelp) {
        printHelp();
        return 0;
    }
    
    // Create experiment
    Phase7Demo demo;
    
    // Create brain and agent
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config.neuronCount);
    cfg->set("region_count", config.regionCount);
    cfg->set("connection_probability", config.connectionProbability);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create world and agent
    auto world = std::make_shared<SimpleWorld>();
    world->configure(config.width, config.height, 8, 8);
    world->reset();
    world->setAgentStart(config.width * 0.5f, config.height * 0.5f);
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable advanced features
    agent->enableRewardModulation(config.enableRewardModulation);
    agent->enableStructuralPlasticity(config.enableStructuralPlasticity);
    agent->enableDevelopment(config.enableDevelopment);
    agent->enableCuriosity(config.enableCuriosity);
    
    // Create visualization
    auto visualization = std::make_shared<ConsoleVisualization>();
    if (!visualization->initialize(config.width, config.height)) {
        std::cerr << "Failed to initialize visualization!" << std::endl;
        return 1;
    }
    
    visualization->setColors(config.enableColors);
    visualization->showNetwork(true);
    visualization->showActivity(true);
    visualization->showSpikes(true);
    visualization->showMetrics(true);
    
    std::cout << "=== Phase 7 Demo Initialized ===" << std::endl;
    std::cout << "Brain: " << brain->getTotalNeuronCount() << " neurons, " 
              << brain->getTotalSynapseCount() << " synapses" << std::endl;
    std::cout << "World: " << config.width << " x " << config.height << " cells" << std::endl;
    std::cout << "Visualization: " << (config.enableColors ? "Colors enabled" : "Colors disabled") << std::endl;
    std::cout << "Export: " << (config.exportData ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
    // Run demo
    demo.run(config, brain, world, agent, visualization);
    
    // Export data if requested
    if (config.exportData) {
        std::cout << "\n=== Exporting data to: " << config.exportPath << " ===" << std::endl;
        if (visualization->saveFrame(config.exportPath + "/frame.png")) {
            std::cout << "Frame saved successfully!" << std::endl;
        }
        std::cout << "Demo data exported to: " << config.exportPath << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== Phase 7 Demo Complete ===" << std::endl;
    
    return 0;
}

void printHelp() {
    std::cout << "=== NLM Phase 7 Demo Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_phase7_demo [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --steps N               Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --width W               Console width in characters (default: 80)" << std::endl;
    std::cout << "  --height H              Console height in characters (default: 40)" << std::endl;
    std::cout << "  --export DIR            Export data to directory" << std::endl;
    std::cout << "  --save-frames           Save visualization frames" << std::endl;
    std::cout << "  --no-colors             Disable color output" << std::endl;
    std::cout << "  --quiet, -q             Suppress informational messages" << std::endl;
    std::cout << std::endl;
    std::cout << "Features demonstrated:" << std::endl;
    std::cout << "  - Real-time console visualization" << std::endl;
    std::cout << "  - Performance monitoring and statistics" << std::endl;
    std::cout << "  - Network visualization" << std::endl;
    std::cout << "  - Activity monitoring" << std::endl;
    std::cout << "  - Spike visualization" << std::endl;
    std::cout << "  - Export capabilities" << std::endl;
    std::cout << "  - Graceful shutdown handling" << std::endl;
    std::cout << std::endl;
    std::cout << "Press 'h' for help, 'q' to quit, 'p' to pause during demo." << std::endl;
}