// PluginBase.hpp
#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <typeinfo>
#include <stdexcept>
#include <chrono>

namespace nlm {

class PluginBase {
public:
    virtual ~PluginBase() = default;

    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;

    // Plugin lifecycle
    virtual bool initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;

    // Plugin identification
    virtual const std::type_info& getTypeInfo() const = 0;

    // Plugin configuration
    virtual void setConfiguration(const Config& config) = 0;
    virtual Config getConfiguration() const = 0;

    // Plugin status
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;

    // Plugin validation
    virtual bool validateConfiguration() const = 0;
    virtual std::string getValidationError() const = 0;

    // Unique identifier for plugins
    virtual std::string getId() const = 0;

    // Timestamp for plugin load time
    virtual std::chrono::steady_clock::time_point getLoadTime() const = 0;
};

} // namespace nlm