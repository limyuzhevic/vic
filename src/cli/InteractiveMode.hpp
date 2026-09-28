// src/cli/InteractiveMode.hpp
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace nlm {

class AdvancedCLI;
class SimulationController;

/**
 * Interactive mode for NLM CLI
 * 
 * Provides real-time text-based visualization and command interface
 * for controlling NLM simulations while they are running.
 */
class InteractiveMode {
public:
    InteractiveMode();
    ~InteractiveMode();
    
    /**
     * Start interactive mode
     * 
     * @param cli Advanced CLI instance
     * @param simulation Simulation controller instance
     * @return true if interactive mode started successfully
     */
    bool start(AdvancedCLI& cli, SimulationController& simulation);
    
    /**
     * Stop interactive mode
     * 
     * @return true if interactive mode stopped successfully
     */
    bool stop();
    
    /**
     * Check if interactive mode is active
     * 
     * @return true if interactive mode is active
     */
    bool isActive() const;
    
    /**
     * Display help information for interactive mode
     */
    void displayHelp() const;
    
    /**
     * Set visualization update frequency
     * 
     * @param frequency Updates per second
     */
    void setUpdateFrequency(int frequency);
    
    /**
     * Get current visualization update frequency
     * 
     * @return Current update frequency
     */
    int getUpdateFrequency() const;
    
    /**
     * Refresh display
     */
    void refreshDisplay();
    
    /**
     * Handle user input
     * 
     * @param input User input string
     * @return true if input was handled
     */
    bool handleInput(const std::string& input);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm