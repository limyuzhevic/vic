#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

namespace nlm {

// Validator base class
class Validator {
public:
    virtual ~Validator() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage(const std::string& key, const ConfigValue& value) const = 0;
};

// Type-specific validators
class IntValidator : public Validator {
public:
    IntValidator(int min = std::numeric_limits<int>::min(), 
                 int max = std::numeric_limits<int>::max())
        : min_(min), max_(max) {}
    bool validate(const std::string& key, const ConfigValue& value) const override {
        try {
            int val = std::get<int>(value);
            return val >= min_ && val <= max_;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        return "Value for key '" + key + "' must be an integer between " + 
               std::to_string(min_) + " and " + std::to_string(max_);
    }
private:
    int min_;
    int max_;
};

class DoubleValidator : public Validator {
public:
    DoubleValidator(double min = -std::numeric_limits<double>::max(),
                    double max = std::numeric_limits<double>::max(),
                    double tolerance = 1e-6)
        : min_(min), max_(max), tolerance_(tolerance) {}
    bool validate(const std::string& key, const ConfigValue& value) const override {
        try {
            double val = std::get<double>(value);
            return (val >= min_ - tolerance_) && (val <= max_ + tolerance_);
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        return "Value for key '" + key + "' must be a double between " + 
               std::to_string(min_) + " and " + std::to_string(max_);
    }
private:
    double min_;
    double max_;
    double tolerance_;
};

class StringValidator : public Validator {
public:
    StringValidator(const std::string& pattern = "", 
                   size_t minLength = 0,
                   size_t maxLength = std::string::max_size)
        : pattern_(pattern), minLength_(minLength), maxLength_(maxLength) {}
    bool validate(const std::string& key, const ConfigValue& value) const override {
        try {
            std::string val = std::get<std::string>(value);
            if (val.length() < minLength_ || val.length() > maxLength_) {
                return false;
            }
            if (!pattern_.empty()) {
                // Simple pattern matching (not regex)
                if (pattern_ == "path") {
                    // Path should not be empty and shouldn't contain invalid characters
                    if (val.empty() || val.find("::") != std::string::npos) {
                        return false;
                    }
                }
            }
            return true;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        std::string msg = "Value for key '" + key + "' must be a string";
        if (minLength_ > 0) {
            msg += " at least " + std::to_string(minLength_) + " characters long";
        }
        if (maxLength_ < std::string::max_size) {
            msg += " at most " + std::to_string(maxLength_) + " characters long";
        }
        if (!pattern_.empty()) {
            msg += " matching pattern '" + pattern_ + "'";
        }
        return msg;
    }
private:
    std::string pattern_;
    size_t minLength_;
    size_t maxLength_;
};

class BoolValidator : public Validator {
public:
    bool validate(const std::string& key, const ConfigValue& value) const override {
        return std::holds_alternative<bool>(value);
    }
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        return "Value for key '" + key + "' must be a boolean (true/false)";
    }
};

class VectorValidator : public Validator {
public:
    VectorValidator(size_t minSize = 0, size_t maxSize = std::string::max_size)
        : minSize_(minSize), maxSize_(maxSize) {}
    bool validate(const std::string& key, const ConfigValue& value) const override {
        try {
            if (std::holds_alternative<std::vector<int>>(value) ||
                std::holds_alternative<std::vector<double>>(value) ||
                std::holds_alternative<std::vector<std::string>>(value)) {
                
                const auto& vec = value;
                if (vec.size() < minSize_ || vec.size() > maxSize_) {
                    return false;
                }
                return true;
            }
            return false;
        } catch (...) {
            return false;
        }
    }
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        std::string msg = "Value for key '" + key + "' must be a vector";
        if (minSize_ > 0) {
            msg += " with at least " + std::to_string(minSize_) + " elements";
        }
        if (maxSize_ < std::string::max_size) {
            msg += " with at most " + std::to_string(maxSize_) + " elements";
        }
        return msg;
    }
private:
    size_t minSize_;
    size_t maxSize_;
};

} // namespace nlm

// Vector get methods template instantiations

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle --key value format
        else if (arg.substr(0, 2) == "--" && i + 1 < argc && argv[i + 1][0] != '-') {
            std::string key = arg.substr(2);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle -key value format (short options)
        else if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-' && i + 1 < argc && argv[i + 1][0] != '-') {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle boolean flags (--key=true/false, -key true/false)
        else if (arg.find('=') != std::string::npos) {
            size_t pos = arg.find('=');
            std::string key = trim(arg.substr(0, pos));
            std::string value = trim(arg.substr(pos + 1));
            
            // Convert string to appropriate type based on value content
            if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                set(key, boolVal, ConfigSource::CommandLine);
            } else if (value.find('.') != std::string::npos) {
                // Try to parse as double
                try {
                    double doubleVal = std::stod(value);
                    set(key, doubleVal, ConfigSource::CommandLine);
                } catch (...) {
                    set(key, value, ConfigSource::CommandLine);
                }
            } else {
                // Try to parse as int
                try {
                    // Try int64_t first (larger range)
                    size_t posEnd;
                    int64_t intVal = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, intVal, ConfigSource::CommandLine);
                    } else {
                        set(key, value, ConfigSource::CommandLine);
                    }
                } catch (...) {
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write file header comment
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Config System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert ConfigValue to string representation
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

// Vector get and set implementations
template<typename T>
std::optional<std::vector<T>> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        const auto& vec = std::get<std::vector<T>>(it->value);
        return vec;
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
void Config::set(const std::string& key, const std::vector<T>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

template<int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Vector get methods template instantiations
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

// Vector get methods template instantiations
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

// Helper method implementations

std::vector<std::pair<std::string, std::string>> Config::getAllEntries() const {
    std::vector<std::pair<std::string, std::string>> entries;
    for (const auto& entry : pImpl->entries) {
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    valueStr += std::to_string(arg[i]);
                    if (i < arg.size() - 1) valueStr += ", ";
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    valueStr += std::to_string(arg[i]);
                    if (i < arg.size() - 1) valueStr += ", ";
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueStr = "[\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    valueStr += arg[i];
                    if (i < arg.size() - 1) valueStr += "\", \";
                }
                valueStr += "\"]";
            } else {
                valueStr = "<unknown>";
            }
        }, entry.value);
        entries.emplace_back(entry.key, valueStr);
    }
    return entries;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& otherEntry : other.pImpl->entries) {
        // Check if this key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(otherEntry);
        } else if (source == ConfigSource::CommandLine || source == ConfigSource::Runtime) {
            // Override existing entry if new source has higher priority
            it->value = otherEntry.value;
            it->source = source;
        }
        // Otherwise keep existing entry (file or default overrides command line/runtime)
    }
}

