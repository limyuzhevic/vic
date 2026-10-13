#pragma once

#include "Config/ConfigurationManager.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

namespace nlm {

// Advanced configuration management commands
class AdvancedConfigurationManager {
public:
    AdvancedConfigurationManager();
    ~AdvancedConfigurationManager();
    
    // Configuration backup and restore
    bool backupConfiguration(const std::string& backupFilepath);
    bool restoreConfiguration(const std::string& backupFilepath);
    std::vector<std::string> listBackups(const std::string& directory = ".");
    
    // Configuration comparison
    std::string compareConfigurations(const std::string& file1, const std::string& file2);
    std::string compareToDefault(const std::string& configFile = "");
    
    // Configuration import/export
    bool exportConfiguration(const std::string& filepath, const std::string& format = "cfg");
    bool importConfiguration(const std::string& filepath, const std::string& format = "cfg");
    
    // Configuration analysis
    std::string analyzeConfiguration(const std::string& configFile = "");
    std::map<std::string, double> getConfigurationStats(const std::string& configFile = "");
    
    // Configuration templates
    bool saveTemplate(const std::string& name, const std::string& description);
    bool loadTemplate(const std::string& name);
    std::vector<std::string> listTemplates();
    bool deleteTemplate(const std::string& name);
    
    // Configuration validation commands
    bool validateConfiguration(const std::string& configFile = "");
    bool validateConfigurationAgainstRules(const std::string& configFile, const std::string& rulesFile);
    
    // Configuration optimization
    std::string optimizeConfiguration(const std::string& configFile = "");
    bool autoTuneParameters(const std::string& configFile = "");
    
    // Configuration statistics
    std::string getConfigurationStatistics(const std::string& configFile = "");
    std::string getPerformanceStatistics(const std::string& configFile = "");
    
    // Configuration diff
    std::string diffConfiguration(const std::string& file1, const std::string& file2);
    std::string generateConfigurationReport(const std::string& configFile = "");
    
    // Configuration commands
    bool executeCommand(const std::string& command, const std::string& configFile = "");
    bool executeBatchCommands(const std::vector<std::string>& commands, const std::string& configFile = "");
    
    // Configuration help
    static std::string getAdvancedCommandHelp();
    static std::string getConfigurationAnalysisHelp();
    static std::string getConfigurationBackupHelp();
    static std::string getConfigurationTemplateHelp();
    
    // Configuration command line parsing
    static std::vector<std::string> parseAdvancedCommandLineArgs(int argc, char** argv);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration command execution functions
bool executeConfigurationCommand(const std::string& command, ConfigurationManager& configManager);
bool executeConfigurationBatch(const std::vector<std::string>& commands, ConfigurationManager& configManager);
std::string getCommandOutput(const std::string& command, ConfigurationManager& configManager);

// Configuration file format support
class ConfigurationFileFormat {
public:
    enum class Format {
        CFG, INI, JSON, YAML, TOML, XML
    };
    
    static Format detectFormat(const std::string& filepath);
    static bool save(const std::string& filepath, const std::string& content, Format format);
    static bool load(std::string& content, const std::string& filepath, Format format);
    static std::string getFormatExtension(Format format);
    static std::string getFormatDescription(Format format);
    
    // Format-specific parsers
    static std::string parseCFG(const std::string& content);
    static std::string parseINI(const std::string& content);
    static std::string parseJSON(const std::string& content);
    static std::string parseYAML(const std::string& content);
    static std::string parseTOML(const std::string& content);
    static std::string parseXML(const std::string& content);
    
    // Format-specific generators
    static std::string generateCFG(const std::string& content);
    static std::string generateINI(const std::string& content);
    static std::string generateJSON(const std::string& content);
    static std::string generateYAML(const std::string& content);
    static std::string generateTOML(const std::string& content);
    static std::string generateXML(const std::string& content);
};

// Configuration comparison utilities
class ConfigurationComparator {
public:
    struct ComparisonResult {
        std::string file1;
        std::string file2;
        bool identical;
        std::vector<std::string> differences;
        std::vector<std::string> similarities;
        double similarityScore;
        
        std::string toString() const;
    };
    
    static ComparisonResult compareFiles(const std::string& file1, const std::string& file2);
    static ComparisonResult compareToDefault(const std::string& configFile = "");
    static std::vector<std::string> findDifferences(const std::string& file1, const std::string& file2);
    static std::vector<std::string> findSimilarities(const std::string& file1, const std::string& file2);
    static double calculateSimilarityScore(const std::string& file1, const std::string& file2);
    static std::string formatComparison(const ComparisonResult& result);
    
    // Configuration diff utilities
    static std::vector<std::string> generateDiff(const std::string& file1, const std::string& file2);
    static std::vector<std::string> generateUnifiedDiff(const std::string& file1, const std::string& file2);
    static std::vector<std::string> generateDetailedDiff(const std::string& file1, const std::string& file2);
};

// Configuration backup utilities
class ConfigurationBackup {
public:
    struct BackupInfo {
        std::string filepath;
        std::chrono::system_clock::time_point timestamp;
        std::string description;
        std::string size;
        
        std::string toString() const;
    };
    
    static bool createBackup(const std::string& configFile = "", const std::string& backupFilepath = "");
    static bool createBackupWithDescription(const std::string& description, const std::string& configFile = "");
    static BackupInfo getBackupInfo(const std::string& backupFilepath);
    static std::vector<BackupInfo> listBackups(const std::string& directory = ".");
    static bool restoreBackup(const std::string& backupFilepath, const std::string& restoreFilepath = "");
    static bool deleteBackup(const std::string& backupFilepath);
    static bool cleanOldBackups(const std::string& directory, int keepCount = 10);
    static std::string getBackupStatistics(const std::string& directory = ".");
    
