// Configuration Management Improvements
// Enhanced Configuration System for NLM

#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <variant>
#include <optional>

namespace nlm {

// Configuration Manager Enhancements
struct AdvancedConfigManager {
private:
    std::shared_ptr<Config> config_;
    std::string config_file_;
    bool auto_save_enabled_ = false;
    double auto_save_interval_ = 60.0;  // seconds
    std::chrono::steady_clock::time_point last_save_time_;
    
    // Advanced configuration features
    bool validate_on_set_ = true;
    std::vector<std::string> required_keys_;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators_;
    
public:
    AdvancedConfigManager(std::shared_ptr<Config> config, const std::string& config_file = "")
        : config_(config), config_file_(config_file), last_save_time_(std::chrono::steady_clock::now()) {}
    
    void enableAutoSave(double interval_seconds = 60.0) {
        auto_save_enabled_ = true;
        auto_save_interval_ = interval_seconds;
        last_save_time_ = std::chrono::steady_clock::now();
    }
    
    void disableAutoSave() {
        auto_save_enabled_ = false;
    }
    
    void enableValidation(bool enable = true) {
        validate_on_set_ = enable;
    }
    
    void addRequiredKey(const std::string& key) {
        required_keys_.push_back(key);
    }
    
    template<typename Validator>
    void addValidator(const std::string& key, Validator validator) {
        validators_[key] = validator;
    }
    
    bool validateConfiguration() const {
        for (const auto& key : required_keys_) {
            if (!config_->has(key)) {
                return false;
            }
        }
        
        for (const auto& [key, validator] : validators_) {
            auto value = config_->get<ConfigValue>(key);
            if (value.has_value()) {
                if (!validator(value.value())) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    bool checkAutoSaveNeeded() {
        if (!auto_save_enabled_) {
            return false;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - last_save_time_).count();
        
        if (elapsed >= auto_save_interval_) {
            last_save_time_ = now;
            return true;
        }
        
        return false;
    }
    
    bool saveWithTimestamp(const std::string& filepath = "") {
        std::string target_file = filepath.empty() ? config_file_ : filepath;
        
        if (target_file.empty()) {
            return false;
        }
        
        // Create backup if file exists
        if (std::filesystem::exists(target_file)) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
            
            std::string backup_file = target_file + ".backup_" + ss.str();
            std::filesystem::copy_file(target_file, backup_file, std::filesystem::copy_options::overwrite_existing);
        }
        
        // Add metadata to configuration
        std::string original_content = config_->summary();
        
        std::ofstream file(target_file);
        if (!file.is_open()) {
            return false;
        }
        
        // Write metadata header
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated: " << std::put_time(std::localtime(&std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now())), "%Y-%m-%d %H:%M:%S") << std::endl;
        file << "# Version: 2.0" << std::endl;
        file << "# Phase: 6 Final Integration" << std::endl;
        file << std::endl;
        
        // Write configuration data
        file << original_content;
        
        // Write footer
        file << std::endl;
        file << "# End of Configuration" << std::endl;
        
        return true;
    }
    
    std::optional<Config> loadWithValidation(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return std::nullopt;
        }
        
        // Load configuration
        std::shared_ptr<Config> new_config = std::make_shared<Config>();
        
        // For now, use the existing loadFromFile method
        if (!new_config->loadFromFile(filepath)) {
            return std::nullopt;
        }
        
        // Validate configuration
        if (validate_on_set_ && !validateConfiguration()) {
            return std::nullopt;
        }
        
        return *new_config;
    }
    
    // Enhanced configuration operations
    bool importFromJSON(const std::string& json_file) {
        // Placeholder for JSON import functionality
        // This would use a proper JSON parser
        return false;
    }
    
    bool exportToJSON(const std::string& json_file) const {
        // Placeholder for JSON export functionality
        // This would use a proper JSON serializer
        return false;
    }
    
    std::string getConfigurationSummary() const {
        std::ostringstream oss;
        oss << "Advanced Configuration Summary:" << std::endl;
        oss << "  File: " << (config_file_.empty() ? "Not specified" : config_file_) << std::endl;
        oss << "  Auto-save: " << (auto_save_enabled_ ? "Enabled" : "Disabled") << std::endl;
        if (auto_save_enabled_) {
            oss << "  Auto-save interval: " << auto_save_interval_ << " seconds" << std::endl;
        }
        oss << "  Validation: " << (validate_on_set_ ? "Enabled" : "Disabled") << std::endl;
        oss << "  Required keys: " << required_keys_.size() << std::endl;
        oss << "  Validators: " << validators_.size() << std::endl;
        oss << std::endl;
        oss << "Configuration Contents:" << std::endl;
        oss << config_->summary();
        return oss.str();
    }
};

// Enhanced Config Class with Modern C++ Features
class EnhancedConfig : public Config {
public:
    EnhancedConfig() : Config() {
        // Initialize enhanced configuration
        setupDefaultValidators();
    }
    
