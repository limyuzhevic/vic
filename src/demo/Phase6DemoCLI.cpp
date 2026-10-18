// Phase6DemoCLI.cpp - Complete CLI interface for Phase 6 demo executable

#include "src/demo/Phase6DemoCLI.hpp"
#include "src/experiments/ExperimentFactory.hpp"
#include "src/experiments/ExperimentUtils.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

namespace nlm {

Phase6DemoOptions Phase6DemoCLI::parseArguments(int argc, char* argv[]) {
    Phase6DemoOptions options;
    options.outputFormat = OutputFormat::TEXT;
    options.verbose = false;
    options.quiet = false;
    options.enableCheckpointing = true;
    options.enableReplay = true;
    options.enableDevelopment = true;
    options.neuronCount = 500;
    options.maxSteps = 2000;
    options.regionCount = 1;
    options.connectionProbability = 0.1f;
    
    std::map<std::string, std::string> parsedOptions;
    ExperimentUtils::parseCommandLineArguments(argc, argv, parsedOptions);
    
    // Parse options
    if (parsedOptions.find("help") != parsedOptions.end()) {
        printHelp();
        std::exit(0);
    }
    
    if (parsedOptions.find("verbose") != parsedOptions.end()) {
        options.verbose = true;
    }
    
    if (parsedOptions.find("quiet") != parsedOptions.end()) {
        options.quiet = true;
    }
    
    if (parsedOptions.find("config") != parsedOptions.end()) {
        options.configFile = parsedOptions["config"];
        loadConfigFromFile(options);
    }
    
    if (parsedOptions.find("output") != parsedOptions.end()) {
        options.outputFile = parsedOptions["output"];
    }
    
    if (parsedOptions.find("format") != parsedOptions.end()) {
        options.outputFormat = ExperimentUtils::stringToOutputFormat(parsedOptions["format"]);
    }
    
    // Parse numeric options
    auto it = parsedOptions.find("neuron-count");
    if (it != parsedOptions.end()) {
        try {
            options.neuronCount = std::stoul(it->second);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid neuron count: " + it->second);
        }
    }
    
    it = parsedOptions.find("max-steps");
    if (it != parsedOptions.end()) {
        try {
            options.maxSteps = std::stoul(it->second);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid max steps: " + it->second);
        }
    }
    
    it = parsedOptions.find("region-count");
    if (it != parsedOptions.end()) {
        try {
            options.regionCount = std::stoul(it->second);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid region count: " + it->second);
        }
    }
    
    it = parsedOptions.find("connection-probability");
    if (it != parsedOptions.end()) {
        try {
            options.connectionProbability = std::stof(it->second);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid connection probability: " + it->second);
        }
    }
    
    // Parse boolean options
    if (parsedOptions.find("checkpointing") != parsedOptions.end()) {
        options.enableCheckpointing = true;
    }
    
    if (parsedOptions.find("replay") != parsedOptions.end()) {
        options.enableReplay = true;
    }
    
    if (parsedOptions.find("development") != parsedOptions.end()) {
        options.enableDevelopment = true;
    }
    
    it = parsedOptions.find("checkpoint");
    if (it != parsedOptions.end()) {
        options.checkpointPath = it->second;
    }
    
    // Set up logging
    if (options.verbose) {
        Logger::getInstance().setLevel(Logger::Level::Info);
    } else if (options.quiet) {
        Logger::getInstance().setLevel(Logger::Level::Error);
    }
    
    return options;
}

bool Phase6DemoCLI::loadConfigFromFile(Phase6DemoOptions& options) {
    try {
        auto config = ExperimentUtils::loadConfigFromFile(options.configFile);
        
        auto neuronCountOpt = config->get<int64_t>("neuron_count");
        if (neuronCountOpt) {
            options.neuronCount = *neuronCountOpt;
        }
        
        auto maxStepsOpt = config->get<int64_t>("max_steps");
        if (maxStepsOpt) {
            options.maxSteps = *maxStepsOpt;
        }
        
        auto regionCountOpt = config->get<int64_t>("region_count");
        if (regionCountOpt) {
            options.regionCount = *regionCountOpt;
        }
        
        auto connectionProbabilityOpt = config->get<double>("connection_probability");
        if (connectionProbabilityOpt) {
            options.connectionProbability = static_cast<float>(*connectionProbabilityOpt);
        }
        
        auto checkpointingOpt = config->get<bool>("enable_checkpointing");
        if (checkpointingOpt) {
            options.enableCheckpointing = *checkpointingOpt;
        }
        
        auto replayOpt = config->get<bool>("enable_replay");
        if (replayOpt) {
            options.enableReplay = *replayOpt;
        }
        
        auto developmentOpt = config->get<bool>("enable_development");
        if (developmentOpt) {
            options.enableDevelopment = *developmentOpt;
        }
        
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to load configuration: " + std::string(e.what()));
        return false;
    }
}

bool Phase6DemoCLI::runPhase6Demo(const Options& options) {
    try {
        Logger::getInstance().logInfo("Starting Phase 6 integration demo");
        Logger::getInstance().logInfo("Configuration: " + std::to_string(options.neuronCount) + 
                                   " neurons, " + std::to_string(options.maxSteps) + " steps");
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount));
        config->set("region_count", static_cast<int64_t>(options.regionCount));
        config->set("connection_probability", options.connectionProbability);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Create Phase 6 experiment
        auto experiment = std::make_shared<Phase6IntegratedExperiment>();
        
