#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {

// Performance metrics structure for tracking AgentBrain execution
struct AgentBrainMetrics {
    double totalExecutionTime;
    double averageExecutionTime;
    double minExecutionTime;
    double maxExecutionTime;
    size_t totalSpikeCount;
    float totalReward;
    float totalPredictionError;
    size_t stepCount;
    double memoryUsage;
    
    AgentBrainMetrics() 
        : totalExecutionTime(0.0), averageExecutionTime(0.0), minExecutionTime(0.0), maxExecutionTime(0.0)
        , totalSpikeCount(0), totalReward(0.0f), totalPredictionError(0.0f), stepCount(0), memoryUsage(0.0) {}
};

// Enhanced logging configuration
struct AgentBrainLoggingConfig {
    bool enableDebugMode;
    bool enablePerformanceMetrics;
    bool enableVerboseLogging;
    bool enableStateDumping;
    bool enableEventLogging;
    bool enablePerformanceMonitoring;
    bool enableMemoryTracking;
    bool enableDecisionLogging;
    
    // Log levels for different components
    enum LogLevel {
        Quiet = 0,
        Error = 1,
        Warning = 2,
        Info = 3,
        Debug = 4,
        Verbose = 5
    };
    
    LogLevel globalLogLevel;
    LogLevel brainLogLevel;
    LogLevel neuromodulationLogLevel;
    LogLevel developmentLogLevel;
    
    // Log destinations
    bool logToConsole;
    bool logToFile;
    std::string logFilePath;
    
    // Performance monitoring settings
    bool trackExecutionTime;
    bool trackMemoryUsage;
    bool trackSpikeCounts;
    bool trackRewardPredictions;
    
    // Debug mode settings
    bool enableStateSnapshots;
    bool enableNeuromodulationDetails;
    bool enableDecisionDetails;
    bool enableMemoryDetails;
    
    AgentBrainLoggingConfig()
        : enableDebugMode(false), enablePerformanceMetrics(true), enableVerboseLogging(false)
        , enableStateDumping(false), enableEventLogging(true), enablePerformanceMonitoring(true)
        , enableMemoryTracking(true), enableDecisionLogging(true)
        , globalLogLevel(LogLevel::Info), brainLogLevel(LogLevel::Info), neuromodulationLogLevel(LogLevel::Info)
        , developmentLogLevel(LogLevel::Info)
        , logToConsole(true), logToFile(false), trackExecutionTime(true)
        , trackMemoryUsage(true), trackSpikeCounts(true), trackRewardPredictions(true)
        , enableStateSnapshots(false), enableNeuromodulationDetails(false)
        , enableDecisionDetails(false), enableMemoryDetails(false) {}
};

// Enhanced logging interface for AgentBrain components
class IAgentBrainLogger {
public:
    virtual ~IAgentBrainLogger() = default;
    
    // State change logging
    virtual void logStateChange(const std::string& component, const std::string& oldState, 
                               const std::string& newState, const std::string& context) = 0;
    
    // Decision process logging
    virtual void logDecision(const std::string& decision, const std::string& reasoning, 
                           const std::string& context) = 0;
    
    // System event logging
    virtual void logSystemEvent(const std::string& event, const std::string& details) = 0;
    
    // Performance metrics logging
    virtual void logPerformanceMetrics(const AgentBrainMetrics& metrics) = 0;
    
    // Neuromodulation event logging
    virtual void logNeuromodulationEvent(const std::string& neuromodulator, float level, 
                                        const std::string& context) = 0;
    
    // Memory update logging
    virtual void logMemoryUpdate(const std::string& component, double memoryUsage, 
                               const std::string& details) = 0;
    
    // Development stage logging
    virtual void logDevelopmentStage(const std::string& stage, double age, 
                                    double plasticityModifier) = 0;
    
    // Debug mode state dumping
    virtual void logStateDump(const std::string& component, const std::string& state) = 0;
    
    // Spike count logging
    virtual void logSpikeCount(size_t spikeCount, const std::string& context) = 0;
    
    // Reward prediction logging
    virtual void logRewardPrediction(float reward, float predictedReward, 
                                    float predictionError) = 0;
};

// Implementation of enhanced AgentBrain logger
class AgentBrainLogger : public IAgentBrainLogger {
public:
    AgentBrainLogger(const AgentBrainLoggingConfig& config) : config_(config) {}
    
