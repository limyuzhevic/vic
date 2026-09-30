#include "include/nlm/plugin/PluginEventSystem.h"
#include <iostream>
#include <algorithm>
#include <chrono>

namespace nlm {
namespace plugin {

PluginEventSystem::PluginEventSystem() : maxEventHistorySize_(100) {
}

PluginEventSystem::~PluginEventSystem() {
}

void PluginEventSystem::publishEvent(const PluginEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (eventFilter_ && !eventFilter_(event)) {
        return;
    }

    eventHistory_.push_back(event);

    if (eventHistory_.size() > maxEventHistorySize_) {
        eventHistory_.erase(eventHistory_.begin());
    }

    eventStatistics_[event.eventName]++;

    for (const auto& pair : eventSubscriptions_) {
        for (const auto& subscription : pair.second) {
            if (subscription.first == event.eventName || subscription.first.empty()) {
                subscription.second->handleEvent(event);
            }
        }
    }
}

void PluginEventSystem::subscribeToEvent(const std::string& pluginId, const std::string& eventName, std::shared_ptr<IPluginEventHandler> handler) {
    std::lock_guard<std::mutex> lock(mutex_);

    eventSubscriptions_[pluginId].push_back(std::make_pair(eventName, handler));
}

void PluginEventSystem::unsubscribeFromEvent(const std::string& pluginId, const std::string& eventName) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = eventSubscriptions_.find(pluginId);
    if (it != eventSubscriptions_.end()) {
        auto& subscriptions = it->second;
        subscriptions.erase(std::remove_if(subscriptions.begin(), subscriptions.end(),
            [&eventName](const std::pair<std::string, std::shared_ptr<IPluginEventHandler>>& sub) {
                return sub.first == eventName;
            }),
            subscriptions.end());
    }
}

std::vector<PluginEvent> PluginEventSystem::getRecentEvents(const std::string& pluginId, const std::string& eventCategory) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PluginEvent> recentEvents;

    for (const auto& event : eventHistory_) {
        if ((pluginId.empty() || event.sourcePluginId == pluginId) &&
            (eventCategory.empty() || event.category == eventCategory)) {
            recentEvents.push_back(event);
        }
    }

    return recentEvents;
}

void PluginEventSystem::setEventFilter(std::function<bool(const PluginEvent&)> filter) {
    std::lock_guard<std::mutex> lock(mutex_);
    eventFilter_ = filter;
}

bool PluginEventSystem::isEventFiltered(const PluginEvent& event) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (eventFilter_) {
        return eventFilter_(event);
    }
    return false;
}

std::map<std::string, int> PluginEventSystem::getEventStatistics(const std::string& eventName) const {
    std::lock_guard<std::mutex> lock(mutex_);

    if (eventName.empty()) {
        return eventStatistics_;
    }

    std::map<std::string, int> stats;
    auto it = eventStatistics_.find(eventName);
    if (it != eventStatistics_.end()) {
        stats[eventName] = it->second;
    }

    return stats;
}

int PluginEventSystem::getEventCount(const std::string& eventName) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = eventStatistics_.find(eventName);
    if (it != eventStatistics_.end()) {
        return it->second;
    }

    return 0;
}

std::vector<std::string> PluginEventSystem::getEventCategories() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> categories;

    for (const auto& event : eventHistory_) {
        if (std::find(categories.begin(), categories.end(), event.category) == categories.end()) {
            categories.push_back(event.category);
        }
    }

    return categories;
}

void PluginEventSystem::clearEventHistory() {
    std::lock_guard<std::mutex> lock(mutex_);
    eventHistory_.clear();
    eventStatistics_.clear();
}

void PluginEventSystem::setMaxEventHistorySize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    maxEventHistorySize_ = maxSize;
}

} // namespace plugin
} // namespace nlm