        // Create Phase 6 configuration
        Phase6Config phase6Config;
        phase6Config.neuronCount = options.neuronCount;
        phase6Config.maxSteps = options.maxSteps;
        phase6Config.regionCount = options.regionCount;
        phase6Config.connectionProbability = options.connectionProbability;
        phase6Config.enableCheckpointing = options.enableCheckpointing;
        phase6Config.enableReplay = options.enableReplay;
        phase6Config.enableDevelopment = options.enableDevelopment;
        phase6Config.checkpointPath = options.checkpointPath;
        
        // Run integration verification
        Logger::getInstance().logInfo("Running integration verification...");
        bool integrationOK = experiment->verifyIntegration();
        
        if (!integrationOK) {
            Logger::getInstance().logError("Integration verification failed!");
            return false;
        }
        
        Logger::getInstance().logInfo("Integration verification passed!");
        
        // Run the experiment
        Logger::getInstance().logInfo("Running Phase 6 integration experiment...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegrationResult result = experiment->run(phase6Config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Format and output results
        std::string output = formatResults(result);
        
        if (!options.outputFile.empty()) {
            std::ofstream outputFile(options.outputFile);
            if (outputFile.is_open()) {
                outputFile << output;
                outputFile.close();
                Logger::getInstance().logInfo("Results saved to " + options.outputFile);
            } else {
                Logger::getInstance().logError("Failed to open output file: " + options.outputFile);
            }
        } else {
            std::cout << output << std::endl;
        }
        
        // Display summary
        printSummary(result);
        
        Logger::getInstance().logInfo("Phase 6 demo completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Phase 6 demo failed: " + std::string(e.what()));
        return false;
    }
}

std::string Phase6DemoCLI::formatResults(const Phase6IntegrationResult& result) {
    switch (outputFormat_) {
        case OutputFormat::TEXT:
            return formatResultsAsText(result);
        case OutputFormat::JSON:
            return formatResultsAsJson(result);
        case OutputFormat::CSV:
            return formatResultsAsCsv(result);
        case OutputFormat::XML:
            return formatResultsAsXml(result);
        default:
            return formatResultsAsText(result);
    }
}

std::string Phase6DemoCLI::formatResultsAsText(const Phase6IntegrationResult& result) {
    std::ostringstream oss;
    oss << "=== Phase 6 Integration Demo Results ===\n";
    oss << "\n--- Integration Status ---\n";
    oss << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    oss << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    oss << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    oss << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    oss << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    oss << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
    oss << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << "\n";
    
    oss << "\n--- Performance Metrics ---\n";
    oss << "Total Reward: " << result.totalReward << "\n";
    oss << "Average Firing Rate: " << result.avgFiringRate << " Hz\n";
    oss << "Average Synaptic Weight: " << result.avgSynapticWeight << "\n";
    oss << "Episodes Stored: " << result.memoryEpisodesStored << "\n";
    oss << "Novelty Level: " << result.noveltyLevel << "\n";
    oss << "Curiosity Level: " << result.curiosityLevel << "\n";
    oss << "Dopamine Level: " << result.dopamineLevel << "\n";
    
    oss << "\n--- Timing ---\n";
    oss << "Start Time: " << (result.startTime > 0 ? std::to_string(result.startTime) : "N/A") << "\n";
    oss << "End Time: " << (result.endTime > 0 ? std::to_string(result.endTime) : "N/A") << "\n";
    oss << "Wall Clock Time: " << result.totalWallClockTime << " seconds\n";
    
    oss << "\n=== End Results ===\n";
    return oss.str();
}

