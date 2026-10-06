#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {

// Enhanced configuration utilities with JSON support and better error handling
class ConfigUtils {
public:
    // JSON string utilities
    static std::string escapeJSON(const std::string& str);
    static std::string minifyJSON(const std::string& json);
    static bool isValidJSON(const std::string& json);
    
    // Configuration validation
    static bool validateKey(const std::string& key);
    static bool validateValue(const ConfigValue& value);
    static std::string getConfigValueTypeName(const ConfigValue& value);
    
    // File utilities
    static bool fileExists(const std::string& path);
    static bool isFileWritable(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static bool backupFile(const std::string& sourcePath, const std::string& backupPath);
    
    // Configuration statistics and analysis
    static struct ConfigStats {
        size_t totalEntries;
        size_t entriesBySource[4]; // Default, File, CommandLine, Runtime
        size_t entriesByType[7];   // int, int64_t, double, bool, string, vector<int>, etc.
        double memoryUsage;        // Approximate memory usage in bytes
    } calculateStats(const Config& config);
    
    // Configuration comparison and diff
    static std::vector<std::string> findDifferences(const Config& config1, const Config& config2);
    static std::string configToString(const Config& config, const std::string& format = "pretty");
    
    // Advanced configuration operations
    static bool mergeConfigs(Config& target, const Config& source);
    static bool overrideConfigs(Config& target, const Config& override);
    static Config getFilteredConfig(const Config& config, const std::vector<std::string>& includeKeys, const std::vector<std::string>& excludeKeys);
    
    // Performance monitoring
    static class ConfigMonitor {
    public:
        struct OperationStats {
            std::string operation;
            uint64_t count{0};
            uint64_t totalTime{0}; // nanoseconds
            uint64_t minTime{std::numeric_limits<uint64_t>::max()};
            uint64_t maxTime{0};
        };
        
        static ConfigMonitor& getInstance() {
            static ConfigMonitor instance;
            return instance;
        }
        
        void startOperation(const std::string& operation) {
            currentOperation = operation;
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        void endOperation() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
            uint64_t nanos = duration.count();
            
            auto it = std::find_if(operations.begin(), operations.end(),
                [&](const OperationStats& op) { return op.operation == currentOperation; });
            
            if (it == operations.end()) {
                operations.push_back({currentOperation, 1, nanos, nanos, nanos});
            } else {
                it->count++;
                it->totalTime += nanos;
                it->minTime = std::min(it->minTime, nanos);
                it->maxTime = std::max(it->maxTime, nanos);
            }
            currentOperation.clear();
        }
        
        void printReport() const {
            std::cout << "\n=== Configuration Monitor Report ===" << std::endl;
            std::cout << std::left << std::setw(30) << "Operation" 
                      << std::right << std::setw(10) << "Count"
                      << std::right << std::setw(20) << "Total Time (ns)"
                      << std::right << std::setw(20) << "Avg Time (ns)"
                      << std::right << std::setw(20) << "Min Time (ns)"
                      << std::right << std::setw(20) << "Max Time (ns)" << std::endl;
            std::cout << std::string(120, '-') << std::endl;
            
            for (const auto& op : operations) {
                uint64_t avg = op.count > 0 ? op.totalTime / op.count : 0;
                std::cout << std::left << std::setw(30) << op.operation
                          << std::right << std::setw(10) << op.count
                          << std::right << std::setw(20) << op.totalTime
                          << std::right << std::setw(20) << avg
                          << std::right << std::setw(20) << op.minTime
                          << std::right << std::setw(20) << op.maxTime << std::endl;
            }
            std::cout << std::string(120, '-') << std::endl;
        }
        
        void clear() {
            operations.clear();
        }
        
    private:
        ConfigMonitor() = default;
        std::string currentOperation;
        std::chrono::high_resolution_clock::time_point startTime;
        std::vector<OperationStats> operations;
    };
    
    // Configuration validation rules
    static class ValidationRules {
    public:
        struct Rule {
            std::string keyPattern;
            std::function<bool(const ConfigValue&)> validator;
            std::string errorMessage;
        };
        
        static void addRule(const std::string& keyPattern, std::function<bool(const ConfigValue&)> validator, const std::string& errorMessage);
        static bool validateConfig(const Config& config, std::vector<std::string>& errors);
        static std::vector<Rule> getAllRules();
        
    private:
        static std::vector<Rule> validationRules;
    };
    
    // Configuration migration support
    static class MigrationManager {
    public:
        struct Migration {
            std::string version;
            std::function<void(Config&)> migrationFunc;
            std::string description;
        };
        
        static void addMigration(const std::string& fromVersion, const std::string& toVersion, std::function<void(Config&)> migrationFunc, const std::string& description);
        static bool migrateConfig(Config& config, const std::string& currentVersion, const std::string& targetVersion);
        static std::vector<Migration> getMigrations();
        
    private:
        static std::vector<Migration> migrations;
    };
    
private:
    static std::unordered_map<std::string, size_t> validationRuleCount;
    static std::unordered_map<std::string, size_t> migrationCount;
};

} // namespace nlm
