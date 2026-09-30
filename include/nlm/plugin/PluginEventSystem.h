#ifndef NLM_PLUGIN_PLUGINEVENTSYSTEM_H
#define NLM_PLUGIN_PLUGINEVENTSYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <functional>

namespace nlm {
namespace plugin {

enum class EventPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

struct PluginEvent {
    std::string eventName;
    std::string sourcePluginId;
    std::string targetPluginId;
    EventPriority priority;
    double timestamp;
    std::map<std::string, std::string> data;
    std::string category;
    bool propagate;
    std::vector<std::string> handledBy;
};

class IPluginEventHandler {
public:
    virtual ~IPluginEventHandler() = default;
    virtual void handleEvent(const PluginEvent& event) = 0;
    virtual std::vector<std::string> getHandledEvents() const = 0;
    virtual std::vector<std::string> getHandledEventCategories() const = 0;
};

class PluginEventSystem {
public:
    PluginEventSystem();
    ~PluginEventSystem();

    // Event handling
    void publishEvent(const PluginEvent& event);
    void subscribeToEvent(const std::string& pluginId, const std::string& eventName, std::shared_ptr<IPluginEventHandler> handler);
    void unsubscribeFromEvent(const std::string& pluginId, const std::string& eventName);
    std::vector<PluginEvent> getRecentEvents(const std::string& pluginId, const std::string& eventCategory) const;

    // Event filtering
    void setEventFilter(std::function<bool(const PluginEvent&)> filter);
    bool isEventFiltered(const PluginEvent& event) const;

    // Event statistics
    std::map<std::string, int> getEventStatistics(const std::string& eventName) const;
    int getEventCount(const std::string& eventName) const;
    std::vector<std::string> getEventCategories() const;

    // System management
    void clearEventHistory();
    void setMaxEventHistorySize(size_t maxSize);

protected:
    std::vector<PluginEvent> eventHistory_;
    std::map<std::string, std::vector<std::pair<std::string, std::shared_ptr<IPluginEventHandler>>>> eventSubscriptions_;
    std::map<std::string, std::map<std::string, int>> eventStatistics_;
    std::function<bool(const PluginEvent&)> eventFilter_;
    mutable std::mutex mutex_;
    size_t maxEventHistorySize_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINEVENTSYSTEM_H
