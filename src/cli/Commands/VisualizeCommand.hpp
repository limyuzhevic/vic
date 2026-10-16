#pragma once

#include "NLMCLI.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

class VisualizeCommand : public ICommand {
public:
    std::string getName() const override { return "visualize"; }
    std::string getDescription() const override { return "Generate visualization outputs"; }
    std::string getUsage() const override { return "visualize <output> [options]"; }
    
    bool validateArguments(const std::vector<std::string>& args) override {
        return !args.empty();
    }
    
    void printHelp() const override {
        std::cout << "Usage: nlm-cli visualize <output> [options]" << std::endl;
        std::cout << "Generate visualization outputs." << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << "  <output>              Output file or directory for visualizations" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --format <format>      Visualization format (graphml, dot, png, svg)" << std::endl;
        std::cout << "  --neuron_count <n>     Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --region_count <n>     Number of brain regions (default: 1)" << std::endl;
        std::cout << "  --viz_type <type>      Visualization type (network, activity, connectivity)" << std::endl;
        std::cout << "  --resolution <width> <height>  Output resolution (e.g., --resolution 1920 1080)" << std::endl;
        std::cout << "  --log-level <level>    Logging level (debug, info, warning, error)" << std::endl;
        std::cout << "  --verbose, -v          Verbose output" << std::endl;
        std::cout << "  --quiet, -q            Quiet output" << std::endl;
        std::cout << "  --help, -h             Show help" << std::endl;
    }
    
    CommandResult execute(const std::vector<std::string>& args, const CLIConfig& config) override {
        CommandResult result;
        
        if (args.empty() || args[0] == "--help" || args[0] == "-h") {
            printHelp();
            result.success = true;
            result.exitCode = 0;
            return result;
        }
        
        std::string output = args[0];
        std::vector<std::string> optionsArgs(args.begin() + 1, args.end());
        
        // Parse options
        std::map<std::string, std::string> options;
        std::string vizFormat;
        std::string vizType;
        int width = 1920;
        int height = 1080;
        
        for (size_t i = 0; i < optionsArgs.size(); ++i) {
            std::string arg = optionsArgs[i];
            
            if (arg.find('=') != std::string::npos) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                options[key] = value;
            } else if (arg == "--format" && i + 1 < optionsArgs.size()) {
                vizFormat = optionsArgs[++i];
            } else if (arg == "--viz_type" && i + 1 < optionsArgs.size()) {
                vizType = optionsArgs[++i];
            } else if (arg == "--resolution" && i + 2 < optionsArgs.size()) {
                width = std::stoi(optionsArgs[++i]);
                height = std::stoi(optionsArgs[++i]);
                i += 2;
            } else if (arg == "--help" || arg == "-h") {
                printHelp();
                result.success = true;
                result.exitCode = 0;
                return result;
            }
        }
        
        try {
            result = generateVisualization(output, options, vizFormat, vizType, width, height, config);
        } catch (const std::exception& e) {
            result.success = false;
            result.message = std::string("Error generating visualization: ") + e.what();
            result.exitCode = 3;
        }
        
        return result;
    }
    
