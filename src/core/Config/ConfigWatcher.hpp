#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>
#include <functional>
#include <memory>
#include <chrono>
#include "ConfigError.hpp"

namespace nlm {

// Configuration watcher
class ConfigWatcher {
public:
    ConfigWatcher() = default;
    
    void addListener(std::shared_ptr<ConfigChangeListener> listener);
    void removeListener(std::shared_ptr<ConfigChangeListener> listener);
    void notifyChange(const std::string& key, const std::any& oldValue, const std::any& newValue);
    void notifyAdd(const std::string& key, const std::any& value);
    void notifyRemove(const std::string& key);
    void notifyValidationFailed(const ConfigError& error);
    
    void setWatchInterval(std::chrono::milliseconds interval) { watchInterval = interval; }
    std::chrono::milliseconds getWatchInterval() const { return watchInterval; }
    
private:
    std::vector<std::shared_ptr<ConfigChangeListener>> listeners;
    std::chrono::milliseconds watchInterval{100};
};

} // namespace nlm