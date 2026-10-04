// PluginFactory.cpp
#include "PluginFactory.hpp"
#include <stdexcept>

namespace nlm {

namespace {
    PluginFactoryMap factories_;
    std::mutex mutex_;
}

template<typename T>
void PluginFactory::registerPluginFactory(const std::type_index& typeIndex,
                                         const std::function<std::unique_ptr<T>()>& factory) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Convert T factory to PluginBase factory
    auto pluginFactory = [factory]() -> std::unique_ptr<PluginBase> {
        auto plugin = factory();
        return std::unique_ptr<PluginBase>(plugin.release());
    };

    factories_[typeIndex] = pluginFactory;
}

std::unique_ptr<PluginBase> PluginFactory::createPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : factories_) {
        // In a real implementation, we would look up by name
        // For now, create from first available factory
        return pair.second();
    }
    
    lastError_ = "No plugin factory registered for name: " + name;
    return nullptr;
}

std::unique_ptr<PluginBase> PluginFactory::createPlugin(const std::type_index& typeIndex) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = factories_.find(typeIndex);
    if (it == factories_.end()) {
        lastError_ = "No plugin factory registered for type index: " + std::to_string(typeIndex.hash_code());
        return nullptr;
    }
    
    return it->second();
}

bool PluginFactory::hasPluginFactory(const std::type_index& typeIndex) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return factories_.find(typeIndex) != factories_.end();
}

std::vector<std::type_index> PluginFactory::getRegisteredPluginTypes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::type_index> types;
    types.reserve(factories_.size());
    for (const auto& pair : factories_) {
        types.push_back(pair.first);
    }
    return types;
}

std::string PluginFactory::getPluginTypeName(const std::type_index& typeIndex) const {
    // In a real implementation, we would use type information to get the name
    return "Unknown Plugin Type";
}

size_t PluginFactory::getPluginFactoryCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return factories_.size();
}

// Template instantiations for common types

template void PluginFactory::registerPluginFactory<NeuronPluginInterface>(const std::type_index&, const std::function<std::unique_ptr<NeuronPluginInterface>()>&);

template void PluginFactory::registerPluginFactory<SynapsePluginInterface>(const std::type_index&, const std::function<std::unique_ptr<SynapsePluginInterface>()>&);

template void PluginFactory::registerPluginFactory<PlasticityRulePluginInterface>(const std::type_index&, const std::function<std::unique_ptr<PlasticityRulePluginInterface>()>&);

} // namespace nlm