// Enhanced logging and diagnostics system for NLM

#pragma once

#include "../src/core/Logger/Logger.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

class DiagnosticReporter {
public:
    struct SystemDiagnostics {
        std::string timestamp;
        std::string hostname;
        std::string version;
        uint64_t processId;
        size_t availableMemory;
        size_t totalMemory;
        float cpuUsage;
        int cpuCores;
    };
    
    struct PerformanceDiagnostics {
        double simulationTime;
        double cpuTime;
        uint64_t stepsPerSecond;
        uint64_t neuronCount;
        uint64_t spikeCount;
        float avgFiringRate;
        float avgSynapticWeight;
        float memoryUsagePercent;
        std::unordered_map<std::string, double> componentTimes;
    };
    
    struct MemoryDiagnostics {
        size_t workingMemoryUsage;
        size_t episodicMemoryUsage;
        size_t semanticMemoryUsage;
        size_t proceduralMemoryUsage;
        size_t associativeMemoryUsage;
        size_t totalMemoryUsage;
        size_t maxMemoryUsage;
        std::vector<std::string> memoryIssues;
    };
    
    struct NeuralDiagnostics {
        size_t totalNeuronCount;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float excitationInhibitionRatio;
        size_t synapseCount;
        float averageMembranePotential;
        float averageThreshold;
        std::vector<std::string> neuralIssues;
    };
    
    struct WorldDiagnostics {
        float simulationTime;
        float worldWidth;
        float worldHeight;
        size_t visionWidth;
        size_t visionHeight;
        float energyDecayRate;
        float maxEnergy;
        size_t activeObjects;
        std::vector<std::string> worldIssues;
    };
    
    struct AgentDiagnostics {
        float curiosityLevel;
        float noveltyLevel;
        float dopamineLevel;
        float predictionError;
        float rewardHistory;
        float developmentalStage;
        std::vector<std::string> agentIssues;
    };
    
    struct SystemHealth {
        std::string overallStatus;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        std::vector<std::string> recommendations;
        double healthScore; // 0.0 to 1.0
    };
    
    DiagnosticReporter() = default;
    ~DiagnosticReporter() = default;
    
    /**
     * Collect system diagnostics
     */
    static SystemDiagnostics collectSystemDiagnostics() {
        SystemDiagnostics diag;
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        diag.timestamp = ss.str();
        
        diag.hostname = getHostname();
        diag.version = getVersion();
        diag.processId = getProcessId();
        diag.cpuCores = getCpuCores();
        
        auto memInfo = getMemoryInfo();
        diag.availableMemory = memInfo.first;
        diag.totalMemory = memInfo.second;
        
        diag.cpuUsage = getCpuUsage();
        
        return diag;
    }
    
    /**
     * Collect performance diagnostics
     */
    static PerformanceDiagnostics collectPerformanceDiagnostics(const std::shared_ptr<Brain>& brain,
                                                              uint64_t steps,
                                                              double totalTime) {
        PerformanceDiagnostics diag;
        
        diag.simulationTime = totalTime;
        diag.stepsPerSecond = steps / (totalTime > 0 ? totalTime : 1.0);
        diag.neuronCount = brain->getTotalNeuronCount();
        diag.spikeCount = brain->getTotalSpikeCount();
        diag.avgFiringRate = brain->getAverageFiringRate();
        diag.avgSynapticWeight = brain->getExcitationInhibitionRatio();
        
        diag.memoryUsagePercent = getMemoryUsagePercent();
        
        // Collect component times
        diag.componentTimes["neural_simulation"] = totalTime * 0.7; // Estimate
        diag.componentTimes["memory_system"] = totalTime * 0.2; // Estimate
        diag.componentTimes["plasticity"] = totalTime * 0.05; // Estimate
        diag.componentTimes["other"] = totalTime * 0.05; // Estimate
        
        return diag;
    }
    