std::string Config::resolvePath(const std::string& path) const {
    if (path.empty() || path.front() == '/') {
        // Absolute path or empty path - return as-is
        return path;
    }
    
    // Try to find relative to current working directory
    std::filesystem::path fullPath = std::filesystem::current_path();
    fullPath /= path;
    return fullPath.string();
}

std::vector<std::pair<std::string, std::string>> Config::getConfigValues(const std::vector<std::string>& keys) const {
    std::vector<std::pair<std::string, std::string>> values;
    for (const auto& key : keys) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            std::string valueStr;
            std::visit([&valueStr](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    valueStr = arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    valueStr = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    valueStr = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    valueStr = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, bool>) {
                    valueStr = arg ? "true" : "false";
                } else {
                    valueStr = "<unknown>";
                }
            }, it->value);
            values.emplace_back(key, valueStr);
        }
    }
    return values;
}

std::string Config::summary(const std::string& filter) const {
    std::ostringstream oss;
    
    if (filter.empty()) {
        oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
        for (const auto& entry : pImpl->entries) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
    } else {
        oss << "Configuration Summary (filtered by '" << filter << "'):\n";
        for (const auto& entry : pImpl->entries) {
            if (entry.key.find(filter) != std::string::npos) {
                oss << "  " << entry.key << " = [";
                std::visit([&oss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << arg << "\"";
                    } else {
                        oss << arg;
                    }
                }, entry.value);
                oss << "] (" << static_cast<int>(entry.source) << ")\n";
            }
        }
    }
    
    return oss.str();
}

// Validation method implementations

void Config::addValidator(const std::string& key, std::shared_ptr<Validator> validator) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->validators[key] = validator;
    }
}

bool Config::validateAll() const {
    std::vector<std::string> errors;
    for (const auto& entry : pImpl->entries) {
        auto valIt = pImpl->validators.find(entry.key);
        if (valIt != pImpl->validators.end()) {
            std::shared_ptr<Validator> validator = valIt->second;
            if (!validator->validate(entry.key, entry.value)) {
                errors.push_back(validator->getErrorMessage(entry.key, entry.value));
            }
        }
    }
    
    if (!errors.empty()) {
        // Store errors for later retrieval
        pImpl->validationErrors = errors;
        return false;
    }
    
    pImpl->validationErrors.clear();
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

bool Config::isValid(const std::string& key) const {
    auto valIt = pImpl->validators.find(key);
    if (valIt == pImpl->validators.end()) {
        return true; // No validator means valid
    }
    
    auto configIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (configIt == pImpl->entries.end()) {
        return true; // Key doesn't exist (optional)
    }
    
    return valIt->second->validate(key, configIt->value);
}

bool Config::isRequired(const std::string& key) const {
    return std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; }) != pImpl->entries.end();
}
