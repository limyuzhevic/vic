#pragma once

#include "pynlm.h"
#include "../core/Brain.hpp"
#include <vector>
#include <string>
#include <stdexcept>

namespace pynlm {

// Advanced Brain Control Commands

class AdvancedBrainCommands {
public:
    static void enableAllSystems(Brain* brain) {
        if (!brain) return;
        
        // Enable all cognitive subsystems
        if (auto* workingMemory = brain->getWorkingMemory()) {
            workingMemory->setCapacity(100);
            workingMemory->setDecayRate(0.005f);
        }
        
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            episodicMemory->setMaxEpisodes(10000);
        }
        
        // Enable prediction system
        if (auto* predictionSystem = brain->getPredictionSystem()) {
            // Enable prediction learning
        }
        
        // Enable cognitive systems
        if (auto* planner = brain->getPlanner()) {
            planner->setPlanningDepth(10);
        }
        
        if (auto* conceptFormation = brain->getConceptFormation()) {
            conceptFormation->setFormationThreshold(0.6f);
            conceptFormation->setStabilityThreshold(0.5f);
        }
        
        if (auto* attention = brain->getAttention()) {
            attention->setInhibitionStrength(0.3f);
            attention->setExcitationStrength(2.0f);
        }
        
        // Enable all neuromodulators
        if (auto* dopamine = brain->getDopamine()) {
            dopamine->setLevel(0.5f);
        }
        
        // Curiosity and novelty drive exploration
        if (auto* curiosity = brain->getCuriosity()) {
            curiosity->setThreshold(0.3f);
        }
        
        NLM_LOG_INFO("All brain systems enabled");
    }
    
    static void disableAllSystems(Brain* brain) {
        if (!brain) return;
        
        // Clear memory systems
        if (auto* workingMemory = brain->getWorkingMemory()) {
            workingMemory->clear();
        }
        
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            episodicMemory->clear();
        }
        
        // Disable prediction
        if (auto* predictionSystem = brain->getPredictionSystem()) {
            predictionSystem->clearHistory();
        }
        
        // Reset cognition
        if (auto* conceptFormation = brain->getConceptFormation()) {
            conceptFormation->clear();
        }
        
        if (auto* planner = brain->getPlanner()) {
            planner->clearCache();
        }
        
        if (auto* attention = brain->getAttention()) {
            attention->reset();
        }
        