    void logStateChange(const std::string& component, const std::string& oldState, 
                       const std::string& newState, const std::string& context) override {
        if (config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Info) {
            std::ostringstream oss;
            oss << "State change: " << component << "; "
                << "Old: " << oldState << "; "
                << "New: " << newState << "; "
                << "Context: " << context;
            
            if (config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Debug) {
                oss << " [STATE_CHANGE]";
            }
            
            logWithLevel(config_.globalLogLevel, oss.str());
        }
    }
    
    void logDecision(const std::string& decision, const std::string& reasoning, 
                   const std::string& context) override {
        if (config_.enableDecisionLogging && 
            config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Debug) {
            std::ostringstream oss;
            oss << "Decision: " << decision << "; "
                << "Reasoning: " << reasoning << "; "
                << "Context: " << context;
            
            oss << " [DECISION]";
            logWithLevel(config_.globalLogLevel, oss.str());
        }
    }
    
    void logSystemEvent(const std::string& event, const std::string& details) override {
        if (config_.enableEventLogging && 
            config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Info) {
            std::ostringstream oss;
            oss << "Event: " << event << "; "
                << "Details: " << details;
            
            oss << " [SYSTEM_EVENT]";
            logWithLevel(config_.globalLogLevel, oss.str());
        }
    }
    
    void logPerformanceMetrics(const AgentBrainMetrics& metrics) override {
        if (config_.enablePerformanceMetrics && 
            config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Info) {
            std::ostringstream oss;
            oss << "Performance metrics: "
                << "AvgExecTime: " << std::fixed << std::setprecision(6) << metrics.averageExecutionTime << "s; "
                << "MinExecTime: " << metrics.minExecutionTime << "s; "
                << "MaxExecTime: " << metrics.maxExecutionTime << "s; "
                << "TotalSpikes: " << metrics.totalSpikeCount << "; "
                << "TotalReward: " << metrics.totalReward << "; "
                << "AvgPredictionError: " << (metrics.stepCount > 0 ? metrics.totalPredictionError / metrics.stepCount : 0.0f) << "; "
                << "MemoryUsage: " << metrics.memoryUsage << "MB";
            
            oss << " [PERFORMANCE]";
            logWithLevel(config_.globalLogLevel, oss.str());
        }
    }
    
    void logNeuromodulationEvent(const std::string& neuromodulator, float level, 
                               const std::string& context) override {
        if (config_.enableNeuromodulationDetails && 
            config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Debug) {
            std::ostringstream oss;
            oss << "Neuromodulation: " << neuromodulator << " = " << level << "; "
                << "Context: " << context;
            
            oss << " [NEUROMODULATION]";
            logWithLevel(config_.globalLogLevel, oss.str());
        }
    }
    
    void logMemoryUpdate(const std::string& component, double memoryUsage, 
                        const std::string& details) override {
        if (config_.enableMemoryDetails && 
            config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Debug) {
            std::ostringstream oss;
            oss << "Memory update: " << component << " = " << memoryUsage << "MB; "
                << "Details: " << details;
            
            oss << " [MEMORY]";
            logWithLevel(config_.globalLogLevel, oss.str());
        }
    }
    
    void logDevelopmentStage(const std::string& stage, double age, 
                           double plasticityModifier) override {
        if (config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Info) {
            std::ostringstream oss;
            oss << "Development: Stage = " << stage << "; "
                << "Age = " << age << "; "
                << "PlasticityModifier = " << plasticityModifier;
            
            oss << " [DEVELOPMENT]";
            logWithLevel(config_.globalLogLevel, oss.str());
        }
    }
    
    void logStateDump(const std::string& component, const std::string& state) override {
        if (config_.enableStateSnapshots && 
            config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Debug) {
            std::ostringstream oss;
            oss << "State dump: " << component << "\n" << state << "\n";
            oss << " [STATE_DUMP]";
            logWithLevel(config_.globalLogLevel, oss.str());
        }
    }
    
    void logSpikeCount(size_t spikeCount, const std::string& context) override {
        if (config_.trackSpikeCounts && 
            config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Info) {
            std::ostringstream oss;
            oss << "Spike count: " << spikeCount << "; "
                << "Context: " << context;
            
            oss << " [SPIKES]";
            logWithLevel(config_.globalLogLevel, oss.str());
        }
    }
    
