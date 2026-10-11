#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Visualizer.hpp"
#include "InteractiveVisualizer.hpp"
#include "NetworkVisualizer.hpp"
#include "SpikeVisualizer.hpp"
#include "DataExporter.hpp"
#include "AnimationGenerator.hpp"
#include "WebDashboard.hpp"
#include "HTMLExporter.hpp"

namespace nlm {

class Brain;

/**
 * CLI integration system for NLM visualization
 * 
 * Provides comprehensive CLI integration for all visualization systems:
 * - Command-line argument parsing
 * - Visualization command execution
 * - Parameter handling
 * - Output format management
 * - Progress reporting
 * - Error handling
 * - Help system
 * - Configuration integration
 * - Batch processing
 * - Integration with existing CLI
 * 
 * Features:
 * - Command-line argument parsing
 * - Subcommands for different visualization types
 * - Output format selection
 * - Parameter validation
 * - Progress reporting
 * - Error handling
 * - Help system
 * - Configuration file support
 * - Environment variable support
 * - Template-based commands
 * - Custom command support
 * - Integration with existing NLM CLI
 * 
 * Integration with:
 * - All visualization systems
 * - Core CLI engine
 * - Configuration system
 * - Experiment management
 * - Data processing pipeline
 * 
 * @see Visualizer
 * @see InteractiveVisualizer
 * @see NetworkVisualizer
 * @see SpikeVisualizer
 * @see DataExporter
 * @see AnimationGenerator
 * @see WebDashboard
 * @see HTMLExporter
 * @see CLIEngine
 */
class VisualizationIntegration {
public:
    /**
     * Command enumeration
     */
    enum class Command {
        HELP,
        VERSION,
        VISUALIZE,
        INTERACTIVE,
        NETWORK,
        SPIKES,
        EXPORT,
        ANIMATE,
        WEB,
        HTML,
        BATCH,
        CONFIG,
        RUN_EXPERIMENT
    };

    /**
     * Command line arguments
     */
    struct CommandArgs {
        Command command = Command::HELP;
        std::string inputFile;
        std::string outputFile;
        std::string format = "html";
        std::string type = "comprehensive";
        double duration = 60.0;
        int fps = 30;
        bool interactive = false;
        bool realTime = false;
        bool exportData = false;
        bool generateAnimation = false;
        bool startWebServer = false;
        int webPort = 8080;
        std::string webHost = "localhost";
        std::string theme = "light";
        bool help = false;
        bool version = false;
        std::string configFile;
        std::string experimentFile;
        bool verbose = false;
        bool quiet = false;
        std::unordered_map<std::string, std::string> extraOptions;
    };

    /**
     * Command result
     */
    struct CommandResult {
        bool success = false;
        std::string message;
        std::string output;
        double executionTime = 0.0;
        std::unordered_map<std::string, std::string> metadata;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };

    /**
     * Visualization integration constructor
     */
    VisualizationIntegration();
    
    /**
     * Visualization integration constructor with configuration
     */
    explicit VisualizationIntegration(const CommandArgs& args);
    
    /**
     * Visualization integration destructor
     */
    ~VisualizationIntegration();

    /**
     * Configure integration
     */
    void configure(const CommandArgs& args);
    
    /**
     * Parse command line arguments
     */
    bool parseArguments(int argc, char* argv[]);
    
    /**
     * Execute command
     */
    CommandResult executeCommand();
    
    /**
     * Handle help command
     */
    std::string handleHelpCommand() const;
    
    /**
     * Handle version command
     */
    std::string handleVersionCommand() const;
    
    /**
     * Handle visualize command
     */
    CommandResult handleVisualizeCommand() const;
    
    /**
     * Handle interactive command
     */
    CommandResult handleInteractiveCommand() const;
    
    /**
     * Handle network command
     */
    CommandResult handleNetworkCommand() const;
    
    /**
     * Handle spikes command
     */
    CommandResult handleSpikesCommand() const;
    
    /**
     * Handle export command
     */
    CommandResult handleExportCommand() const;
    
    /**
     * Handle animate command
     */
    CommandResult handleAnimateCommand() const;
    
    /**
     * Handle web command
     */
    CommandResult handleWebCommand() const;
    
    /**
     * Handle html command
     */
    CommandResult handleHtmlCommand() const;
    
    /**
     * Handle batch command
     */
    CommandResult handleBatchCommand() const;
    
    /**
     * Handle config command
     */
    CommandResult handleConfigCommand() const;
    
    /**
     * Handle run experiment command
     */
    CommandResult handleRunExperimentCommand() const;
    
    /**
     * Initialize visualization systems
     */
    bool initializeVisualizationSystems() const;
    
    /**
     * Execute visualization
     */
    CommandResult executeVisualization(const Brain& brain, const std::vector<SpikeEvent>& spikes) const;
    
    /**
     * Get command arguments
     */
    CommandArgs getArgs() const;
    
    /**
     * Set progress callback
     */
    void setProgressCallback(const std::function<void(double, const std::string&)>& callback);
    
    /**
     * Check if help is requested
     */
    bool isHelpRequested() const;
    
    /**
     * Check if version is requested
     */
    bool isVersionRequested() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Suppress warnings about unused private members
    VisualizationIntegration(const VisualizationIntegration&) = delete;
    VisualizationIntegration& operator=(const VisualizationIntegration&) = delete;
};

} // namespace nlm
