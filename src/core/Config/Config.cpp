#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<std::pair<std::string, nlohmann::json>> entries;
    std::vector<std::string> keys;
    std::vector<ConfigSource> sources;
    std::vector<std::string> descriptions;
    
    Impl() : entries(), keys(), sources(), descriptions() {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadFromJson(content);
}

bool Config::loadFromJson(const std::string& jsonContent) {
    try {
        pImpl->entries.clear();
        pImpl->keys.clear();
        pImpl->sources.clear();
        pImpl->descriptions.clear();
        
        auto j = nlohmann::json::parse(jsonContent);
        fromJson(j);
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string jsonContent;
    if (saveToJson(jsonContent)) {
        file << jsonContent;
        return true;
    }
    return false;
}

bool Config::saveToJson(std::string& jsonContent) const {
    try {
        auto j = toJson();
        jsonContent = j.dump(2);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find(pImpl->keys.begin(), pImpl->keys.end(), key);
    if (it == pImpl->keys.end()) {
        return std::nullopt;
    }
    
    size_t index = std::distance(pImpl->keys.begin(), it);
    const auto& value = pImpl->entries[index].second;
    
    try {
        return value.get<T>();
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    auto it = std::find(pImpl->keys.begin(), pImpl->keys.end(), key);
    if (it != pImpl->keys.end()) {
        size_t index = std::distance(pImpl->keys.begin(), it);
        pImpl->entries[index].second = value;
        pImpl->sources[index] = source;
    } else {
        pImpl->keys.push_back(key);
        pImpl->entries.push_back(std::make_pair(key, value));
        pImpl->sources.push_back(source);
        pImpl->descriptions.push_back("");
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, value, source);
}

bool Config::has(const std::string& key) const {
    return std::find(pImpl->keys.begin(), pImpl->keys.end(), key) != pImpl->keys.end();
}

void Config::remove(const std::string& key) {
    auto it = std::find(pImpl->keys.begin(), pImpl->keys.end(), key);
    if (it != pImpl->keys.end()) {
        size_t index = std::distance(pImpl->keys.begin(), it);
        pImpl->keys.erase(it);
        pImpl->entries.erase(pImpl->entries.begin() + index);
        pImpl->sources.erase(pImpl->sources.begin() + index);
        pImpl->descriptions.erase(pImpl->descriptions.begin() + index);
    }
}

std::vector<std::string> Config::getKeys() const {
    return pImpl->keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keys.clear();
    pImpl->sources.clear();
    pImpl->descriptions.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        oss << "  " << pImpl->entries[i].first << " = [";
        const auto& value = pImpl->entries[i].second;
        if (value.is_string()) {
            oss << "\"" << value.get<std::string>() << "\"";
        } else if (value.is_number()) {
            if (value.is_number_integer()) {
                oss << value.get<int64_t>();
            } else {
                oss << value.get<double>();
            }
        } else if (value.is_boolean()) {
            oss << (value.get<bool>() ? "true" : "false");
        } else if (value.is_array()) {
            oss << "[" << value.size() << " elements]";
        } else {
            oss << value.dump();
        }
        oss << "] (" << static_cast<int>(pImpl->sources[i]) << ")\n";
    }
    return oss.str();
}

void Config::merge(const Config& other) {
    for (size_t i = 0; i < other.pImpl->entries.size(); ++i) {
        set(other.pImpl->entries[i].first, other.pImpl->entries[i].second, other.pImpl->sources[i]);
    }
}

nlohmann::json Config::toJson() const {
    nlohmann::json j;
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        j[pImpl->entries[i].first] = pImpl->entries[i].second;
    }
    return j;
}

void Config::fromJson(const nlohmann::json& j) {
    clear();
    for (auto it = j.begin(); it != j.end(); ++it) {
        set(it.key(), *it, ConfigSource::File);
    }
}

} // namespace nlm
