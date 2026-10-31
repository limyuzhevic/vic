#pragma once

/**
 * Common utilities for NLM demos
 * Consolidates duplicated code and provides shared functionality
 */

#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <sstream>

namespace nlm {

/**
 * Common configuration helper functions
 * Consolidates configuration setup that's duplicated across demos
 */
class DemoUtils {
public:
    /**
     * Create a standard configuration for demos
     * Used by Phase 3, Phase 5, and other demos
     */
    static std::shared_ptr<Config> makeConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        return config;
    }
    
    /**
     * Configure environment settings
     * Standard environment setup for most demos
     */
    static void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    }
    
    /**
     * Configure brain settings
     * Standard brain configuration for demos
     */
    static void configureBrain(std::shared_ptr<Config> config) {
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
    }
    
    /**
     * Create a standard percept for demos
     * Used by Phase 5 probe pattern function
     */
    static SensoryPercept makePercept(const std::vector<float>& vision,
                                    const std::vector<float>& touch) {
        SensoryPercept p;
        p.setVision(vision);
        p.setTouch(touch);
        p.setInternal({1.0f, 1.0f, 0.0f, 0.0f});
        p.setProprioception({0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f});
        p.setAudio({});
        return p;
    }
    
    /**
     * Convert MotorCommand to string
     * Common function used across demos
     */
    static const char* motorCommandToString(MotorCommand cmd) {
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
    
    /**
     * Convert DevelopmentalStage to string
     * Common function for development stage reporting
     */
    static const char* stageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    /**
     * Configure agent brain with common subsystems
     * Sets up standard agent configuration for demos
     */
    static void configureAgent(AgentBrain& agent, bool enableReward = true,
                             bool enableCuriosity = true, bool enableDevelopment = true,
                             bool enableStructuralPlasticity = true) {
        if (enableReward) {
            agent.enableRewardModulation(true);
        }
        if (enableCuriosity) {
            agent.enableCuriosity(true);
        }
        if (enableDevelopment) {
            agent.enableDevelopment(true);
        }
        if (enableStructuralPlasticity) {
            agent.enableStructuralPlasticity(true);
        }
    }
    
    /**
     * Standard world setup for demos
     * Creates and configures a SimpleWorld with common settings
     */
    static SimpleWorld makeWorld(int width = 20, int height = 20,
                                int visionWidth = 16, int visionHeight = 16,
                                uint64_t seed = 42) {
        SimpleWorld world;
        world.configure(static_cast<float>(width), static_cast<float>(height),
                       visionWidth, visionHeight);
        world.setRandomSeed(seed);
        return world;
    }
    
    /**
     * Create agent brain for a demo
     * Standard agent creation with common initialization
     */
    static AgentBrain makeAgent(std::shared_ptr<Brain> brain, SimpleWorld& world,
                              bool enableReward = true, bool enableCuriosity = true,
                              bool enableDevelopment = true, bool enableStructuralPlasticity = true) {
        AgentBrain agent(brain);
        agent.initialize(world);
        configureAgent(agent, enableReward, enableCuriosity, enableDevelopment, enableStructuralPlasticity);
        return agent;
    }
    
    /**
     * Run common metrics tracking for demo episodes
     * Extracts common metrics collection logic
     */
    struct DemoMetrics {
        int episodes = 0;
        float totalReward = 0.0f;
        size_t totalSpikes = 0;
        size_t totalSynapses = 0;
        float averageFiringRate = 0.0f;
        int forwardCount = 0;
        int interactCount = 0;
        
        void reset() {
            totalReward = 0.0f;
            totalSpikes = 0;
            totalSynapses = 0;
            averageFiringRate = 0.0f;
            forwardCount = 0;
            interactCount = 0;
        }
        
        void update(float reward, size_t spikes, size_t synapses,
                   float firingRate, int forward, int interact) {
            totalReward += reward;
            totalSpikes += spikes;
            totalSynapses = synapses;
            averageFiringRate = firingRate;
            forwardCount += forward;
            interactCount += interact;
        }
        
        std::string getSummary() const {
            std::ostringstream oss;
            oss << "Episodes: " << episodes
                << " | Total Reward: " << totalReward
                << " | Total Spikes: " << totalSpikes
                << " | Synapses: " << totalSynapses
                << " | Avg Firing Rate: " << averageFiringRate
                << " | Forward Actions: " << forwardCount
                << " | Interact Actions: " << interactCount;
            return oss.str();
        }
    };
    
    /**
     * Print demonstration summary
     * Common summary printing for demo results
     */
    static void printDemoSummary(const DemoMetrics& metrics, const std::string& title) {
        std::cout << "===========================================\n";
        std::cout << title << std::endl;
        std::cout << "===========================================\n\n";
        std::cout << metrics.getSummary() << std::endl;
        std::cout << "\n";
    }
    
    /**
     * Get current timestamp as string
     */
    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

/**
 * Performance measurement utilities
 * Tools for benchmarking and profiling
 */
class PerformanceUtils {
public:
    /**
     * Measure execution time of a function
     */
    template<typename Func, typename... Args>
    static double measureTime(Func&& func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        func(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    /**
     * Format milliseconds for display
     */
    static std::string formatTime(double ms) {
        if (ms < 1.0) {
            return std::to_string(ms) + " ms";
        } else if (ms < 1000.0) {
            return std::to_string(ms) + " ms";
        } else {
            return std::to_string(ms / 1000.0) + " s";
        }
    }
    
    /**
     * Calculate average of values
     */
    template<typename T>
    static double calculateAverage(const std::vector<T>& values) {
        if (values.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& v : values) sum += v;
        return sum / values.size();
    }
    
    /**
     * Calculate standard deviation
     */
    template<typename T>
    static double calculateStdDev(const std::vector<T>& values, double mean) {
        if (values.size() <= 1) return 0.0;
        double sumSq = 0.0;
        for (const auto& v : values) {
            sumSq += (v - mean) * (v - mean);
        }
        return std::sqrt(sumSq / (values.size() - 1));
    }
};

/**
 * Debug and logging utilities
 * Enhanced logging and debugging capabilities
 */
class DebugUtils {
public:
    /**
     * Log with timestamp
     */
    static void logWithTimestamp(const std::string& message) {
        std::cout << "[" << DemoUtils::getTimestamp() << "] " << message << std::endl;
    }
    
    /**
     * Log level comparison
     */
    enum class LogLevel {
        Error,
        Warning,
        Info,
        Debug,
        Trace
    };
    
    /**
     * Log with level
     */
    static void logWithLevel(LogLevel level, const std::string& message) {
        std::string levelStr;
        switch (level) {
            case LogLevel::Error: levelStr = "ERROR"; break;
            case LogLevel::Warning: levelStr = "WARNING"; break;
            case LogLevel::Info: levelStr = "INFO"; break;
            case LogLevel::Debug: levelStr = "DEBUG"; break;
            case LogLevel::Trace: levelStr = "TRACE"; break;
        }
        std::cout << "[" << levelStr << "] " << message << std::endl;
    }
    
    /**
     * Log brain state snapshot
     */
    static void logBrainState(const std::shared_ptr<Brain>& brain, const std::string& label) {
        std::cout << "=== Brain State: " << label << " ===" << std::endl;
        std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Pending Spike Events: " << brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "=====================" << std::endl;
    }
};

} // namespace nlm