private:
    CommandResult generateVisualization(const std::string& output,
                                        const std::map<std::string, std::string>& options,
                                        const std::string& vizFormat,
                                        const std::string& vizType,
                                        int width,
                                        int height,
                                        const CLIConfig& config) {
        CommandResult result;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        LogLevel logLevel = LogLevel::Info;
        if (config.logLevel == "debug") logLevel = LogLevel::Debug;
        else if (config.logLevel == "warning") logLevel = LogLevel::Warning;
        else if (config.logLevel == "error") logLevel = LogLevel::Error;
        else if (config.logLevel == "critical") logLevel = LogLevel::Critical;
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
        consoleLogger->setUseColors(!config.quiet);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Generating visualization: " + output);
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Set default values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", std::stoll(options.count("neuron_count") ? options.at("neuron_count") : "1000"), ConfigSource::Default);
        config->set("region_count", std::stoll(options.count("region_count") ? options.at("region_count") : "1"), ConfigSource::Default);
        
        // Create brain
        NLM_LOG_INFO("Creating brain for visualization...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            result.success = false;
            result.message = "Failed to initialize brain for visualization";
            return result;
        }
        
        NLM_LOG_INFO("Brain initialized with " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
        
        // Run simulation to get activity data
        NLM_LOG_INFO("Running simulation for visualization data...");
        
        int64_t steps = 500;  // Fixed number of steps for visualization
        for (int64_t step = 0; step < steps; ++step) {
            brain->step(step, config->getOr<double>("simulation_timestep", 0.001));
            
            if (config.verbose || (step % 100 == 0)) {
                NLM_LOG_INFO("Simulation step: " + std::to_string(step) + "/" + std::to_string(steps));
            }
        }
        
        // Generate visualization based on type
        NLM_LOG_INFO("Generating visualization: " + vizType + " (" + vizFormat + ")");
        
        std::ostringstream outputData;
        
        if (vizType == "network" || vizType.empty()) {
            outputData << "Network Visualization" << std::endl;
            outputData << "====================" << std::endl;
            outputData << "Format: " << (vizFormat.empty() ? "graphml" : vizFormat) << std::endl;
            outputData << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
            outputData << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
            outputData << "Resolution: " << width << " x " << height << std::endl;
            outputData << "Steps: " << steps << std::endl;
            
            // Create a simple graph representation
            outputData << "# Network Graph" << std::endl;
            outputData << "# Nodes: " << brain->getTotalNeuronCount() << std::endl;
            outputData << "# Edges: " << brain->getTotalSynapseCount() << std::endl;
            
            // Create nodes (neurons)
            outputData << "[nodes]" << std::endl;
            for (size_t i = 0; i < brain->getTotalNeuronCount() && i < 100; ++i) {  // Limit to first 100 for readability
                outputData << "node" << i << ",\"Neuron " << i << "\",\"type:neuron\"" << std::endl;
            }
            
            // Create edges (synapses) - limit to avoid huge files
            outputData << "[edges]" << std::endl;
            size_t synapseCount = 0;
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                for (const auto& syn : region->getSynapses()) {
                    if (synapseCount >= 1000) break;  // Limit to avoid huge files
                    outputData << "synapse" << synapseCount << ",\"pre:neuron0\",\"post:neuron1\",\"weight:" << syn->getWeight() << "\"" << std::endl;
                    synapseCount++;
                }
            }
            
        } else if (vizType == "activity") {
            outputData << "Activity Visualization" << std::endl;
            outputData << "======================" << std::endl;
            outputData << "Format: " << (vizFormat.empty() ? "csv" : vizFormat) << std::endl;
            outputData << "Resolution: " << width << " x " << height << std::endl;
            outputData << "Steps: " << steps << std::endl;
            
            // Create activity data
            outputData << "step,time,total_spikes,firing_rate" << std::endl;
            double timeStep = config->getOr<double>("simulation_timestep", 0.001);
            
            // Simulate some activity data
            for (int64_t step = 0; step < steps; step += 50) {
                outputData << step << ","
                          << (step * timeStep) << ","
                          << (brain->getTotalSpikeCount() * (step / steps)) << ","
                          << (brain->getAverageFiringRate() * (step / steps)) << std::endl;
            }
            
        } else if (vizType == "connectivity") {
            outputData << "Connectivity Visualization" << std::endl;
            outputData << "==========================" << std::endl;
            outputData << "Format: " << (vizFormat.empty() ? "json" : vizFormat) << std::endl;
            
            // Create connectivity matrix (simplified)
            outputData << "{" << std::endl;
            outputData << "  \"nodes\": [" << std::endl;
            for (size_t i = 0; i < brain->getTotalNeuronCount() && i < 50; ++i) {
                outputData << "    {\"id\": \"neuron" << i << "\", \"type\": \"neuron\"";
                if (i < brain->getTotalNeuronCount() - 1 && i < 49) outputData << ",";
                outputData << std::endl;
            }
            outputData << "  ]," << std::endl;
            outputData << "  \"edges\": [" << std::endl;
            
            size_t edgeCount = 0;
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                for (const auto& syn : region->getSynapses()) {
                    if (edgeCount >= 100) break;  // Limit to avoid huge files
                    outputData << "    {\"source\": \"neuron0\", \"target\": \"neuron1\", \"weight\": " << syn->getWeight() << "";
                    if (edgeCount < 99) outputData << ",";
                    outputData << std::endl;
                    edgeCount++;
                }
            }
            
            outputData << "  ]" << std::endl;
            outputData << "}" << std::endl;
            
        } else {
            outputData << "Unknown visualization type: " << vizType << std::endl;
            outputData << "Supported types: network, activity, connectivity" << std::endl;
        }
        
        // Write output
        std::ofstream outFile(output);
        if (!outFile.is_open()) {
            result.success = false;
            result.message = "Failed to create output file: " + output;
            result.exitCode = 2;
            return result;
        }
        
        outFile << outputData.str();
        outFile.close();
        
        NLM_LOG_INFO("Visualization saved to: " + output);
        NLM_LOG_INFO("File size: " + std::to_string(outputData.str().size()) + " bytes");
        
        std::ostringstream resultOutput;
        resultOutput << "=== Visualization Results ===" << std::endl;
        resultOutput << "Output: " << output << std::endl;
        resultOutput << "Type: " << (vizType.empty() ? "network" : vizType) << std::endl;
        resultOutput << "Format: " << (vizFormat.empty() ? "graphml" : vizFormat) << std::endl;
        resultOutput << "Resolution: " << width << " x " << height << std::endl;
        resultOutput << "File size: " << outputData.str().size() << " bytes" << std::endl;
        
        result.success = true;
        result.message = "Visualization generated successfully";
        result.output = resultOutput.str();
        
        return result;
    }
    
}; // class VisualizeCommand

} // namespace nlm