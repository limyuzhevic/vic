#ifndef NLM_MAIN_HPP
#define NLM_MAIN_HPP

#include "Brain.hpp"
#include "Config/Config.hpp"
#include <string>
#include <memory>
#include <iostream>
#include <map>
#include <vector>

namespace nlm {

// Main NLM interface for easy usage
class NLMMain {
public:
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    struct SimulationConfig {
        std::string name;
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
        double timestep;
        std::map<std::string, std::string> customParameters;
        bool enableNeuromodulation;
        bool enableMemorySystems;
        bool enablePrediction;
        bool enableCognition;
        bool enableDevelopment;
        size_t maxCheckpoints;
        std::string checkpointDirectory;
        bool enablePerformanceMonitoring;
        size_t performanceSampleInterval;
    };
    
    struct SimulationStatus {
        bool isRunning;
        SimulationStep currentStep;
        Timestamp currentTime;
        size_t neuronCount;
        size_t regionCount;
        size_t activeNeurons;
        size_t firingNeurons;
        size_t totalSpikes;
        float averageFiringRate;
        float excitationInhibitionRatio;
        bool isResting;
        size_t stepsSinceLastEpisode;
        size_t replayInterval;
        std::chrono::high_resolution_clock::duration lastStepTime;
        size_t stepsPerSecond;
    };
    
    struct PerformanceStats {
        std::chrono::high_resolution_clock::duration totalTime;
        size_t totalSteps;
        float averageStepTime;  // in milliseconds
        size_t spikesPerSecond;
        size_t memoryUsage;  // in MB
        float cpuUsage;  // percentage
        size_t checkpointCount;
        std::chrono::high_resolution_clock::duration uptime;
    };
    
    NLMMain();
    ~NLMMain();
    
    // Initialize NLM with default configuration
    bool initialize();
    
    // Initialize NLM with custom configuration
    bool initialize(const SimulationConfig& config);
    
    // Load configuration from file
    bool loadConfiguration(const std::string& filepath);
    
    // Save configuration to file
    bool saveConfiguration(const std::string& filepath) const;
    
    // Start simulation
    bool startSimulation();
    
    // Stop simulation
    void stopSimulation();
    
    // Run simulation for specified number of steps
    bool runSteps(size_t steps);
    
    // Run simulation for specified duration
    bool runDuration(const std::chrono::high_resolution_clock::duration& duration);
    
    // Execute single simulation step
    bool step();
    
    // Reset simulation
    void reset();
    
    // Save current brain state
    bool saveBrain(const std::string& filepath) const;
    
    // Load brain state
    bool loadBrain(const std::string& filepath);
    
    // Get current brain reference
    Brain& getBrain();
    
    // Get current brain reference (const)
    const Brain& getBrain() const;
    
    // Get current configuration
    const SimulationConfig& getConfig() const;
    
    // Get current simulation status
    SimulationStatus getStatus() const;
    
    // Get performance statistics
    PerformanceStats getPerformanceStats() const;
    
    // Set log level
    void setLogLevel(LogLevel level);
    
    // Get log level
    LogLevel getLogLevel() const;
    
    // Enable/disable verbose output
    void setVerbose(bool verbose);
    
    // Is verbose mode enabled
    bool isVerbose() const;
    
    // Get version information
    static std::string getVersion();
    
    // Get build information
    static std::string getBuildInfo();
    
    // Print help information
    static void printHelp();
    
    // Print version information
    static void printVersion();
    
    // Print build information
    static void printBuildInfo();
    
    // Generate comprehensive report
    std::string generateReport() const;
    
    // Export metrics to file
    bool exportMetrics(const std::string& filepath) const;
    
    // Import metrics from file
    bool importMetrics(const std::string& filepath);
    
    // Compare with another brain state
    bool compareWith(const Brain& otherBrain, float& similarityScore) const;
    
    // Get system information
    static std::map<std::string, std::string> getSystemInfo();
    
    // Get available commands
    static std::vector<std::string> getAvailableCommands();
    
    // Execute command
    bool executeCommand(const std::string& command, const std::map<std::string, std::string>& parameters = {});
    
    // Check if command is available
    static bool isCommandAvailable(const std::string& command);
    
    // Get command help
    static std::string getCommandHelp(const std::string& command);
    
    // List all plugins
    static std::vector<std::string> listPlugins();
    
    // Enable plugin
    bool enablePlugin(const std::string& pluginName);
    
