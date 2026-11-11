#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>

// Forward declarations
namespace nlm {
    class Config;
    class Brain;
    class Logger;
    
    enum class Phase : int {
        Phase2 = 2,
        Phase3 = 3,
        Phase4 = 4,
        Phase5 = 5,
        Phase6 = 6
    };
    
    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error
    };
}

// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual std::vector<std::string> getCommands() const = 0;
    virtual bool handleCommand(const std::string& command, const std::vector<std::string>& args) = 0;
};

// Command handler type
using CommandHandler = std::function<bool(const std::vector<std::string>&)>;

// Progress reporting
class ProgressReporter {
public:
    void startProgress(const std::string& task, size_t totalSteps) {
        currentTask = task;
        progressTotal = totalSteps;
        progressCurrent = 0;
        startTime = std::chrono::steady_clock::now();
    }
    
    void updateProgress(size_t currentStep, const std::string& status = "") {
        progressCurrent = currentStep;
        statusMsg = status;
        
        // Calculate ETA
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
        if (progressCurrent > 0 && elapsed > 0) {
            auto estimatedTotal = static_cast<double>(elapsed) * static_cast<double>(progressTotal) / static_cast<double>(progressCurrent);
            auto remaining = estimatedTotal - elapsed;
            etaString = formatDuration(remaining / 1000.0);
        }
    }
    
    void finishProgress() {
        progressCurrent = progressTotal;
        updateProgress(progressTotal);
    }
    
    void printProgress(bool force = false) {
        if (progressTotal == 0) return;
        
        double percentage = static_cast<double>(progressCurrent) / static_cast<double>(progressTotal) * 100.0;
        int barWidth = 50;
        int filled = static_cast<int>(percentage * barWidth / 100.0);
        
        std::cout << "\r[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "=";
            else if (i == filled) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "%";
        
        if (!etaString.empty()) {
            std::cout << " ETA: " << etaString;
        }
        
        if (!statusMsg.empty()) {
            std::cout << " - " << statusMsg;
        }
        
        std::cout.flush();
        
        if (force || percentage >= 100.0) {
            std::cout << std::endl;
        }
    }
    
    static std::string formatDuration(double seconds) {
        if (seconds < 60) {
            return std::to_string(static_cast<int>(seconds)) + "s";
        } else if (seconds < 3600) {
            int minutes = static_cast<int>(seconds / 60);
            int secs = static_cast<int>(seconds) % 60;
            return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
        } else {
            int hours = static_cast<int>(seconds / 3600);
            int minutes = static_cast<int>((seconds - hours * 3600) / 60);
            int secs = static_cast<int>(seconds) % 60;
            return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
        }
    }
    
private:
    std::string currentTask;
    size_t progressTotal = 0;
    size_t progressCurrent = 0;
    std::string etaString;
    std::string statusMsg;
    std::chrono::steady_clock::time_point startTime;
};

// Configuration manager
class ConfigurationManager {
public:
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    bool loadFromArgs(int argc, char** argv);
    
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = configMap.find(key);
        if (it != configMap.end()) {
            return std::any_cast<T>(it->second);
        }
        return std::nullopt;
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto value = get<T>(key);
        return value.has_value() ? value.value() : defaultValue;
    }
    
    void set(const std::string& key, const std::any& value, int sourcePriority = 0);
    bool has(const std::string& key) const { return configMap.find(key) != configMap.end(); }
    void remove(const std::string& key) { configMap.erase(key); }
    std::vector<std::string> getKeys() const;
    std::string summary() const;
    bool validate() const;
    std::vector<std::string> getSuggestions(const std::string& partialKey) const;
    
    struct ConfigEntry {
        std::string key;
        std::any value;
        int sourcePriority;
        std::string description;
        std::chrono::steady_clock::time_point lastUsed;
    };
    
private:
    std::unordered_map<std::string, ConfigEntry> configMap;
    mutable std::mutex configMutex;
};

// Performance monitor
class PerformanceMonitor {
public:
    struct PerformanceStats {
        double cpuUsage = 0.0;
        double memoryUsage = 0.0;
        double memoryAvailable = 0.0;
        std::chrono::steady_clock::time_point simulationTime;
        double fps = 0.0;
        size_t spikeCount = 0;
        size_t neuronCount = 0;
        size_t synapseCount = 0;
    };
    
    void startMonitoring();
    void stopMonitoring();
    void updateStats(const PerformanceStats& stats);
    PerformanceStats getStats() const;
    bool isMonitoring() const { return monitoring; }
    
    // Memory monitoring
    void startMemoryMonitoring();
    void stopMemoryMonitoring();
    double getMemoryUsage() const;
    double getMemoryAvailable() const;
    
    // CPU monitoring
    void startCPUMonitoring();
    void stopCPUMonitoring();
    double getCPUUsage() const;
    
    // FPS tracking
    void frameStart();
    void frameEnd();
    double getFPS() const { return currentFPS; }
    
private:
    mutable std::mutex statsMutex;
    bool monitoring = false;
    PerformanceStats currentStats;
    std::chrono::steady_clock::time_point lastFrameTime;
    double currentFPS = 0.0;
    size_t frameCount = 0;
    
    // System monitoring
    std::atomic<double> cpuUsage{0.0};
    std::atomic<double> memoryUsage{0.0};
    std::atomic<double> memoryAvailable{0.0};
};

