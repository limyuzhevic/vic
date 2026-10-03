#include "VisualizeCLI.hpp"
#include "BrainVisualizer.hpp"
#include "VisualizationFactory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../memory/Memory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

VisualizeCLI::VisualizeCLI() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

VisualizeCLI::~VisualizeCLI() {
}

bool VisualizeCLI::parseArguments(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            showHelp_ = true;
        } else if (arg == "--version") {
            showVersion_ = true;
        } else if (arg == "--list-visualizers") {
            listVisualizers_ = true;
        } else if (arg.substr(0, 6) == "--type=") {
            visualizerType_ = arg.substr(6);
        } else if (arg.substr(0, 6) == "--output=") {
            outputFile_ = arg.substr(6);
        } else if (arg.substr(0, 8) == "--format=") {
            outputFormat_ = arg.substr(8);
        } else if (arg.substr(0, 7) == "--region=") {
            regionFilter_ = arg.substr(7);
        } else if (arg.substr(0, 12) == "--focus-neuron=") {
            focusNeuron_ = arg.substr(12);
        } else if (arg == "--interactive") {
            interactive_ = true;
        } else if (arg.substr(0, 7) == "--step=") {
            try {
                step_ = std::stoi(arg.substr(7));
            } catch (...) {
                step_ = 0;
            }
        } else if (arg == "--show-options") {
            showOptions_ = true;
        } else if (arg.substr(0, 12) == "--detail-level=") {
            try {
                detailLevel_ = std::stoi(arg.substr(12));
            } catch (...) {
                detailLevel_ = 1;
            }
        } else if (arg.substr(0, 15) == "--config-file=") {
            configFile_ = arg.substr(15);
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
            return false;
        }
    }
    
    // Load config file if specified
    if (!configFile_.empty()) {
        if (!loadConfigFile(configFile_)) {
            std::cerr << "Failed to load config file: " << configFile_ << std::endl;
            return false;
        }
    }
    
    return validateConfig();
}

int VisualizeCLI::run(const Brain& brain) {
    // Show help/version information if requested
    if (showHelp_) {
        printHelp();
        return 0;
    }
    
    if (showVersion_) {
        printVersion();
        return 0;
    }
    
    if (listVisualizers_) {
        printAvailableVisualizers();
        return 0;
    }
    
    if (showOptions_) {
        printVisualizerOptions(!visualizerType_.empty() ? visualizerType_ : "neural_network");
        return 0;
    }
    
    // Validate that we have a brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create the requested visualizer
    std::string visualizerName = !visualizerType_.empty() ? visualizerType_ : "neural_network";
    auto visualizer = VisualizationFactory::createVisualizer(visualizerName);
    
    if (!visualizer) {
        std::cerr << "Unknown visualizer type: " << visualizerName << std::endl;
        return 1;
    }
    
    // Configure the visualizer
    visualizer->setUpdateRate(30.0);
    visualizer->setOutputFormat(outputFormat_);
    visualizer->setDetailLevel(detailLevel_);
    visualizer->setColorScheme("default");
    
    // Parse region filter if provided
    if (!regionFilter_.empty()) {
        // Parse comma-separated list of region IDs
        std::stringstream ss(regionFilter_);
        std::string token;
        std::vector<RegionId> regions;
        while (std::getline(ss, token, ',')) {
            try {
                RegionId regionId(std::stoul(token));
                regions.push_back(regionId);
            } catch (...) {
                std::cerr << "Invalid region ID: " << token << std::endl;
            }
        }
        visualizer->setRegionFilter(regions);
    }
    
    // Parse focus neuron if provided
    if (!focusNeuron_.empty()) {
        try {
            NeuronId neuronId(std::stoul(focusNeuron_));
            visualizer->setFocusNeuron(neuronId);
        } catch (...) {
            std::cerr << "Invalid focus neuron ID: " << focusNeuron_ << std::endl;
        }
    }
    
    // Initialize visualizer
    visualizer->initialize();
    
    // If we have a specific step, simulate to that step first
    if (step_ > 0) {
        for (SimulationStep s = 0; s < step_; ++s) {
            brain.step(s, s * 0.001);
        }
    }
    
    // Update and render the visualization
    visualizer->update(brain);
    visualizer->render();
    
    // Save output if requested
    if (!outputFile_.empty()) {
        std::ofstream file(outputFile_);
        if (file.is_open()) {
            // In a complete implementation, we would serialize the visualization data
            file << "Visualization output saved to: " << outputFile_ << std::endl;
            file << "Visualizer type: " << visualizerName_ << std::endl;
            file << "Output format: " << outputFormat_ << std::endl;
            file << "Step: " << step_ << std::endl;
            file.close();
        }
    }
    
    // Clean up
    visualizer->close();
    
    return 0;
}

