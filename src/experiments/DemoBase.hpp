// Demo framework base class
// This provides a common foundation for all NLM demos, enabling
// configurable demos with standardized setup, configuration, and results collection

#pragma once

#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <functional>

namespace nlm {

// Demo metrics structure
struct DemoMetrics {
    uint64_t episode = 0;
    uint64_t steps = 0;
    float totalReward = 0.0f;
    uint64_t totalSpikes = 0;
    uint64_t finalNovelty = 0;
    uint64_t finalCuriosity = 0;
    float finalEnergy = 0.0f;
    float predictionError = 0.0f;
    uint64_t developmentalAge = 0;
    std::string developmentalStage = "Unknown";
    uint64_t totalSynapses = 0;
    float averageFiringRate = 0.0f;
    
    // Phase 5 metrics
    uint64_t forwardCount = 0;
    uint64_t backwardCount = 0;
    uint64_t turnLeftCount = 0;
    uint64_t turnRightCount = 0;
    uint64_t interactCount = 0;
    uint64_t waitCount = 0;
    uint64_t lookLeftCount = 0;
    uint64_t lookRightCount = 0;
    
    // Visit tracking
    uint64_t visitedCells = 0;
    uint64_t resourcesConsumed = 0;
    uint64_t hazardsHit = 0;
    uint64_t emptyInteracts = 0;
    uint64_t oscillations = 0;
    uint64_t tooSoonCount = 0;
    
    // Additional metrics (from Phase 5)
    uint64_t reflexBonusCount = 0;
    uint64_t reflexSuppressCount = 0;
    uint64_t wtaPulses = 0;
    uint64_t plannerOverrideCount = 0;
    uint64_t plannerAgreementCount = 0;
    uint64_t plannerDecisionCount = 0;
    uint64_t conceptBiasCount = 0;
    uint64_t selfBiasCount = 0;
    uint64_t socialBiasCount = 0;
    uint64_t conceptCount = 0;
    float selfModelConfidence = 0.0f;
    uint64_t wmTraces = 0;
};

// Base demo class
class DemoBase {
public:
    DemoBase(const std::string& name) : demoName(name) {}
    virtual ~DemoBase() {}
    
    // Run the demo with default configuration
    virtual void run() = 0;
    
    // Run the demo with specific parameters
    virtual void run(uint64_t param1, float param2) {
        // Default implementation - override in derived classes
        run();
    }
    
    // Set configuration
    void setConfig(std::shared_ptr<Config> config) {
        this->config = config;
    }
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const {
        return config;
    }
    
    // Set world
    void setWorld(std::shared_ptr<SimpleWorld> world) {
        this->world = world;
    }
    
    // Set brain
    void setBrain(std::shared_ptr<Brain> brain) {
        this->brain = brain;
    }
    
    // Set agent brain interface
    void setAgentBrain(std::shared_ptr<AgentBrain> agent) {
        this->agentBrain = agent;
    }
    
    // Get demo name
    const std::string& getName() const {
        return demoName;
    }
    
    // Initialize demo components
    virtual bool initialize() {
        if (!config) {
            config = std::make_shared<Config>();
            configureEnvironment(config);
            configureBrain(config);
        }
        
        if (!brain) {
            brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain");
                return false;
            }
        }
        
        if (!agentBrain) {
            agentBrain = std::make_shared<AgentBrain>(brain);
        }
        
        if (!world) {
            world = std::make_shared<SimpleWorld>();
            if (config) {
                float width = config->getOr<float>("environment_width", 20.0f);
                float height = config->getOr<float>("environment_height", 20.0f);
                int visionWidth = config->getOr<int>("vision_width", 16);
                int visionHeight = config->getOr<int>("vision_height", 16);
                world->configure(width, height, visionWidth, visionHeight);
                world->setRandomSeed(config->getOr<uint64_t>("random_seed", 42));
            }
        }
        
        return true;
    }
    
    // Reset demo state
    virtual void reset() {
        if (world) world->reset();
        if (agentBrain) agentBrain->reset();
        if (brain) brain->reset();
    }
    
    // Print demo header
    virtual void printHeader() {
        std::cout << "===========================================\n";
        std::cout << "NLM DEMO: " << demoName << "\n";
        std::cout << "===========================================\n\n";
    }
    
    // Print metrics for an episode
    virtual void printEpisodeMetrics(const DemoMetrics& metrics, uint64_t episode) {
        std::cout << "--- Episode " << episode << " ended ---\n";
        std::cout << "  Steps: " << metrics.steps << "\n";
        std::cout << "  Total Reward: " << metrics.totalReward << "\n";
        std::cout << "  Final Energy: " << metrics.finalEnergy << "\n";
        std::cout << "  Synapses: " << metrics.totalSynapses << "\n";
        std::cout << "  Action counts: fwd=" << metrics.forwardCount
                  << ", bwd=" << metrics.backwardCount
                  << ", left=" << metrics.turnLeftCount
                  << ", right=" << metrics.turnRightCount
                  << ", interact=" << metrics.interactCount
                  << ", wait=" << metrics.waitCount
                  << ", lookL=" << metrics.lookLeftCount
                  << ", lookR=" << metrics.lookRightCount
                  << "\n";
        std::cout << "  Phase5 metrics: visitedCells=" << metrics.visitedCells
                  << ", resources=" << metrics.resourcesConsumed
                  << ", hazards=" << metrics.hazardsHit
                  << ", emptyInteracts=" << metrics.emptyInteracts
                  << ", oscillations=" << metrics.oscillations
                  << ", finalNovelty=" << metrics.finalNovelty
                  << ", finalCuriosity=" << metrics.finalCuriosity
                  << ", tooSoon=" << metrics.tooSoonCount
                  << ", executed=" << (metrics.steps - metrics.tooSoonCount)
                  << ", reflexBonus=" << metrics.reflexBonusCount
                  << ", reflexSuppress=" << metrics.reflexSuppressCount
                  << ", wtaPulses=" << metrics.wtaPulses
                  << ", plannerOverride=" << metrics.plannerOverrideCount
                  << ", plannerAgree=" << metrics.plannerAgreementCount
                  << "/" << metrics.plannerDecisionCount
                  << ", conceptBias=" << metrics.conceptBiasCount
                  << ", selfBias=" << metrics.selfBiasCount
                  << ", socialBias=" << metrics.socialBiasCount
                  << ", concepts=" << metrics.conceptCount
                  << ", selfConf=" << metrics.selfModelConfidence
                  << ", wmTraces=" << metrics.wmTraces
                  << "\n\n";
    }
    
    // Print demo summary
    virtual void printSummary(const std::vector<DemoMetrics>& allMetrics) {
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
    }
    
    // Add demo options to command line parsing
    virtual void addDemoOptions(int& argc, char** argv, int& currentArg) {
        // Default implementation - check for demo-specific options
        while (currentArg < argc) {
            std::string arg = argv[currentArg];
            if (arg == "--help") {
                printHelp();
                exit(0);
            }
            currentArg++;
        }
    }
    
    // Print demo help
    virtual void printHelp() {
        std::cout << "Demo: " << demoName << "\n";
        std::cout << "Usage: [demo_name] [optional_parameters]\n";
    }
    
protected:
    std::string demoName;
    std::shared_ptr<Config> config;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agentBrain;
    
    // Virtual configuration methods
    virtual void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("environment_width", 20.0f);
        config->set("environment_height", 20.0f);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    }
    
    virtual void configureBrain(std::shared_ptr<Config> config) {
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001f);
    }
};