    // Disable plugin
    bool disablePlugin(const std::string& pluginName);
    
    // Get plugin information
    static std::map<std::string, std::string> getPluginInfo(const std::string& pluginName);
    
    // Set plugin parameter
    bool setPluginParameter(const std::string& pluginName, const std::string& parameter,
                          const std::string& value);
    
    // Get plugin parameter
    static std::string getPluginParameter(const std::string& pluginName, const std::string& parameter);
    
    // Validate configuration
    bool validateConfig(const SimulationConfig& config, std::string& errorMessage) const;
    
    // Get default configuration
    static SimulationConfig getDefaultConfig();
    
    // Get configuration schema
    static std::map<std::string, std::string> getConfigurationSchema();
    
    // Backup current state
    bool createBackup(const std::string& description = "");
    
    // Restore from backup
    bool restoreFromBackup(const std::string& backupName);
    
    // List backups
    static std::vector<std::string> listBackups();
    
    // Delete backup
    static bool deleteBackup(const std::string& backupName);
    
    // Get current backup directory
    static std::string getBackupDirectory();
    
    // Set backup directory
    static bool setBackupDirectory(const std::string& directory);
    
    // Enable automatic backups
    void enableAutomaticBackups(bool enable, size_t intervalSteps = 1000);
    
    // Set maximum backup count
    void setMaxBackups(size_t maxBackups);
    
    // Get current backup count
    size_t getBackupCount() const;
    
    // Clean up old backups
    size_t cleanupOldBackups(size_t daysToKeep = 30);
    
    // Enable checkpoint compression
    void enableCheckpointCompression(bool enable, CompressionLevel level = CompressionLevel::Balanced);
    
    // Set checkpoint retention
    void setCheckpointRetention(size_t days);
    
    // Enable performance profiling
    void enablePerformanceProfiling(bool enable, size_t sampleInterval = 100);
    
    // Get performance profile
    std::vector<PerformanceMetrics> getPerformanceProfile() const;
    
    // Clear performance profile
    void clearPerformanceProfile();
    
    // Export performance profile
    bool exportPerformanceProfile(const std::string& filepath) const;
    
    // Import performance profile
    bool importPerformanceProfile(const std::string& filepath);
    
    // Generate performance report
    std::string generatePerformanceReport() const;
    
    // Get memory usage information
    static std::map<std::string, size_t> getMemoryUsage();
    
    // Get CPU usage information
    static std::map<std::string, float> getCPUUsage();
    
    // Get disk usage information
    static std::map<std::string, std::map<std::string, size_t>> getDiskUsage();
    
    // Get network usage information
    static std::map<std::string, std::map<std::string, size_t>> getNetworkUsage();
    
    // Get process information
    static std::map<std::string, std::string> getProcessInfo();
    
    // Get thread information
    static std::vector<std::map<std::string, std::string>> getThreadInfo();
    
    // Get file information
    static std::map<std::string, std::map<std::string, std::string>> getFileInfo(const std::string& path);
    
    // Check system resources
    static std::map<std::string, bool> checkSystemResources();
    
    // Get system health status
    static std::map<std::string, std::string> getSystemHealth();
    
    // Get alerts
    static std::vector<std::string> getAlerts();
    
    // Clear alerts
    static void clearAlerts();
    
    // Add alert
    static void addAlert(const std::string& message, const std::string& severity = "INFO");
    
    // Get current alerts count
    static size_t getAlertsCount();
    
    // Enable debug mode
    void enableDebugMode(bool enable);
    
    // Is debug mode enabled
    bool isDebugMode() const;
    
    // Get debug information
    static std::map<std::string, std::string> getDebugInfo();
    
    // Set custom configuration
    void setCustomConfiguration(const std::map<std::string, std::string>& config);
    
    // Get custom configuration
    static std::map<std::string, std::string> getCustomConfiguration();
    
    // Reset to defaults
    void resetToDefaults();
    
    // Save session
    bool saveSession(const std::string& sessionName);
    
    // Load session
    bool loadSession(const std::string& sessionName);
    
    // List sessions
    static std::vector<std::string> listSessions();
    
    // Delete session
    static bool deleteSession(const std::string& sessionName);
    
    // Get current session name
    static std::string getCurrentSession();
    
    // Set current session
    static bool setCurrentSession(const std::string& sessionName);
    
    // Get session directory
    static std::string getSessionDirectory();
    
    // Set session directory
    static bool setSessionDirectory(const std::string& directory);
    