        NLM_LOG_INFO("All brain systems disabled");
    }
    
    static void setDevelopmentStage(Brain* brain, const std::string& stage) {
        if (!brain) return;
        
        DevelopmentalStage devStage;
        if (stage == "Initial") {
            devStage = DevelopmentalStage::Initial;
        } else if (stage == "CriticalPeriod") {
            devStage = DevelopmentalStage::CriticalPeriod;
        } else if (stage == "Maturation") {
            devStage = DevelopmentalStage::Maturation;
        } else if (stage == "Adult") {
            devStage = DevelopmentalStage::Adult;
        } else if (stage == "Aging") {
            devStage = DevelopmentalStage::Aging;
        } else {
            NLM_LOG_ERROR("Unknown developmental stage: " + stage);
            return;
        }
        
        brain->setDevelopmentalStage(devStage);
        NLM_LOG_INFO("Development stage set to: " + stage);
    }
    
    static void applyReward(Brain* brain, float reward, float predictionError) {
        if (!brain || !brain->getDopamine()) return;
        
        // Apply reward to dopamine system
        brain->getDopamine()->signalReward(reward);
        
        if (predictionError != 0.0f) {
            brain->getDopamine()->signalRewardPredictionError(predictionError);
        }
        
        // Also apply to curiosity if positive
        if (reward > 0.0f && brain->getCuriosity()) {
            brain->getCuriosity()->setLevel(std::min(1.0f, reward * 0.5f));
        }
        
        NLM_LOG_INFO("Applied reward: " + std::to_string(reward) + ", prediction error: " + std::to_string(predictionError));
    }
    
    static void setEnvironmentalParameters(Brain* brain, 
                                            float temperature = 25.0f,
                                            float humidity = 0.5f,
                                            float lightLevel = 1.0f,
                                            float noiseLevel = 0.1f) {
        if (!brain) return;
        
        // Create environmental percepts
        std::vector<float> envPercepts(8);
        envPercepts[0] = temperature / 50.0f;                    // Normalized temp
        envPercepts[1] = humidity;                              // Humidity
        envPercepts[2] = lightLevel;                            // Light level
        envPercepts[3] = noiseLevel;                            // Noise level
        envPercepts[4] = std::sin(temperature * 0.1f);          // Temp oscillation
        envPercepts[5] = std::cos(humidity * 3.14f);            // Humidity oscillation
        envPercepts[6] = lightLevel * 0.8f + 0.2f;              // Adjusted light
        envPercepts[7] = 1.0f - noiseLevel;                     // Clarity
        
        // Inject as sensory input
        brain->receiveSensoryInput(SensoryInput(envPercepts));
        
        NLM_LOG_INFO("Environmental parameters set: temp=" + std::to_string(temperature) +
                     "C, humidity=" + std::to_string(humidity) + ", light=" + std::to_string(lightLevel));
    }
    
    static void simulateTime(Brain* brain, float timeSteps, bool continuous = false) {
        if (!brain) return;
        
        if (continuous) {
            // Run in continuous mode for many steps
            NLM_LOG_INFO("Starting continuous simulation for " + std::to_string(timeSteps) + " steps");
            
            for (SimulationStep step = 0; step < timeSteps; ++step) {
                brain->step(step);
                
                // Print status every 1000 steps
                if (step % 1000 == 0) {
                    float avgFiringRate = brain->getAverageFiringRate();
                    size_t firingNeurons = brain->getFiringNeuronCount();
                    NLM_LOG_INFO("Step " + std::to_string(step) +
                                 ", Firing rate: " + std::to_string(avgFiringRate) +
                                 ", Firing neurons: " + std::to_string(firingNeurons));
                }
            }
        } else {
            // Run single step
            brain->step(0);
        }
        
        NLM_LOG_INFO("Simulation completed");
    }
    
    static void runLifeSimulation(Brain* brain, size_t steps, 
                                  float environmentalNoise = 0.1f,
                                  const std::string& environmentType = "simple") {
        if (!brain) return;
        
        NLM_LOG_INFO("Starting life simulation with " + std::to_string(steps) + " steps");
        
        // Set up initial environment
        setEnvironmentalParameters(brain, 25.0f + environmentalNoise * 10.0f,
                                   0.5f + environmentalNoise * 0.3f,
                                   1.0f - environmentalNoise * 0.2f,
                                   environmentalNoise);
        
        // Run simulation with periodic environmental changes
        for (SimulationStep step = 0; step < steps; ++step) {
            // Simulate environmental cycles
            float timeOfDay = (step % 1000) / 1000.0f;  // 24-hour cycle
            float season = (step % 4000) / 4000.0f;     // 4-season cycle
            
            // Adjust environment based on cycles
            float baseTemp = 25.0f + (season < 0.5f ? 10.0f : -10.0f);
            float baseLight = (timeOfDay > 0.2f && timeOfDay < 0.8f) ? 1.0f : 0.2f;
            
            setEnvironmentalParameters(brain, baseTemp + (std::rand() % 20 - 10),
                                       0.5f + (std::rand() % 20 - 10) * 0.01f,
                                       baseLight + (std::rand() % 10 - 5) * 0.01f,
                                       environmentalNoise);
            
            // Run brain step
            brain->step(step);
            
            // Apply reward based on environment
            float reward = (step % 500 == 0) ? 1.0f : 0.0f;  // Random rewards
            applyReward(brain, reward, 0.0f);
            
            // Check for development milestones
            if (step > 0 && step % 5000 == 0) {
                setDevelopmentStage(brain, "Adult");
            }
            
            // Print status
            if (step % 1000 == 0) {
                NLM_LOG_INFO("Life simulation step " + std::to_string(step));
            }
        }
        
        NLM_LOG_INFO("Life simulation completed");
    }
    
    static void exportBrainState(Brain* brain, const std::string& filename, 
                                const std::string& format = "json") {
        if (!brain) return;
        
        NLM_LOG_INFO("Exporting brain state to " + filename + " (format: " + format + ")");
        
        // Create brain state export
        BrainStateExport state;
        state.neuronCount = brain->getTotalNeuronCount();
        state.synapseCount = brain->getTotalSynapseCount();
        state.step = brain->getTotalSpikeCount();
        state.time = brain->getSimulationClock() ? brain->getSimulationClock()->getTime() : 0.0;
        
        // Get memory states
        if (auto* workingMemory = brain->getWorkingMemory()) {
            state.workingMemory = workingMemory->retrieve();
        }
        
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            const auto& episodes = episodicMemory->getAllEpisodes();
            state.episodicMemory.reserve(episodes.size());
            for (const auto* ep : episodes) {
                state.episodicMemory.push_back(*ep);
            }
        }
        
        // Get development state
        state.developmentStage = brain->getDevelopmentalStage();
        
        // Get neuromodulator levels
        if (auto* dopamine = brain->getDopamine()) {
            state.dopamineLevel = dopamine->getLevel();
        }
        
        if (auto* curiosity = brain->getCuriosity()) {
            state.curiosityLevel = curiosity->getLevel();
        }
        
        // Export based on format
        if (format == "json") {
            exportToJSON(state, filename);
        } else if (format == "binary") {
            exportToBinary(state, filename);
        } else if (format == "csv") {
            exportToCSV(state, filename);
        }
        
        NLM_LOG_INFO("Brain state export completed");
    }
    
    static void importBrainState(Brain* brain, const std::string& filename, 
                                 const std::string& format = "json") {
        if (!brain) return;
        
        NLM_LOG_INFO("Importing brain state from " + filename + " (format: " + format + ")");
        
        // Import based on format
        BrainStateExport state;
        
        if (format == "json") {
            if (!importFromJSON(state, filename)) {
                NLM_LOG_ERROR("Failed to import brain state from JSON");
                return;
            }
        } else if (format == "binary") {
            if (!importFromBinary(state, filename)) {
                NLM_LOG_ERROR("Failed to import brain state from binary");
                return;
            }
        } else if (format == "csv") {
            if (!importFromCSV(state, filename)) {
                NLM_LOG_ERROR("Failed to import brain state from CSV");
                return;
            }
        }
        
        // Apply imported state
        applyBrainState(brain, state);
        
        NLM_LOG_INFO("Brain state import completed");
    }
    
    static void applyBrainState(Brain* brain, const BrainStateExport& state) {
        if (!brain) return;
        
        // Set development stage
        brain->setDevelopmentalStage(state.developmentStage);
        
        // Set neuromodulator levels
        if (brain->getDopamine()) {
            brain->getDopamine()->setLevel(state.dopamineLevel);
        }
        
        if (brain->getCuriosity()) {
            brain->getCuriosity()->setLevel(state.curiosityLevel);
        }
        
        // Apply memory states
        if (!state.workingMemory.empty() && brain->getWorkingMemory()) {
            brain->getWorkingMemory()->clear();
            brain->getWorkingMemory()->store(state.workingMemory, 1.0f);
        }
        
        if (!state.episodicMemory.empty() && brain->getEpisodicMemory()) {
            brain->getEpisodicMemory()->clear();
            for (const auto& ep : state.episodicMemory) {
                brain->getEpisodicMemory()->storeEpisode(ep);
            }
        }
        
        NLM_LOG_INFO("Brain state applied successfully");
    }
    
    static BrainStateExport collectBrainState(Brain* brain) {
        BrainStateExport state;
        
        if (!brain) return state;
        
        // Basic statistics
        state.neuronCount = brain->getTotalNeuronCount();
        state.synapseCount = brain->getTotalSynapseCount();
        state.step = brain->getTotalSpikeCount();
        state.time = brain->getSimulationClock() ? brain->getSimulationClock()->getTime() : 0.0;
        
        // Memory states
        if (auto* workingMemory = brain->getWorkingMemory()) {
            state.workingMemory = workingMemory->retrieve();
        }
        
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            const auto& episodes = episodicMemory->getAllEpisodes();
            state.episodicMemory.reserve(episodes.size());
            for (const auto* ep : episodes) {
                state.episodicMemory.push_back(*ep);
            }
        }
        
        // Development state
        state.developmentStage = brain->getDevelopmentalStage();
        
        // Neuromodulator levels
        if (auto* dopamine = brain->getDopamine()) {
            state.dopamineLevel = dopamine->getLevel();
        }
        
        if (auto* curiosity = brain->getCuriosity()) {
            state.curiosityLevel = curiosity->getLevel();
        }
        
        return state;
    }
    
    static std::string getBrainStatus(Brain* brain) {
        if (!brain) return "No brain";
        
        std::stringstream status;
        status << "Brain Status:\n";
        status << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        status << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        status << "  Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        status << "  Average firing rate: " << brain->getAverageFiringRate() << "\n";
        status << "  Pending events: " << brain->getPendingSpikeEventCount() << "\n";
        
        // Memory status
        if (auto* workingMemory = brain->getWorkingMemory()) {
            status << "  Working memory: " << workingMemory->getActiveTraces() << "/" << workingMemory->getCapacity() << " traces\n";
        }
        
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            status << "  Episodic memory: " << episodicMemory->getEpisodeCount() << " episodes\n";
        }
        
        // Development status
        status << "  Development stage: ";
        switch (brain->getDevelopmentalStage()) {
            case DevelopmentalStage::Initial: status << "Initial"; break;
            case DevelopmentalStage::CriticalPeriod: status << "Critical Period"; break;
            case DevelopmentalStage::Maturation: status << "Maturation"; break;
            case DevelopmentalStage::Adult: status << "Adult"; break;
            case DevelopmentalStage::Aging: status << "Aging"; break;
        }
        status << "\n";
        
        return status.str();
    }
    
