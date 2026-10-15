// ConfigHistory implementation
#include "config/ConfigHistory.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>

namespace nlm {

struct ConfigHistory::Impl {
    std::vector<std::shared_ptr<ConfigHistoryEntry>> entries;
    bool enabled = true;
    size_t maxEntries = 1000;
};

ConfigHistoryEntry::ConfigHistoryEntry(const std::string& aid, const std::string& atimestamp,
                                     const std::string& aaction, std::shared_ptr<Config> aconfig,
                                     const std::string& auser, const std::string& adescription)
    : id(aid), timestamp(atimestamp), action(aaction), configSnapshot(aconfig),
      user(auser), description(adescription) {}

ConfigHistory::ConfigHistory() : pImpl(std::make_unique<Impl>()) {}

ConfigHistory::~ConfigHistory() = default;

std::string ConfigHistory::recordChange(const std::string& action, const std::shared_ptr<Config>& config,
                                        const std::string& user, const std::string& description) {
    if (!pImpl->enabled || !config) {
        return "";
    }
    
    // Generate unique ID
    std::string id = "entry_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Generate timestamp
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S");
    std::string timestampStr = ss.str();
    
    // Create entry
    auto entry = std::make_shared<ConfigHistoryEntry>(id, timestampStr, action, config, user, description);
    
    // Add to history
    pImpl->entries.push_back(entry);
    
    // Trim if too many entries
    if (pImpl->entries.size() > pImpl->maxEntries) {
        pImpl->entries.erase(pImpl->entries.begin());
    }
    
    return id;
}

std::vector<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getHistory() const {
    return pImpl->entries;
}

std::optional<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getEntry(const std::string& id) const {
    for (const auto& entry : pImpl->entries) {
        if (entry->id == id) {
            return entry;
        }
    }
    return std::nullopt;
}

std::vector<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getRecentEntries(size_t count) const {
    std::vector<std::shared_ptr<ConfigHistoryEntry>> recent;
    if (count > pImpl->entries.size()) {
        recent = pImpl->entries;
    } else {
        recent = std::vector<std::shared_ptr<ConfigHistoryEntry>>(
            pImpl->entries.end() - count, pImpl->entries.end());
    }
    return recent;
}

bool ConfigHistory::restoreFromHistory(const std::string& id, std::shared_ptr<Config>& restoredConfig) {
    auto entryOpt = getEntry(id);
    if (!entryOpt) {
        return false;
    }
    
    restoredConfig = entryOpt->configSnapshot;
    return true;
}

bool ConfigHistory::rollback(const std::string& id, std::shared_ptr<Config>& rolledBackConfig) {
    return restoreFromHistory(id, rolledBackConfig);
}

bool ConfigHistory::deleteEntry(const std::string& id) {
    for (auto it = pImpl->entries.begin(); it != pImpl->entries.end(); ++it) {
        if ((*it)->id == id) {
            pImpl->entries.erase(it);
            return true;
        }
    }
    return false;
}

bool ConfigHistory::clearHistory() {
    pImpl->entries.clear();
    return true;
}

std::vector<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getHistoryByAction(const std::string& action) const {
    std::vector<std::shared_ptr<ConfigHistoryEntry>> result;
    for (const auto& entry : pImpl->entries) {
        if (entry->action == action) {
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getHistoryByUser(const std::string& user) const {
    std::vector<std::shared_ptr<ConfigHistoryEntry>> result;
    for (const auto& entry : pImpl->entries) {
        if (entry->user == user) {
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getHistoryByTimeRange(
    const std::string& startTime, const std::string& endTime) const {
    // Simplified time range filtering (would need proper date parsing)
    std::vector<std::shared_ptr<ConfigHistoryEntry>> result;
    for (const auto& entry : pImpl->entries) {
        if (entry->timestamp >= startTime && entry->timestamp <= endTime) {
            result.push_back(entry);
        }
    }
    return result;
}

bool ConfigHistory::exportHistory(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json;
        json["entries"] = nlohmann::json::array();
        
        for (const auto& entry : pImpl->entries) {
            nlohmann::json entryJson;
            entryJson["id"] = entry->id;
            entryJson["timestamp"] = entry->timestamp;
            entryJson["action"] = entry->action;
            entryJson["user"] = entry->user;
            entryJson["description"] = entry->description;
            
            // Simplified config serialization
            nlohmann::json configJson;
            for (const auto& configEntry : entry->configSnapshot->pImpl->entries) {
                configJson[configEntry.key] = "ConfigValue"; // Simplified
            }
            
            entryJson["config"] = configJson;
            json["entries"].push_back(entryJson);
        }
        
        file << json.dump(2);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigHistory::importHistory(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        
        pImpl->entries.clear();
        
        for (const auto& entryJson : json["entries"]) {
            std::string id = entryJson.value("id", "");
            std::string timestamp = entryJson.value("timestamp", "");
            std::string action = entryJson.value("action", "");
            std::string user = entryJson.value("user", "");
            std::string description = entryJson.value("description", "");
            
            // Simplified config deserialization
            auto config = std::make_shared<Config>();
            
            // Create entry
            auto entry = std::make_shared<ConfigHistoryEntry>(id, timestamp, action, config, user, description);
            pImpl->entries.push_back(entry);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace nlm
