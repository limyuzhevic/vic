#include "tools/CommandLineTool.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cmath>

BrainManager::BrainManager() : isRunning_(false), isPaused_(false), 
                              stepCount_(0), totalTime_(0.0) {}

BrainManager::~BrainManager() {}

bool BrainManager::createBrain(const std::string& configPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        config_ = std::make_unique<Config>();
        
        // Load configuration from file if provided
        if (!configPath.empty()) {
            if (!config_->loadFromFile(configPath)) {
                std::cerr << "Warning: Failed to load config from " << configPath << ". Using defaults." << std::endl;
            }
        }
        
        // Set default values for Phase 6
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        config_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config_->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Create brain
        brain_ = std::make_unique<Brain>(config_);
        if (!brain_->initialize()) {
            std::cerr << "Error: Failed to initialize brain!" << std::endl;
            return false;
        }
        
        isRunning_ = false;
        isPaused_ = false;
        stepCount_ = 0;
        totalTime_ = 0.0;
        startTime_ = std::chrono::high_resolution_clock::now();
        
        std::cout << "Brain created successfully:" << std::endl;
        std::cout << "  Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "  Regions: " << brain_->getRegionCount() << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error creating brain: " << e.what() << std::endl;
        return false;
    }
}

bool BrainManager::loadBrain(const std::string& checkpointPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) {
        std::cerr << "Error: No brain exists. Create a brain first." << std::endl;
        return false;
    }
    
    if (brain_->load(checkpointPath)) {
        std::cout << "Brain loaded successfully from: " << checkpointPath << std::endl;
        isRunning_ = false;
        isPaused_ = false;
        stepCount_ = 0;
        totalTime_ = 0.0;
        startTime_ = std::chrono::high_resolution_clock::now();
        return true;
    }
    
    std::cerr << "Error: Failed to load brain from: " << checkpointPath << std::endl;
    return false;
}

bool BrainManager::saveBrain(const std::string& checkpointPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) {
        std::cerr << "Error: No brain exists. Create a brain first." << std::endl;
        return false;
    }
    
    if (brain_->save(checkpointPath)) {
        std::cout << "Brain saved successfully to: " << checkpointPath << std::endl;
        return true;
    }
    
    std::cerr << "Error: Failed to save brain to: " << checkpointPath << std::endl;
    return false;
}

bool BrainManager::runSimulation(int steps, double timeStep) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) {
        std::cerr << "Error: No brain exists. Create a brain first." << std::endl;
        return false;
    }
    
    if (isRunning_) {
        std::cout << "Simulation is already running." << std::endl;
        return false;
    }
    
    if (steps < 0) {
        // Run indefinitely until interrupted
        std::cout << "Starting continuous simulation... (Press Ctrl+C to stop)" << std::endl;
        isRunning_ = true;
        
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (isRunning_ && !isPaused_) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration<double>(currentTime - lastTime).count();
            
            if (elapsed >= timeStep) {
                brain_->step(stepCount_, totalTime_);
                stepCount_++;
                totalTime_ += timeStep;
                lastTime = currentTime;
                
                if (stepCount_ % 1000 == 0) {
                    std::cout << "Step " << stepCount_ << ", Time: " << std::fixed << std::setprecision(3) 
                             << totalTime_ << "s, Rate: " << (1.0 / timeStep) << " Hz" << std::endl;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        isRunning_ = false;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime_).count();
        std::cout << "Simulation stopped. Total time: " << duration << "s, Steps: " << stepCount_ << std::endl;
        
    } else {
        // Run fixed number of steps
        std::cout << "Running " << steps << " simulation steps..." << std::endl;
        
        for (int i = 0; i < steps; ++i) {
            brain_->step(i, i * timeStep);
            
            if ((i + 1) % 1000 == 0) {
                std::cout << "Step " << (i + 1) << "/" << steps << ", Time: " << std::fixed << std::setprecision(3) 
                         << (i + 1) * timeStep << "s, Rate: " << (1.0 / timeStep) << " Hz" << std::endl;
            }
        }
        
        stepCount_ += steps;
        totalTime_ += steps * timeStep;
        std::cout << "Simulation completed: " << steps << " steps, Total time: " << totalTime_ << "s" << std::endl;
    }
    
    return true;
}

bool BrainManager::stepSimulation() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) {
        std::cerr << "Error: No brain exists. Create a brain first." << std::endl;
        return false;
    }
    
    brain_->step(stepCount_, totalTime_);
    stepCount_++;
    totalTime_ += config_->getOr<double>("simulation_timestep", 0.001);
    
    std::cout << "Completed step " << stepCount_ - 1 << ", Total time: " << totalTime_ << "s" << std::endl;
    return true;
}

