// NLM Advanced Features - Main Application with Enhanced CLI

#include "AdvancedFeatures.hpp"
#include "EnhancedCLI.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace nlm;

class NLMAdvancedApplication {
public:
    NLMAdvancedApplication() : brain(nullptr), featureManager(nullptr), advancedAPI(nullptr), integrationManager(nullptr) {
    }

    ~NLMAdvancedApplication() {
        shutdown();
    }

    bool initialize() {
        std::cout << "=== NLM Advanced Application Initialization ===" << std::endl;

        // Initialize core brain
        std::cout << "Initializing core brain..." << std::endl;
        brain = std::make_shared<Brain>(std::make_shared<Config>());

        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }

        // Initialize advanced features
        std::cout << "Initializing advanced features..." << std::endl;
        featureManager = std::make_shared<AdvancedFeatureManager>(brain);

        // Initialize advanced API
        std::cout << "Initializing advanced API..." << std::endl;
        advancedAPI = std::make_shared<AdvancedAPI>(brain, featureManager);

        // Initialize brain integration
        std::cout << "Initializing brain integration..." << std::endl;
        integrationManager = std::make_shared<BrainIntegrationManager>(brain);

        if (!integrationManager->initialize()) {
            std::cerr << "Failed to initialize brain integration!" << std::endl;
            return false;
        }

        // Initialize enhanced CLI
        std::cout << "Initializing enhanced CLI..." << std::endl;
        // CLI initialization would happen here

