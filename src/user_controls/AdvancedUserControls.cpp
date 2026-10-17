#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <algorithm>
#include <thread>
#include <atomic>
#include "core/Config/Config.hpp"

namespace nlm {

class AdvancedUserControls {
public:
    AdvancedUserControls() : running_(false), config_(std::make_shared<Config>()) {}
    ~AdvancedUserControls() {
        stopControlLoop();
    }
    
    bool initialize() {
        // Load saved controls
        loadUserControls();
        
        // Start control monitoring thread
        running_ = true;
        controlThread_ = std::thread(&AdvancedUserControls::controlLoop, this);
        
        return true;
    }
    
    void startControlInterface() {
        // Simple command-line interface for advanced controls
        std::cout << "=== Advanced User Controls ===\n";
        std::cout << "Available commands:\n";
        std::cout << "  help                - Show this help\n";
        std::cout << "  status             - Show current status\n";
        std::cout << "  set <key> <value>  - Set configuration value\n";
        std::cout << "  get <key>          - Get configuration value\n";
        std::cout << "  list               - List all configuration values\n";
        std::cout << "  save               - Save current controls\n";
        std::cout << "  load <file>        - Load controls from file\n";
        std::cout << "  visualize          - Toggle visualization\n";
        std::cout << "  profiling          - Toggle performance profiling\n";
        std::cout << "  quit               - Exit\n";
        
        std::string input;
        while (running_) {
            std::cout << "\n> ";
            std::getline(std::cin, input);
            
            if (input.empty()) {
                continue;
            }
            
            processCommand(input);
        }
    }
    
    void stopControlInterface() {
        running_ = false;
        if (controlThread_.joinable()) {
            controlThread_.join();
        }
    }
    
    void updateControls() {
        // Auto-update based on conditions
        checkParameterLimits();
        autoSaveIfNeeded();
        monitorPerformance();
    }
    
    // Configuration management
    void setConfigValue(const std::string& key, const std::string& value) {
        config_->set(key, value, ConfigSource::Runtime);
        logCommand(std::string("set ") + key + " " + value);
    }
    
    void setConfigValue(const std::string& key, double value) {
        config_->set(key, value, ConfigSource::Runtime);
        logCommand(std::string("set ") + key + " " + std::to_string(value));
    }
    
    void setConfigValue(const std::string& key, int value) {
        config_->set(key, value, ConfigSource::Runtime);
        logCommand(std::string("set ") + key + " " + std::to_string(value));
    }
    
    std::string getConfigValue(const std::string& key) const {
        auto value = config_->get<std::string>(key);
        return value.has_value() ? value.value() : "";
    }
    
    double getConfigDouble(const std::string& key) const {
        auto value = config_->get<double>(key);
        return value.has_value() ? value.value() : 0.0;
    }
    
    int getConfigInt(const std::string& key) const {
        auto value = config_->get<int>(key);
        return value.has_value() ? value.value() : 0;
    }
    
    // Real-time parameter adjustment
    void adjustParameter(const std::string& key, double delta) {
        auto current = getConfigDouble(key);
        setConfigValue(key, current + delta);
        logCommand(std::string("adjust ") + key + " by " + std::to_string(delta));
    }
    
    // Visualization controls
    void toggleVisualization() {
        visualizationEnabled_ = !visualizationEnabled_;
        logCommand(visualizationEnabled_ ? "visualization on" : "visualization off");
    }
    
    bool isVisualizationEnabled() const {
        return visualizationEnabled_;
    }
    
    // Performance monitoring
    void toggleProfiling() {
        profilingEnabled_ = !profilingEnabled_;
        logCommand(profilingEnabled_ ? "profiling on" : "profiling off");
    }
    
    bool isProfilingEnabled() const {
        return profilingEnabled_;
    }
    
    // Advanced visualization controls
    void setVisualizationStyle(const std::string& style) {
        visualizationStyle_ = style;
        logCommand(std::string("visualization style ") + style);
    }
    
    void setVisualizationSpeed(double speed) {
        visualizationSpeed_ = speed;
        logCommand(std::string("visualization speed ") + std::to_string(speed));
    }
    
