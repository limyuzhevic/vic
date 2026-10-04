// PluginFactory.hpp
#pragma once

#include "PluginBase.hpp"
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <mutex>

namespace nlm {

class PluginFactory {
public:
    // Factory registration
    template<typename T>
    static void registerPluginFactory(const std::type_index& typeIndex,
                                     const std::function<std::unique_ptr<T>()>& factory);

    // Plugin creation
    static std::unique_ptr<PluginBase> createPlugin(const std::string& name);
    static std::unique_ptr<PluginBase> createPlugin(const std::type_index& typeIndex);

    // Plugin discovery
    static bool hasPluginFactory(const std::type_index& typeIndex) const;
    static std::vector<std::type_index> getRegisteredPluginTypes() const;

    // Plugin type information
    static std::string getPluginTypeName(const std::type_index& typeIndex) const;

    // Statistics
    static size_t getPluginFactoryCount() const;

private:
    using PluginFactoryFunc = std::function<std::unique_ptr<PluginBase>()>;
    using PluginFactoryMap = std::unordered_map<std::type_index, PluginFactoryFunc>;

    static PluginFactoryMap factories_;
    static std::mutex mutex_;
};

} // namespace nlm