#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <chrono>
#include "DebugUtils.hpp"
#include "ConfigManager.hpp"
#include "CheckpointSystem.hpp"
#include "Logger.hpp"

namespace nlm {

/**
 * Command-line interface utilities for NLM
 * 
 * Provides enhanced command-line parsing, configuration management,
 * brain checkpoint utilities, and experiment session management.
 * 
 * Features:
 * - Argument parsing (both --key=value and --key value formats)
 * - Configuration file I/O utilities
 * - Brain checkpoint utilities
 * - Experiment session management
 * - Logging configuration
 */
class CommandLineUtils {
public:
    CommandLineUtils();
    ~CommandLineUtils();
    
    // Disable copying, enable moving
    CommandLineUtils(const CommandLineUtils&) = delete;
    CommandLineUtils& operator=(const CommandLineUtils&) = delete;
    CommandLineUtils(CommandLineUtils&&) noexcept;
    CommandLineUtils& operator=(CommandLineUtils&&) noexcept;
    
    /**
     * Parse command-line arguments
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @return Map of parsed arguments
     */
    static std::unordered_map<std::string, std::string> parseArguments(int argc, char** argv);
    
    /**
     * Load configuration from file
     * 
     * @param filepath Configuration file path
     * @param validate Whether to validate configuration
     * @return Pointer to loaded ConfigManager or nullptr on failure
     */
    static std::shared_ptr<ConfigManager> loadConfig(const std::string& filepath, bool validate = true);
    
    /**
     * Save configuration to file
     * 
     * @param config ConfigManager to save
     * @param filepath Output file path
     * @param pretty Whether to format JSON with indentation
     * @return true if successful
     */
    static bool saveConfig(const std::shared_ptr<ConfigManager>& config, 
                          const std::string& filepath, bool pretty = true);
    
    /**
     * Create brain checkpoint
     * 
     * @param filepath Output checkpoint file path
     * @param compression Compression level (0-9)
     * @param config Configuration to include in checkpoint
     * @return true if successful
     */
    static bool createCheckpoint(const std::string& filepath, int compression = 6,
                                 const std::shared_ptr<ConfigManager>& config = nullptr);
    
    /**
     * Load brain checkpoint
     * 
     * @param filepath Checkpoint file path
     * @return true if successful
     */
    static bool loadCheckpoint(const std::string& filepath);
    
    /**
     * Save experiment session
     * 
     * @param sessionName Session name/identifier
     * @param config Configuration to save
     * @param step Current simulation step
     * @param simTime Current simulation time
     * @return true if successful
     */
    static bool saveSession(const std::string& sessionName,
                            const std::shared_ptr<ConfigManager>& config,
                            uint64_t step = 0, double simTime = 0.0);
    
    /**
     * Load experiment session
     * 
     * @param sessionName Session name/identifier
     * @return true if successful
     */
    static bool loadSession(const std::string& sessionName);
    
    /**
     * List available checkpoints
     * 
     * @param pattern Wildcard pattern for filtering
     * @return Vector of checkpoint info strings
     */
    static std::vector<std::string> listCheckpoints(const std::string& pattern = "*");
    
    /**
     * Set logging configuration
     * 
     * @param level Logging level (Debug, Info, Warning, Error, Critical)
     * @param file Output file path (empty for console)
     * @param format Log message format
     * @param maxSize Maximum log file size in bytes
     */
    static void configureLogging(const std::string& level = "Info",
                                 const std::string& file = "",
                                 const std::string& format = "default",
                                 size_t maxSize = 10 * 1024 * 1024); // 10MB
    
    /**
     * Get configuration summary
     * 
     * @param config ConfigManager to summarize
     * @return Formatted summary string
     */
    static std::string getConfigSummary(const std::shared_ptr<ConfigManager>& config);
    
    /**
     * Run simulation with configuration
     * 
     * @param config Configuration
     * @param steps Number of simulation steps
     * @return Simulation result string
     */
    static std::string runSimulation(const std::shared_ptr<ConfigManager>& config, 
                                    uint64_t steps = 1000);
    
    /**
     * Export brain state
     * 
     * @param brainState Brain state data
     * @param filepath Output file path
     * @return true if successful
     */
    static bool exportBrainState(const std::vector<uint8_t>& brainState, 
                                const std::string& filepath);
    
    /**
     * Import brain state
     * 
     * @param filepath Input file path
     * @return Brain state data vector
     */
    static std::vector<uint8_t> importBrainState(const std::string& filepath);
    
    /**
     * Get session info
     * 
     * @param sessionName Session name/identifier
     * @return Session info string
     */
    static std::string getSessionInfo(const std::string& sessionName);
    
    /**
     * Delete checkpoint/session
     * 
     * @param sessionName Session name/identifier
     * @return true if successful
     */
    static bool deleteSession(const std::string& sessionName);
    
    /**
     * Validate configuration against schema
     * 
     * @param config ConfigManager to validate
     * @return Validation result string
     */
    static std::string validateConfig(const std::shared_ptr<ConfigManager>& config);
    
    /**
     * Get command-line help
     * 
     * @param programName Program name
     * @return Help string
     */
    static std::string getHelp(const std::string& programName = "nlm");
    
    /**
     * Process command-line commands
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @param defaultConfig Default configuration
     * @return Result string
     */
    static std::string processCommandLine(int argc, char** argv,
                                          const std::shared_ptr<ConfigManager>& defaultConfig = nullptr);
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