bool BrainManager::resetBrain() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) {
        std::cerr << "Error: No brain exists. Create a brain first." << std::endl;
        return false;
    }
    
    brain_->reset();
    brain_->initialize();
    stepCount_ = 0;
    totalTime_ = 0.0;
    isPaused_ = false;
    
    std::cout << "Brain reset and re-initialized." << std::endl;
    return true;
}

bool BrainManager::pauseBrain() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (isRunning_) {
        isPaused_ = true;
        std::cout << "Simulation paused." << std::endl;
        return true;
    }
    
    std::cout << "Simulation is not running." << std::endl;
    return false;
}

bool BrainManager::resumeBrain() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (isPaused_) {
        isPaused_ = false;
        std::cout << "Simulation resumed." << std::endl;
        return true;
    }
    
    std::cout << "Simulation is not paused." << std::endl;
    return false;
}

bool BrainManager::exportBrain(const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) {
        std::cerr << "Error: No brain exists. Create a brain first." << std::endl;
        return false;
    }
    
    // Create JSON export
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create export file: " << outputPath << std::endl;
        return false;
    }
    
    file << "{\n";
    file << "  \"brain_info\": {\n";
    file << "    \"neuron_count\": " << brain_->getTotalNeuronCount() << ",\n";
    file << "    \"synapse_count\": " << brain_->getTotalSynapseCount() << ",\n";
    file << "    \"region_count\": " << brain_->getRegionCount() << ",\n";
    file << "    \"active_neuron_count\": " << brain_->getActiveNeuronCount() << ",\n";
    file << "    \"firing_rate\": " << brain_->getAverageFiringRate() << ",\n";
    file << "    \"total_spikes\": " << brain_->getTotalSpikeCount() << ",\n";
    file << "    \"simulation_steps\": " << stepCount_ << ",\n";
    file << "    \"simulation_time\": " << totalTime_ << "\n";
    file << "  },\n";
    
    // Export region information
    file << "  \"regions\": [\n";
    auto regionIds = brain_->getRegionIds();
    for (size_t i = 0; i < regionIds.size(); ++i) {
        auto* region = brain_->getRegion(regionIds[i]);
        if (region) {
            file << "    {\n";
            file << "      \"id\": " << regionIds[i].index() << ",\n";
            file << "      \"name\": \"Region_" << regionIds[i].index() << "\",\n";
            file << "      \"neuron_count\": " << region->getTotalNeuronCount() << ",\n";
            file << "      \"population_count\": " << region->getPopulationCount() << ",\n";
            file << "      \"synapse_count\": " << region->getSynapseCount() << ",\n";
            file << "      \"average_weight\": " << region->getAverageSynapticWeight() << "\n";
            
            if (i < regionIds.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
    }
    file << "  ],\n";
    
    // Export neuron statistics
    file << "  \"neurons_by_type\": {\n";
    file << "    \"sensory\": 0,\n";
    file << "    \"internal\": 0,\n";
    file << "    \"motor\": 0\n";
    file << "  },\n";
    
    file << "}\n";
    file.close();
    
    std::cout << "Brain exported to: " << outputPath << std::endl;
    return true;
}

bool BrainManager::analyzeBrain(const std::string& analysisType) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) {
        std::cerr << "Error: No brain exists. Create a brain first." << std::endl;
        return false;
    }
    
    std::cout << "=== Brain Analysis Report ===" << std::endl;
    std::cout << "Brain Analysis Type: " << analysisType << std::endl;
    std::cout << "Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    std::cout << "Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
    std::cout << "Average Firing Rate: " << brain_->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
    std::cout << "Simulation Steps: " << stepCount_ << std::endl;
    std::cout << "Simulation Time: " << totalTime_ << "s" << std::endl;
    std::cout << "E/I Ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
    
    if (analysisType == "full" || analysisType == "memory") {
        auto* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            std::cout << "Working Memory Traces: " << workingMemory->getActiveTraces() << std::endl;
        }
        
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "Episodic Memory Episodes: " << episodicMemory->getEpisodeCount() << std::endl;
        }
        
        auto* associativeMemory = brain_->getAssociativeMemory();
        if (associativeMemory) {
            std::cout << "Associative Memory Connections: " << associativeMemory->getConnectionCount() << std::endl;
        }
    }
    
    if (analysisType == "full" || analysisType == "neuromodulation") {
        auto* dopamine = brain_->getDopamine();
        if (dopamine) {
            std::cout << "Dopamine Level: " << dopamine->getLevel() << std::endl;
        }
        
        auto* curiosity = brain_->getCuriosity();
        if (curiosity) {
            std::cout << "Curiosity Level: " << curiosity->getLevel() << std::endl;
        }
        
        auto* novelty = brain_->getNovelty();
        if (novelty) {
            std::cout << "Novelty Level: " << novelty->getLevel() << std::endl;
        }
        
        auto* predictionError = brain_->getPredictionErrorSignal();
        if (predictionError) {
            std::cout << "Prediction Error Level: " << predictionError->getLevel() << std::endl;
        }
    }
    
    if (analysisType == "full" || analysisType == "development") {
        std::cout << "Developmental Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
    }
    
    std::cout << "=== Analysis Complete ===" << std::endl;
    return true;
}