std::string Phase6DemoCLI::formatResultsAsJson(const Phase6IntegrationResult& result) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"integration_status\": {\n";
    oss << "    \"memory_working_memory_integrated\": " << std::boolalpha << result.memoryWorkingMemoryIntegrated << ",\n";
    oss << "    \"memory_episodic_memory_integrated\": " << std::boolalpha << result.memoryEpisodicMemoryIntegrated << ",\n";
    oss << "    \"neuromodulation_integrated\": " << std::boolalpha << result.neuromodulationIntegrated << ",\n";
    oss << "    \"prediction_integrated\": " << std::boolalpha << result.predictionIntegrated << ",\n";
    oss << "    \"development_integrated\": " << std::boolalpha << result.developmentIntegrated << ",\n";
    oss << "    \"checkpointing_works\": " << std::boolalpha << result.checkpointingWorks << ",\n";
    oss << "    \"replay_works\": " << std::boolalpha << result.replayWorks << "\n";
    oss << "  },\n";
    oss << "  \"performance_metrics\": {\n";
    oss << "    \"total_reward\": " << result.totalReward << ",\n";
    oss << "    \"avg_firing_rate\": " << result.avgFiringRate << ",\n";
    oss << "    \"avg_synaptic_weight\": " << result.avgSynapticWeight << ",\n";
    oss << "    \"memory_episodes_stored\": " << result.memoryEpisodesStored << ",\n";
    oss << "    \"novelty_level\": " << result.noveltyLevel << ",\n";
    oss << "    \"curiosity_level\": " << result.curiosityLevel << ",\n";
    oss << "    \"dopamine_level\": " << result.dopamineLevel << "\n";
    oss << "  },\n";
    oss << "  \"timing\": {\n";
    oss << "    \"start_time\": " << (result.startTime > 0 ? std::to_string(result.startTime) : "null") << ",\n";
    oss << "    \"end_time\": " << (result.endTime > 0 ? std::to_string(result.endTime) : "null") << ",\n";
    oss << "    \"wall_clock_time\": " << result.totalWallClockTime << "\n";
    oss << "  }\n";
    oss << "}\n";
    return oss.str();
}

std::string Phase6DemoCLI::formatResultsAsCsv(const Phase6IntegrationResult& result) {
    std::ostringstream oss;
    oss << "integration_status,performance_metrics,timing\n";
    oss << std::boolalpha << result.memoryWorkingMemoryIntegrated << ","
        << std::boolalpha << result.memoryEpisodicMemoryIntegrated << ","
        << std::boolalpha << result.neuromodulationIntegrated << ","
        << std::boolalpha << result.predictionIntegrated << ","
        << std::boolalpha << result.developmentIntegrated << ","
        << std::boolalpha << result.checkpointingWorks << ","
        << std::boolalpha << result.replayWorks << ","
        << result.totalReward << ","
        << result.avgFiringRate << ","
        << result.avgSynapticWeight << ","
        << result.memoryEpisodesStored << ","
        << result.noveltyLevel << ","
        << result.curiosityLevel << ","
        << result.dopamineLevel << ","
        << (result.startTime > 0 ? std::to_string(result.startTime) : "") << ","
        << (result.endTime > 0 ? std::to_string(result.endTime) : "") << ","
        << result.totalWallClockTime << "\n";
    return oss.str();
}