    void setupDefaultValidators() {
        // Add default validators for common configuration patterns
        addValidator<int>("neuron_count", [](const ConfigValue& val) {
            if (const int* v = std::get_if<int>(&val)) {
                return *v > 0 && *v <= 1000000;
            }
            return false;
        });
        
        addValidator<double>("simulation_timestep", [](const ConfigValue& val) {
            if (const double* v = std::get_if<double>(&val)) {
                return *v > 0.0 && *v <= 1.0;
            }
            return false;
        });
        
        addValidator<float>("learning_rate", [](const ConfigValue& val) {
            if (const float* v = std::get_if<float>(&val)) {
                return *v >= 0.0f && *v <= 1.0f;
            }
            return false;
        });
        
        addValidator<std::string>("brain_type", [](const ConfigValue& val) {
            if (const std::string* v = std::get_if<std::string>(&val)) {
                return v->empty() || (*v == "basic" || *v == "advanced" || *v == "expert");
            }
            return false;
        });
    }
    
    // Enhanced setters with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) override {
        if (validate_on_set_) {
            auto validator_it = validators_.find(key);
            if (validator_it != validators_.end() && !validator_it->second(value)) {
                throw std::invalid_argument("Validation failed for key: " + key);
            }
        }
        
        Config::set(key, value, source);
    }
    
    // Additional enhanced methods
    template<typename T>
    T getOrWithValidation(const std::string& key, const T& defaultValue) {
        auto value = get<T>(key);
        if (value.has_value()) {
            auto validator_it = validators_.find(key);
            if (validator_it != validators_.end() && !validator_it->second(value.value())) {
                return defaultValue;
            }
            return value.value();
        }
        return defaultValue;
    }
    
    bool validateKey(const std::string& key) const {
        auto validator_it = validators_.find(key);
        if (validator_it == validators_.end()) {
            return true;  // No validator, so no validation failure
        }
        
        auto value = get<ConfigValue>(key);
        if (!value.has_value()) {
            return false;  // Required key missing
        }
        
        return validator_it->second(value.value());
    }
    
    std::vector<std::string> getInvalidKeys() const {
        std::vector<std::string> invalid_keys;
        
        for (const auto& validator : validators_) {
            if (!validateKey(validator.first)) {
                invalid_keys.push_back(validator.first);
            }
        }
        
        return invalid_keys;
    }
    
