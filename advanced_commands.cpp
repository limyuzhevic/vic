#!/usr/bin/env cpp
// NLM Advanced Command Line Interface
// Provides advanced functionality for expert users

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/ConfigUtils.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

using namespace nlm;

// Advanced command functions
void printAdvancedHelp() {
    std::cout << "=== NLM Advanced Commands ===" << std::endl;
    std::cout << "Advanced functionality for expert users:" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Configuration commands:" << std::endl;
    std::cout << "  --export-config <file>        Export current configuration to JSON" << std::endl;
    std::cout << "  --import-config <file>        Import configuration from JSON file" << std::endl;
    std::cout << "  --validate-config             Validate current configuration" << std::endl;
    std::cout << "  --diff-config <file>         Compare current config with file" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Brain management commands:" << std::endl;
    std::cout << "  --checkpoint <file>           Create a checkpoint of current brain state" << std::endl;
    std::cout << "  --restore <file>             Restore brain state from checkpoint" << std::endl;
    std::cout << "  --analyze <file>             Analyze brain structure and performance" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Memory analysis commands:" << std::endl;
    std::cout << "  --memory-stats                Display detailed memory system statistics" << std::endl;
    std::cout << "  --memory-export <file>       Export memory data to file" << std::endl;
    std::cout << "  --memory-import <file>       Import memory data from file" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Performance commands:" << std::endl;
    std::cout << "  --benchmark <steps>           Run performance benchmark" << std::endl;
    std::cout << "  --profile <steps>            Profile brain performance" << std::endl;
    std::cout << "  --stats                       Display current brain statistics" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Neuromodulation commands:" << std::endl;
    std::cout << "  --neuromodulate <level>       Set neuromodulation level" << std::endl;
    std::cout << "  --curiosity <level>          Set curiosity level" << std::endl;
    std::cout << "  --novelty <level>             Set novelty level" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Development commands:" << std::endl;
    std::cout << "  --stage <stage>              Set developmental stage" << std::endl;
    std::cout << "  --develop <steps>             Advance development by steps" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Export/import commands:" << std::endl;
    std::cout << "  --export-brain <file>         Export complete brain state" << std::endl;
    std::cout << "  --import-brain <file>         Import complete brain state" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Experiment control commands:" << std::endl;
    std::cout << "  --run-phase6 <steps>          Run Phase 6 integration experiment" << std::endl;
    std::cout << "  --run-phase4 <trials>         Run Phase 4 cognition experiments" << std::endl;
    std::cout << "  --run-phase3 <episodes>      Run Phase 3 world interaction" << std::endl;
}

void exportConfig(const std::string& filepath, std::shared_ptr<Config> config) {
    std::cout << "Exporting configuration to " << filepath << "..." << std::endl;
    
    if (config->saveToFile(filepath)) {
        std::cout << "Configuration exported successfully!" << std::endl;
    } else {
        std::cerr << "Failed to export configuration!" << std::endl;
    }
}

void importConfig(const std::string& filepath, std::shared_ptr<Config> config) {
    std::cout << "Importing configuration from " << filepath << "..." << std::endl;
    
    if (config->loadFromFile(filepath)) {
        std::cout << "Configuration imported successfully!" << std::endl;
        std::cout << "Configuration summary:" << std::endl;
        std::cout << config->summary() << std::endl;
    } else {
        std::cerr << "Failed to import configuration!" << std::endl;
    }
}

void validateConfig(std::shared_ptr<Config> config) {
    std::cout << "Validating configuration..." << std::endl;
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep",
        "random_seed"
    };
    
    bool valid = true;
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            std::cout << "  [ERROR] Missing required key: " << key << std::endl;
            valid = false;
        } else {
            std::cout << "  [OK] Found key: " << key << std::endl;
        }
    }
    
    if (valid) {
        std::cout << "Configuration is valid!" << std::endl;
    } else {
        std::cout << "Configuration has errors!" << std::endl;
    }
}

void exportBrainState(const std::string& filepath, std::shared_ptr<Brain> brain) {
    std::cout << "Exporting brain state to " << filepath << "..." << std::endl;
    
    if (brain->save(filepath)) {
        std::cout << "Brain state exported successfully!" << std::endl;
    } else {
        std::cerr << "Failed to export brain state!" << std::endl;
    }
}

void importBrainState(const std::string& filepath, std::shared_ptr<Brain> brain) {
    std::cout << "Importing brain state from " << filepath << "..." << std::endl;
    
    if (brain->load(filepath)) {
        std::cout << "Brain state imported successfully!" << std::endl;
    } else {
        std::cerr << "Failed to import brain state!" << std::endl;
    }
}

void showMemoryStats(std::shared_ptr<Brain> brain) {
    std::cout << "=== Memory System Statistics ===" << std::endl;
    
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        std::cout << "Working Memory:" << std::endl;
        std::cout << "  Capacity: " << workingMemory->getCapacity() << std::endl;
        std::cout << "  Current size: " << workingMemory->getCurrentSize() << std::endl;
        std::cout << "  Active traces: " << workingMemory->getActiveTraces() << std::endl;
    } else {
        std::cout << "Working Memory: Not available" << std::endl;
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "Episodic Memory:" << std::endl;
        std::cout << "  Episode count: " << episodicMemory->getEpisodeCount() << std::endl;
    } else {
        std::cout << "Episodic Memory: Not available" << std::endl;
    }
    
    auto* associativeMemory = brain->getAssociativeMemory();
    if (associativeMemory) {
        std::cout << "Associative Memory: Available" << std::endl;
    } else {
        std::cout << "Associative Memory: Not available" << std::endl;
    }
}

void runBenchmark(std::shared_ptr<Brain> brain, size_t steps) {
    std::cout << "Running performance benchmark for " << steps << " steps..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject some activity to keep the brain active
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0) {
            std::cout << "  Step " << step << "/" << steps << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Benchmark completed in " << duration.count() << " ms" << std::endl;
    std::cout << "Average time per step: " << (duration.count() / steps) << " ms" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Advanced Command Line Interface ===" << std::endl;
    
    // Parse commands
    if (argc < 2) {
        printAdvancedHelp();
        return 0;
    }
    
    std::string command = argv[1];
    
    // Create configuration
    auto config = createConsistentConfig(500);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    try {
        if (command == "--help" || command == "-h") {
            printAdvancedHelp();
        } else if (command == "--export-config" && argc > 2) {
            exportConfig(argv[2], config);
        } else if (command == "--import-config" && argc > 2) {
            importConfig(argv[2], config);
        } else if (command == "--validate-config") {
            validateConfig(config);
        } else if (command == "--memory-stats") {
            showMemoryStats(brain);
        } else if (command == "--benchmark" && argc > 2) {
            size_t steps = std::stoull(argv[2]);
            runBenchmark(brain, steps);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printAdvancedHelp();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
