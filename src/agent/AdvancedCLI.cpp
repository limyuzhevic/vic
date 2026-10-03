#pragma once

#include "AdvancedCLI.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>

namespace nlm {

AdvancedCLI::AdvancedCLI(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent)
    : brain_(brain), agent_(agent) {}

void AdvancedCLI::run(const std::string& command) {
    const auto& subcommand = std::string();
    const auto& arg = std::string();
    run(command, subcommand, arg);
}

void AdvancedCLI::run(const std::string& command, const std::string& subcommand) {
    const auto& arg = std::string();
    run(command, subcommand, arg);
}

void AdvancedCLI::run(const std::string& command, const std::string& subcommand, const std::string& arg) {
    if (command == "inspect") {
        inspectBrain();
    } else if (command == "memory") {
        manageMemory(subcommand);
    } else if (command == "neuromod") {
        controlNeuromodulation(subcommand);
    } else if (command == "checkpoint") {
        manageCheckpoints(subcommand);
    } else if (command == "performance") {
        analyzePerformance();
    } else if (command == "config") {
        exploreConfiguration(subcommand);
    } else if (command == "replay") {
        replayExperience();
    } else if (command == "develop") {
        manageDevelopment();
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        printHelp();
    }
}

void AdvancedCLI::inspectBrain() {
    std::cout << "=== NLM Brain Inspection ===" << std::endl;
    
    // Basic statistics
    std::cout << "Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    std::cout << "Regions: " << brain_->getRegionCount() << std::endl;
    std::cout << "Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
    std::cout << "Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
    std::cout << "Pending events: " << brain_->getPendingSpikeEventCount() << std::endl;
    std::cout << "Average firing rate: " << brain_->getAverageFiringRate() << std::endl;
    std::cout << "E/I ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
    
    // Memory system status
    if (brain_->getWorkingMemory()) {
        std::cout << "Working memory traces: " << brain_->getWorkingMemory()->getActiveTraces() << std::endl;
    }
    if (brain_->getEpisodicMemory()) {
        std::cout << "Episodic episodes: " << brain_->getEpisodicMemory()->getEpisodeCount() << std::endl;
    }
    if (brain_->getPredictionSystem()) {
        std::cout << "Prediction error: " << brain_->getPredictionSystem()->getPredictionError() << std::endl;
        std::cout << "Prediction confidence: " << brain_->getPredictionSystem()->getConfidence() << std::endl;
    }
    
    // Development stage
    std::cout << "Development stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
    
    // Neuromodulation levels
    if (brain_->getDopamine()) {
        std::cout << "Dopamine level: " << brain_->getDopamine()->getLevel() << std::endl;
    }
    if (brain_->getCuriosity()) {
        std::cout << "Curiosity level: " << brain_->getCuriosity()->getLevel() << std::endl;
    }
    if (brain_->getNovelty()) {
        std::cout << "Novelty level: " << brain_->getNovelty()->getLevel() << std::endl;
    }
    
    std::cout << "=== Inspection Complete ===" << std::endl;
}

void AdvancedCLI::manageMemory(const std::string& subcommand) {
    if (subcommand == "status") {
        if (brain_->getWorkingMemory()) {
            std::cout << "Working memory capacity: " << brain_->getWorkingMemory()->getCapacity() << std::endl;
            std::cout << "Working memory traces: " << brain_->getWorkingMemory()->getActiveTraces() << std::endl;
        }
        if (brain_->getEpisodicMemory()) {
            std::cout << "Episodic memory max episodes: " << brain_->getEpisodicMemory()->getMaxEpisodes() << std::endl;
            std::cout << "Episodic episodes stored: " << brain_->getEpisodicMemory()->getEpisodeCount() << std::endl;
            std::cout << "Episodic replay interval: " << brain_->getReplayInterval() << std::endl;
        }
        if (brain_->getAssociativeMemory()) {
            std::cout << "Associative memory facts: " << brain_->getAssociativeMemory()->getFactCount() << std::endl;
        }
    } else if (subcommand == "consolidate") {
        float threshold = 0.5f;
        if (brain_->getEpisodicMemory()) {
            brain_->getEpisodicMemory()->consolidate(threshold);
            std::cout << "Consolidated episodic memory with threshold: " << threshold << std::endl;
        }
    } else if (subcommand == "replay") {
        size_t count = 3;
        if (brain_->getEpisodicMemory()) {
            brain_->getEpisodicMemory()->replayRecent(count);
            std::cout << "Replayed " << count << " recent episodes" << std::endl;
        }
    } else {
        std::cerr << "Unknown memory subcommand: " << subcommand << std::endl;
    }
}

void AdvancedCLI::controlNeuromodulation(const std::string& subcommand) {
    if (subcommand == "set") {
        // Parse dopamine, curiosity, novelty levels from additional args
        std::cout << "Neuromodulation control not fully implemented yet" << std::endl;
    } else if (subcommand == "status") {
        if (brain_->getDopamine()) {
            std::cout << "Dopamine: " << brain_->getDopamine()->getLevel() << std::endl;
            std::cout << "Plasticity factor: " << brain_->getDopamine()->getPlasticityFactor() << std::endl;
        }
        if (brain_->getCuriosity()) {
            std::cout << "Curiosity: " << brain_->getCuriosity()->getLevel() << std::endl;
        }
        if (brain_->getNovelty()) {
            std::cout << "Novelty: " << brain_->getNovelty()->getLevel() << std::endl;
        }
    } else {
        std::cerr << "Unknown neuromodulation subcommand: " << subcommand << std::endl;
    }
}

void AdvancedCLI::manageCheckpoints(const std::string& subcommand) {
    if (subcommand == "save") {
        std::string path = "checkpoint.bin";
        if (brain_->save(path)) {
            std::cout << "Brain saved to: " << path << std::endl;
        } else {
            std::cerr << "Failed to save brain state" << std::endl;
        }
    } else if (subcommand == "load") {
        std::string path = "checkpoint.bin";
        if (brain_->load(path)) {
            std::cout << "Brain loaded from: " << path << std::endl;
        } else {
            std::cerr << "Failed to load brain state" << std::endl;
        }
    } else if (subcommand == "list") {
        std::cout << "Checkpoint management not fully implemented" << std::endl;
    } else {
        std::cerr << "Unknown checkpoint subcommand: " << subcommand << std::endl;
    }
}

void AdvancedCLI::analyzePerformance() {
    std::cout << "=== Performance Analysis ===" << std::endl;
    // Simplified performance metrics
    std::cout << "Performance analysis not fully implemented" << std::endl;
    std::cout << "=== Analysis Complete ===" << std::endl;
}

void AdvancedCLI::exploreConfiguration(const std::string& subcommand) {
    if (subcommand == "show") {
        std::cout << "Configuration exploration not fully implemented" << std::endl;
    } else {
        std::cerr << "Unknown config subcommand: " << subcommand << std::endl;
    }
}

void AdvancedCLI::replayExperience() {
    std::cout << "Experience replay not fully implemented" << std::endl;
}

void AdvancedCLI::manageDevelopment() {
    std::cout << "Development management not fully implemented" << std::endl;
}

void AdvancedCLI::printHelp() {
    std::cout << "Advanced NLM CLI Commands:" << std::endl;
    std::cout << "  inspect             - Inspect brain state and statistics" << std::endl;
    std::cout << "  memory <subcommand> - Manage memory systems" << std::endl;
    std::cout << "  neuromod <subcommand> - Control neuromodulators" << std::endl;
    std::cout << "  checkpoint <subcommand> - Manage checkpoints" << std::endl;
    std::cout << "  performance         - Analyze performance metrics" << std::endl;
    std::cout << "  config <subcommand>   - Explore configuration" << std::endl;
    std::cout << "  replay              - Replay experiences" << std::endl;
    std::cout << "  develop             - Manage development stages" << std::endl;
    std::cout << std::endl;
    std::cout << "Memory subcommands: status, consolidate, replay" << std::endl;
    std::cout << "Neuromodulation subcommands: status, set" << std::endl;
    std::cout << "Checkpoint subcommands: save, load, list" << std::endl;
    std::cout << "Config subcommands: show, edit" << std::endl;
}

} // namespace nlm
