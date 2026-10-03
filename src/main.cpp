#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/visualization/VisualizationInterfaceAdvanced.hpp"

namespace nlm {

class VisualizationCommand {
public:
    VisualizationCommand(int argc, char* argv[]);
    ~VisualizationCommand();

    // Parse command line arguments
    bool parseArgs(int argc, char* argv[]);

    // Execute visualization command
    int execute();

    // Get help text
    static std::string getHelpText();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

VisualizationCommand::VisualizationCommand(int argc, char* argv[]) : pImpl(std::make_unique<Impl>()) {
    parseArgs(argc, argv);
}

VisualizationCommand::~VisualizationCommand() = default;

bool VisualizationCommand::parseArgs(int argc, char* argv[]) {
    // Simple argument parsing for demonstration
    std::vector<std::string> args(argv, argv + argc);
    
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--help" || args[i] == "-h") {
            std::cout << getHelpText() << std::endl;
            return true;
        } else if (args[i] == "--network") {
            if (i + 1 < args.size()) {
                pImpl->networkFile = args[++i];
            }
        } else if (args[i] == "--activity") {
            pImpl->visualizeActivity = true;
        } else if (args[i] == "--spikes") {
            pImpl->visualizeSpikes = true;
        } else if (args[i] == "--weights") {
            pImpl->visualizeWeights = true;
        } else if (args[i] == "--live") {
            pImpl->liveMode = true;
        } else if (args[i] == "--update-rate") {
            if (i + 1 < args.size()) {
                pImpl->updateRate = std::stod(args[++i]);
            }
        } else if (args[i] == "--output") {
            if (i + 1 < args.size()) {
                pImpl->outputFile = args[++i];
            }
        } else if (args[i] == "--web") {
            pImpl->webMode = true;
        } else if (args[i] == "--port") {
            if (i + 1 < args.size()) {
                pImpl->webPort = std::stoi(args[++i]);
            }
        }
    }
    
    return true;
}

int VisualizationCommand::execute() {
    std::cout << "NLM Visualization Command" << std::endl;
    std::cout << "==========================" << std::endl;
    
    if (pImpl->visualizeActivity || pImpl->visualizeSpikes || pImpl->visualizeWeights) {
        std::cout << "Visualization modes enabled: ";
        if (pImpl->visualizeActivity) std::cout << "Activity ";
        if (pImpl->visualizeSpikes) std::cout << "Spikes ";
        if (pImpl->visualizeWeights) std::cout << "Weights ";
        std::cout << std::endl;
    }
    
    if (pImpl->liveMode) {
        std::cout << "Live mode enabled with update rate: " << pImpl->updateRate << " Hz" << std::endl;
    }
    
    if (!pImpl->outputFile.empty()) {
        std::cout << "Output file: " << pImpl->outputFile << std::endl;
    }
    
    if (pImpl->webMode) {
        std::cout << "Web mode enabled on port: " << pImpl->webPort << std::endl;
    }
    
    // Create visualization configuration
    VisualizationConfig config;
    config.backend = "SDL";
    config.width = 1024;
    config.height = 768;
    config.updateRate = pImpl->updateRate;
    config.autoRefresh = pImpl->liveMode;
    config.enable3D = true;
    config.exportFormat = "PNG";
    config.outputPath = ".";
    config.enablePlugins = true;
    config.enablePerformanceMonitoring = true;
    config.enableRealTime = pImpl->liveMode;
    config.maxDataPoints = 10000;
    config.enableAnnotations = true;
    config.enableInteractivity = true;
    
    // Initialize visualization interface
    VisualizationInterface visualization;
    if (!visualization.initialize(config)) {
        std::cerr << "Failed to initialize visualization" << std::endl;
        return 1;
    }
    
    std::cout << "Visualization initialized successfully" << std::endl;
    
    return 0;
}

std::string VisualizationCommand::getHelpText() {
    return R"(
NLM Visualization Command Line Interface

USAGE:
    ./nlm visualize [options]

OPTIONS:
    --help, -h                    Show this help message
    --network FILE               Load network from file
    --activity                    Visualize neural activity
    --spikes                      Visualize spike raster plot
    --weights                     Visualize weight matrix
    --live                        Enable live visualization updates
    --update-rate RATE            Set update rate in Hz (default: 30)
    --output FILE                 Export visualization to file
    --web                         Enable web-based visualization
    --port PORT                   Set web port (default: 8080)

EXAMPLES:
    # Basic visualization
    ./nlm visualize --network brain.json

    # Real-time activity visualization
    ./nlm visualize --activity --live --update-rate 60

    # Export visualization
    ./nlm visualize --spikes --output spikes.svg

    # Web-based visualization
    ./nlm visualize --web --port 8080

    # Multiple visualization types
    ./nlm visualize --activity --spikes --weights --live

DESCRIPTION:
    This command provides comprehensive visualization capabilities for the NLM system,
    including network graphs, activity heatmaps, spike raster plots, weight matrices,
    3D neural views, time series, memory heatmaps, and prediction systems.
    Visualization can be exported in multiple formats (PNG, SVG, PDF, etc.) and
    can be accessed via a web interface.
    )";
}

} // namespace nlm

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << nlm::VisualizationCommand::getHelpText() << std::endl;
        return 0;
    }
    
    if (argv[1] == std::string("visualize")) {
        nlm::VisualizationCommand cmd(argc, argv);
        return cmd.execute();
    }
    
    std::cout << "Unknown command: " << argv[1] << std::endl;
    return 1;
}