    std::string generateConfigurationReport() const {
        std::ostringstream oss;
        oss << "=== Configuration Report ===" << std::endl;
        oss << "Generated: " << getTimestamp() << std::endl;
        oss << "Total Entries: " << config_->getKeys().size() << std::endl;
        oss << std::endl;
        
        oss << "Valid Keys: " << (config_->getKeys().size() - getInvalidKeys().size()) << std::endl;
        oss << "Invalid Keys: " << getInvalidKeys().size() << std::endl;
        
        if (!getInvalidKeys().empty()) {
            oss << std::endl << "Invalid Keys:" << std::endl;
            for (const auto& key : getInvalidKeys()) {
                oss << "  - " << key << std::endl;
            }
        }
        
        oss << std::endl << "Configuration Summary:" << std::endl;
        oss << summary();
        
        return oss.str();
    }
    
private:
    std::chrono::steady_clock::time_point getCurrentTime() const {
        return std::chrono::steady_clock::now();
    }
    
    std::string getTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators_;
    bool validate_on_set_ = true;
};

// Configuration Migration Utilities
class ConfigurationMigrator {
public:
    static bool migrateFromVersion1(const std::string& old_config_file, const std::string& new_config_file) {
        // Placeholder for version migration functionality
        // This would handle migrating configuration from older versions
        return false;
    }
    
    static bool validateConfigurationCompatibility(const Config& config) {
        // Placeholder for compatibility validation
        // This would validate configuration compatibility with different versions
        return true;
    }
    
    static std::string getMigrationReport(const std::string& old_version, const std::string& new_version) {
        std::ostringstream oss;
        oss << "=== Configuration Migration Report ===" << std::endl;
        oss << "From Version: " << old_version << std::endl;
        oss << "To Version: " << new_version << std::endl;
        oss << "Migration Status: Pending Implementation" << std::endl;
        oss << std::endl;
        oss << "Migration tasks:" << std::endl;
        oss << "  - Configuration schema updates" << std::endl;
        oss << "  - Default value migrations" << std::endl;
        oss << "  - Parameter name updates" << std::endl;
        oss << "  - Validation rule updates" << std::endl;
        
        return oss.str();
    }
};

// Configuration Snapshot Management
class ConfigurationSnapshot {
private:
    struct SnapshotEntry {
        std::string timestamp;
        std::shared_ptr<Config> config;
        std::string description;
    };
    
    std::vector<SnapshotEntry> snapshots_;
    std::string snapshot_file_;
    
public:
    ConfigurationSnapshot(const std::string& snapshot_file = "config_snapshots.json")
        : snapshot_file_(snapshot_file) {}
    
    void createSnapshot(const std::shared_ptr<Config>& config, const std::string& description = "") {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        
        SnapshotEntry entry;
        entry.timestamp = ss.str();
        entry.config = config;
        entry.description = description;
        
        snapshots_.push_back(entry);
    }
    
    std::optional<std::shared_ptr<Config>> restoreSnapshot(const std::string& timestamp) {
        for (const auto& snapshot : snapshots_) {
            if (snapshot.timestamp == timestamp) {
                return snapshot.config;
            }
        }
        return std::nullopt;
    }
    
    std::vector<std::string> getAvailableTimestamps() const {
        std::vector<std::string> timestamps;
        for (const auto& snapshot : snapshots_) {
            timestamps.push_back(snapshot.timestamp);
        }
        return timestamps;
    }
    
    std::string getSnapshotReport() const {
        std::ostringstream oss;
        oss << "=== Configuration Snapshot Report ===" << std::endl;
        oss << "Total Snapshots: " << snapshots_.size() << std::endl;
        oss << std::endl << "Available Snapshots:" << std::endl;
        
        for (size_t i = 0; i < snapshots_.size(); ++i) {
            oss << i + 1 << ". " << snapshots_[i].timestamp;
            if (!snapshots_[i].description.empty()) {
                oss << " - " << snapshots_[i].description;
            }
            oss << std::endl;
        }
        
        return oss.str();
    }
};

// Configuration Batch Operations
class ConfigurationBatch {
public:
    struct BatchOperation {
        std::string operation_type;  // "set", "remove", "update"
        std::string key;
        ConfigValue value;
        ConfigSource source;
        std::string description;
    };
    
    ConfigurationBatch() = default;
    
