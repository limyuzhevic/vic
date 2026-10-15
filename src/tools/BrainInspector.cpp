#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>

// NLM core includes
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

using namespace nlm;

// Simple brain state visualization
class BrainVisualizer {
public:
    void visualizeBrain(const Brain& brain, std::ostream& out = std::cout) {
        out << "=== NLM Brain State Visualization ===\n\n";
        
        // Basic statistics
        out << "Brain Statistics:" << std::endl;
        out << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
        out << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
        out << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
        out << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
        out << "  Total Spike Count: " << brain.getTotalSpikeCount() << std::endl;
        out << "  E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        out << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        out << "  Developmental Stage: " << getDevelopmentalStageName(brain.getDevelopmentalStage()) << std::endl;
        
        // Region information
        out << "\nBrain Regions:" << std::endl;
        auto regionIds = brain.getRegionIds();
        for (auto regionId : regionIds) {
            auto* region = brain.getRegion(regionId);
            if (region) {
                out << "  Region " << regionId.value() << ": ";
                out << region->getName() << " (" << region->getTotalNeuronCount() << " neurons, " << region->getPopulationCount() << " populations)" << std::endl;
            }
        }
        
        // Memory system status
        out << "\nMemory Systems:" << std::endl;
        auto* workingMemory = brain.getWorkingMemory();
        if (workingMemory) {
            out << "  Working Memory: Active" << std::endl;
        } else {
            out << "  Working Memory: Not Available" << std::endl;
        }
        
        auto* episodicMemory = brain.getEpisodicMemory();
        if (episodicMemory) {
            out << "  Episodic Memory: Active (" << episodicMemory->getEpisodeCount() << " episodes)" << std::endl;
        } else {
            out << "  Episodic Memory: Not Available" << std::endl;
        }
        
        // Neuromodulation status
        out << "\nNeuromodulation:" << std::endl;
        auto* dopamine = brain.getDopamine();
        if (dopamine) {
            out << "  Dopamine: Active (level: " << dopamine->getLevel() << ")" << std::endl;
        } else {
            out << "  Dopamine: Not Available" << std::endl;
        }
        
        auto* curiosity = brain.getCuriosity();
        if (curiosity) {
            out << "  Curiosity: Active (level: " << curiosity->getLevel() << ")" << std::endl;
        } else {
            out << "  Curiosity: Not Available" << std::endl;
        }
        
        auto* novelty = brain.getNovelty();
        if (novelty) {
            out << "  Novelty: Active (level: " << novelty->getLevel() << ")" << std::endl;
        } else {
            out << "  Novelty: Not Available" << std::endl;
        }
        
        auto* predictionError = brain.getPredictionErrorSignal();
        if (predictionError) {
            out << "  Prediction Error: Active (level: " << predictionError->getLevel() << ")" << std::endl;
        } else {
            out << "  Prediction Error: Not Available" << std::endl;
        }
        
        // Plasticity status
        out << "\nPlasticity Systems:" << std::endl;
        auto* stdp = brain.getSTDP();
        if (stdp) {
            out << "  STDP: Active" << std::endl;
        } else {
            out << "  STDP: Not Available" << std::endl;
        }
        
        auto* hebbian = brain.getHebbian();
        if (hebbian) {
            out << "  Hebbian: Active" << std::endl;
        } else {
            out << "  Hebbian: Not Available" << std::endl;
        }
        
        auto* structural = brain.getStructuralPlasticity();
        if (structural) {
            out << "  Structural Plasticity: Active" << std::endl;
        } else {
            out << "  Structural Plasticity: Not Available" << std::endl;
        }
        
        // Development status
        out << "\nDevelopment:" << std::endl;
        auto* development = brain.getDevelopmentSystem();
        if (development) {
            out << "  Development System: Active" << std::endl;
        } else {
            out << "  Development System: Not Available" << std::endl;
        }
    }
    
private:
    std::string getDevelopmentalStageName(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "Critical Period";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
};

// Agent Inspector - inspects agent-brain interface with world
class AgentInspector {
public:
    void inspectAgent(const Brain& brain, const SimpleWorld& world, std::ostream& out = std::cout) {
        out << "=== Agent-Brain Interface Inspection ===\n\n";
        
        // Create agent brain interface
        auto agentBrain = std::make_shared<AgentBrain>(std::make_shared<Brain>(brain));
        
        // Agent statistics
        out << "Agent Statistics:" << std::endl;
        out << "  Sensory Input Size: " << agentBrain->getSensoryInputSize() << std::endl;
        out << "  Motor Output Size: " << agentBrain->getMotorOutputSize() << std::endl;
        out << "  Reward Modulation: " << (agentBrain->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
        out << "  Structural Plasticity: " << (agentBrain->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
        out << "  Development: " << (agentBrain->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
        out << "  Curiosity: " << (agentBrain->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
        
        // Current agent state
        out << "\nCurrent Agent State:" << std::endl;
        out << "  Neuromodulation Level: " << agentBrain->getNeuromodulationLevel() << std::endl;
        out << "  Curiosity Level: " << agentBrain->getCuriosityLevel() << std::endl;
        out << "  Novelty Level: " << agentBrain->getNoveltyLevel() << std::endl;
        out << "  Prediction Error: " << agentBrain->getPredictionError() << std::endl;
        out << "  Developmental Stage: " << getDevelopmentalStageName(agentBrain->getDevelopmentalStage()) << std::endl;
        
        // World state
        out << "\nWorld State:" << std::endl;
        out << "  Width: " << world.getWidth() << std::endl;
        out << "  Height: " << world.getHeight() << std::endl;
        out << "  Simulation Time: " << world.getSimulationTime() << std::endl;
        out << "  Agent Position: (" << world.getAgentBody().x << ", " << world.getAgentBody().y << ")" << std::endl;
        out << "  Agent Orientation: " << world.getAgentBody().orientation << " rad" << std::endl;
        out << "  Agent Energy: " << world.getAgentBody().energy << std::endl;
        out << "  Max Energy: " << world.getMaxEnergy() << std::endl;
    }
    
private:
    std::string getDevelopmentalStageName(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "Critical Period";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
};

// Main inspection function
void inspectBrain(const std::string& configFile = "", bool verbose = false) {
    std::cout << "=== NLM Brain Inspector ===\n\n";
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    try {
        // Load configuration
        auto config = std::make_shared<Config>();
        if (!configFile.empty()) {
            if (!config->loadFromFile(configFile)) {
                std::cerr << "Warning: Could not load configuration from " << configFile << std::endl;
                std::cerr << "Using default configuration.\n" << std::endl;
            }
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain!" << std::endl;
            return;
        }
        
        // Create simple world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        // Run a few steps for inspection
        std::cout << "Running simulation for inspection..." << std::endl;
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
            
            if (verbose && step % 20 == 0) {
                std::cout << "Step " << step << std::endl;
            }
        }
        
        // Perform inspection
        BrainVisualizer visualizer;
        visualizer.visualizeBrain(*brain);
        
        AgentInspector agentInspector;
        agentInspector.inspectAgent(*brain, *world);
        
        std::cout << "\n=== Inspection Complete ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during inspection: " << e.what() << std::endl;
        return;
    }
}

int main(int argc, char* argv[]) {
    std::string configFile = "";
    bool verbose = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--help") {
            std::cout << "Usage: nlm_inspect [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --config <file>    Configuration file" << std::endl;
            std::cout << "  --verbose          Verbose output" << std::endl;
            std::cout << "  --help             Show this help message" << std::endl;
            return 0;
        }
    }
    
    inspectBrain(configFile, verbose);
    
    return 0;
}