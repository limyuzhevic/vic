#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "CLIParser.hpp"

int main(int argc, char** argv) {
    // Parse command line arguments using our enhanced CLI parser
    nlm::CLIParser parser(argc, argv);
    
    if (!parser.isValid()) {
        std::cerr << "Error: " << parser.getError() << std::endl;
        return 1;
    }
    
    const auto& options = parser.getOptions();
    
    // Initialize the logger with the specified log level from CLI
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Log startup information
    std::cout << "NLM Neural Learning Machine - CLI Enhanced" << std::endl;
    NLM_LOG_INFO("=== NLM Neural Learning Machine (CLI Enhanced) ===");
    
    // Create brain configuration and brain object
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize the brain with error checking
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run tests if requested via CLI flags
    if (options.runAllTests || options.runNeuronTests || 
        options.runSynapseTests || options.runPlasticityTests || 
        options.runBrainTests) {
        NLM_LOG_INFO("Running requested tests...");
        parser.runTests(brain);
    }
    
    // Run simulation if no tests were requested
    if (!options.runAllTests && !(options.runNeuronTests || options.runSynapseTests || 
                                  options.runPlasticityTests || options.runBrainTests)) {
        NLM_LOG_INFO("Running simulation...");
        auto stats = parser.runSimulation(brain);
        parser.writeOutput(stats);
    }
    
    // Save checkpoint if checkpoint save path was provided
    if (!options.checkpointSave.empty()) {
        NLM_LOG_INFO("Saving checkpoint to: " + options.checkpointSave);
        parser.saveCheckpoint(brain, options.checkpointSave);
    }
    
    // Load checkpoint if checkpoint load path was provided
    if (!options.checkpointLoad.empty()) {
        NLM_LOG_INFO("Loading checkpoint from: " + options.checkpointLoad);
        parser.loadCheckpoint(brain, options.checkpointLoad);
    }
    
    // Display final brain status
    NLM_LOG_INFO("=== Simulation Complete ===");
    brain->logStatus();
    
    return 0;
}
