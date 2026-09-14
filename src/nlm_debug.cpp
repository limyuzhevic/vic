// NLM Advanced Debug Command
// Provides detailed output for simulation debugging

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>

using namespace nlm;

struct DebugInfo {
    SimulationStep step;
    double time;
    size_t firingNeurons;
    size_t totalSpikes;
    float avgFiringRate;
    float avgSynapticWeight;
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    std::vector<float> neuronPotentials;
    std::vector<float> synapticWeights;
    std::vector<float> neuromodulators;
};

DebugInfo collectDebugInfo(std::shared_ptr<Brain> brain) {
    DebugInfo info;
    info.step = 0;  // Would be current step
    info.time = 0.0;  // Would be current time
    info.firingNeurons = brain->getFiringNeuronCount();
    info.totalSpikes = brain->getTotalSpikeCount();
    info.avgFiringRate = brain->getAverageFiringRate();
    info.avgSynapticWeight = 0.0f;  // Would need to calculate
    info.dopamineLevel = 0.0f;     // Would come from AgentBrain
    info.curiosityLevel = 0.0f;     // Would come from AgentBrain
    info.noveltyLevel = 0.0f;       // Would come from AgentBrain
    info.predictionError = 0.0f;    // Would come from AgentBrain
    
    // Collect sample neuron data
    if (brain->getTotalNeuronCount() > 0) {
        info.neuronPotentials = std::vector<float>(std::min(size_t(100), brain->getTotalNeuronCount()), 0.0f);
        for (size_t i = 0; i < info.neuronPotentials.size(); ++i) {
            // Would need to access individual neuron
            info.neuronPotentials[i] = 0.0f;
        }
    }
    
    // Collect sample synaptic data
    if (brain->getTotalSynapseCount() > 0) {
        info.synapticWeights = std::vector<float>(std::min(size_t(100), brain->getTotalSynapseCount()), 0.0f);
        for (size_t i = 0; i < info.synapticWeights.size(); ++i) {
            // Would need to access individual synapse
            info.synapticWeights[i] = 0.5f;
        }
    }
    
    // Collect neuromodulator levels
    info.neuromodulators = std::vector<float>(4, 0.0f);  // Dopamine, Serotonin, etc.
    
    return info;
}

