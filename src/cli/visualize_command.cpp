// NLM Advanced CLI - Visualization Command Implementation
#pragma once

#include "cli/main_cli.hpp"
#include "cli/command_factory.hpp"
#include "brain/Brain.hpp"
#include "visualization/terminal_renderer.hpp"
#include "visualization/activity_monitor.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <iomanip>

namespace nlm {

class VisualizeCommand : public BaseCommand {
public:
    VisualizeCommand() : BaseCommand("visualize", "Neural network visualization (ASCII/terminal)") {}
    
protected:
    CLIResult executeImpl(const std::map<std::string, std::string>& options) override {
        if (options.empty()) {
            displayHelpImpl();
            return CLIResult::HelpDisplayed;
        }
        
        std::string action = getOption(options, "action", "network");
        
        if (action == "network") {
            int width = getOption(options, "width", "80") != "0" ? std::stoi(getOption(options, "width", "80")) : 0;
            int height = getOption(options, "height", "40") != "0" ? std::stoi(getOption(options, "height", "40")) : 0;
            bool animate = getOption(options, "animate", "false") == "true" || getOption(options, "animate", "false") == "1" || getOption(options, "animate", "false") == "yes";
            int steps = getOption(options, "steps", "0") != "0" ? std::stoi(getOption(options, "steps", "0")) : 0;
            return handleNetworkVisualization(width, height, animate, steps);
        } else if (action == "activity") {
            int width = getOption(options, "width", "80") != "0" ? std::stoi(getOption(options, "width", "80")) : 0;
            int height = getOption(options, "height", "40") != "0" ? std::stoi(getOption(options, "height", "40")) : 0;
            bool animate = getOption(options, "animate", "false") == "true" || getOption(options, "animate", "false") == "1" || getOption(options, "animate", "false") == "yes";
            int steps = getOption(options, "steps", "0") != "0" ? std::stoi(getOption(options, "steps", "0")) : 0;
            return handleActivityVisualization(width, height, animate, steps);
        } else if (action == "memory") {
            return handleMemoryVisualization();
        } else if (action == "progress") {
            return handleProgressVisualization();
        } else if (action == "weights") {
            return handleWeightsVisualization();
        } else {
            std::cout << "Unknown visualize action: " << action << std::endl;
            displayHelpImpl();
            return CLIResult::Error;
        }
    }
    
    void displayHelpImpl() const override {
        std::cout << "\n=== Visualize Command Help ===\n" << std::endl;
        std::cout << "Usage: nlm visualize [options]\n" << std::endl;
        std::cout << "Actions:" << std::endl;
        std::cout << "  --action=network [--width=<w>] [--height=<h>] [--animate] [--steps=<n>]\n" << std::endl;
        std::cout << "  --action=activity [--width=<w>] [--height=<h>] [--animate] [--steps=<n>]\n" << std::endl;
        std::cout << "  --action=memory\n" << std::endl;
        std::cout << "  --action=progress\n" << std::endl;
        std::cout << "  --action=weights\n" << std::endl;
        std::cout << "\nOptions:" << std::endl;
        std::cout << "  --width=<w>      Display width in characters (default: auto)" << std::endl;
        std::cout << "  --height=<h>     Display height in characters (default: auto)" << std::endl;
        std::cout << "  --animate         Enable animation" << std::endl;
        std::cout << "  --steps=<n>       Number of simulation steps to run" << std::endl;
    }
    
private:
    CLIResult handleNetworkVisualization(int width, int height, bool animate, int steps) {
        std::cout << "=== Network Visualization ===\n" << std::endl;
        
        if (!globalState.terminalRenderer) {
            std::cout << "Error: Terminal renderer not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        // Configure renderer
        if (width > 0) globalState.terminalRenderer->setWidth(width);
        if (height > 0) globalState.terminalRenderer->setHeight(height);
        
        if (animate) {
            std::cout << "Running animation for " << steps << " steps...\n" << std::endl;
            for (int i = 0; i < steps && globalState.running; ++i) {
                globalState.terminalRenderer->visualizeNetwork(*globalState.brain);
                globalState.terminalRenderer->update();
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
                if (globalState.debugTools) {
                    globalState.debugTools->logEvent("Visualization step " + std::to_string(i));
                }
            }
        } else {
            std::cout << "Displaying static network visualization...\n" << std::endl;
            globalState.terminalRenderer->visualizeNetwork(*globalState.brain);
            globalState.terminalRenderer->update();
        }
        
        std::cout << "\nNetwork visualization completed.\n" << std::endl;
        return CLIResult::Success;
    }
    
    CLIResult handleActivityVisualization(int width, int height, bool animate, int steps) {
        std::cout << "=== Activity Visualization ===\n" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        activity_monitor monitor;
        
        if (width > 0) monitor.setWidth(width);
        if (height > 0) monitor.setHeight(height);
        
        std::cout << "Setting up activity monitor...\n" << std::endl;
        monitor.initialize();
        
        if (animate) {
            std::cout << "Running activity animation for " << steps << " steps...\n" << std::endl;
            for (int i = 0; i < steps && globalState.running; ++i) {
                monitor.update(*globalState.brain);
                monitor.render();
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
                if (globalState.debugTools) {
                    globalState.debugTools->logEvent("Activity monitor step " + std::to_string(i));
                }
            }
        } else {
            std::cout << "Displaying static activity visualization...\n" << std::endl;
            monitor.update(*globalState.brain);
            monitor.render();
        }
        
        monitor.close();
        std::cout << "\nActivity visualization completed.\n" << std::endl;
        return CLIResult::Success;
    }
    
    CLIResult handleMemoryVisualization() {
        std::cout << "=== Memory Visualization ===\n" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "Memory system visualization not yet implemented.\n" << std::endl;
        
        return CLIResult::Success;
    }
    
    CLIResult handleProgressVisualization() {
        std::cout << "=== Progress Visualization ===\n" << std::endl;
        
        std::cout << "Simulation progress tracking visualization.\n" << std::endl;
        std::cout << "This would show experiment progress, learning curves, etc.\n" << std::endl;
        
        return CLIResult::Success;
    }
    
    CLIResult handleWeightsVisualization() {
        std::cout << "=== Weight Matrix Visualization ===\n" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "Weight matrix visualization would be displayed here.\n" << std::endl;
        
        return CLIResult::Success;
    }
    
    // Helper methods
    std::string getOption(const std::map<std::string, std::string>& options, const std::string& key, const std::string& defaultValue) const {
        auto it = options.find(key);
        return it != options.end() ? it->second : defaultValue;
    }
};

} // namespace nlm

