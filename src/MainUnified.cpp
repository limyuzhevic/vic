#include "CommandLineParser.hpp"
#include "CoreBrainController.hpp"
#include "DebugConsole.hpp"
#include "ScriptingEngine.hpp"
#include <iostream>
#include <memory>

int mainUnified(int argc, char** argv) {
    try {
        // Parse command line arguments
        CommandLineParser parser;
        auto args = parser.parse(argc, argv);
        
        if (!args.isValid) {
            std::cerr << "Error parsing command line: " << args.errorMessage << std::endl;
            parser.printHelp();
            return 1;
        }
        
        // Check for help or version requests
        if (args.helpRequested) {
            parser.printHelp();
            return 0;
        }
        
        if (args.versionRequested) {
            parser.printVersion();
            return 0;
        }
        
        // Create configuration options from parsed arguments
        ConfigurationOptions options;
        
        // Parse command line options
        if (args.options.find("neuron_count") != args.options.end()) {
            options.neuronCount = std::stoull(args.options["neuron_count"]);
        }
        
        if (args.options.find("region_count") != args.options.end()) {
            options.regionCount = std::stoull(args.options["region_count"]);
        }
        
        if (args.options.find("connection_probability") != args.options.end()) {
            options.connectionProbability = std::stof(args.options["connection_probability"]);
        }
        
        if (args.options.find("timestep") != args.options.end()) {
            options.timestep = std::stod(args.options["timestep"]);
        }
        
        if (args.options.find("steps") != args.options.end()) {
            options.maxSteps = std::stoull(args.options["steps"]);
        }
        
        // Set boolean options
        if (args.options.find("debug") != args.options.end() && args.options["debug"] == "true") {
            options.logLevel = "DEBUG";
        }
        
        if (args.options.find("verbose") != args.options.end() && args.options["verbose"] == "true") {
            options.verboseLogging = true;
        }
        
        if (args.options.find("auto_restart") != args.options.end() && args.options["auto_restart"] == "true") {
            // Auto-restart can be implemented
        }
        
        if (args.options.find("world") != args.options.end() && args.options["world"] == "true") {
            options.enableWorldSimulation = true;
        }
        
        if (args.options.find("memory") != args.options.end() && args.options["memory"] == "true") {
            options.enableMemorySystems = true;
        }
        
        if (args.options.find("cognition") != args.options.end() && args.options["cognition"] == "true") {
            options.enableCognition = true;
        }
        
        if (args.options.find("prediction") != args.options.end() && args.options["prediction"] == "true") {
            options.enablePrediction = true;
        }
        
        if (args.options.find("neuromodulation") != args.options.end() && args.options["neuromodulation"] == "true") {
            options.enableNeuromodulation = true;
        }
        
        if (args.options.find("development") != args.options.end() && args.options["development"] == "true") {
            options.enableDevelopment = true;
        }
        
        if (args.options.find("visualize") != args.options.end() && args.options["visualize"] == "true") {
            options.enableVisualization = true;
        }
        
        if (args.options.find("console") != args.options.end() && args.options["console"] == "true") {
            options.enableDebugConsole = true;
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Create brain controller
        CoreBrainController controller(config, options);
        
        // Handle different command types
        switch (args.command) {
            case CommandType::RUN:
                controller.run();
                break;
            case CommandType::STEP:
                controller.step(1);
                break;
            case CommandType::PAUSE:
                controller.pause();
                break;
            case CommandType::RESUME:
                controller.resume();
                break;
            case CommandType::SAVE:
                if (args.options.find("path") != args.options.end()) {
                    controller.saveState(args.options["path"]);
                } else {
                    std::cerr << "Error: Save path required" << std::endl;
                    return 1;
                }
                break;
            case CommandType::LOAD:
                if (args.options.find("path") != args.options.end()) {
                    controller.loadState(args.options["path"]);
                } else {
                    std::cerr << "Error: Load path required" << std::endl;
                    return 1;
                }
                break;
            case CommandType::CONFIG:
                controller.printConfiguration();
                break;
            case CommandType::STATUS:
                controller.printStatus();
                break;
            case CommandType::DEBUG:
                DebugConsole::startInteractiveSession(controller);
                break;
            case CommandType::VISUALIZE:
                // Visualization would go here
                std::cout << "Visualization mode would start here" << std::endl;
                break;
            case CommandType::SCRIPT:
                if (args.options.find("script_path") != args.options.end()) {
                    ScriptingEngine::executeScript(args.options["script_path"], controller);
                } else {
                    std::cerr << "Error: Script path required" << std::endl;
                    return 1;
                }
                break;
            case CommandType::HELP:
                parser.printHelp();
                break;
            case CommandType::VERSION:
                parser.printVersion();
                break;
            case CommandType::QUIT:
            case CommandType::EXIT:
                std::cout << "Exiting NLM..." << std::endl;
                break;
            default:
                std::cerr << "Error: Unknown command" << std::endl;
                return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}