    /**
     * Collect memory diagnostics
     */
    static MemoryDiagnostics collectMemoryDiagnostics() {
        MemoryDiagnostics diag;
        
        // Get memory usage from system
        auto memInfo = getMemoryInfo();
        diag.totalMemoryUsage = memInfo.second;
        diag.maxMemoryUsage = memInfo.second * 0.9; // Estimate
        
        // Estimate memory usage by component
        diag.workingMemoryUsage = diag.totalMemoryUsage * 0.3;
        diag.episodicMemoryUsage = diag.totalMemoryUsage * 0.25;
        diag.semanticMemoryUsage = diag.totalMemoryUsage * 0.2;
        diag.proceduralMemoryUsage = diag.totalMemoryUsage * 0.15;
        diag.associativeMemoryUsage = diag.totalMemoryUsage * 0.1;
        
        return diag;
    }
    
    /**
     * Collect neural diagnostics
     */
    static NeuralDiagnostics collectNeuralDiagnostics(const std::shared_ptr<Brain>& brain) {
        NeuralDiagnostics diag;
        
        diag.totalNeuronCount = brain->getTotalNeuronCount();
        diag.activeNeuronCount = brain->getActiveNeuronCount();
        diag.firingNeuronCount = brain->getFiringNeuronCount();
        diag.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        diag.synapseCount = brain->getTotalSynapseCount();
        
        // Get average membrane potential and threshold
        // This would require accessing brain internals
        diag.averageMembranePotential = 0.0f; // Estimate
        diag.averageThreshold = 0.0f; // Estimate
        
        return diag;
    }
    
    /**
     * Collect world diagnostics
     */
    static WorldDiagnostics collectWorldDiagnostics(const std::shared_ptr<SimpleWorld>& world) {
        WorldDiagnostics diag;
        
        if (world) {
            diag.simulationTime = world->getSimulationTime();
            diag.worldWidth = world->getWidth();
            diag.worldHeight = world->getHeight();
            diag.visionWidth = world->getVisionWidth();
            diag.visionHeight = world->getVisionHeight();
            diag.energyDecayRate = world->getEnergyDecayRate();
            diag.maxEnergy = world->getMaxEnergy();
            diag.activeObjects = world->getActiveObjectCount(); // Assuming this method exists
        }
        
        return diag;
    }
    
    /**
     * Collect agent diagnostics
     */
    static AgentDiagnostics collectAgentDiagnostics(const std::shared_ptr<AgentBrain>& agent) {
        AgentDiagnostics diag;
        
        if (agent) {
            diag.curiosityLevel = agent->getCuriosityLevel();
            diag.noveltyLevel = agent->getNoveltyLevel();
            diag.dopamineLevel = agent->getNeuromodulationLevel();
            diag.predictionError = agent->getPredictionError();
            diag.developmentalStage = static_cast<float>(agent->getDevelopmentalStage());
        }
        
        return diag;
    }
    
