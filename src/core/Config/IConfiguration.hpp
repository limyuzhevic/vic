// Configuration Interface
#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <optional>
#include <functional>

namespace nlm {

class IConfiguration {
public:
    virtual ~IConfiguration() = default;
    
    // Basic configuration operations
    virtual void set(const std::string& key, const std::string& value) = 0;
    virtual std::optional<std::string> getString(const std::string& key) const = 0;
    virtual std::optional<int> getInt(const std::string& key) const = 0;
    virtual std::optional<double> getDouble(const std::string& key) const = 0;
    virtual std::optional<bool> getBool(const std::string& key) const = 0;
    
    // Type-safe convenience methods
    virtual std::string getString(const std::string& key, const std::string& defaultValue) const = 0;
    virtual int getInt(const std::string& key, int defaultValue) const = 0;
    virtual double getDouble(const std::string& key, double defaultValue) const = 0;
    virtual bool getBool(const std::string& key, bool defaultValue) const = 0;
    
    // Configuration management
    virtual bool has(const std::string& key) const = 0;
    virtual void remove(const std::string& key) = 0;
    virtual std::vector<std::string> getKeys() const = 0;
    virtual void clear() = 0;
    
    // Configuration validation
    virtual bool validate() const { return true; }
    
    // Configuration source
    enum class Source {
        Default,
        File,
        CommandLine,
        Runtime,
        Environment
    };
    
    virtual Source getSource(const std::string& key) const = 0;
    virtual std::string getDescription(const std::string& key) const = 0;
    
    // Configuration hierarchy
    virtual std::shared_ptr<IConfiguration> getSection(const std::string& section) const = 0;
    virtual void setSection(const std::string& section, const std::shared_ptr<IConfiguration>& config) = 0;
    
    // Configuration observers
    using ConfigurationChangeCallback = std::function<void(const std::string&, const std::string&)>;
    virtual void addChangeCallback(const ConfigurationChangeCallback& callback) = 0;
    virtual void removeChangeCallback(const ConfigurationChangeCallback& callback) = 0;
    
    // Configuration serialization
    virtual std::string toString() const = 0;
    virtual bool fromString(const std::string& data) = 0;
    
    // Configuration comparison
    virtual bool equals(const IConfiguration& other) const = 0;
    
    // Configuration statistics
    virtual size_t getSize() const = 0;
    virtual double getMemoryUsage() const = 0;
};

} // namespace nlm
