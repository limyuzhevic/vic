// NLM Advanced User Commands
// Provides utilities for advanced users to interact with the NLM brain

#ifndef NLM_ADVANCED_COMMANDS_H
#define NLM_ADVANCED_COMMANDS_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

/**
 * Advanced command system for NLM users
 * 
 * Provides enhanced command-line interface with:
 * - Save/load brain states
 * - Reset brain with different options
 * - Debug and profiling tools
 * - Performance monitoring
 * - Configuration management
 * - Experiment control
 */
class AdvancedCommands {
public:
    AdvancedCommands(std::shared_ptr<Brain> brain);
    ~AdvancedCommands();
    
    /**
     * Parse and execute advanced commands
     * @param commandLine Command line string
     * @return true if command executed successfully
     */
    bool executeCommand(const std::string& commandLine);
    
    /**
     * Register a custom command
     * @param name Command name
     * @param handler Handler function
     */
    void registerCommand(const std::string& name, 
                        std::function<bool(const std::vector<std::string>&)> handler);
    
    /**
     * Print help for all registered commands
     */
    void printHelp() const;
    
    /**
     * Get command history
     */
    const std::vector<std::string>& getCommandHistory() const { return commandHistory_; }
    
    /**
     * Clear command history
     */
    void clearHistory() { commandHistory_.clear(); }
    
    /**
     * Enable/disable verbose output
     */
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    /**
     * Check if verbose mode is enabled
     */
    bool isVerbose() const { return verbose_; }
    
private:
    // Command handlers
    bool handleSaveCommand(const std::vector<std::string>& args);
    bool handleLoadCommand(const std::vector<std::string>& args);
    bool handleResetCommand(const std::vector<std::string>& args);
    bool handleDebugCommand(const std::vector<std::string>& args);
    bool handleProfileCommand(const std::vector<std::string>& args);
    bool handlePerfCommand(const std::vector<std::string>& args);
    bool handleStatsCommand(const std::vector<std::string>& args);
    bool handleCheckpointCommand(const std::vector<std::string>& args);
    bool handleExperimentCommand(const std::vector<std::string>& args);
    bool handleConfigCommand(const std::vector<std::string>& args);
    bool handleExitCommand(const std::vector<std::string>& args);
    bool handleHelpCommand(const std::vector<std::string>& args);
    
    // Command parsing
    std::vector<std::string> parseCommandLine(const std::string& commandLine);
    std::string getCurrentWorkingDirectory() const;
    std::string getTimestamp() const;
    
    // Utility functions
    std::string formatBytes(uint64_t bytes) const;
    std::string formatTime(double seconds) const;
    std::string formatFloat(float value, int precision = 2) const;
    
    // State management
    std::shared_ptr<Brain> brain_;
    std::map<std::string, std::function<bool(const std::vector<std::string>&)>> commands_;
    std::vector<std::string> commandHistory_;
    bool verbose_;
    std::chrono::steady_clock::time_point sessionStart_;
};

} // namespace nlm

#endif // NLM_ADVANCED_COMMANDS_H