    /**
     * Generate comprehensive system report
     */
    static std::string generateSystemReport(const SystemDiagnostics& system,
                                           const PerformanceDiagnostics& performance,
                                           const MemoryDiagnostics& memory,
                                           const NeuralDiagnostics& neural,
                                           const WorldDiagnostics& world,
                                           const AgentDiagnostics& agent) {
        std::stringstream report;
        
        report << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        report << "║                             NLM SYSTEM DIAGNOSTIC REPORT                        ║" << std::endl;
        report << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        report << std::endl;
        
        report << "🖥️ SYSTEM INFORMATION:" << std::endl;
        report << "  Timestamp:     " << system.timestamp << std::endl;
        report << "  Hostname:      " << system.hostname << std::endl;
        report << "  Process ID:    " << system.processId << std::endl;
        report << "  Version:       " << system.version << std::endl;
        report << "  CPU Cores:     " << system.cpuCores << std::endl;
        report << "  CPU Usage:     " << std::fixed << std::setprecision(1) << system.cpuUsage << "%" << std::endl;
        report << "  Memory:        " << formatMemory(system.availableMemory) << " / " 
               << formatMemory(system.totalMemory) << std::endl;
        report << std::endl;
        
        report << "⚡ PERFORMANCE METRICS:" << std::endl;
        report << "  Simulation Time:     " << std::fixed << std::setprecision(3) << performance.simulationTime << "s" << std::endl;
        report << "  Steps/sec:           " << performance.stepsPerSecond << std::endl;
        report << "  Neurons:             " << performance.neuronCount << std::endl;
        report << "  Total Spikes:        " << performance.spikeCount << std::endl;
        report << "  Avg Firing Rate:     " << std::fixed << std::setprecision(2) << performance.avgFiringRate << " Hz" << std::endl;
        report << "  Avg Synaptic Weight: " << std::fixed << std::setprecision(4) << performance.avgSynapticWeight << std::endl;
        report << "  Memory Usage:        " << std::fixed << std::setprecision(1) << performance.memoryUsagePercent << "%" << std::endl;
        report << std::endl;
        
        report << "🧠 NEURAL SYSTEM:" << std::endl;
        report << "  Total Neurons:       " << neural.totalNeuronCount << std::endl;
        report << "  Active Neurons:      " << neural.activeNeuronCount << std::endl;
        report << "  Firing Neurons:     " << neural.firingNeuronCount << std::endl;
        report << "  Excitation/Inhibition: " << std::fixed << std::setprecision(3) << neural.excitationInhibitionRatio << ":1" << std::endl;
        report << "  Total Synapses:      " << neural.synapseCount << std::endl;
        report << std::endl;
        
        report << "🌍 WORLD SYSTEM:" << std::endl;
        report << "  Simulation Time:     " << std::fixed << std::setprecision(3) << world.simulationTime << "s" << std::endl;
        report << "  World Size:          " << world.worldWidth << " x " << world.worldHeight << std::endl;
        report << "  Vision:              " << world.visionWidth << " x " << world.visionHeight << std::endl;
        report << "  Energy Decay:        " << std::fixed << std::setprecision(4) << world.energyDecayRate << "/s" << std::endl;
        report << "  Max Energy:          " << world.maxEnergy << std::endl;
        report << "  Active Objects:      " << world.activeObjects << std::endl;
        report << std::endl;
        
        report << "🧠 AGENT SYSTEM:" << std::endl;
        report << "  Curiosity Level:     " << std::fixed << std::setprecision(3) << agent.curiosityLevel << std::endl;
        report << "  Novelty Level:       " << std::fixed << std::setprecision(3) << agent.noveltyLevel << std::endl;
        report << "  Dopamine Level:      " << std::fixed << std::setprecision(3) << agent.dopamineLevel << std::endl;
        report << "  Prediction Error:    " << std::fixed << std::setprecision(3) << agent.predictionError << std::endl;
        report << "  Developmental Stage: " << agent.developmentalStage << std::endl;
        report << std::endl;
        
        report << "💾 MEMORY SYSTEM:" << std::endl;
        report << "  Working Memory:      " << formatMemory(memory.workingMemoryUsage) << std::endl;
        report << "  Episodic Memory:     " << formatMemory(memory.episodicMemoryUsage) << std::endl;
        report << "  Semantic Memory:     " << formatMemory(memory.semanticMemoryUsage) << std::endl;
        report << "  Procedural Memory:   " << formatMemory(memory.proceduralMemoryUsage) << std::endl;
        report << "  Associative Memory:  " << formatMemory(memory.associativeMemoryUsage) << std::endl;
        report << "  Total Usage:         " << formatMemory(memory.totalMemoryUsage) << std::endl;
        report << std::endl;
        
        return report.str();
    }
    
    /**
     * Generate health report
     */
    static SystemHealth generateHealthReport(const PerformanceDiagnostics& performance,
                                           const MemoryDiagnostics& memory,
                                           const NeuralDiagnostics& neural,
                                           const WorldDiagnostics& world,
                                           const AgentDiagnostics& agent) {
        SystemHealth health;
        
        health.healthScore = calculateHealthScore(performance, memory, neural, world, agent);
        health.overallStatus = determineOverallStatus(health.healthScore);
        
        // Generate warnings
        if (performance.stepsPerSecond < 100) {
            health.warnings.push_back("Low performance: steps per second below optimal");
        }
        
        if (performance.memoryUsagePercent > 80.0f) {
            health.warnings.push_back("High memory usage: above 80%");
        }
        
        if (neural.excitationInhibitionRatio > 10.0f) {
            health.warnings.push_back("High excitation/inhibition ratio may indicate neural instability");
        }
        
        if (agent.curiosityLevel < 0.1f) {
            health.warnings.push_back("Low curiosity level may reduce exploration");
        }
        
        // Generate recommendations
        if (health.healthScore < 0.7f) {
            health.recommendations.push_back("Consider reducing simulation parameters for better performance");
            health.recommendations.push_back("Check memory usage and consider cleanup");
        }
        
        if (!health.warnings.empty()) {
            health.recommendations.push_back("Monitor system resources regularly");
            health.recommendations.push_back("Review configuration settings for optimization");
        }
        
        return health;
    }
    