    void logRewardPrediction(float reward, float predictedReward, 
                            float predictionError) override {
        if (config_.trackRewardPredictions && 
            config_.globalLogLevel >= AgentBrainLoggingConfig::LogLevel::Info) {
            std::ostringstream oss;
            oss << "Reward prediction: " << reward << "; "
                << "Predicted: " << predictedReward << "; "
                << "Error: " << predictionError << "; "
                << "Accuracy: " << (predictedReward > 0 ? (reward / predictedReward) * 100 : 0) << "%";
            
            oss << " [REWARD_PRED]";
            logWithLevel(config_.globalLogLevel, oss.str());
        }
    }
    
    const AgentBrainLoggingConfig& getConfig() const { return config_; }
    
private:
    void logWithLevel(AgentBrainLoggingConfig::LogLevel level, const std::string& message) {
        if (config_.logToConsole) {
            switch (level) {
                case AgentBrainLoggingConfig::LogLevel::Debug:
                    NLM_LOG_DEBUG(message);
                    break;
                case AgentBrainLoggingConfig::LogLevel::Info:
                    NLM_LOG_INFO(message);
                    break;
                case AgentBrainLoggingConfig::LogLevel::Warning:
                    NLM_LOG_WARNING(message);
                    break;
                case AgentBrainLoggingConfig::LogLevel::Error:
                    NLM_LOG_ERROR(message);
                    break;
                case AgentBrainLoggingConfig::LogLevel::Critical:
                    NLM_LOG_CRITICAL(message);
                    break;
                default:
                    NLM_LOG_INFO(message);
                    break;
            }
        }
        
        if (config_.logToFile && !config_.logFilePath.empty()) {
            std::ofstream logFile(config_.logFilePath, std::ios::app);
            if (logFile.is_open()) {
                auto now = std::chrono::system_clock::now();
                auto time = std::chrono::system_clock::to_time_t(now);
                logFile << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") 
                       << "] [" << static_cast<int>(level) << "] " << message << std::endl;
            }
        }
    }
    
    AgentBrainLoggingConfig config_;
};

// Global AgentBrain logger instance management
class AgentBrainLoggerManager {
public:
    static AgentBrainLoggerManager& getInstance() {
        static AgentBrainLoggerManager instance;
        return instance;
    }
    
    void setConfig(const AgentBrainLoggingConfig& config) {
        config_ = config;
        
        if (logger_) {
            logger_.reset();
        }
        
        logger_ = std::make_unique<AgentBrainLogger>(config);
    }
    
    AgentBrainLogger& getLogger() {
        if (!logger_) {
            logger_ = std::make_unique<AgentBrainLogger>(config_);
        }
        return *logger_;
    }
    
    const AgentBrainLoggingConfig& getConfig() const { return config_; }
    
private:
    AgentBrainLoggerManager() = default;
    ~AgentBrainLoggerManager() = default;
    
    AgentBrainLoggingConfig config_;
    std::unique_ptr<AgentBrainLogger> logger_;
};

// Convenience functions for logging AgentBrain events
inline void logBrainInitialization(const AgentBrain& brain, bool success) {
    auto& logger = AgentBrainLoggerManager::getInstance().getLogger();
    if (success) {
        logger.logSystemEvent("BrainInitialization", "Successfully initialized AgentBrain");
    } else {
        logger.logSystemEvent("BrainInitialization", "Failed to initialize AgentBrain");
    }
}

inline void logBrainUpdate(const AgentBrain& brain, double timestep, size_t stepCount) {
    auto& logger = AgentBrainLoggerManager::getInstance().getLogger();
    std::ostringstream oss;
    oss << "Brain update: step=" << stepCount << ", timestep=" << timestep;
    logger.logSystemEvent("BrainUpdate", oss.str());
}

inline void logSensoryProcessing(const AgentBrain& brain, const SensoryPercept& percept) {
    auto& logger = AgentBrainLoggerManager::getInstance().getLogger();
    std::ostringstream oss;
    oss << "Sensory processing: vision=" << percept.getVision().size() 
        << ", touch=" << percept.getTouch().size()
        << ", internal=" << percept.getInternal().size()
        << ", proprioception=" << percept.getProprioception().size();
    logger.logSystemEvent("SensoryProcessing", oss.str());
}

inline void logMotorDecoding(const AgentBrain& brain, MotorCommand command, float confidence) {
    auto& logger = AgentBrainLoggerManager::getInstance().getLogger();
    std::ostringstream oss;
    oss << "Motor decoding: command=" << static_cast<int>(command) << ", confidence=" << confidence;
    logger.logDecision("MotorDecoding", oss.str(), "AgentBrain");
}