bool BrainManager::debugBrain(const std::string& debugLevel) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) {
        std::cerr << "Error: No brain exists. Create a brain first." << std::endl;
        return false;
    }
    
    std::cout << "=== Debug Information ===" << std::endl;
    std::cout << "Debug Level: " << debugLevel << std::endl;
    std::cout << "Brain Status:" << std::endl;
    brain_->logStatus();
    
    if (debugLevel == "detailed" || debugLevel == "full") {
        std::cout << "\n=== Detailed Debug Information ===" << std::endl;
        
        // Memory system debug
        auto* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            std::cout << "Working Memory Details:" << std::endl;
            std::cout << "  Capacity: " << workingMemory->getCapacity() << std::endl;
            std::cout << "  Active Traces: " << workingMemory->getActiveTraces() << std::endl;
            std::cout << "  Memory Trace Density: " << (workingMemory->getActiveTraces() / workingMemory->getCapacity()) << std::endl;
        }
        
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "Episodic Memory Details:" << std::endl;
            std::cout << "  Max Episodes: " << episodicMemory->getMaxEpisodes() << std::endl;
            std::cout << "  Episode Count: " << episodicMemory->getEpisodeCount() << std::endl;
            std::cout << "  Replay Interval: " << episodicMemory->getReplayInterval() << std::endl;
        }
        
        auto* associativeMemory = brain_->getAssociativeMemory();
        if (associativeMemory) {
            std::cout << "Associative Memory Details:" << std::endl;
            std::cout << "  Connection Count: " << associativeMemory->getConnectionCount() << std::endl;
            std::cout << "  Pattern Count: " << associativeMemory->getPatternCount() << std::endl;
        }
        
        // Neuromodulation debug
        auto* dopamine = brain_->getDopamine();
        if (dopamine) {
            std::cout << "Dopamine System Details:" << std::endl;
            std::cout << "  Level: " << dopamine->getLevel() << std::endl;
            std::cout << "  Plasticity Factor: " << dopamine->getPlasticityFactor() << std::endl;
        }
        
        auto* curiosity = brain_->getCuriosity();
        if (curiosity) {
            std::cout << "Curiosity System Details:" << std::endl;
            std::cout << "  Level: " << curiosity->getLevel() << std::endl;
            std::cout << "  Exploration Rate: " << curiosity->getExplorationRate() << std::endl;
        }
        
        auto* novelty = brain_->getNovelty();
        if (novelty) {
            std::cout << "Novelty System Details:" << std::endl;
            std::cout << "  Level: " << novelty->getLevel() << std::endl;
            std::cout << "  Novelty Threshold: " << novelty->getNoveltyThreshold() << std::endl;
        }
        
        // Development debug
        std::cout << "Development System Details:" << std::endl;
        std::cout << "  Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
        auto* devSystem = brain_->getDevelopmentSystem();
        if (devSystem) {
            std::cout << "  Synaptogenesis Rate: " << devSystem->getSynaptogenesisRate() << std::endl;
            std::cout << "  Pruning Rate: " << devSystem->getPruningRate() << std::endl;
        }
        
        // Performance debug
        std::cout << "Performance Metrics:" << std::endl;
        std::cout << "  Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
        std::cout << "  Pending Spike Events: " << brain_->getPendingSpikeEventCount() << std::endl;
        std::cout << "  Average Firing Rate: " << brain_->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Current Step: " << stepCount_ << std::endl;
        std::cout << "  Total Time: " << totalTime_ << "s" << std::endl;
    }
    
    std::cout << "=== Debug Complete ===" << std::endl;
    return true;
}

