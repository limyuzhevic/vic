// Base Experiment Framework for NLM Phase 3-6
// Eliminates code duplication across experiment demo files

#pragma once

#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "motor/Action.hpp"
#include "development/DevelopmentSystem.hpp"

namespace nlm {

// ===== BASE EXPERIMENT CLASS =====
class ExperimentBase {
protected:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agentBrain;
    std::shared_ptr<SimpleWorld> world;
    
    // Metrics collection
    struct ExperimentMetrics {
        int episode = 0;
        int steps = 0;
        float totalReward = 0.0f;
        float finalEnergy = 0.0f;
        float finalNovelty = 0.0f;
        float finalCuriosity = 0.0f;
        float predictionError = 0.0f;
        float developmentalAge = 0.0f;
        std::string developmentalStage = "Initial";
        size_t totalSynapses = 0;
        float averageWeight = 0.0f;
        float maxWeight = 0.0f;
        size_t totalSpikes = 0;
        float averageFiringRate = 0.0f;
        int forwardCount = 0;
        int backwardCount = 0;
        int turnLeftCount = 0;
        int turnRightCount = 0;
        int interactCount = 0;
        int waitCount = 0;
        
        void reset() {
            episode = steps = 0;
            totalReward = finalEnergy = finalNovelty = finalCuriosity = predictionError = developmentalAge = 0.0f;
            developmentalStage = "Initial";
            totalSynapses = 0;
            averageWeight = maxWeight = 0.0f;
            totalSpikes = averageFiringRate = 0;
            forwardCount = backwardCount = turnLeftCount = turnRightCount = interactCount = waitCount = 0;
        }
        
        std::string toCSV() const {
            std::ostringstream oss;
            oss << episode << "," << steps << "," << totalReward << "," << finalEnergy << "," << finalNovelty << "," << finalCuriosity << "," << predictionError << "," << developmentalAge << "," << developmentalStage << "," << totalSynapses << "," << averageWeight << "," << maxWeight << ",\"" << totalSpikes << "\"" << averageFiringRate << "," << forwardCount << "," << backwardCount << "," << turnLeftCount << "," << turnRightCount << "," << interactCount << "," << waitCount;
            return oss.str();
        }
    };
    
    std::vector<ExperimentMetrics> allMetrics;
    
public:
    ExperimentBase() {}
    virtual ~ExperimentBase() {}
    
    // Initialize all core components
    void initialize() {
        config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return;
        }
        
        world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->setRandomSeed(42);
        
        agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        // Configure standard agent settings
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        
        printStatus();
    }
    
    // Common configuration methods (to be overridden)
    virtual void configureEnvironment(std::shared_ptr<Config> config) {}
    virtual void configureBrain(std::shared_ptr<Config> config) {}
    
    // Main experiment execution method (to be overridden)
    virtual void runExperiment(int numEpisodes, int stepsPerEpisode) = 0;
    
    // Standard action string conversion
    const char* motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
    
    // Standard developmental stage string conversion
    const char* stageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    // Print initial status
    void printStatus() {
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain->getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain->getMotorOutputSize() << "\n\n";
    }
    
    // Print experiment summary
    void printSummary() {
        std::cout << "===========================================\n";
        std::cout << "SUMMARY\n";
        std::cout << "===========================================\n";
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):\n";
            std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << "\n";
            std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << "\n";
            std::cout << "  Exploration (wait): " << first.waitCount << " -> " << last.waitCount << "\n";
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            std::cout << "\nBehavioral change:\n";
            if (rewardChange > 0) {
                std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)\n";
            } else if (rewardChange < -5) {
                std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)\n";
            }
            
            if (std::abs(actionChange) > 10) {
                std::cout << "  Forward actions changed by " << actionChange << "\n";
            }
            
            // Synapse change
            if (last.totalSynapses != first.totalSynapses) {
                std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << "\n";
            }
        }
        
        std::cout << "\nExperiment complete.\n";
    }
};

// ===== BASE DEMO CLASS =====
class BaseDemo : public ExperimentBase {
public:
    BaseDemo() : ExperimentBase() {}
    virtual ~BaseDemo() {}
    
    // Main demo runner with configurable episode/step count
    void runDemo(int numEpisodes = 5, int stepsPerEpisode = 200) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3-6 DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Initialize components using base class
        initialize();
        
        // Run the actual demo using base experiment functionality
        runExperiment(numEpisodes, stepsPerEpisode);
        
        // Print summary
        printSummary();
        
        std::cout << "\nPhase 3-6 demonstration complete.\n";
    }
    
protected:
    // Override to provide specific configuration
    void configureEnvironment(std::shared_ptr<Config> config) override {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    }
    
    void configureBrain(std::shared_ptr<Config> config) override {
        config->set("neuron_count", size_t(500));
        config->set("region_count", size_t(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", uint64_t(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
    }
};

// ===== EXPERIMENT LOGGER =====
class ExperimentLogger {
protected:
    std::ofstream logFile;
    bool isOpen;
    
public:
    ExperimentLogger(const std::string& filename) : isOpen(false) {
        logFile.open(filename);
        if (logFile.is_open()) {
            isOpen = true;
        }
    }
    
    virtual ~ExperimentLogger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    void log(const std::string& message) {
        if (isOpen) {
            logFile << message << std::endl;
        }
        std::cout << message << std::endl;
    }
    
    void logEpisodeStart(int episode) {
        log("--- Episode " + std::to_string(episode) + " started ---");
    }
    
    void logEpisodeEnd(int episode, float totalReward, int steps, float finalNovelty) {
        std::ostringstream oss;
        oss << "--- Episode " << episode << " ended ---\n"
            << "  Steps: " << steps << "\n"
            << "  Total Reward: " << totalReward << "\n"
            << "  Final Novelty: " << finalNovelty << "\n"
            << "-------------------------------------";
        log(oss.str());
    }
    
    void flush() {
        if (isOpen) {
            logFile.flush();
        }
    }
};

} // namespace nlm
