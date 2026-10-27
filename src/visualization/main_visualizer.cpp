// NLM Visualization Main Entry Point
// Entry point for the NLM visualization system

#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // Create visualization interface
    nlm::VisualizationInterface visualizer;
    
    // Create default visualization options
    nlm::VisualizationOptions options;
    options.outputFormat = nlm::OutputFormat::ANSI_COLOR;
    options.debugLevel = nlm::DebugLevel::DETAILED;
    options.showHelp = false;
    options.showTimestamps = true;
    options.showLegend = true;
    options.animated = true;
    options.refreshRate = 60.0;
    options.width = 1280;
    options.height = 720;
    options.theme = "dark";
    options.autoSave = false;
    options.maxHistory = 1000;
    options.enableInteractivity = true;
    options.enableFiltering = true;
    
    // Parse command line arguments
    bool helpRequested = false;
    std::string outputFile;
    std::string outputFormat;
    std::string visualizerType = "ansi_color";
    bool debugMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
        } else if (arg == "--version") {
            std::cout << "NLM Visualization System v1.0.0" << std::endl;
            return 0;
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
                options.outputFile = outputFile;
            }
        } else if (arg == "--format") {
            if (i + 1 < argc) {
                outputFormat = argv[++i];
                if (outputFormat == "ansi") {
                    options.outputFormat = nlm::OutputFormat::ANSI_COLOR;
                } else if (outputFormat == "html") {
                    options.outputFormat = nlm::OutputFormat::HTML;
                } else if (outputFormat == "svg") {
                    options.outputFormat = nlm::OutputFormat::SVG;
                } else if (outputFormat == "json") {
                    options.outputFormat = nlm::OutputFormat::JSON;
                } else if (outputFormat == "csv") {
                    options.outputFormat = nlm::OutputFormat::CSV;
                } else if (outputFormat == "3d") {
                    options.outputFormat = nlm::OutputFormat::THREEJS;
                }
            }
        } else if (arg == "--visualizer") {
            if (i + 1 < argc) {
                visualizerType = argv[++i];
                if (visualizerType == "ansi") {
                    options.outputFormat = nlm::OutputFormat::ANSI_COLOR;
                } else if (visualizerType == "html") {
                    options.outputFormat = nlm::OutputFormat::HTML;
                } else if (visualizerType == "svg") {
                    options.outputFormat = nlm::OutputFormat::SVG;
                }
            }
        } else if (arg == "--debug") {
            debugMode = true;
            options.debugLevel = nlm::DebugLevel::EXTENSIVE;
        } else if (arg == "--simple") {
            options.outputFormat = nlm::OutputFormat::TERMINAL;
        } else if (arg == "--fullscreen") {
            options.width = 1920;
            options.height = 1080;
        }
    }
    
    if (helpRequested) {
        printHelp();
        return 0;
    }
    
    // Initialize visualization
    std::cout << "=== NLM Visualization System ===" << std::endl;
    std::cout << "Initializing visualization..." << std::endl;
    
    if (!visualizer.initialize(options)) {
        std::cerr << "Failed to initialize visualization!" << std::endl;
        return 1;
    }
    
    std::cout << "Visualization initialized successfully." << std::endl;
    std::cout << "Visualize type: " << visualizerType << std::endl;
    std::cout << "Debug mode: " << (debugMode ? "enabled" : "disabled") << std::endl;
    
    // Main visualization loop (simplified)
    std::cout << "Starting visualization loop..." << std::endl;
    std::cout << "Press Ctrl+C to exit." << std::endl;
    
    try {
        while (visualizer.isActive()) {
            // Update visualization
            visualizer.update();
            
            // Render current frame
            visualizer.render();
            
            // Small delay to prevent excessive CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during visualization: " << e.what() << std::endl;
    }
    
    std::cout << "Visualization stopped." << std::endl;
    
    return 0;
}

void printHelp() {
    std::cout << "=== NLM Visualization Help ===" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm_visualizer [OPTIONS]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --output <file>         Set output file" << std::endl;
    std::cout << "  --format <format>       Set output format (ansi, html, svg, json, csv, 3d)" << std::endl;
    std::cout << "  --visualizer <type>     Set visualizer type" << std::endl;
    std::cout << "  --debug                 Enable debug mode" << std::endl;
    std::cout << "  --simple                Use terminal visualization" << std::endl;
    std::cout << "  --fullscreen            Use fullscreen mode" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_visualizer --help" << std::endl;
    std::cout << "  nlm_visualizer --format ansi" << std::endl;
    std::cout << "  nlm_visualizer --format html --output viz.html" << std::endl;
    std::cout << "  nlm_visualizer --debug" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}