void printDebugHeader() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                    NLM DEBUG OUTPUT                         ║
╠═══════════════════════════════════════════════════════════════╣
║                                                               ║
║  DEBUG MODE: DETAILED SIMULATION OUTPUT                      ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void printStepDebug(const DebugInfo& info, int step) {
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "[STEP " << step << "] Time: " << info.time << "s | "
              << "Firing: " << info.firingNeurons << " | "
              << "Total Spikes: " << info.totalSpikes << " | "
              << "Firing Rate: " << info.avgFiringRate << std::endl;
    
    if (info.firingNeurons > 0) {
        std::cout << "  Neuron Potentials (sample): ";
        for (size_t i = 0; i < info.neuronPotentials.size(); ++i) {
            std::cout << info.neuronPotentials[i];
            if (i < info.neuronPotentials.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    
    if (!info.synapticWeights.empty()) {
        std::cout << "  Synaptic Weights (sample): min=" << *std::min_element(info.synapticWeights.begin(), info.synapticWeights.end())
                  << ", max=" << *std::max_element(info.synapticWeights.begin(), info.synapticWeights.end())
                  << ", avg=" << std::accumulate(info.synapticWeights.begin(), info.synapticWeights.end(), 0.0f) / info.synapticWeights.size()
                  << std::endl;
    }
    
    std::cout << "  Neuromodulators: Dopamine=" << info.dopamineLevel 
              << ", Curiosity=" << info.curiosityLevel
              << ", Novelty=" << info.noveltyLevel
              << ", Prediction Error=" << info.predictionError << std::endl;
    std::cout << std::endl;
}

void saveDebugInfo(const DebugInfo& info, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "DEBUG INFO" << std::endl;
        file << "=========" << std::endl;
        file << "Step: " << info.step << std::endl;
        file << "Time: " << info.time << std::endl;
        file << "Firing Neurons: " << info.firingNeurons << std::endl;
        file << "Total Spikes: " << info.totalSpikes << std::endl;
        file << "Average Firing Rate: " << info.avgFiringRate << std::endl;
        file << "Average Synaptic Weight: " << info.avgSynapticWeight << std::endl;
        file << "Dopamine Level: " << info.dopamineLevel << std::endl;
        file << "Curiosity Level: " << info.curiosityLevel << std::endl;
        file << "Novelty Level: " << info.noveltyLevel << std::endl;
        file << "Prediction Error: " << info.predictionError << std::endl;
        file << "Neuron Potentials (sample):";
        for (float v : info.neuronPotentials) {
            file << " " << v;
        }
        file << std::endl;
        file << "Synaptic Weights (sample):";
        for (float v : info.synapticWeights) {
            file << " " << v;
        }
        file << std::endl;
        file << "Neuromodulators:";
        for (float v : info.neuromodulators) {
            file << " " << v;
        }
        file << std::endl;
        file.close();
        std::cout << "Debug info saved to: " << filename << std::endl;
    } else {
        std::cerr << "ERROR: Could not open file for writing: " << filename << std::endl;
    }
}

int main(int argc, char** argv) {
    printDebugHeader();
    
    // Parse debug options
    bool fileOutput = false;
    std::string outputFile;
    bool continuousMode = false;
    size_t stepInterval = 1;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--file" || arg == "-f") {
            fileOutput = true;
            if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else if (arg == "--continuous" || arg == "-c") {
            continuousMode = true;
        } else if (arg == "--interval") {
            if (i + 1 < argc) {
                stepInterval = std::stoul(argv[++i]);
            }
        }
    }
    
    // Initialize logger with debug level
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize brain with default config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    NLM_LOG_INFO("Debug mode initialized - detailed output enabled");
    
    // Create debug log file if needed
    std::ofstream debugFile;
    if (fileOutput && !outputFile.empty()) {
        debugFile.open(outputFile);
        if (debugFile.is_open()) {
            debugFile << "NLM DEBUG SESSION" << std::endl;
            debugFile << "================" << std::endl;
            debugFile << "Start time: " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
        } else {
            std::cerr << "ERROR: Could not open debug file: " << outputFile << std::endl;
        }
    }
    
    // Run simulation with debug output
    for (SimulationStep step = 0; ; ++step) {
        // Check for exit condition
        if (step >= 1000 && !continuousMode) {
            break;
        }
        
        // Run one simulation step
        brain->step(step, step * 0.001);
        
        // Collect debug information
        DebugInfo info = collectDebugInfo(brain);
        info.step = step;
        info.time = step * 0.001;
        
        // Print debug info
        printStepDebug(info, step);
        
        // Save to file if requested
        if (fileOutput && debugFile.is_open()) {
            debugFile << "--- STEP " << step << " ---" << std::endl;
            debugFile << "Time: " << info.time << std::endl;
            debugFile << "Firing Neurons: " << info.firingNeurons << std::endl;
            debugFile << "Total Spikes: " << info.totalSpikes << std::endl;
            debugFile << "Firing Rate: " << info.avgFiringRate << std::endl;
            debugFile << "Neuromodulators: " << info.dopamineLevel << " " 
                      << info.curiosityLevel << " " << info.noveltyLevel << " " << info.predictionError << std::endl;
            debugFile << std::endl;
        }
        
        // Print progress
        if (step % stepInterval == 0) {
            std::cout << "Progress: Step " << step << " completed" << std::endl;
        }
        
        // Check for exit
        if (step % 100 == 0 && !continuousMode) {
            std::cout << "Press Ctrl+C to exit or let simulation complete..." << std::endl;
        }
    }
    
    if (debugFile.is_open()) {
        debugFile << "\nSimulation complete." << std::endl;
        debugFile.close();
    }
    
    NLM_LOG_INFO("Debug simulation complete.");
    
    return 0;
}
