#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    Impl() : entries() {}
};

Config::Config() : pImpl(new Impl()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement JSON file loading
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // TODO: Implement command line argument parsing
    return false;
}

bool Config::saveToFile(const std::string& filepath) const {
    // TODO: Implement file saving
    return false;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            if constexpr (std::is_same_v<T, int>) {
                if (std::holds_alternative<int>(entry.value)) {
                    return std::get<int>(entry.value);
                }
            } else if constexpr (std::is_same_v<T, int64_t>) {
                if (std::holds_alternative<int64_t>(entry.value)) {
                    return std::get<int64_t>(entry.value);
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (std::holds_alternative<double>(entry.value)) {
                    return std::get<double>(entry.value);
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                if (std::holds_alternative<bool>(entry.value)) {
                    return std::get<bool>(entry.value);
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (std::holds_alternative<std::string>(entry.value)) {
                    return std::get<std::string>(entry.value);
                }
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                if (std::holds_alternative<std::vector<int>>(entry.value)) {
                    return std::get<std::vector<int>>(entry.value);
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                if (std::holds_alternative<std::vector<double>>(entry.value)) {
                    return std::get<std::vector<double>>(entry.value);
                }
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                    return std::get<std::vector<std::string>>(entry.value);
                }
            }
        }
    }
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Remove existing entry if any
    remove(key);
    
    // Add new entry
    pImpl->entries.push_back(ConfigEntry(key, value, source));
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, std::string(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, int(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, double(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, bool(value), source);
}

bool Config::has(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return true;
        }
    }
    return false;
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& entry) { return entry.key == key; });
    pImpl->entries.erase(it, pImpl->entries.end());
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
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[vector of strings]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[vector of doubles]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[vector of ints]";
            } else if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << "[unknown type]";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
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
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm