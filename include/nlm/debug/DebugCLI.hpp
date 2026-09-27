// ==============================================================================
// Debug CLI Interface
// ==============================================================================
#ifndef DEBUGCLI_HPP
#define DEBUGCLI_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <chrono>
#include <mutex>
#include <thread>
#include "../include/nlm/plugin/PluginManager.hpp"
#include "../config/OptimizedConfig.hpp"
#include "../performance/PerformanceProfiler.hpp"

namespace nlm {
namespace debug {

/**
 * @class DebugCLI
 * @brief Command-line debugging interface for NLM
 * 
 * Provides interactive debugging capabilities for neural network systems
 * with command history, auto-completion, and real-time monitoring.
 */
class DebugCLI {
public:
    /**
     * @enum CommandCategory
     * @brief Categories of debug commands
     */
    enum class CommandCategory {
        GENERAL,
        NEURAL,
        MEMORY,
        PERFORMANCE,
        PLUGIN,
        CONFIGURATION,
        VISUALIZATION,
        EXPERIMENT
    };

    /**
     * @struct CommandInfo
     * @brief Information about a debug command
     */
    struct CommandInfo {
        std::string name;
        std::string description;
        CommandCategory category;
        std::function<bool(const std::vector<std::string>&)> handler;
        std::string usage;
        bool requiresAdmin;
    };

    DebugCLI();
    ~DebugCLI();

    /**
     * @brief Initialize the debug CLI with configuration
     * @param config Configuration for debugging
     */
    bool initialize(const Config& config);

    /**
     * @brief Start the interactive debugging session
     */
    void startSession();

    /**
     * @brief Stop the debugging session
     */
    void stopSession();

    /**
     * @brief Process a single command
     * @param command Command to process
     * @return true if command was successful
     */
    bool processCommand(const std::string& command);

    /**
     * @brief Register a debug command
     * @param cmdInfo Command information
     * @return true if command was registered successfully
     */
    bool registerCommand(const CommandInfo& cmdInfo);

    /**
     * @brief Get command history
     * @return Vector of recent commands
     */
    std::vector<std::string> getCommandHistory() const;

    /**
     * @brief Clear command history
     */
    void clearCommandHistory();

    /**
     * @brief Get available commands by category
     * @param category Command category
     * @return Vector of command names
     */
    std::vector<std::string> getCommandsByCategory(CommandCategory category) const;

    /**
     * @brief Enable or disable auto-completion
     * @param enabled True to enable auto-completion
     */
    void setAutoCompletion(bool enabled);

    /**
     * @brief Get current session statistics
     * @return Session statistics
     */
    struct SessionStats {
        uint64_t commandsProcessed;
        uint64_t errorsEncountered;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::duration totalProcessingTime;
    };

    SessionStats getSessionStats() const;

protected:
    /**
     * @brief Auto-complete command based on partial input
     * @param partial Partial command input
     * @return Vector of possible completions
     */
    std::vector<std::string> autoComplete(const std::string& partial) const;

    /**
     * @brief Print help information for a command
     * @param command Command name
     */
    void printCommandHelp(const std::string& command) const;

    /**
     * @brief Print all available commands
     */
    void printAllCommands() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Global debug CLI instance
 */
extern DebugCLI& getDebugCLI();

/**
 * @brief Initialize global debug CLI
 * @param config Configuration for debugging
 * @return true if initialization was successful
 */
bool initializeDebugCLI(const Config& config);

/**
 * @brief Shutdown global debug CLI
 */
void shutdownDebugCLI();

} // namespace debug
} // namespace nlm

#endif // DEBUGCLI_HPP