    /**
     * Generate diagnostic report with all collected data
     */
    static std::string generateDiagnosticReport(const std::shared_ptr<Brain>& brain,
                                               const std::shared_ptr<SimpleWorld>& world,
                                               const std::shared_ptr<AgentBrain>& agent,
                                               uint64_t steps,
                                               double totalTime) {
        SystemDiagnostics system = collectSystemDiagnostics();
        PerformanceDiagnostics performance = collectPerformanceDiagnostics(brain, steps, totalTime);
        MemoryDiagnostics memory = collectMemoryDiagnostics();
        NeuralDiagnostics neural = collectNeuralDiagnostics(brain);
        WorldDiagnostics worldDiag = collectWorldDiagnostics(world);
        AgentDiagnostics agentDiag = collectAgentDiagnostics(agent);
        
        SystemHealth health = generateHealthReport(performance, memory, neural, worldDiag, agentDiag);
        
        std::stringstream report;
        report << generateSystemReport(system, performance, memory, neural, worldDiag, agentDiag);
        report << "HEALTH STATUS:" << std::endl;
        report << "  Overall Score: " << std::fixed << std::setprecision(1) << (health.healthScore * 100) << "%" << std::endl;
        report << "  Status:        " << health.overallStatus << std::endl;
        report << std::endl;
        
        if (!health.warnings.empty()) {
            report << "⚠️ WARNINGS:" << std::endl;
            for (const auto& warning : health.warnings) {
                report << "  - " << warning << std::endl;
            }
            report << std::endl;
        }
        
        if (!health.errors.empty()) {
            report << "❌ ERRORS:" << std::endl;
            for (const auto& error : health.errors) {
                report << "  - " << error << std::endl;
            }
            report << std::endl;
        }
        
        if (!health.recommendations.empty()) {
            report << "💡 RECOMMENDATIONS:" << std::endl;
            for (const auto& recommendation : health.recommendations) {
                report << "  - " << recommendation << std::endl;
            }
            report << std::endl;
        }
        
        report << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        report << "║                             DIAGNOSTIC REPORT COMPLETE                         ║" << std::endl;
        report << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        return report.str();
    }
    
private:
    static std::string getHostname() {
        char hostname[1024];
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            return std::string(hostname);
        }
        return "unknown";
    }
    
    static std::string getVersion() {
        return "NLM 0.1.0 (Phase 6)";
    }
    
    static uint64_t getProcessId() {
        return getpid();
    }
    
    static int getCpuCores() {
        return std::thread::hardware_concurrency();
    }
    
    static std::pair<size_t, size_t> getMemoryInfo() {
        // Get system memory information
        // This is platform-specific implementation
        return std::make_pair(1024 * 1024 * 1024, 2048 * 1024 * 1024); // 1GB / 2GB example
    }
    
    static float getCpuUsage() {
        // Get CPU usage percentage
        // This is platform-specific implementation
        return 0.0f; // Placeholder
    }
    
    static float getMemoryUsagePercent() {
        auto memInfo = getMemoryInfo();
        if (memInfo.second == 0) return 0.0f;
        return (static_cast<float>(memInfo.first) / memInfo.second) * 100.0f;
    }
    
    static std::string formatMemory(size_t bytes) {
        if (bytes >= 1024 * 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
        } else if (bytes >= 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024)) + " MB";
        } else if (bytes >= 1024) {
            return std::to_string(bytes / 1024) + " KB";
        } else {
            return std::to_string(bytes) + " B";
        }
    }
    
    static double calculateHealthScore(const PerformanceDiagnostics& performance,
                                      const MemoryDiagnostics& memory,
                                      const NeuralDiagnostics& neural,
                                      const WorldDiagnostics& world,
                                      const AgentDiagnostics& agent) {
        double score = 1.0;
        
        // Performance factors
        if (performance.stepsPerSecond < 100) score *= 0.8;
        if (performance.memoryUsagePercent > 80.0f) score *= 0.9;
        
        // Neural factors
        if (neural.excitationInhibitionRatio > 10.0f) score *= 0.9;
        if (neural.firingNeuronCount < neural.totalNeuronCount * 0.01) score *= 0.95;
        
        // Agent factors
        if (agent.curiosityLevel < 0.1f) score *= 0.95;
        if (agent.predictionError > 1.0f) score *= 0.9;
        
        return score;
    }
    
    static std::string determineOverallStatus(double healthScore) {
        if (healthScore >= 0.9) return "EXCELLENT";
        if (healthScore >= 0.8) return "GOOD";
        if (healthScore >= 0.7) return "FAIR";
        if (healthScore >= 0.6) return "POOR";
        return "CRITICAL";
    }
};