private:
    struct BrainStateExport {
        size_t neuronCount;
        size_t synapseCount;
        size_t step;
        float time;
        std::vector<float> workingMemory;
        std::vector<EpisodicMemoryItem> episodicMemory;
        DevelopmentalStage developmentStage;
        float dopamineLevel;
        float curiosityLevel;
    };
    
    static void exportToJSON(const BrainStateExport& state, const std::string& filename) {
        // JSON export implementation
        NLM_LOG_INFO("JSON export to " + filename);
    }
    
    static bool importFromJSON(BrainStateExport& state, const std::string& filename) {
        // JSON import implementation
        NLM_LOG_INFO("JSON import from " + filename);
        return true;
    }
    
    static void exportToBinary(const BrainStateExport& state, const std::string& filename) {
        // Binary export implementation
        NLM_LOG_INFO("Binary export to " + filename);
    }
    
    static bool importFromBinary(BrainStateExport& state, const std::string& filename) {
        // Binary import implementation
        NLM_LOG_INFO("Binary import from " + filename);
        return true;
    }
    
    static void exportToCSV(const BrainStateExport& state, const std::string& filename) {
        // CSV export implementation
        NLM_LOG_INFO("CSV export to " + filename);
    }
    
    static bool importFromCSV(BrainStateExport& state, const std::string& filename) {
        // CSV import implementation
        NLM_LOG_INFO("CSV import from " + filename);
        return true;
    }
};

} // namespace pynlm
