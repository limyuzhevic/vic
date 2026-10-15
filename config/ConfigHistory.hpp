#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "core/Config/Config.hpp"

namespace nlm {

// Configuration history and auditing system

// Forward declarations
class Config;

// Configuration history entry
struct ConfigHistoryEntry {
    std::string id;
    std::string timestamp;
    std::string action; // "create", "update", "delete", "restore", "merge"
    std::shared_ptr<Config> configSnapshot;
    std::string user;
    std::string description;
    std::map<std::string, std::string> changes; // parameter -> old->new value
    
    ConfigHistoryEntry() = default;
    ConfigHistoryEntry(const std::string& aid, const std::string& atimestamp,
                      const std::string& aaction, std::shared_ptr<Config> aconfig,
                      const std::string& auser, const std::string& adescription);
};

// Configuration history manager
class ConfigHistory {
public:
    ConfigHistory();
    ~ConfigHistory();
    
    // History management
    std::string recordChange(const std::string& action, const std::shared_ptr<Config>& config,
                           const std::string& user = "", const std::string& description = "");
    std::vector<std::shared_ptr<ConfigHistoryEntry>> getHistory() const;
    std::optional<std::shared_ptr<ConfigHistoryEntry>> getEntry(const std::string& id) const;
    std::vector<std::shared_ptr<ConfigHistoryEntry>> getRecentEntries(size_t count = 10) const;
    
    // History operations
    bool restoreFromHistory(const std::string& id, std::shared_ptr<Config>& restoredConfig);
    bool rollback(const std::string& id, std::shared_ptr<Config>& rolledBackConfig);
    bool deleteEntry(const std::string& id);
    bool clearHistory();
    
    // History filtering
    std::vector<std::shared_ptr<ConfigHistoryEntry>> getHistoryByAction(const std::string& action) const;
    std::vector<std::shared_ptr<ConfigHistoryEntry>> getHistoryByUser(const std::string& user) const;
    std::vector<std::shared_ptr<ConfigHistoryEntry>> getHistoryByTimeRange(
        const std::string& startTime, const std::string& endTime) const;
    
    // History export/import
    bool exportHistory(const std::string& filepath) const;
    bool importHistory(const std::string& filepath);
    
    // Version control integration (placeholder for future implementation)
    bool commitToVersionControl(const std::string& message);
    bool restoreFromVersionControl(const std::string& version);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