class EnhancedLogger : public Logger {
public:
    EnhancedLogger() : Logger() {
        setupDiagnosticLoggers();
    }
    
    void logWithContext(const std::string& message, 
                       LogLevel level,
                       const std::string& context = "") {
        std::stringstream fullMessage;
        if (!context.empty()) {
            fullMessage << "[" << context << "] ";
        }
        fullMessage << message;
        
        log(level, fullMessage.str());
    }
    
    void logPerformance(const std::string& operation, 
                       double time,
                       uint64_t neurons,
                       uint64_t steps) {
        std::stringstream message;
        message << "Performance: " << operation << " - "
                << "Time: " << std::fixed << std::setprecision(3) << time << "s, "
                << "Neurons: " << neurons << ", "
                << "Steps/s: " << (steps / (time > 0 ? time : 1.0));
        
        logWithContext(message.str(), LogLevel::Info, "PERFORMANCE");
    }
    
    void logMemory(const std::string& operation,
                  size_t memoryUsage,
                  const std::string& details = "") {
        std::stringstream message;
        message << "Memory: " << operation << " - "
                << formatMemory(memoryUsage) << ". "
                << details;
        
        logWithContext(message.str(), LogLevel::Info, "MEMORY");
    }
    
    void logNeural(const std::string& operation,
                  size_t neurons,
                  size_t activeNeurons,
                  float firingRate) {
        std::stringstream message;
        message << "Neural: " << operation << " - "
                << "Neurons: " << neurons << ", "
                << "Active: " << activeNeurons << ", "
                << "Firing Rate: " << std::fixed << std::setprecision(2) << firingRate << " Hz";
        
        logWithContext(message.str(), LogLevel::Info, "NEURAL");
    }
    
    void logWorld(const std::string& operation,
                 float time,
                 float width,
                 float height) {
        std::stringstream message;
        message << "World: " << operation << " - "
                << "Time: " << std::fixed << std::setprecision(3) << time << "s, "
                << "Size: " << width << "x" << height;
        
        logWithContext(message.str(), LogLevel::Info, "WORLD");
    }
    
    void logAgent(const std::string& operation,
                 float curiosity,
                 float novelty,
                 float reward) {
        std::stringstream message;
        message << "Agent: " << operation << " - "
                << "Curiosity: " << std::fixed << std::setprecision(3) << curiosity << ", "
                << "Novelty: " << std::fixed << std::setprecision(3) << novelty << ", "
                << "Reward: " << std::fixed << std::setprecision(3) << reward;
        
        logWithContext(message.str(), LogLevel::Info, "AGENT");
    }
    
private:
    void setupDiagnosticLoggers() {
        // Setup various log targets for diagnostics
        // This would include file logging, console logging, etc.
    }
}; 

} // namespace nlm