// Checkpoint manager
class CheckpointManager {
public:
    struct Checkpoint {
        std::string name;
        std::string path;
        std::chrono::system_clock::time_point timestamp;
        std::string description;
        size_t neuronCount;
        size_t synapseCount;
        double simulationTime;
    };
    
    bool createCheckpoint(const std::string& name, const std::string& description = "");
    bool loadCheckpoint(const std::string& path);
    bool deleteCheckpoint(const std::string& name);
    std::vector<Checkpoint> listCheckpoints() const;
    Checkpoint getCurrentCheckpoint() const;
    bool saveCurrentCheckpoint();
    
    // Checkpoint validation
    bool validateCheckpoint(const std::string& path) const;
    std::string getCheckpointInfo(const std::string& path) const;
    
private:
    mutable std::mutex checkpointMutex;
    std::vector<Checkpoint> checkpoints;
    std::string checkpointDir;
};

// Export manager
class ExportManager {
public:
    enum class ExportType {
        Results,
        Neurons,
        Synapses,
        Simulation,
        Memory,
        Config,
        Performance
    };
    
    enum class ExportFormat {
        JSON,
        CSV,
        Neural,
        Visualization,
        Binary,
        Text,
        XML
    };
    
    bool exportData(const std::string& type, const std::string& format, const std::string& path, const std::map<std::string, std::any>& options = {});
    bool exportBrain(const std::string& format, const std::string& path, const nlm::Brain& brain);
    bool exportSimulation(const std::string& format, const std::string& path, const nlm::Brain& brain, size_t step);
    bool exportResults(const std::string& format, const std::string& path, const std::vector<std::any>& results);
    
private:
    mutable std::mutex exportMutex;
};

// Plugin manager
class PluginManager {
public:
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    std::vector<std::string> listPlugins() const;
    std::vector<std::string> listEnabledPlugins() const;
    std::string getPluginInfo(const std::string& pluginName) const;
    bool registerCommandHandler(const std::string& command, CommandHandler handler);
    bool unregisterCommandHandler(const std::string& command);
    
    // Plugin lifecycle
    void initializeAllPlugins();
    void shutdownAllPlugins();
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins() const;
    
private:
    mutable std::mutex pluginMutex;
    std::map<std::string, std::unique_ptr<IPlugin>> plugins;
    std::map<std::string, bool> pluginEnabled;
    std::map<std::string, std::string> pluginPaths;
};

// Main NLM CLI class
class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    bool initialize(int argc, char** argv);
    bool run();
    void shutdown();
    
    // Core functionality
    bool executeCommand(const std::string& command, const std::vector<std::string>& args);
    bool executePhase(const std::string& phase, const std::vector<std::string>& args);
    bool executeHelp(const std::vector<std::string>& args);
    bool executeVersion(const std::vector<std::string>& args);
    bool executeDocs(const std::vector<std::string>& args);
    
    // Command management
    bool registerCommand(const std::string& command, CommandHandler handler, const std::string& description = "");
    bool unregisterCommand(const std::string& command);
    std::vector<std::string> getAvailableCommands() const;
    
    // Configuration management
    bool setConfiguration(const std::string& key, const std::string& value);
    bool getConfiguration(const std::string& key);
    bool loadConfiguration(const std::string& filepath);
    bool saveConfiguration(const std::string& filepath);
    
    // Interactive mode
    bool enterInteractiveMode();
    bool processInteractiveInput(const std::string& input);
    
    // Progress reporting
    void setProgressReporter(ProgressReporter* reporter) { progressReporter = reporter; }
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    
    // Checkpoint management
    void setCheckpointManager(CheckpointManager* manager) { checkpointManager = manager; }
    
    // Export management
    void setExportManager(ExportManager* manager) { exportManager = manager; }
    
    // Plugin management
    void setPluginManager(PluginManager* manager) { pluginManager = manager; }
    
    // Error handling
    struct Error {
        int code;
        std::string message;
        std::string context;
        std::string suggestion;
    };
    
    bool handleError(const Error& error);
    
private:
    // Core components
    std::unique_ptr<ConfigurationManager> configManager;
    std::unique_ptr<PerformanceMonitor> performanceMonitor;
    std::unique_ptr<CheckpointManager> checkpointManager;
    std::unique_ptr<ExportManager> exportManager;
    std::unique_ptr<PluginManager> pluginManager;
    ProgressReporter* progressReporter;
    
    // Command handlers
    std::map<std::string, std::pair<CommandHandler, std::string>> commandHandlers;
    
    // State management
    std::string currentPhase;
    bool interactiveMode;
    bool performanceMonitoring;
    std::chrono::steady_clock::time_point startTime;
    
    // Threading
    mutable std::mutex stateMutex;
    std::atomic<bool> running;
    
    // Error handling
    std::vector<Error> errorHistory;
    
    // Private helper methods
    void initializeCommands();
    void loadPlugins();
    void setupSignalHandlers();
    std::string formatError(const Error& error) const;
    std::vector<std::string> parseCommandLine(const std::string& line);
    void cleanup() { shutdown(); }
};

// Global instances (for use by plugins)
extern NLMCLI* globalCLI;
extern ConfigurationManager* globalConfig;
extern PerformanceMonitor* globalPerformanceMonitor;
extern CheckpointManager* globalCheckpointManager;
extern ExportManager* globalExportManager;
extern PluginManager* globalPluginManager;