std::string BrainManager::getStatusReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream report;
    report << "=== NLM Brain Status Report ===\n";
    report << "Brain State: " << (brain_ ? "Active" : "Not Created") << "\n";
    if (brain_) {
        report << "  Neurons: " << brain_->getTotalNeuronCount() << "\n";
        report << "  Synapses: " << brain_->getTotalSynapseCount() << "\n";
        report << "  Regions: " << brain_->getRegionCount() << "\n";
        report << "  Active Neurons: " << brain_->getActiveNeuronCount() << "\n";
        report << "  Total Spikes: " << brain_->getTotalSpikeCount() << "\n";
        report << "  Average Firing Rate: " << brain_->getAverageFiringRate() << " Hz\n";
        report << "  Simulation Steps: " << stepCount_ << "\n";
        report << "  Simulation Time: " << totalTime_ << "s\n";
        report << "  E/I Ratio: " << brain_->getExcitationInhibitionRatio() << "\n";
    }
    report << "Running: " << (isRunning_ ? "Yes" : "No") << "\n";
    report << "Paused: " << (isPaused_ ? "Yes" : "No") << "\n";
    report << "=== End Status Report ===\n";
    
    return report.str();
}

std::string BrainManager::getPerformanceStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream stats;
    stats << "=== NLM Brain Performance Statistics ===\n";
    
    if (brain_) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(now - startTime_).count();
        
        stats << "Performance Metrics:\n";
        stats << "  Total Time: " << totalTime_ << "s\n";
        stats << "  Wall Clock Time: " << elapsed << "s\n";
        stats << "  Step Count: " << stepCount_ << "\n";
        if (elapsed > 0.0) {
            stats << "  Steps per Second: " << (stepCount_ / elapsed) << " Hz\n";
        }
        if (totalTime_ > 0.0) {
            stats << "  Simulation Speed Factor: " << (stepCount_ * config_->getOr<double>("simulation_timestep", 0.001) / totalTime_) << "x\n";
        }
        
        stats << "\nMemory Statistics:\n";
        stats << "  Total Neurons: " << brain_->getTotalNeuronCount() << "\n";
        stats << "  Total Synapses: " << brain_->getTotalSynapseCount() << "\n";
        stats << "  Active Neurons: " << brain_->getActiveNeuronCount() << "\n";
        stats << "  Neuron Utilization: " << (brain_->getActiveNeuronCount() / brain_->getTotalNeuronCount() * 100) << "%\n";
        
        auto* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            stats << "\nWorking Memory:\n";
            stats << "  Active Traces: " << workingMemory->getActiveTraces() << "\n";
            stats << "  Capacity: " << workingMemory->getCapacity() << "\n";
            stats << "  Utilization: " << (workingMemory->getActiveTraces() / workingMemory->getCapacity() * 100) << "%\n";
        }
        
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (episodicMemory) {
            stats << "\nEpisodic Memory:\n";
            stats << "  Episodes: " << episodicMemory->getEpisodeCount() << "\n";
            stats << "  Max Episodes: " << episodicMemory->getMaxEpisodes() << "\n";
        }
        
        stats << "\nNetwork Statistics:\n";
        stats << "  E/I Ratio: " << brain_->getExcitationInhibitionRatio() << "\n";
        stats << "  Firing Rate: " << brain_->getAverageFiringRate() << " Hz\n";
        stats << "  Connectivity Density: " << (brain_->getTotalSynapseCount() / (brain_->getTotalNeuronCount() * brain_->getTotalNeuronCount())) << "\n";
    }
    
    stats << "=== End Performance Statistics ===\n";
    
    return stats.str();
}

std::vector<std::string> BrainManager::getAvailableCommands() const {
    return {
        "help", "brain create", "brain load", "brain save", "brain step", "brain run", 
        "brain reset", "brain pause", "brain resume", "brain export", "brain analyze", "brain debug",
        "config show", "config set", "config load", "config save",
        "stats", "trace", "compare", "replay", "trace start", "trace stop", "trace export",
        "explore", "explore regions", "explore neurons", "explore synapses",
        "memory", "memory working", "memory episodic", "memory associative",
        "neuromodulation", "neuromodulation dopamine", "neuromodulation curiosity", 
        "development", "development stage", "development effects",
        "plot", "plot neurons", "plot synapses", "plot spikes",
        "script", "script run", "script save", "script load",
        "batch", "batch step", "batch run", "batch export",
        "help brain", "help simulation", "help analysis", "help debug", "help config", "help export"
    };
}