#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <regex>
#include <sstream>

namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

Config::~Config() = default;

void Config::validateConfigValue(const ConfigValue& value) {
    // Basic validation rules
    std::visit([](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int>) {
            if (val < std::numeric_limits<int>::min() || val > std::numeric_limits<int>::max()) {
                throw std::invalid_argument("Integer value out of range");
            }
        }
        else if constexpr (std::is_same_v<T, int64_t>) {
            if (val < std::numeric_limits<int64_t>::min() || val > std::numeric_limits<int64_t>::max()) {
                throw std::invalid_argument("int64_t value out of range");
            }
        }
        else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(val) || std::isinf(val)) {
                throw std::invalid_argument("Invalid double value (NaN or infinity)");
            }
        }
        else if constexpr (std::is_same_v<T, bool>) {
            // No validation needed for bool
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            if (val.empty()) {
                // Empty string is allowed but log warning
            }
        }
        else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : val) {
                if (item < std::numeric_limits<int>::min() || item > std::numeric_limits<int>::max()) {
                    throw std::invalid_argument("Vector int value out of range");
                }
            }
        }
        else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : val) {
                if (std::isnan(item) || std::isinf(item)) {
                    throw std::invalid_argument("Vector double value is NaN or infinity");
                }
            }
        }
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            // String vector validation
        }
    }, value);
}

template<typename T>
const ConfigValue& Config::getValue(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        static const ConfigValue nullValue = T{};
        return nullValue;
    }
    return it->second.value;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second.value);
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
    validateConfigValue(value);
    pImpl->entries[key] = ConfigEntry(key, value, source);
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

const std::string& Config::getString(const std::string& key) const {
    const ConfigValue& value = getValue<std::string>(key);
    return std::get<std::string>(value);
}

int Config::getInt(const std::string& key) const {
    const ConfigValue& value = getValue<int>(key);
    return std::get<int>(value);
}

double Config::getDouble(const std::string& key) const {
    const ConfigValue& value = getValue<double>(key);
    return std::get<double>(value);
}

bool Config::getBool(const std::string& key) const {
    const ConfigValue& value = getValue<bool>(key);
    return std::get<bool>(value);
}

bool Config::has(const std::string& key) const {
    return pImpl->entries.find(key) != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.first);
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
        oss << "  " << entry.first << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.second.value);
        oss << "] (" << static_cast<int>(entry.second.source) << ")\n";
    }
    return oss.str();
}

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.second.description << "\n";
        file << entry.first << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
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