    // Performance profiling
    void startPerformanceMeasurement() {
        performanceStartTime_ = std::chrono::high_resolution_clock::now();
        performanceSamples_.clear();
    }
    
    void stopPerformanceMeasurement() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - performanceStartTime_);
        
        logCommand(std::string("performance measurement: ") + std::to_string(duration.count()) + "ms");
    }
    
    void samplePerformance(const std::string& metric, double value) {
        performanceSamples_.push_back({metric, value, performanceStartTime_});
    }
    
    // Command history
    void showCommandHistory() const {
        std::cout << "\n=== Command History ===\n";
        for (const auto& cmd : commandHistory_) {
            std::cout << cmd << std::endl;
        }
    }
    
    void saveControls() {
        config_->saveToFile("user_controls.cfg");
        logCommand("controls saved to user_controls.cfg");
    }
    
    bool loadControls(const std::string& filename) {
        if (config_->loadFromFile(filename)) {
            logCommand(std::string("controls loaded from ") + filename);
            return true;
        }
        return false;
    }
    
    // Help and status
    void showStatus() const {
        std::cout << "\n=== System Status ===\n";
        std::cout << "Running: " << (running_ ? "Yes" : "No") << std::endl;
        std::cout << "Visualization: " << (visualizationEnabled_ ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Profiling: " << (profilingEnabled_ ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Style: " << visualizationStyle_ << std::endl;
        std::cout << "Speed: " << visualizationSpeed_ << std::endl;
        std::cout << "Config entries: " << config_->getKeys().size() << std::endl;
        std::cout << "Command history: " << commandHistory_.size() << " entries\n";
    }
    
    void showHelp() const {
        std::cout << "\n=== Advanced Controls Help ===\n";
        std::cout << "Real-time control of NLM simulation parameters.\n\n";
        std::cout << "Commands:\n";
        std::cout << "  help              - Show this help\n";
        std::cout << "  status            - Show current status\n";
        std::cout << "  set <key> <value> - Set configuration value\n";
        std::cout << "  get <key>         - Get configuration value\n";
        std::cout << "  adjust <key> <delta> - Adjust value by delta\n";
        std::cout << "  list              - List all configuration values\n";
        std::cout << "  save              - Save current controls\n";
        std::cout << "  load <file>       - Load controls from file\n";
        std::cout << "  visualize         - Toggle visualization\n";
        std::cout << "  profiling         - Toggle performance profiling\n";
        std::cout << "  history           - Show command history\n";
        std::cout << "  quit              - Exit\n\n";
        std::cout << "Visualization controls:\n";
        std::cout << "  set style <name>  - Set visualization style\n";
        std::cout << "  set speed <value>  - Set visualization speed\n\n";
        std::cout << "Performance controls:\n";
        std::cout << "  measure          - Start performance measurement\n";
        std::cout << "  stop             - Stop performance measurement\n";
    }
    
private:
    struct PerformanceSample {
        std::string metric;
        double value;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    void processCommand(const std::string& input) {
        std::istringstream iss(input);
        std::string command;
        iss >> command;
        
        if (command == "help") {
            showHelp();
        } else if (command == "status") {
            showStatus();
        } else if (command == "quit") {
            running_ = false;
        } else if (command == "list") {
            listConfigValues();
        } else if (command == "visualize") {
            toggleVisualization();
        } else if (command == "profiling") {
            toggleProfiling();
        } else if (command == "history") {
            showCommandHistory();
        } else if (command == "save") {
            saveControls();
        } else if (command == "measure") {
            startPerformanceMeasurement();
        } else if (command == "stop") {
            stopPerformanceMeasurement();
        } else if (command == "set") {
            handleSetCommand(iss);
        } else if (command == "get") {
            handleGetCommand(iss);
        } else if (command == "adjust") {
            handleAdjustCommand(iss);
        } else if (command == "load") {
            handleLoadCommand(iss);
        } else if (command == "style") {
            handleStyleCommand(iss);
        } else if (command == "speed") {
            handleSpeedCommand(iss);
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands.\n";
        }
    }
    
    void handleSetCommand(std::istringstream& iss) {
        std::string key, value;
        if (iss >> key >> value) {
            // Try to determine value type
            char* end;
            double doubleVal = std::strtod(value.c_str(), &end);
            if (end != value.c_str() && *end == '\0') {
                // It's a number
                int intVal = std::stoi(value);
                if (doubleVal == static_cast<double>(intVal)) {
                    setConfigValue(key, intVal);
                    return;
                }
                setConfigValue(key, doubleVal);
                return;
            }
            
            // Try boolean
            if (value == "true" || value == "false") {
                setConfigValue(key, value == "true");
                return;
            }
            
            // Default to string
            setConfigValue(key, value);
        } else {
            std::cout << "Usage: set <key> <value>\n";
        }
    }
    
    void handleGetCommand(std::istringstream& iss) {
        std::string key;
        if (iss >> key) {
            std::cout << key << " = " << getConfigValue(key) << std::endl;
        } else {
            std::cout << "Usage: get <key>\n";
        }
    }
    
    void handleAdjustCommand(std::istringstream& iss) {
        std::string key, deltaStr;
        if (iss >> key >> deltaStr) {
            char* end;
            double delta = std::strtod(deltaStr.c_str(), &end);
            if (end != deltaStr.c_str()) {
                adjustParameter(key, delta);
                return;
            }
        }
        std::cout << "Usage: adjust <key> <delta>\n";
    }
    
    void handleLoadCommand(std::istringstream& iss) {
        std::string filename;
        if (iss >> filename) {
            loadControls(filename);
        } else {
            std::cout << "Usage: load <file>\n";
        }
    }
    
    void handleStyleCommand(std::istringstream& iss) {
        std::string style;
        if (iss >> style) {
            setVisualizationStyle(style);
        } else {
            std::cout << "Usage: set style <name>\n";
        }
    }
    
    void handleSpeedCommand(std::istringstream& iss) {
        std::string speedStr;
        if (iss >> speedStr) {
            char* end;
            double speed = std::strtod(speedStr.c_str(), &end);
            if (end != speedStr.c_str()) {
                setVisualizationSpeed(speed);
                return;
            }
        }
        std::cout << "Usage: set speed <value>\n";
    }
    
    void listConfigValues() const {
        std::cout << "\n=== Configuration Values ===\n";
        for (const auto& key : config_->getKeys()) {
            std::cout << key << " = " << getConfigValue(key) << std::endl;
        }
    }
    
    void logCommand(const std::string& command) {
        commandHistory_.push_back(command);
        std::cout << "[" << commandHistory_.size() - 1 << "] " << command << std::endl;
    }
    
    void checkParameterLimits() {
        // Check if parameters exceed safe limits
        double neuronCount = getConfigDouble("brain.neuron_count");
        if (neuronCount < 100.0) {
            adjustParameter("brain.neuron_count", 50.0);
            logCommand("warning: neuron count too low, adjusted");
        }
        
        double connectionProb = getConfigDouble("brain.connection_probability");
        if (connectionProb < 0.01 || connectionProb > 0.5) {
            adjustParameter("brain.connection_probability", 0.1);
            logCommand("warning: connection probability out of range, adjusted");
        }
    }
    
    void autoSaveIfNeeded() {
        // Auto-save every 100 commands
        if (commandHistory_.size() % 100 == 0) {
            saveControls();
        }
    }
    
    void monitorPerformance() {
        if (profilingEnabled_) {
            // Sample performance metrics
            samplePerformance("cpu_time", 0.0); // Would need actual measurement
            samplePerformance("memory_usage", 0.0); // Would need actual measurement
        }
    }
    
    void controlLoop() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            updateControls();
        }
    }
    
    void loadUserControls() {
        // Load user controls if they exist
        if (config_->loadFromFile("user_controls.cfg")) {
            logCommand("user controls loaded from user_controls.cfg");
        }
    }
    
    // Member variables
    std::shared_ptr<Config> config_;
    bool running_;
    bool visualizationEnabled_;
    bool profilingEnabled_;
    std::string visualizationStyle_;
    double visualizationSpeed_;
    std::vector<std::string> commandHistory_;
    std::vector<PerformanceSample> performanceSamples_;
    std::chrono::high_resolution_clock::time_point performanceStartTime_;
    std::thread controlThread_;
};

} // namespace nlm