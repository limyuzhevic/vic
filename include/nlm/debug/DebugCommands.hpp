// ==============================================================================
// Debug CLI Commands
// ==============================================================================
#ifndef DEBUGCOMMANDS_HPP
#define DEBUGCOMMANDS_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "DebugCLI.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../config/OptimizedConfig.hpp"
#include "../performance/PerformanceProfiler.hpp"

namespace nlm {
namespace debug {

/**
 * @class DebugCommands
 * @brief Command implementations for NLM debugging
 * 
 * Provides implementations for all debugging commands
 * including neural, memory, performance, plugin, and configuration commands.
 */
class DebugCommands {
public:
    DebugCommands(DebugCLI& cli);
    ~DebugCommands();

    /**
     * @brief Execute a debug command
     * @param command Command to execute
     * @return true if command was executed successfully
     */
    bool execute(const std::string& command);

    /**
     * @brief Register all default debug commands
     */
    void registerDefaultCommands();

    /**
     * @brief Get the associated CLI instance
     * @return Reference to the CLI instance
     */
    DebugCLI& getCLI() const;

protected:
    /**
     * @brief Execute a command by name
     * @param commandName Command name
     * @param args Command arguments
     * @return true if command was executed
     */
    bool executeCommand(const std::string& commandName, 
                       const std::vector<std::string>& args);

    /**
     * @brief Parse command string into name and arguments
     * @param command Command string
     * @return Pair of command name and argument vector
     */
    std::pair<std::string, std::vector<std::string>> parseCommand(const std::string& command);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class DebugHelpers
 * @brief Utility functions for debugging
 * 
 * Provides helper functions for debugging operations including
 * formatting, validation, and data processing.
 */
class DebugHelpers {
public:
    DebugHelpers();
    ~DebugHelpers();

    /**
     * @brief Format neural network state for display
     * @param brain Neural network to format
     * @return Formatted string representation
     */
    static std::string formatNeuralNetwork(const Brain& brain);

    /**
     * @brief Format memory system state for display
     * @param memory Memory system to format
     * @return Formatted string representation
     */
    static std::string formatMemorySystem(const NeuralWorkingMemory& memory);

    /**
     * @brief Format neuromodulation state for display
     * @param neuromodulator Neuromodulation system to format
     * @return Formatted string representation
     */
    static std::string formatNeuromodulation(const Neuromodulator& neuromodulator);

    /**
     * @brief Format performance metrics for display
     * @param profiler Performance profiler
     * @return Formatted string representation
     */
    static std::string formatPerformanceMetrics(const PerformanceProfiler& profiler);

    /**
     * @brief Format configuration for display
     * @param config Configuration to format
     * @return Formatted string representation
     */
    static std::string formatConfiguration(const Config& config);

    /**
     * @brief Validate neural network configuration
     * @param brain Neural network to validate
     * @return Vector of validation errors
     */
    static std::vector<std::string> validateNeuralNetwork(const Brain& brain);

    /**
     * @brief Validate memory system configuration
     * @param memory Memory system to validate
     * @return Vector of validation errors
     */
    static std::vector<std::string> validateMemorySystem(const NeuralWorkingMemory& memory);

    /**
     * @brief Validate neuromodulation configuration
     * @param neuromodulator Neuromodulation system to validate
     * @return Vector of validation errors
     */
    static std::vector<std::string> validateNeuromodulation(const Neuromodulator& neuromodulator);

    /**
     * @brief Format spike data for analysis
     * @param spikeData Spike data to format
     * @return Formatted string representation
     */
    static std::string formatSpikeData(const std::vector<double>& spikeData);

    /**
     * @brief Format plasticity data for analysis
     * @param plasticityData Plasticity data to format
     * @return Formatted string representation
     */
    static std::string formatPlasticityData(const std::vector<std::pair<double, double>>& plasticityData);

    /**
     * @brief Calculate statistics for data
     * @param data Data to analyze
     * @return Statistics
     */
    struct Statistics {
        double mean;
        double median;
        double standardDeviation;
        double min;
        double max;
        double sum;
    };

    static Statistics calculateStatistics(const std::vector<double>& data);

    /**
     * @brief Format statistics for display
     * @param stats Statistics to format
     * @return Formatted string representation
     */
    static std::string formatStatistics(const Statistics& stats);

    /**
     * @brief Check if data is approximately equal
     * @param a First value
     * @param b Second value
     * @param tolerance Tolerance
     * @return true if values are approximately equal
     */
    static bool approximatelyEqual(double a, double b, double tolerance = 1e-6);

    /**
     * @brief Round value to specified precision
     * @param value Value to round
     * @param precision Precision
     * @return Rounded value
     */
    static double round(double value, int precision = 6);

    /**
     * @brief Format number with thousands separator
     * @param number Number to format
     * @return Formatted string
     */
    static std::string formatNumber(double number);

    /**
     * @brief Format duration for display
     * @param duration Duration in microseconds
     * @return Formatted string
     */
    static std::string formatDuration(uint64_t duration);

    /**
     * @brief Format memory size for display
     * @param bytes Memory size in bytes
     * @return Formatted string
     */
    static std::string formatMemorySize(uint64_t bytes);
};

} // namespace debug
} // namespace nlm

#endif // DEBUGCOMMANDS_HPP
