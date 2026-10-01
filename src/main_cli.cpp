// NLM (熙然) - Neural Learning Machine CLI
// Phase 2: Real Neural Computation
// Command-line interface implementation

#include "core/CommandLineParser.hpp"
#include "core/CLIFactory.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include <iostream>
#include <memory>
#include <string>

using namespace nlm;

void printNLMHeader() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     Command Line Interface (CLI)                             ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printVersionInfo() {
    std::cout << "NLM Version " << CommandLineParser::getVersion() << std::endl;
    std::cout << "Phase 2: Real Neural Computation with LIF neurons and plasticity" << std::endl;
    std::cout << "Compiler: " << __VERSION__ << std::endl;
}

void printUsage() {
    std::cout << "=== NLM Command Line Interface ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [OPTIONS] <command> [args]" << std::endl;
    std::cout << std::endl;
    std::cout << "Global Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  -v, --verbose           Enable verbose output" << std::endl;
    std::cout << "  --debug                 Enable debug mode" << std::endl;
    std::cout << "  --config-file <file>    Load configuration from file" << std::endl;
    std::cout << std::endl;
    std::cout << "Command Categories:" << std::endl;
    std::cout << "  Configuration: --set key=value" << std::endl;
    std::cout << "  Brain Control: save, load, reset, checkpoint" << std::endl;
    std::cout << "  Simulation Control: run, --steps, --timestep, --duration" << std::endl;
    std::cout << "  Visualization: --plot, --export-stats, debug" << std::endl;
    std::cout << "  Advanced: --profile, --benchmark, --validate, batch" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --config-file config.json --steps 1000" << std::endl;
    std::cout << "  nlm save --filepath brain_state.json" << std::endl;
    std::cout << "  nlm load --filepath brain_state.json --steps 5000" << std::endl;
    std::cout << "  nlm --set neuron_count=2000 --set region_count=4" << std::endl;
    std::cout << std::endl;
    std::cout << "For detailed command help, use: nlm <command> --help" << std::endl;
}

int main(int argc, char** argv) {
    printNLMHeader();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM CLI Starting ===");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    NLM_LOG_INFO("Brain initialized successfully");
    
    // Create command line parser with factory
    auto commandHandler = CLIFactory::createCommandHandler(config, brain);
    auto parser = std::make_unique<CommandLineParser>();
    parser->setCommandHandler(commandHandler);
    
    // Parse and execute command line arguments
    CommandResult result = parser->parseAndExecute(argc, argv);
    
    if (result.status == CommandStatus::Success) {
        NLM_LOG_INFO("Command executed successfully: " + result.message);
    } else if (result.status == CommandStatus::InvalidArguments) {
        NLM_LOG_ERROR("Invalid command line arguments: " + result.message);
        printUsage();
        return 1;
    } else if (result.status == CommandStatus::Failed) {
        NLM_LOG_ERROR("Command failed: " + result.message);
        return 1;
    }
    
    NLM_LOG_INFO("=== NLM CLI Complete ===");
    
    return 0;
}
