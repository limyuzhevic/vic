#include <unordered_map>
#include <functional>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool hasError = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
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
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": " + line);
            hasError = true;
        }
    }
    
    return !hasError;
}

// Overloaded set method for string values
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set empty key");
        return;
    }
    
    // Validate key name (alphanumeric + underscores)
    bool validKey = true;
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            validKey = false;
            break;
        }
    }
    
    if (!validKey) {
        NLM_LOG_WARNING("Config key \"" + key + "\" contains unusual characters");
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set empty key");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set empty key");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set empty key");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Invalid command line argument: " + arg);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << "PLACEHOLDER_TIMESTAMP" << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        // Write description if present
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write key and value
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
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

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
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
    return oss.str();
}

bool Config::validateKey(const std::string& key, const std::string& expectedType) const {
    if (!has(key)) {
        return false;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    // Check type based on expectedType
    std::string type = toLower(expectedType);
    bool isValid = false;
    
    if (type == "int" || type == "integer") {
        isValid = std::holds_alternative<int>(it->value);
    } else if (type == "double" || type == "float" || type == "number") {
        isValid = std::holds_alternative<double>(it->value);
    } else if (type == "string") {
        isValid = std::holds_alternative<std::string>(it->value);
    } else if (type == "bool" || type == "boolean") {
        isValid = std::holds_alternative<bool>(it->value);
    }
    
    return isValid;
}

std::vector<std::string> Config::getMissingKeys(const std::vector<std::string>& requiredKeys) const {
    std::vector<std::string> missing;
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            missing.push_back(key);
        }
    }
    
    return missing;
}

bool Config::validateRange(const std::string& key, double min, double max) const {
    if (!has(key)) {
        NLM_LOG_WARNING("Cannot validate range for missing key: " + key);
        return false;
    }
    
    auto value = get<double>(key);
    if (!value.has_value()) {
        NLM_LOG_WARNING("Key \"" + key + "\" is not a numeric type");
        return false;
    }
    
    if (value.value() < min || value.value() > max) {
        NLM_LOG_WARNING("Key \"" + key + "\" value " + std::to_string(value.value()) +
                       " is outside valid range [" + std::to_string(min) + ", " + std::to_string(max) + "]");
        return false;
    }
    
    return true;
}

bool Config::validatePattern(const std::string& key, const std::string& pattern) const {
    if (!has(key)) {
        NLM_LOG_WARNING("Cannot validate pattern for missing key: " + key);
        return false;
    }
    
    auto value = get<std::string>(key);
    if (!value.has_value()) {
        NLM_LOG_WARNING("Key \"" + key + "\" is not a string type");
        return false;
    }
    
    return matchesPattern(value.value(), pattern);
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Basic validation
        if (entry.key.empty()) {
            errors.push_back("Empty key found");
        }
        
        // Type-specific validation
        std::string typeStr;
        if (std::holds_alternative<int>(entry.value)) {
            typeStr = "int";
        } else if (std::holds_alternative<double>(entry.value)) {
            typeStr = "double";
        } else if (std::holds_alternative<bool>(entry.value)) {
            typeStr = "bool";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            typeStr = "string";
        }
        
        if (entry.description.empty()) {
            errors.push_back("Key \"" + entry.key + "\" has no description");
        }
    }
    
    return errors;
}

// Validation implementation
bool Config::isValidKey(const std::string& key) {
    if (key.empty()) {
        return false;
    }
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Config::matchesPattern(const std::string& value, const std::string& pattern) {
    // Simple pattern matching for common cases
    if (pattern == "alphanumeric") {
        return std::all_of(value.begin(), value.end(), ::isalnum);
    } else if (pattern == "alphanumeric_with_underscores") {
        return std::all_of(value.begin(), value.end(), [](char c) {
            return std::isalnum(c) || c == '_';
        });
    }
    
    // For other patterns, just return true for now (can be extended)
    return true;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