        std::cout << "NLM Advanced Application initialized successfully!" << std::endl;
        return true;
    }

    void run() {
        std::cout << "=== NLM Advanced Application Started ===" << std::endl;

        // Enable key advanced features
        enableAdvancedFeatures();

        // Main application loop
        while (running) {
            // Check for user input
            std::string input;
            std::cout << ">>> ";
            if (!std::getline(std::cin, input)) {
                break;  // EOF
            }

            handleCommand(input);
        }

        shutdown();
    }

    void shutdown() {
        std::cout << "Shutting down NLM Advanced Application..." << std::endl;

        // Stop integration
        if (integrationManager) {
            integrationManager->reset();
        }

        // Clean up resources
        advancedAPI.reset();
        featureManager.reset();
        integrationManager.reset();
        brain.reset();

        running = false;
        std::cout << "NLM Advanced Application shutdown completed." << std::endl;
    }

    void enableAdvancedFeatures() {
        std::cout << "Enabling advanced features..." << std::endl;

        // Enable key features
        std::vector<std::string> keyFeatures = {
            "memory_optimization",
            "prediction_enhancement",
            "neuromodulation_optimization",
            "cognitive_enhancement",
            "visualization",
            "export_analysis",
            "performance_monitoring",
            "scripting"
        };

        for (const std::string& featureName : keyFeatures) {
            if (featureManager) {
                featureManager->enableFeature(featureName);
                std::cout << "  Enabled feature: " << featureName << std::endl;
            }
        }

        // Execute all enabled features
        if (featureManager) {
            AdvancedFeatureConfig config;
            config.enabled = true;
            featureManager->executeAllFeatures(config);
        }

        std::cout << "Advanced features enabled successfully." << std::endl;
    }

    void handleCommand(const std::string& command) {
        std::istringstream stream(command);
        std::string cmd;
        stream >> cmd;

        if (cmd == "help" || cmd == "?") {
            showHelp();
        } else if (cmd == "exit" || cmd == "quit") {
            running = false;
        } else if (cmd == "features") {
            listFeatures();
        } else if (cmd == "brain") {
            showBrainStatus();
        } else if (cmd == "performance") {
            showPerformanceMetrics();
        } else if (cmd == "integration") {
            showIntegrationStatus();
        } else if (cmd == "export") {
            handleExportCommand(stream);
        } else if (cmd == "import") {
            handleImportCommand(stream);
        } else if (cmd == "script") {
            handleScriptCommand(stream);
        } else if (cmd == "analyze") {
            handleAnalyzeCommand(stream);
        } else if (cmd == "optimize") {
            handleOptimizeCommand(stream);
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
            showHelp();
        }
    }

    void showHelp() {
        std::cout << "=== NLM Advanced Application Commands ===" << std::endl;
        std::cout << "help, ?          - Show this help message" << std::endl;
        std::cout << "exit, quit      - Exit the application" << std::endl;
        std::cout << "features        - List all features and their status" << std::endl;
        std::cout << "brain           - Show brain system status" << std::endl;
        std::cout << "performance      - Show performance metrics" << std::endl;
        std::cout << "integration      - Show integration status" << std::endl;
        std::cout << "export          - Export system state or features" << std::endl;
        std::cout << "import          - Import system state or features" << std::endl;
        std::cout << "script          - Script execution commands" << std::endl;
        std::cout << "analyze         - Analyze system state or features" << std::endl;
        std::cout << "optimize        - Run optimization routines" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information about a specific command, type 'help <command>'" << std::endl;
    }

    void listFeatures() {
        std::cout << "=== Available Features ===" << std::endl;

        if (featureManager) {
            std::vector<std::string> features = featureManager->getAllFeatureNames();
            for (const std::string& featureName : features) {
                bool enabled = featureManager->isFeatureEnabled(featureName);
                AdvancedFeature feature = featureManager->getFeature(featureName);

                std::cout << featureName << ": " << (enabled ? "[ENABLED] " : "[DISABLED] ");
                std::cout << feature.description << std::endl;
                std::cout << "  Performance Impact: " << feature.performanceImpact << std::endl;
                std::cout << "  Dependencies: ";
                for (size_t i = 0; i < feature.dependencies.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << feature.dependencies[i];
                }
                std::cout << std::endl << std::endl;
            }
        }
    }

    void showBrainStatus() {
        std::cout << "=== Brain System Status ===" << std::endl;

        if (brain) {
            std::cout << "Neural Network Statistics:" << std::endl;
            std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
            std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
            std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "  Region Count: " << brain->getRegionCount() << std::endl;
            std::cout << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            std::cout << "  Pending Spike Events: " << brain->getPendingSpikeEventCount() << std::endl;
        }

        if (integrationManager) {
            std::cout << "\nIntegration Status:" << std::endl;
            std::cout << integrationManager->getIntegrationStatus() << std::endl;
        }
    }

    void showPerformanceMetrics() {
        std::cout << "=== Performance Metrics ===" << std::endl;

        if (featureManager) {
            PerformanceMetrics metrics = featureManager->getPerformanceMetrics();

            std::cout << "System Performance:" << std::endl;
            std::cout << "  CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
            std::cout << "  Memory Usage: " << metrics.memoryUsage << " MB" << std::endl;
            std::cout << "  Simulation Speed: " << metrics.simulationSpeed << " steps/second" << std::endl;
            std::cout << "  Real-time Performance: " << metrics.realTimePerformance << std::endl;
            std::cout << "  Last Update: " << getCurrentTimestamp() << std::endl;

            std::cout << "\nFeature Performance:" << std::endl;
            for (const auto& pair : metrics.featurePerformance) {
                std::cout << "  " << pair.first << ": " << pair.second << std::endl;
            }

            FeatureUsageReport usageReport = featureManager->generateUsageReport();
            std::cout << "\nUsage Summary:" << std::endl;
            std::cout << "  Total Features: " << usageReport.totalFeatures << std::endl;
            std::cout << "  Enabled Features: " << usageReport.enabledFeatures << std::endl;
            std::cout << "  Average Performance: " << usageReport.averagePerformance << std::endl;

            std::cout << "\nOptimization Recommendations:" << std::endl;
            for (const std::string& recommendation : usageReport.recommendations) {
                std::cout << "  - " << recommendation << std::endl;
            }
        }
    }

    void showIntegrationStatus() {
        std::cout << "=== Integration Status ===" << std::endl;

        if (integrationManager) {
            std::cout << integrationManager->getIntegrationStatus() << std::endl;

            // Show system health
            std::cout << "\nSystem Health:" << std::endl;
            auto healthStatus = integrationManager->getIntegrationStats();
            std::cout << "  Total Systems: " << healthStatus.totalSystems << std::endl;
            std::cout << "  Active Systems: " << healthStatus.activeSystems << std::endl;
            std::cout << "  Connected Systems: " << healthStatus.connectedSystems << std::endl;
            std::cout << "  Integration Health: " << healthStatus.integrationHealth << "%" << std::endl;

            // Show integration issues
            std::vector<std::string> issues = integrationManager->getIntegrationIssues();
            if (!issues.empty()) {
                std::cout << "\nIntegration Issues:" << std::endl;
                for (const std::string& issue : issues) {
                    std::cout << "  - " << issue << std::endl;
                }
            } else {
                std::cout << "\nNo integration issues detected." << std::endl;
            }
        }
    }

    void handleExportCommand(std::istringstream& stream) {
        std::string type;
        stream >> type;

        if (type == "brain" || type == "state") {
            std::string filepath;
            stream >> filepath;
            if (!filepath.empty()) {
                std::cout << "Exporting brain state to: " << filepath << std::endl;
                if (advancedAPI) {
                    advancedAPI->exportState(filepath);
                }
            } else {
                std::cout << "Usage: export brain <filepath>" << std::endl;
            }
        } else if (type == "features") {
            std::string filepath;
            stream >> filepath;
            if (!filepath.empty()) {
                std::cout << "Exporting feature configurations to: " << filepath << std::endl;
                if (advancedAPI) {
                    advancedAPI->exportFeatures(filepath);
                }
            } else {
                std::cout << "Usage: export features <filepath>" << std::endl;
            }
        } else {
            std::cout << "Unknown export type: " << type << std::endl;
            std::cout << "Usage: export <brain|features> <filepath>" << std::endl;
        }
    }

    void handleImportCommand(std::istringstream& stream) {
        std::string type;
        stream >> type;

        if (type == "brain" || type == "state") {
            std::string filepath;
            stream >> filepath;
            if (!filepath.empty()) {
                std::cout << "Importing brain state from: " << filepath << std::endl;
                if (advancedAPI) {
                    advancedAPI->importState(filepath);
                }
            } else {
                std::cout << "Usage: import brain <filepath>" << std::endl;
            }
        } else if (type == "features") {
            std::string filepath;
            stream >> filepath;
            if (!filepath.empty()) {
                std::cout << "Importing feature configurations from: " << filepath << std::endl;
                if (advancedAPI) {
                    advancedAPI->importFeatures(filepath);
                }
            } else {
                std::cout << "Usage: import features <filepath>" << std::endl;
            }
        } else {
            std::cout << "Unknown import type: " << type << std::endl;
            std::cout << "Usage: import <brain|features> <filepath>" << std::endl;
        }
    }

    void handleScriptCommand(std::istringstream& stream) {
        std::string subcommand;
        stream >> subcommand;

        if (subcommand == "list") {
            std::cout << "=== Available Scripts ===" << std::endl;
            if (advancedAPI) {
                std::vector<std::string> scripts = advancedAPI->listScripts();
                for (const std::string& script : scripts) {
                    std::cout << "  " << script << std::endl;
                }
            }
        } else if (subcommand == "load") {
            std::string name, scriptFile;
            stream >> name >> scriptFile;
            if (!name.empty() && !scriptFile.empty()) {
                std::cout << "Loading script: " << name << " from " << scriptFile << std::endl;
                if (advancedAPI) {
                    std::ifstream file(scriptFile);
                    if (file.is_open()) {
                        std::string scriptContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                        advancedAPI->loadScript(name, scriptContent);
                    }
                }
            } else {
                std::cout << "Usage: script load <name> <script_file>" << std::endl;
            }
        } else if (subcommand == "execute") {
            std::string name;
            stream >> name;
            if (!name.empty()) {
                std::cout << "Executing script: " << name << std::endl;
                if (advancedAPI) {
                    std::string result = advancedAPI->executeScript(name);
                    std::cout << "Script output:" << std::endl;
                    std::cout << result << std::endl;
                }
            } else {
                std::cout << "Usage: script execute <name>" << std::endl;
            }
        } else if (subcommand == "remove") {
            std::string name;
            stream >> name;
            if (!name.empty()) {
                std::cout << "Removing script: " << name << std::endl;
                if (advancedAPI) {
                    advancedAPI->removeScript(name);
                }
            } else {
                std::cout << "Usage: script remove <name>" << std::endl;
            }
        } else {
            std::cout << "Unknown script command: " << subcommand << std::endl;
            std::cout << "Available commands: list, load, execute, remove" << std::endl;
        }
    }

    void handleAnalyzeCommand(std::istringstream& stream) {
        std::string type;
        stream >> type;

        if (type == "brain") {
            std::cout << advancedAPI->analyzeBrainState() << std::endl;
        } else if (type == "feature") {
            std::string name;
            stream >> name;
            if (!name.empty()) {
                std::cout << advancedAPI->analyzeFeaturePerformance(name) << std::endl;
            } else {
                std::cout << "Usage: analyze feature <name>" << std::endl;
            }
        } else {
            std::cout << "Unknown analyze type: " << type << std::endl;
            std::cout << "Usage: analyze <brain|feature> [name]" << std::endl;
        }
    }

    void handleOptimizeCommand(std::istringstream& stream) {
        std::string type;
        stream >> type;

        if (type == "features") {
            std::cout << "Running feature optimization..." << std::endl;
            if (featureManager) {
                std::vector<std::string> recommendations = featureManager->getOptimizationSuggestions();
                if (recommendations.empty()) {
                    std::cout << "No optimization recommendations available." << std::endl;
                } else {
                    std::cout << "Optimization recommendations:" << std::endl;
                    for (const std::string& recommendation : recommendations) {
                        std::cout << "  - " << recommendation << std::endl;
                    }
                }
            }
        } else if (type == "performance") {
            std::cout << "Running performance optimization..." << std::endl;
            // Performance optimization would be implemented here
        } else {
            std::cout << "Unknown optimize type: " << type << std::endl;
            std::cout << "Usage: optimize <features|performance>" << std::endl;
        }
    }

    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    bool running = true;

private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AdvancedFeatureManager> featureManager;
    std::shared_ptr<AdvancedAPI> advancedAPI;
    std::shared_ptr<BrainIntegrationManager> integrationManager;
};  // End of NLMAdvancedApplication

// Main application entry point
int main() {
    std::cout << "=== NLM Advanced Application ===" << std::endl;
    std::cout << "Version: Phase 6 Advanced Features" << std::endl;
    std::cout << "Initializing NLM with advanced capabilities..." << std::endl;

    NLMAdvancedApplication app;

    if (!app.initialize()) {
        std::cerr << "Failed to initialize NLM Advanced Application!" << std::endl;
        return 1;
    }

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error during application execution: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "NLM Advanced Application terminated normally." << std::endl;
    return 0;
}
