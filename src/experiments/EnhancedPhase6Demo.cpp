/**
 * Enhanced NLM Phase 6 Demo
 * 
 * Comprehensive Phase 6 integration demo with advanced features:
 * - Advanced CLI interface
 * - Configuration file support (.json, .cfg)
 * - Interactive modes
 * - Help system with examples
 * - Progress indicators and real-time feedback
 * - Detailed statistics and metrics
 * - Colorized output
 * - Logging integration
 * - Results persistence
 * - Visualization integration
 * - Educational features
 * - Power user features
 * - Advanced error handling
 * 
 * Maintains full backward compatibility with existing CLI usage
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/CLIParser.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/AdvancedConfigManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace nlm;

/**
 * Enhanced demo runner with all advanced features
 */
class EnhancedPhase6Demo {
public:
    EnhancedPhase6Demo() = default;
    ~EnhancedPhase6Demo() = default;
    
    /**
     * Run the enhanced Phase 6 demo
     * 
     * @param options Parsed command line options
     * @return Demo result
     */
    Phase6IntegrationResult run(const CLIParser::CommandLineOptions& options);
    
    /**
     * Display interactive menu
     */
    void showInteractiveMenu() const;
    
    /**
     * Display educational content
     * 
     * @param topic Educational topic
     */
    void showEducation(const std::string& topic) const;
    
    /**
     * Display power user menu
     */
    void showPowerUserMenu() const;
    
    /**
     * Export results to file
     * 
     * @param result Demo results
     * @param filepath Output file path
     * @param format Export format
     * @return true if successful
     */
    bool exportResults(const Phase6IntegrationResult& result, const std::string& filepath, const std::string& format = "json");
    
    /**
     * Import configuration from file
     * 
     * @param configPath Configuration file path
     * @return AdvancedConfigManager instance
     */
    AdvancedConfigManager importConfig(const std::string& configPath);
    
    /**
     * Display progress indicator
     * 
     * @param current Current step
     * @param total Total steps
     * @param width Progress bar width
     */
    void displayProgress(int current, int total, int width = 50) const;
    
    /**
     * Display formatted output with colors
     * 
     * @param message Message to display
     * @param level Log level/color
     * @param useColor Whether to use colors
     */
    void displayOutput(const std::string& message, const std::string& level = "INFO", bool useColor = true) const;
    
    /**
     * Save demo results to file
     * 
     * @param result Demo results
     * @param filepath Output file path
     * @return true if successful
     */
    bool saveResults(const Phase6IntegrationResult& result, const std::string& filepath) const;
    
    /**
     * Load demo results from file
     * 
     * @param filepath Input file path
     * @return Demo result
     */
    Phase6IntegrationResult loadResults(const std::string& filepath) const;
    
    /**
     * Display detailed statistics
     * 
     * @param result Demo results
     */
    void displayStatistics(const Phase6IntegrationResult& result) const;
    
    /**
     * Display system integration status
     * 
     * @param result Demo results
     */
    void displayIntegrationStatus(const Phase6IntegrationResult& result) const;
    
    /**
     * Display performance metrics
     * 
     * @param result Demo results
     */
    void displayPerformanceMetrics(const Phase6IntegrationResult& result) const;
    
    /**
     * Enable/disable colors
     * 
     * @param enable Whether to enable colors
     */
    void setColorsEnabled(bool enable) { colorsEnabled_ = enable; }
    
    /**
     * Check if colors are enabled
     * 
     * @return true if colors enabled
     */
    bool isColorsEnabled() const { return colorsEnabled_; }
    
    /**
     * Set progress bar enabled
     * 
     * @param enable Whether to enable progress bar
     */
    void setProgressEnabled(bool enable) { progressEnabled_ = enable; }
    
    /**
     * Set logging enabled
     * 
     * @param enable Whether to enable logging
     */
    void setLoggingEnabled(bool enable) { loggingEnabled_ = enable; }

private:
    bool colorsEnabled_ = true;
    bool progressEnabled_ = true;
    bool loggingEnabled_ = true;
    
    /**
     * Run demo with progress tracking
     * 
     * @param experiment Experiment instance
     * @param config Demo configuration
     * @return Demo result
     */
    Phase6IntegrationResult runWithProgress(Phase6IntegratedExperiment& experiment, const Phase6Config& config);
    
    /**
     * Handle educational mode
     * 
     * @param experiment Experiment instance
     * @param topic Educational topic
     * @param config Demo configuration
     * @return Demo result
     */
    Phase6IntegrationResult runEducationalMode(Phase6IntegratedExperiment& experiment, const std::string& topic, const Phase6Config& config);
    
    /**
     * Handle power user mode
     * 
     * @param experiment Experiment instance
     * @param config Demo configuration
     * @return Demo result
     */
    Phase6IntegrationResult runPowerUserMode(Phase6IntegratedExperiment& experiment, const Phase6Config& config);
    
    /**
     * Display tutorial content
     */
    void displayTutorial() const;
    
    /**
     * Display error message
     * 
     * @param message Error message
     * @param errorCode Error code
     */
    void displayError(const std::string& message, int errorCode = 1) const;
};