std::string Phase6DemoCLI::formatResultsAsXml(const Phase6IntegrationResult& result) {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\"?>\n";
    oss << "<phase6_results>\n";
    oss << "  <integration_status>\n";
    oss << "    <memory_working_memory_integrated>" << std::boolalpha << result.memoryWorkingMemoryIntegrated << "</memory_working_memory_integrated>\n";
    oss << "    <memory_episodic_memory_integrated>" << std::boolalpha << result.memoryEpisodicMemoryIntegrated << "</memory_episodic_memory_integrated>\n";
    oss << "    <neuromodulation_integrated>" << std::boolalpha << result.neuromodulationIntegrated << "</neuromodulation_integrated>\n";
    oss << "    <prediction_integrated>" << std::boolalpha << result.predictionIntegrated << "</prediction_integrated>\n";
    oss << "    <development_integrated>" << std::boolalpha << result.developmentIntegrated << "</development_integrated>\n";
    oss << "    <checkpointing_works>" << std::boolalpha << result.checkpointingWorks << "</checkpointing_works>\n";
    oss << "    <replay_works>" << std::boolalpha << result.replayWorks << "</replay_works>\n";
    oss << "  </integration_status>\n";
    oss << "  <performance_metrics>\n";
    oss << "    <total_reward>" << result.totalReward << "</total_reward>\n";
    oss << "    <avg_firing_rate>" << result.avgFiringRate << "</avg_firing_rate>\n";
    oss << "    <avg_synaptic_weight>" << result.avgSynapticWeight << "</avg_synaptic_weight>\n";
    oss << "    <memory_episodes_stored>" << result.memoryEpisodesStored << "</memory_episodes_stored>\n";
    oss << "    <novelty_level>" << result.noveltyLevel << "</novelty_level>\n";
    oss << "    <curiosity_level>" << result.curiosityLevel << "</curiosity_level>\n";
    oss << "    <dopamine_level>" << result.dopamineLevel << "</dopamine_level>\n";
    oss << "  </performance_metrics>\n";
    oss << "  <timing>\n";
    oss << "    <start_time>" << (result.startTime > 0 ? std::to_string(result.startTime) : "null") << "</start_time>\n";
    oss << "    <end_time>" << (result.endTime > 0 ? std::to_string(result.endTime) : "null") << "</end_time>\n";
    oss << "    <wall_clock_time>" << result.totalWallClockTime << "</wall_clock_time>\n";
    oss << "  </timing>\n";
    oss << "</phase6_results>\n";
    return oss.str();
}

void Phase6DemoCLI::printSummary(const Phase6IntegrationResult& result) {
    std::cout << "\n=== Phase 6 Integration Summary ===\n";
    std::cout << "✓ Integration Verification: PASSED\n";
    std::cout << "✓ Memory Systems: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "✓ Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "✓ Prediction System: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "✓ Development System: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "✓ Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
    std::cout << "✓ Replay System: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << "\n";
    std::cout << "\nPerformance:\n";
    std::cout << "  Total Reward: " << result.totalReward << "\n";
    std::cout << "  Average Firing Rate: " << result.avgFiringRate << " Hz\n";
    std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << "\n";
    std::cout << "  Wall Clock Time: " << result.totalWallClockTime << " seconds\n";
    std::cout << "\n=== Summary Complete ===\n";
}

void Phase6DemoCLI::printHelp() {
    std::cout << "=== NLM Phase 6 Demo ===\n";
    std::cout << "Usage: nlm_phase6_demo [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help                    Show this help message\n";
    std::cout << "  -v, --verbose                 Enable verbose output\n";
    std::cout << "  -q, --quiet                   Suppress output\n";
    std::cout << "  -c, --config FILE             Load configuration from file\n";
    std::cout << "  -o, --output FILE             Save output to file\n";
    std::cout << "  --format TYPE                 Output format (text, json, csv, xml)\n";
    std::cout << "  --checkpoint PATH              Checkpoint file path\n";
    std::cout << "  --neuron-count N              Set number of neurons (default: 500)\n";
    std::cout << "  --max-steps N                 Set maximum steps (default: 2000)\n";
    std::cout << "  --region-count N              Set number of regions (default: 1)\n";
    std::cout << "  --connection-probability P    Set connection probability (default: 0.1)\n";
    std::cout << "  --enable-checkpointing        Enable checkpointing\n";
    std::cout << "  --disable-checkpointing       Disable checkpointing\n";
    std::cout << "  --enable-replay               Enable replay\n";
    std::cout << "  --disable-replay              Disable replay\n";
    std::cout << "  --enable-development          Enable development\n";
    std::cout << "  --disable-development         Disable development\n";
    std::cout << "\nExamples:\n";
    std::cout << "  nlm_phase6_demo --neuron-count 1000 --max-steps 5000\n";
    std::cout << "  nlm_phase6_demo --config config.json --output results.json\n";
    std::cout << "  nlm_phase6_demo --format json --quiet\n";
    std::cout << "  nlm_phase6_demo --enable-checkpointing --checkpoint ./checkpoint.bin\n";
}

} // namespace nlm
