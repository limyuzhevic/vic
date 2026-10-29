#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

Config::Config() : root_(json::object()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : root_(std::move(other.root_)), entries_(std::move(other.entries_)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        root_ = std::move(other.root_);
        entries_ = std::move(other.entries_);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file >> root_;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromString(const std::string& jsonString) {
    try {
        root_ = json::parse(jsonString);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --section.key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Try to parse value as number/bool
                try {
                    // Try as integer
                    size_t pos_int = value.find('.');
                    if (pos_int == std::string::npos) {
                        int intVal = std::stoi(value);
                        set<std::string>({key}, intVal, ConfigSource::CommandLine, "From command line");
                        continue;
                    }
                } catch (...) {}
                
                try {
                    // Try as double
                    double doubleVal = std::stod(value);
                    set<std::string>({key}, doubleVal, ConfigSource::CommandLine, "From command line");
                    continue;
                } catch (...) {}
                
                try {
                    // Try as boolean
                    if (value == "true" || value == "false") {
                        bool boolVal = (value == "true");
                        set<std::string>({key}, boolVal, ConfigSource::CommandLine, "From command line");
                        continue;
                    }
                } catch (...) {}
                
                // Fall back to string
                set<std::string>({key}, value, ConfigSource::CommandLine, "From command line");
            }
        }
        // Handle -key value format (legacy)
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set<std::string>({key}, value, ConfigSource::CommandLine, "From command line");
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Add comments from entries
        file << root_.dump(2);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::vector<std::string>& path) const {
    json current = root_;
    for (const auto& key : path) {
        if (current.contains(key)) {
            current = current[key];
        } else {
            return std::nullopt;
        }
    }
    
    try {
        return current.get<T>();
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::vector<std::string>& path, const T& defaultValue) const {
    auto val = get<T>(path);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
void Config::set(const std::vector<std::string>& path, const T& value, ConfigSource source, const std::string& description) {
    if (path.empty()) {
        return;
    }
    
    // Navigate to parent
    json& current = root_;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        if (!current.contains(path[i])) {
            current[path[i]] = json::object();
        }
        current = current[path[i]];
    }
    
    // Set the value
    current[path.back()] = value;
    
    // Record entry for tracking
    entries_.emplace_back(path, value, source, description);
}

void Config::set(const std::string& key, const json& value, ConfigSource source, const std::string& description) {
    std::vector<std::string> path;
    size_t pos = 0;
    std::string token;
    std::string remaining = key;
    while ((pos = remaining.find('.')) != std::string::npos) {
        token = remaining.substr(0, pos);
        path.push_back(token);
        remaining.erase(0, pos + 1);
    }
    path.push_back(remaining);
    
    set<std::string>(path, value, source, description);
}

void Config::set(const std::string& key, int value, ConfigSource source, const std::string& description) {
    set<int>({key}, value, source, description);
}

void Config::set(const std::string& key, double value, ConfigSource source, const std::string& description) {
    set<double>({key}, value, source, description);
}

void Config::set(const std::string& key, bool value, ConfigSource source, const std::string& description) {
    set<bool>({key}, value, source, description);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, const std::string& description) {
    set<std::string>({key}, value, source, description);
}

bool Config::has(const std::vector<std::string>& path) const {
    json current = root_;
    for (const auto& key : path) {
        if (current.contains(key)) {
            current = current[key];
        } else {
            return false;
        }
    }
    return true;
}

void Config::remove(const std::vector<std::string>& path) {
    if (path.empty()) {
        return;
    }
    
    json& current = root_;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        if (current.contains(path[i])) {
            current = current[path[i]];
        } else {
            return;
        }
    }
    
    if (current.contains(path.back())) {
        current.erase(path.back());
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    
    auto addKeys = [&](const json& j, const std::string& prefix) {
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string newPrefix = prefix.empty() ? it.key() : prefix + "." + it.key();
                keys.push_back(newPrefix);
                addKeys(it.value(), newPrefix);
            }
        }
    };
    
    addKeys(root_, "");
    return keys;
}

void Config::clear() {
    root_ = json::object();
    entries_.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << entries_.size() << " entries):\n";
    
    auto addEntrySummary = [&](const json& j, const std::string& prefix) {
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string newPrefix = prefix.empty() ? it.key() : prefix + "." + it.key();
                oss << "  " << newPrefix << " = " << it.value().dump() << "\n";
                addEntrySummary(it.value(), newPrefix);
            }
        } else {
            oss << "  " << prefix << " = " << j.dump() << "\n";
        }
    };
    
    addEntrySummary(root_, "");
    return oss.str();
}

void Config::merge(const Config& other) {
    if (other.root_.is_object() && root_.is_object()) {
        root_ = root_.get<json>();
        for (const auto& item : other.root_.items()) {
            if (!root_.contains(item.key())) {
                root_[item.key()] = item.value();
            }
        }
    }
}

} // namespace nlm