    // Enable auto-save
    void enableAutoSave(bool enable, const std::string& interval = "1h");
    
    // Get auto-save status
    static std::map<std::string, std::string> getAutoSaveStatus();
    
    // Disable all features
    void disableAllFeatures();
    
    // Re-enable all features
    void reEnableAllFeatures();
    
    // Get feature status
    static std::map<std::string, bool> getFeatureStatus();
    
    // Check if feature is available
    static bool isFeatureAvailable(const std::string& featureName);
    
    // Get feature information
    static std::map<std::string, std::string> getFeatureInfo(const std::string& featureName);
    
    // Enable feature
    bool enableFeature(const std::string& featureName);
    
    // Disable feature
    bool disableFeature(const std::string& featureName);
    
    // Toggle feature
    bool toggleFeature(const std::string& featureName);
    
    // Get all feature names
    static std::vector<std::string> getAllFeatureNames();
    
    // Get feature dependencies
    static std::vector<std::string> getFeatureDependencies(const std::string& featureName);
    
    // Check feature dependencies
    static bool checkFeatureDependencies(const std::string& featureName, std::string& errorMessage);
    
    // Get feature compatibility
    static std::map<std::string, std::string> getFeatureCompatibility(const std::string& featureName);
    
    // Get feature version
    static std::string getFeatureVersion(const std::string& featureName);
    
    // Get feature author
    static std::string getFeatureAuthor(const std::string& featureName);
    
    // Get feature description
    static std::string getFeatureDescription(const std::string& featureName);
    
    // Get feature license
    static std::string getFeatureLicense(const std::string& featureName);
    
    // Get feature contact
    static std::string getFeatureContact(const std::string& featureName);
    
    // Get feature tags
    static std::vector<std::string> getFeatureTags(const std::string& featureName);
    
    // Search features by tag
    static std::vector<std::string> searchFeaturesByTag(const std::string& tag);
    
    // Get feature count
    static size_t getFeatureCount();
    
    // Get enabled feature count
    static size_t getEnabledFeatureCount();
    
    // Get all feature names alphabetically
    static std::vector<std::string> getAllFeatureNamesAlphabetically();
    
    // Get all feature names by category
    static std::map<std::string, std::vector<std::string>> getAllFeatureNamesByCategory();
    
    // Get feature category
    static std::string getFeatureCategory(const std::string& featureName);
    
    // Set feature category
    static bool setFeatureCategory(const std::string& featureName, const std::string& category);
    
    // Get all categories
    static std::vector<std::string> getAllCategories();
    
    // Get features by category
    static std::vector<std::string> getFeaturesByCategory(const std::string& category);
    
    // Get feature count by category
    static size_t getFeatureCountByCategory(const std::string& category);
    
    // Get feature percentage by category
    static float getFeaturePercentageByCategory(const std::string& category);
    
    // Get feature utilization score
    static float getFeatureUtilizationScore(const std::string& featureName);
    
    // Get average feature utilization score
    static float getAverageFeatureUtilizationScore();
    
    // Get feature popularity score
    static float getFeaturePopularityScore(const std::string& featureName);
    
    // Get average feature popularity score
    static float getAverageFeaturePopularityScore();
    
    // Get feature installation date
    static std::chrono::high_resolution_clock::time_point getFeatureInstallationDate(const std::string& featureName);
    
    // Get feature last used date
    static std::chrono::high_resolution_clock::time_point getFeatureLastUsedDate(const std::string& featureName);
    
    // Update feature usage
    static void updateFeatureUsage(const std::string& featureName);
    
    // Get feature usage count
    static size_t getFeatureUsageCount(const std::string& featureName);
    
    // Get all features sorted by usage
    static std::vector<std::pair<std::string, size_t>> getAllFeaturesSortedByUsage();
    
    // Get all features sorted by popularity
    static std::vector<std::pair<std::string, float>> getAllFeaturesSortedByPopularity();
    
    // Get all features sorted by installation date
    static std::vector<std::pair<std::string, std::chrono::high_resolution_clock::time_point>> getAllFeaturesSortedByInstallation();
    
    // Get all features sorted by last used date
    static std::vector<std::pair<std::string, std::chrono::high_resolution_clock::time_point>> getAllFeaturesSortedByLastUsed();
    
    // Get feature statistics
    static std::map<std::string, std::string> getFeatureStatistics(const std::string& featureName);
    