void VisualizeCLI::printHelp() const {
    std::cout << R"(
NLM Visualization Tool
=======================

Usage: nlm-visualize [options]

Options:
  --help, -h                    Show this help message
  --version                     Show version information
  --list-visualizers            List all available visualizers
  --show-options                Show options for current visualizer
  --interactive                 Run in interactive mode

Visualization Options:
  --type=<type>                 Visualizer type (neural_network, memory, neuromodulation, cognitive, development, performance)
  --output=<file>               Output file for visualization data
  --format=<format>             Output format (text, html, json, csv)
  --region=<regions>            Comma-separated list of region IDs to focus on
  --focus-neuron=<id>           Focus on a specific neuron
  --step=<step>                 Simulate to this step before visualizing
  --detail-level=<level>        Detail level (0-5)
  --config-file=<file>          Configuration file

Examples:
  nlm-visualize --type=neural_network
  nlm-visualize --type=memory --output=memory_viz.csv
  nlm-visualize --type=neuromodulation --format=json --interactive
  nlm-visualize --type=cognitive --region=1,2,3
)" << std::endl;
}

void VisualizeCLI::printVersion() const {
    std::cout << "NLM Visualization Tool v1.0.0" << std::endl;
}

void VisualizeCLI::printAvailableVisualizers() const {
    std::cout << "Available visualizers:" << std::endl;
    
    auto visualizers = VisualizationFactory::getAvailableVisualizers();
    for (const auto& name : visualizers) {
        auto visualizer = VisualizationFactory::createVisualizer(name);
        if (visualizer) {
            std::cout << "  " << name << ": " << visualizer->getDescription() << std::endl;
        }
    }
}

void VisualizeCLI::printVisualizerOptions(const std::string& name) const {
    auto visualizer = VisualizationFactory::createVisualizer(name);
    if (visualizer) {
        std::cout << "Options for visualizer: " << name << std::endl;
        std::cout << "  Description: " << visualizer->getDescription() << std::endl;
        
        // In a complete implementation, we would show more detailed options
        std::cout << "  Available output formats: text, html, json, csv" << std::endl;
        std::cout << "  Detail levels: 0 (minimal) to 5 (detailed)" << std::endl;
        std::cout << "  Color schemes: default, scientific, artistic, mono" << std::endl;
    }
}

bool VisualizeCLI::parseStringArg(const std::string& arg, const std::string& prefix, std::string& output) {
    if (arg.substr(0, prefix.size()) == prefix) {
        output = arg.substr(prefix.size());
        return true;
    }
    return false;
}

bool VisualizeCLI::parseIntArg(const std::string& arg, const std::string& prefix, int& output) {
    if (arg.substr(0, prefix.size()) == prefix) {
        try {
            output = std::stoi(arg.substr(prefix.size()));
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

bool VisualizeCLI::parseFloatArg(const std::string& arg, const std::string& prefix, float& output) {
    if (arg.substr(0, prefix.size()) == prefix) {
        try {
            output = std::stof(arg.substr(prefix.size()));
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

bool VisualizeCLI::parseBoolArg(const std::string& arg, const std::string& prefix, bool& output) {
    if (arg.substr(0, prefix.size()) == prefix) {
        output = true;
        return true;
    }
    return false;
}

bool VisualizeCLI::loadConfigFile(const std::string& filename) {
    // In a complete implementation, we would parse the config file
    // For now, just return success if file exists
    std::ifstream file(filename);
    return file.good();
}

bool VisualizeCLI::validateConfig() const {
    // Validate configuration - in a complete implementation this would be more comprehensive
    return true;
}

} // namespace nlm