    void addSetOperation(const std::string& key, const ConfigValue& value, 
                        ConfigSource source = ConfigSource::Runtime,
                        const std::string& description = "") {
        BatchOperation op;
        op.operation_type = "set";
        op.key = key;
        op.value = value;
        op.source = source;
        op.description = description;
        operations_.push_back(op);
    }
    
    void addRemoveOperation(const std::string& key, const std::string& description = "") {
        BatchOperation op;
        op.operation_type = "remove";
        op.key = key;
        op.description = description;
        operations_.push_back(op);
    }
    
    void addUpdateOperation(const std::string& key, const ConfigValue& value,
                           ConfigSource source = ConfigSource::Runtime,
                           const std::string& description = "") {
        BatchOperation op;
        op.operation_type = "update";
        op.key = key;
        op.value = value;
        op.source = source;
        op.description = description;
        operations_.push_back(op);
    }
    
    bool executeBatch(std::shared_ptr<Config>& config) {
        bool all_success = true;
        
        for (const auto& op : operations_) {
            try {
                if (op.operation_type == "set") {
                    config->set(op.key, op.value, op.source);
                } else if (op.operation_type == "remove") {
                    config->remove(op.key);
                } else if (op.operation_type == "update") {
                    config->set(op.key, op.value, op.source);
                }
                
                std::cout << "Batch operation executed: " << op.description << std::endl;
                
            } catch (const std::exception& e) {
                std::cout << "Batch operation failed: " << op.description << " - " << e.what() << std::endl;
                all_success = false;
            }
        }
        
        return all_success;
    }
    
    void clear() {
        operations_.clear();
    }
    
    size_t getOperationCount() const {
        return operations_.size();
    }
    
    std::string getBatchReport() const {
        std::ostringstream oss;
        oss << "=== Configuration Batch Report ===" << std::endl;
        oss << "Total Operations: " << operations_.size() << std::endl;
        oss << std::endl << "Operations:" << std::endl;
        
        for (size_t i = 0; i < operations_.size(); ++i) {
            oss << i + 1 << ". " << operations_[i].operation_type << " " << operations_[i].key;
            if (!operations_[i].description.empty()) {
                oss << " - " << operations_[i].description;
            }
            oss << std::endl;
        }
        
        return oss.str();
    }
    
private:
    std::vector<BatchOperation> operations_;
};

// Export/Import Utilities
class ConfigurationExportImport {
public:
    static bool exportToXML(const Config& config, const std::string& xml_file) {
        // Placeholder for XML export functionality
        return false;
    }
    
    static bool importFromXML(const std::string& xml_file, std::shared_ptr<Config>& config) {
        // Placeholder for XML import functionality
        return false;
    }
    
    static bool exportToINI(const Config& config, const std::string& ini_file) {
        // Placeholder for INI export functionality
        return false;
    }
    
    static bool importFromINI(const std::string& ini_file, std::shared_ptr<Config>& config) {
        // Placeholder for INI import functionality
        return false;
    }
    
    static std::string getSupportedFormatsReport() {
        std::ostringstream oss;
        oss << "=== Supported Configuration Formats ===" << std::endl;
        oss << "Primary Format: Binary (Custom)" << std::endl;
        oss << "Secondary Formats: JSON, XML, INI (Placeholder)" << std::endl;
        oss << std::endl << "Export Options:" << std::endl;
        oss << "  - saveToFile() - Native binary format" << std::endl;
        oss << "  - exportToXML() - XML format (pending)" << std::endl;
        oss << "  - exportToINI() - INI format (pending)" << std::endl;
        oss << std::endl << "Import Options:" << std::endl;
        oss << "  - loadFromFile() - Native binary format" << std::endl;
        oss << "  - importFromXML() - XML format (pending)" << std::endl;
        oss << "  - importFromINI() - INI format (pending)" << std::endl;
        
        return oss.str();
    }
};

} // namespace nlm