    // Automatic backup
    static void startAutoBackup(const std::string& directory, int intervalMinutes = 60);
    static void stopAutoBackup();
    static bool isAutoBackupRunning();
};

// Configuration template utilities
class ConfigurationTemplate {
public:
    struct TemplateInfo {
        std::string name;
        std::string description;
        std::chrono::system_clock::time_point created;
        std::chrono::system_clock::time_point lastUsed;
        std::string version;
        
        std::string toString() const;
    };
    
    static bool createTemplate(const std::string& name, const std::string& description, const std::string& configFile = "");
    static bool loadTemplate(const std::string& name);
    static bool saveTemplateToFile(const std::string& name, const std::string& filepath);
    static bool loadTemplateFromFile(const std::string& filepath);
    static std::vector<TemplateInfo> listTemplates();
    static bool deleteTemplate(const std::string& name);
    static bool updateTemplate(const std::string& name, const std::string& description);
    static std::string getTemplateContent(const std::string& name);
    static bool importTemplate(const std::string& filepath);
    static bool exportTemplate(const std::string& name, const std::string& filepath);
    
    // Template management
    static std::vector<std::string> getTemplateNames();
    static bool renameTemplate(const std::string& oldName, const std::string& newName);
    static bool copyTemplate(const std::string& sourceName, const std::string& targetName);
    static bool mergeTemplate(const std::string& baseName, const std::string& overlayName, const std::string& resultName);
};

// Configuration analysis utilities
class ConfigurationAnalyzer {
public:
    struct AnalysisResult {
        std::string configFile;
        double complexityScore;
        double completenessScore;
        double performanceScore;
        std::vector<std::string> issues;
        std::vector<std::string> recommendations;
        std::map<std::string, double> parameterStats;
        
        std::string toString() const;
    };
    
    static AnalysisResult analyzeConfiguration(const std::string& configFile = "");
    static std::map<std::string, double> calculateParameterStats(const std::string& configFile = "");
    static std::vector<std::string> findConfigurationIssues(const std::string& configFile = "");
    static std::vector<std::string> generateRecommendations(const std::string& configFile = "");
    static std::string formatAnalysis(const AnalysisResult& result);
    
    // Configuration optimization
    static std::string optimizeConfiguration(const std::string& configFile = "");
    static bool autoTuneParameters(const std::string& configFile = "");
    static std::map<std::string, double> getPerformanceMetrics(const std::string& configFile = "");
    
    // Configuration validation
    static bool validateAgainstBestPractices(const std::string& configFile = "");
    static bool validateAgainstConstraints(const std::string& configFile = "");
    static std::vector<std::string> getValidationErrors(const std::string& configFile = "");
};

// Configuration command line interface
class ConfigurationCLI {
public:
    static bool processCommand(const std::string& command, ConfigurationManager& configManager);
    static bool processAdvancedCommand(const std::string& command, AdvancedConfigurationManager& advancedManager);
    static std::string getCommandHelp(const std::string& command);
    static std::string getAllCommandHelp();
    static bool executeConfigurationScript(const std::string& scriptFile, ConfigurationManager& configManager);
    static bool executeBatchConfiguration(const std::vector<std::string>& commands, ConfigurationManager& configManager);
    
    // Command processing
    static bool handleBackupCommands(const std::vector<std::string>& args, ConfigurationManager& configManager);
    static bool handleTemplateCommands(const std::vector<std::string>& args, ConfigurationManager& configManager);
    static bool handleAnalysisCommands(const std::vector<std::string>& args, ConfigurationManager& configManager);
    static bool handleComparisonCommands(const std::vector<std::string>& args, ConfigurationManager& configManager);
    static bool handleImportExportCommands(const std::vector<std::string>& args, ConfigurationManager& configManager);
    static bool handleValidationCommands(const std::vector<std::string>& args, ConfigurationManager& configManager);
    static bool handleOptimizationCommands(const std::vector<std::string>& args, ConfigurationManager& configManager);
};

// Convenience functions
void initializeAdvancedConfiguration();
void cleanupAdvancedConfiguration();
AdvancedConfigurationManager& getAdvancedConfigurationManager();
void setAdvancedConfigurationManager(std::shared_ptr<AdvancedConfigurationManager> manager);

// Configuration command line argument parsing
std::vector<std::string> parseAdvancedConfigurationArgs(int argc, char** argv);
bool processAdvancedConfigurationArgs(const std::vector<std::string>& args, ConfigurationManager& configManager);

// Configuration script execution
bool executeConfigurationScript(const std::string& scriptContent, ConfigurationManager& configManager);
bool executeConfigurationScriptFile(const std::string& scriptFile, ConfigurationManager& configManager);

// Configuration export/import
bool exportConfiguration(const std::string& configFile, const std::string& outputFile, const std::string& format = "cfg");
bool importConfiguration(const std::string& inputFile, std::string& configFile, const std::string& format = "cfg");

// Configuration backup/restore
bool backupConfiguration(const std::string& configFile = "", const std::string& backupFilepath = "");
bool restoreConfiguration(const std::string& backupFilepath, const std::string& configFile = "");

// Configuration analysis
std::string analyzeConfiguration(const std::string& configFile = "");
std::string compareConfigurations(const std::string& file1, const std::string& file2);

} // namespace nlm