    // Get all feature statistics
    static std::map<std::string, std::map<std::string, std::string>> getAllFeatureStatistics();
    
    // Get feature dependency graph
    static std::map<std::string, std::vector<std::string>> getFeatureDependencyGraph();
    
    // Get feature dependency depth
    static size_t getFeatureDependencyDepth(const std::string& featureName);
    
    // Get feature dependency breadth
    static size_t getFeatureDependencyBreadth(const std::string& featureName);
    
    // Get feature dependency cycle detection
    static bool detectFeatureDependencyCycles();
    
    // Get feature dependency resolution
    static bool resolveFeatureDependencies();
    
    // Get feature dependency status
    static std::map<std::string, bool> getFeatureDependencyStatus();
    
    // Get feature dependency errors
    static std::vector<std::string> getFeatureDependencyErrors();
    
    // Get feature dependency warnings
    static std::vector<std::string> getFeatureDependencyWarnings();
    
    // Get feature dependency info
    static std::map<std::string, std::string> getFeatureDependencyInfo(const std::string& featureName);
    
    // Get feature dependency configuration
    static std::map<std::string, std::string> getFeatureDependencyConfiguration(const std::string& featureName);
    
    // Set feature dependency configuration
    static bool setFeatureDependencyConfiguration(const std::string& featureName,
                                                const std::map<std::string, std::string>& config);
    
    // Get feature dependency statistics
    static std::map<std::string, std::string> getFeatureDependencyStatistics(const std::string& featureName);
    
    // Get all feature dependency statistics
    static std::map<std::string, std::map<std::string, std::string>> getAllFeatureDependencyStatistics();
    
    // Get feature dependency health
    static std::map<std::string, bool> getFeatureDependencyHealth();
    
    // Get feature dependency recommendations
    static std::vector<std::string> getFeatureDependencyRecommendations();
    
    // Get feature dependency alerts
    static std::vector<std::string> getFeatureDependencyAlerts();
    
    // Get feature dependency status report
    static std::string getFeatureDependencyStatusReport();
    
    // Get feature dependency error report
    static std::string getFeatureDependencyErrorReport();
    
    // Get feature dependency warning report
    static std::string getFeatureDependencyWarningReport();
    
    // Get feature dependency info report
    static std::string getFeatureDependencyInfoReport();
    
    // Get feature dependency configuration report
    static std::string getFeatureDependencyConfigurationReport();
    
    // Get feature dependency statistics report
    static std::string getFeatureDependencyStatisticsReport();
    
    // Get feature dependency health report
    static std::string getFeatureDependencyHealthReport();
    
    // Get feature dependency recommendations report
    static std::string getFeatureDependencyRecommendationsReport();
    
    // Get feature dependency alerts report
    static std::string getFeatureDependencyAlertsReport();
    
    // Get feature dependency comprehensive report
    static std::string getFeatureDependencyComprehensiveReport();
    
    // Get feature dependency detailed report
    static std::string getFeatureDependencyDetailedReport();
    
    // Get feature dependency summary report
    static std::string getFeatureDependencySummaryReport();
    
    // Get feature dependency quick report
    static std::string getFeatureDependencyQuickReport();
    
    // Get feature dependency status quick report
    static std::string getFeatureDependencyStatusQuickReport();
    
    // Get feature dependency error quick report
    static std::string getFeatureDependencyErrorQuickReport();
    
    // Get feature dependency warning quick report
    static std::string getFeatureDependencyWarningQuickReport();
    
    // Get feature dependency info quick report
    static std::string getFeatureDependencyInfoQuickReport();
    
    // Get feature dependency configuration quick report
    static std::string getFeatureDependencyConfigurationQuickReport();
    
    // Get feature dependency statistics quick report
    static std::string getFeatureDependencyStatisticsQuickReport();
    
    // Get feature dependency health quick report
    static std::string getFeatureDependencyHealthQuickReport();
    
    // Get feature dependency recommendations quick report
    static std::string getFeatureDependencyRecommendationsQuickReport();
    
    // Get feature dependency alerts quick report
    static std::string getFeatureDependencyAlertsQuickReport();
    
    // Get feature dependency comprehensive quick report
    static std::string getFeatureDependencyComprehensiveQuickReport();
    
    // Get feature dependency detailed quick report
    static std::string getFeatureDependencyDetailedQuickReport();
    
    // Get feature dependency summary quick report
    static std::string getFeatureDependencySummaryQuickReport();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_MAIN_HPP
