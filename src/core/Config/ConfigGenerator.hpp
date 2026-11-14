// NLM (熙然) - Enhanced Configuration File Generator
// Creates and validates configuration files with advanced features
// Supports preset profiles, validation, and configuration management

#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>

using namespace std;

// Configuration preset types
enum class ConfigPreset {
    DEVELOPMENT = 0,
    PRODUCTION = 1,
    BENCHMARK = 2,
    DEBUG = 3,
    HIGH_PERFORMANCE = 4,
    BALANCED = 5,
    LOW_MEMORY = 6,
    CUSTOM = 7
};

// Configuration profile
struct ConfigProfile {
    string name;
    map<string, string> settings;
    string description;
    bool isActive = false;
};

// Configuration backup information
struct ConfigBackup {
    string filename;
    string timestamp;
    map<string, string> settings;
    size_t size;
};

class ConfigGenerator {
public:
    ConfigGenerator();
    virtual ~ConfigGenerator() = default;
    
    // Configuration file generation
    bool generateConfiguration(const string& filename, const ConfigPreset& preset);
    bool generateConfiguration(const string& filename, const map<string, string>& settings);
    bool generateConfiguration(const string& filename, const ConfigProfile& profile);
    
    // Configuration validation
    bool validateConfiguration(const string& filename, vector<string>& errors) const;
    bool validateConfiguration(const map<string, string>& settings, vector<string>& errors) const;
    
    // Configuration backup and restore
    bool backupConfiguration(const string& originalFile, string& backupFile);
    bool restoreConfiguration(const string& backupFile, const string& originalFile);
    
    // Configuration comparison
    bool compareConfigurations(const string& file1, const string& file2, map<string, pair<string, string>>& differences);
    
    // Configuration analysis
    bool analyzeConfiguration(const string& filename, map<string, string>& analysis);
    double calculateConfigurationScore(const map<string, string>& settings) const;
    
    // Configuration management
    bool mergeConfigurations(const vector<string>& filenames, string& outputFile);
    bool exportConfiguration(const string& filename, const string& outputFormat);
    
    // Profile management
    bool saveProfile(const ConfigProfile& profile);
    bool loadProfile(const string& filename, ConfigProfile& profile);
    vector<ConfigProfile> getAvailableProfiles() const;
    
    // Export formats
    enum class ExportFormat {
        INTELLI_CONFIG,    // IntelliJ-style config
        ENV_FILE,          // Environment file
        JSON,              // JSON format
        YAML,              // YAML format
        PLAIN_TEXT,        // Plain text format
        DOCKER_COMPOSE,    // Docker compose format
        MAKEFILE,          // Makefile includes
        PYTHON,            // Python config
        CPP_HEADER,        // C++ header
        XML               // XML format
    };
    
    // Configuration templates
    static map<string, string> getTemplate(const string& templateName);
    static vector<string> getAvailableTemplates() const;
    
protected:
    vector<ConfigProfile> profiles_;
    map<string, map<string, string>> templates_;
    
    // Template management
    void initializeTemplates();
    bool validateTemplate(const map<string, string>& template) const;
    
    // Configuration file parsing
    static map<string, string> parseConfigurationFile(const string& filename);
    static bool writeConfigurationFile(const string& filename, const map<string, string>& settings);
    static bool validateConfigurationKey(const string& key);
    static bool validateConfigurationValue(const string& value);
    
    // Configuration analysis helpers
    static double calculateSettingScore(const string& key, const string& value);
    static string normalizeValue(const string& value);
    static string formatValue(const string& value, const string& key);
    
    // Export format helpers
    static string exportToIntelliConfig(const map<string, string>& settings);
    static string exportToEnvFile(const map<string, string>& settings);
    static string exportToJSON(const map<string, string>& settings);
    static string exportToYAML(const map<string, string>& settings);
    static string exportToPlainText(const map<string, string>& settings);
    static string exportToDockerCompose(const map<string, string>& settings);
    static string exportToMakefile(const map<string, string>& settings);
    static string exportToPython(const map<string, string>& settings);
    static string exportToCPPHeader(const map<string, string>& settings);
    static string exportToXML(const map<string, string>& settings);
    
    // Configuration preset helpers
    static map<string, string> getDevelopmentPreset();
    static map<string, string> getProductionPreset();
    static map<string, string> getBenchmarkPreset();
    static map<string, string> getDebugPreset();
    static map<string, string> getHighPerformancePreset();
    static map<string, string> getBalancedPreset();
    static map<string, string> getLowMemoryPreset();
};
