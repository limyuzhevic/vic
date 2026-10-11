#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {
namespace plugins {

class Plugin {
public:
    virtual ~Plugin() = default;
    
    virtual const std::string& getName() const = 0;
    virtual const std::string& getVersion() const = 0;
    virtual const std::string& getAuthor() const = 0;
    virtual const std::string& getDescription() const = 0;
    
    virtual bool initialize(std::shared_ptr<class PluginManager> manager, 
                           std::shared_ptr<class Config> config) = 0;
    virtual void shutdown() = 0;
    virtual void update(double dt) = 0;
    
    virtual bool onPluginMessage(const std::string& message, 
                               const std::vector<std::string>& data) = 0;
    
    virtual std::string getPluginSpecificInfo() const = 0;
    
    virtual bool requires(const std::string& dependency) const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    
protected:
    std::shared_ptr<class PluginManager> pluginManager_;
    std::shared_ptr<class Config> config_;
};

} // namespace plugins
} // namespace nlm