inline void logRewardModulation(const AgentBrain& brain, float reward, float predictedReward, 
                               float predictionError) {
    auto& logger = AgentBrainLoggerManager::getInstance().getLogger();
    logger.logRewardPrediction(reward, predictedReward, predictionError);
}

inline void logDevelopmentUpdate(const AgentBrain& brain, double timestep, double age) {
    auto& logger = AgentBrainLoggerManager::getInstance().getLogger();
    auto stage = brain.getDevelopmentalStage();
    std::string stageStr;
    switch (stage) {
        case DevelopmentalStage::Initial: stageStr = "Initial"; break;
        case DevelopmentalStage::CriticalPeriod: stageStr = "CriticalPeriod"; break;
        case DevelopmentalStage::Maturation: stageStr = "Maturation"; break;
        case DevelopmentalStage::Adult: stageStr = "Adult"; break;
        case DevelopmentalStage::Aging: stageStr = "Aging"; break;
    }
    
    logger.logDevelopmentStage(stageStr, age, brain.getPlasticityFactor());
}

inline void logPerformanceMetrics(const AgentBrainMetrics& metrics) {
    auto& logger = AgentBrainLoggerManager::getInstance().getLogger();
    logger.logPerformanceMetrics(metrics);
}

// Debug mode functionality
class AgentBrainDebugMode {
public:
    static void enableDebugMode() {
        AgentBrainLoggingConfig config = AgentBrainLoggerManager::getInstance().getConfig();
        config.enableDebugMode = true;
        config.enableStateDumping = true;
        config.enableNeuromodulationDetails = true;
        config.enableDecisionDetails = true;
        config.enableMemoryDetails = true;
        config.globalLogLevel = AgentBrainLoggingConfig::LogLevel::Verbose;
        
        AgentBrainLoggerManager::getInstance().setConfig(config);
        
        auto& logger = AgentBrainLoggerManager::getInstance().getLogger();
        logger.logSystemEvent("DebugMode", "Debug mode enabled with detailed logging");
    }
    
    static void disableDebugMode() {
        AgentBrainLoggingConfig config = AgentBrainLoggerManager::getInstance().getConfig();
        config.enableDebugMode = false;
        config.enableStateDumping = false;
        config.enableNeuromodulationDetails = false;
        config.enableDecisionDetails = false;
        config.enableMemoryDetails = false;
        config.globalLogLevel = AgentBrainLoggingConfig::LogLevel::Info;
        
        AgentBrainLoggerManager::getInstance().setConfig(config);
        
        auto& logger = AgentBrainLoggerManager::getInstance().getLogger();
        logger.logSystemEvent("DebugMode", "Debug mode disabled");
    }
    
    static bool isDebugModeEnabled() {
        return AgentBrainLoggerManager::getInstance().getConfig().enableDebugMode;
    }
    
    static void logStateSnapshot(const AgentBrain& brain, const std::string& snapshotName) {
        auto& logger = AgentBrainLoggerManager::getInstance().getLogger();
        std::ostringstream oss;
        oss << "=== State Snapshot: " << snapshotName << " ===\n";
        oss << "Brain state:\n";
        oss << "  Neuromodulation level: " << brain.getNeuromodulationLevel() << "\n";
        oss << "  Curiosity level: " << brain.getCuriosityLevel() << "\n";
        oss << "  Novelty level: " << brain.getNoveltyLevel() << "\n";
        oss << "  Prediction error: " << brain.getPredictionError() << "\n";
        oss << "  Developmental stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
        oss << "  Plasticity factor: " << brain.getPlasticityFactor() << "\n";
        oss << "  Reward modulation enabled: " << (brain.isRewardModulationEnabled() ? "YES" : "NO") << "\n";
        oss << "  Development enabled: " << (brain.isDevelopmentEnabled() ? "YES" : "NO") << "\n";
        oss << "  Curiosity enabled: " << (brain.isCuriosityEnabled() ? "YES" : "NO") << "\n";
        oss << "  Structural plasticity enabled: " << (brain.isStructuralPlasticityEnabled() ? "YES" : "NO") << "\n";
        oss << "========================================\n";
        
        logger.logStateDump("AgentBrain", oss.str());
    }
};

} // namespace nlm
