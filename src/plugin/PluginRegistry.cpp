// PluginRegistry.cpp
#include "PluginRegistry.hpp"
#include "PluginBase.hpp"
#include <stdexcept>
#include <sstream>

namespace nlm {

template<typename T>
bool PluginRegistry::registerPlugin(const std::string& name,
                                   const std::string& version,
                                   const std::string& description,
                                   const std::string& author,
                                   const std::function<std::unique_ptr<T>()>& factory,
                                   const std::vector<std::string>& dependencies) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (hasPlugin(name)) {
        lastError_ = "Plugin with name '" + name + "' already registered";
        return false;
    }

    PluginInfo info;
    info.name = name;
    info.version = version;
    info.description = description;
    info.author = author;
    info.typeIndex = std::type_index(typeid(T));
    info.dependencies = dependencies;
    info.loaded = false;
    info.error = "";

    // Store factory function
    info.factory = [factory]() -> std::unique_ptr<PluginBase> {
        auto plugin = factory();
        return std::unique_ptr<PluginBase>(plugin.release());
    };

    plugins_[name] = info;
    return true;
}

std::vector<std::string> PluginRegistry::getPluginNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(plugins_.size());
    for (const auto& pair : plugins_) {
        names.push_back(pair.first);
    }
    return names;
}

bool PluginRegistry::hasPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return plugins_.find(name) != plugins_.end();
}

std::string PluginRegistry::getPluginVersion(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        return "";
    }
    return it->second.version;
}

std::string PluginRegistry::getPluginDescription(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        return "";
    }
    return it->second.description;
}

std::string PluginRegistry::getPluginAuthor(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        return "";
    }
    return it->second.author;
}

std::vector<std::string> PluginRegistry::getPluginDependencies(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        return {};
    }
    return it->second.dependencies;
}

std::type_index PluginRegistry::getPluginTypeIndex(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        return std::type_index(typeid(void));
    }
    return it->second.typeIndex;
}

std::type_index PluginRegistry::getPluginBaseTypeIndex() const {
    return std::type_index(typeid(PluginBase));
}

bool PluginRegistry::initializeAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool success = true;
    for (auto& pair : plugins_) {
        auto& info = pair.second;
        if (!info.loaded && !info.error.empty()) {
            continue;
        }

        try {
            // In a real implementation, we would actually create and initialize plugins
            // For now, mark as loaded
            info.loaded = true;
        } catch (const std::exception& e) {
            info.error = "Failed to initialize plugin " + pair.first + ": " + std::string(e.what());
            success = false;
        }
    }
    return success;
}

bool PluginRegistry::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool success = true;
    for (auto& pair : plugins_) {
        auto& info = pair.second;
        if (info.loaded) {
            try {
                // In a real implementation, we would shutdown plugins
                info.loaded = false;
            } catch (const std::exception& e) {
                info.error = "Failed to shutdown plugin " + pair.first + ": " + std::string(e.what());
                success = false;
            }
        }
    }
    return success;
}

bool PluginRegistry::validatePluginConfiguration(const std::string& name, const Config& config) const {
    // Basic validation - in a real implementation would validate plugin-specific config
    return true;
}

size_t PluginRegistry::getPluginCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return plugins_.size();
}

size_t PluginRegistry::getLoadedPluginCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& pair : plugins_) {
        if (pair.second.loaded) {
            count++;
        }
    }
    return count;
}

std::string PluginRegistry::getLastError() const {
    return lastError_;
}

// Template instantiations for common interfaces

template bool PluginRegistry::registerPlugin<NeuronPluginInterface>(const std::string&, const std::string&, const std::string&, const std::string&, const std::function<std::unique_ptr<NeuronPluginInterface>()>&, const std::vector<std::string>&);

template bool PluginRegistry::registerPlugin<SynapsePluginInterface>(const std::string&, const std::string&, const std::string&, const std::string&, const std::function<std::unique_ptr<SynapsePluginInterface>()>&, const std::vector<std::string>&);

template bool PluginRegistry::registerPlugin<PlasticityRulePluginInterface>(const std::string&, const std::string&, const std::string&, const std::string&, const std::function<std::unique_ptr<PlasticityRulePluginInterface>()>&, const std::vector<std::string>&);

} // namespace nlm