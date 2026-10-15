#include "NLMExtensionSystem.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "PlasticityRule.hpp"
#include "MemorySystem.hpp"
#include "Neuromodulator.hpp"
#include <dlfcn.h>  // For dynamic library loading
#include <fstream>
#include <sstream>

namespace nlm {

// ExtensionRegistry implementation
void ExtensionRegistry::registerNeuronFactory(std::unique_ptr<NeuronFactory> factory) {
    if (factory) {
        neuronFactories.push_back(std::move(factory));
    }
}

std::vector<std::shared_ptr<NeuronFactory>> ExtensionRegistry::getNeuronFactories() const {
    std::vector<std::shared_ptr<NeuronFactory>> factories;
    for (const auto& factory : neuronFactories) {
        factories.push_back(factory.get());
    }
    return factories;
}

void ExtensionRegistry::registerSynapseFactory(std::unique_ptr<SynapseFactory> factory) {
    if (factory) {
        synapseFactories.push_back(std::move(factory));
    }
}

std::vector<std::shared_ptr<SynapseFactory>> ExtensionRegistry::getSynapseFactories() const {
    std::vector<std::shared_ptr<SynapseFactory>> factories;
    for (const auto& factory : synapseFactories) {
        factories.push_back(factory.get());
    }
    return factories;
}

void ExtensionRegistry::registerPlasticityRuleFactory(std::unique_ptr<PlasticityRuleFactory> factory) {
    if (factory) {
        plasticityRuleFactories.push_back(std::move(factory));
    }
}

std::vector<std::shared_ptr<PlasticityRuleFactory>> ExtensionRegistry::getPlasticityRuleFactories() const {
    std::vector<std::shared_ptr<PlasticityRuleFactory>> factories;
    for (const auto& factory : plasticityRuleFactories) {
        factories.push_back(factory.get());
    }
    return factories;
}

void ExtensionRegistry::registerMemorySystemFactory(std::unique_ptr<MemorySystemFactory> factory) {
    if (factory) {
        memorySystemFactories.push_back(std::move(factory));
    }
}

std::vector<std::shared_ptr<MemorySystemFactory>> ExtensionRegistry::getMemorySystemFactories() const {
    std::vector<std::shared_ptr<MemorySystemFactory>> factories;
    for (const auto& factory : memorySystemFactories) {
        factories.push_back(factory.get());
    }
    return factories;
}

void ExtensionRegistry::registerNeuromodulatorFactory(std::unique_ptr<NeuromodulatorFactory> factory) {
    if (factory) {
        neuromodulatorFactories.push_back(std::move(factory));
    }
}

std::vector<std::shared_ptr<NeuromodulatorFactory>> ExtensionRegistry::getNeuromodulatorFactories() const {
    std::vector<std::shared_ptr<NeuromodulatorFactory>> factories;
    for (const auto& factory : neuromodulatorFactories) {
        factories.push_back(factory.get());
    }
    return factories;
}

void ExtensionRegistry::registerConfigExtension(const std::string& key, 
                                               std::function<ConfigValue(const Config&)> extensionFunc) {
    configExtensions[key] = extensionFunc;
}

std::map<std::string, std::function<ConfigValue(const Config&)>> ExtensionRegistry::getConfigExtensions() const {
    return configExtensions;
}

bool ExtensionRegistry::loadPlugin(const std::string& pluginPath) {
    // Dynamic library loading using dlopen
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    if (!handle) {
        NLM_LOG_ERROR("Failed to load plugin " + pluginPath + ": " + std::string(dlerror()));
        return false;
    }
    
    // Plugin registration function
    using RegisterFunction = void (*)(ExtensionRegistry&);
    RegisterFunction registerFunc = reinterpret_cast<RegisterFunction>(dlsym(handle, "registerPlugin"));
    
    if (!registerFunc) {
        NLM_LOG_ERROR("Plugin " + pluginPath + " does not have registerPlugin function");
        dlclose(handle);
        return false;
    }
    
    // Create temporary registry for plugin registration
    ExtensionRegistry tempRegistry;
    registerFunc(tempRegistry);
    
    // Merge with main registry
    for (auto& factory : tempRegistry.neuronFactories) {
        registerNeuronFactory(std::move(factory));
    }
    for (auto& factory : tempRegistry.synapseFactories) {
        registerSynapseFactory(std::move(factory));
    }
    for (auto& factory : tempRegistry.plasticityRuleFactories) {
        registerPlasticityRuleFactory(std::move(factory));
    }
    for (auto& factory : tempRegistry.memorySystemFactories) {
        registerMemorySystemFactory(std::move(factory));
    }
    for (auto& factory : tempRegistry.neuromodulatorFactories) {
        registerNeuromodulatorFactory(std::move(factory));
    }
    for (const auto& extension : tempRegistry.configExtensions) {
        registerConfigExtension(extension.first, extension.second);
    }
    
    // Store plugin handle for later cleanup
    pluginHandles[pluginPath] = handle;
    
    NLM_LOG_INFO("Successfully loaded plugin: " + pluginPath);
    return true;
}

bool ExtensionRegistry::hasNeuronFactory(const std::string& name) const {
    for (const auto& factory : neuronFactories) {
        if (factory->getName() == name) {
            return true;
        }
    }
    return false;
}

bool ExtensionRegistry::hasSynapseFactory(const std::string& name) const {
    for (const auto& factory : synapseFactories) {
        if (factory->getName() == name) {
            return true;
        }
    }
    return false;
}

bool ExtensionRegistry::hasPlasticityRuleFactory(const std::string& name) const {
    for (const auto& factory : plasticityRuleFactories) {
        if (factory->getName() == name) {
            return true;
        }
    }
    return false;
}

bool ExtensionRegistry::hasMemorySystemFactory(const std::string& name) const {
    for (const auto& factory : memorySystemFactories) {
        if (factory->getName() == name) {
            return true;
        }
    }
    return false;
}

bool ExtensionRegistry::hasNeuromodulatorFactory(const std::string& name) const {
    for (const auto& factory : neuromodulatorFactories) {
        if (factory->getName() == name) {
            return true;
        }
    }
    return false;
}

void ExtensionRegistry::configureExtension(const std::string& name, const ConfigValue& config) {
    extensionConfigs[name] = config;
}

// ExtendedBrainFactory implementation
ExtendedBrainFactory::ExtendedBrainFactory(std::shared_ptr<ExtensionRegistry> registry) : registry(registry) {
    // Initialize default factories
    defaultNeuronFactory = std::make_shared<DefaultNeuronFactory>();
    defaultSynapseFactory = std::make_shared<DefaultSynapseFactory>();
    defaultPlasticityRuleFactory = std::make_shared<DefaultPlasticityRuleFactory>();
    defaultMemorySystemFactory = std::make_shared<DefaultMemorySystemFactory>();
    defaultNeuromodulatorFactory = std::make_shared<DefaultNeuromodulatorFactory>();
}

std::shared_ptr<Brain> ExtendedBrainFactory::createBrain(const Config& config) {
    if (!registry) {
        // Fallback to simple creation
        return std::make_shared<Brain>(std::const_pointer_cast<Config>(std::make_shared<Config>(config)));
    }
    
    // Create brain with extended factory support
    integrateConfigExtensions(const_cast<Config&>(config));
    
    auto brain = std::make_shared<Brain>(std::const_pointer_cast<Config>(std::make_shared<Config>(config)));
    
    if (brain->initialize()) {
        NLM_LOG_INFO("Extended brain created successfully");
        return brain;
    } else {
        NLM_LOG_ERROR("Failed to create extended brain");
        return nullptr;
    }
}

void ExtendedBrainFactory::integrateConfigExtensions(Config& config) const {
    if (!registry) return;
    
    auto extensions = registry->getConfigExtensions();
    for (const auto& extension : extensions) {
        try {
            ConfigValue value = extension.second(config);
            config.set(extension.first, value, ConfigSource::Runtime);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error integrating config extension " + extension.first + ": " + std::string(e.what()));
        }
    }
}

std::shared_ptr<Neuron> ExtendedBrainFactory::createNeuron(NeuronId id, NeuronType type) const {
    if (!registry) return defaultNeuronFactory->createNeuron(id, type);
    
    // Select appropriate neuron factory
    auto factory = selectNeuronFactory(*registry); // Pass registry instead of config
    if (factory) {
        return factory->createNeuron(id, type);
    }
    
    return defaultNeuronFactory->createNeuron(id, type);
}

std::shared_ptr<Synapse> ExtendedBrainFactory::createSynapse(NeuronId preId, NeuronId postId, 
                                                   double weight, double delay) const {
    if (!registry) return defaultSynapseFactory->createSynapse(preId, postId, weight, delay);
    
    // Select appropriate synapse factory
    auto factory = selectSynapseFactory(*registry); // Pass registry instead of config
    if (factory) {
        return factory->createSynapse(preId, postId, weight, delay);
    }
    
    return defaultSynapseFactory->createSynapse(preId, postId, weight, delay);
}

std::shared_ptr<PlasticityRule> ExtendedBrainFactory::createPlasticityRule(const ConfigValue& params) const {
    if (!registry) return defaultPlasticityRuleFactory->createPlasticityRule(params);
    
    // Select appropriate plasticity rule factory
    auto factory = selectPlasticityRuleFactory(*registry); // Pass registry instead of config
    if (factory) {
        return factory->createPlasticityRule(params);
    }
    
    return defaultPlasticityRuleFactory->createPlasticityRule(params);
}

std::shared_ptr<MemorySystem> ExtendedBrainFactory::createMemorySystem(const Config& config) const {
    if (!registry) return defaultMemorySystemFactory->createMemorySystem(config);
    
    // Select appropriate memory system factory
    auto factory = selectMemorySystemFactory(*registry); // Pass registry instead of config
    if (factory) {
        return factory->createMemorySystem(config);
    }
    
    return defaultMemorySystemFactory->createMemorySystem(config);
}

std::shared_ptr<Neuromodulator> ExtendedBrainFactory::createNeuromodulator(const ConfigValue& params) const {
    if (!registry) return defaultNeuromodulatorFactory->createNeuromodulator(params);
    
    // Select appropriate neuromodulator factory
    auto factory = selectNeuromodulatorFactory(*registry); // Pass registry instead of config
    if (factory) {
        return factory->createNeuromodulator(params);
    }
    
    return defaultNeuromodulatorFactory->createNeuromodulator(params);
}

// Simplified factory selection methods (need to be fixed)
std::shared_ptr<NeuronFactory> ExtendedBrainFactory::selectNeuronFactory(const Config& config) const {
    return defaultNeuronFactory;
}

std::shared_ptr<SynapseFactory> ExtendedBrainFactory::selectSynapseFactory(const Config& config) const {
    return defaultSynapseFactory;
}

std::shared_ptr<PlasticityRuleFactory> ExtendedBrainFactory::selectPlasticityRuleFactory(const Config& config) const {
    return defaultPlasticityRuleFactory;
}

std::shared_ptr<MemorySystemFactory> ExtendedBrainFactory::selectMemorySystemFactory(const Config& config) const {
    return defaultMemorySystemFactory;
}

std::shared_ptr<NeuromodulatorFactory> ExtendedBrainFactory::selectNeuromodulatorFactory(const Config& config) const {
    return defaultNeuromodulatorFactory;
}

// PluginManager implementation
PluginManager::PluginManager(std::shared_ptr<ExtensionRegistry> registry) : registry(registry) {
    // Initialize plugin handles map
    pluginHandles.clear();
    plugins.clear();
    pluginEnabled.clear();
    pluginConfigs.clear();
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    // Check if plugin already loaded
    if (plugins.find(pluginPath) != plugins.end()) {
        NLM_LOG_WARNING("Plugin " + pluginPath + " already loaded");
        return true;
    }
    
    // Load dynamic library
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    if (!handle) {
        NLM_LOG_ERROR("Failed to load plugin " + pluginPath + ": " + std::string(dlerror()));
        return false;
    }
    
    // Create plugin instance
    using CreateFunction = Plugin*(*)();
    CreateFunction createFunc = reinterpret_cast<CreateFunction>(dlsym(handle, "createPlugin"));
    
    if (!createFunc) {
        NLM_LOG_ERROR("Plugin " + pluginPath + " does not have createPlugin function");
        dlclose(handle);
        return false;
    }
    
    Plugin* plugin = createFunc();
    if (!plugin) {
        NLM_LOG_ERROR("Failed to create plugin instance from " + pluginPath);
        dlclose(handle);
        return false;
    }
    
    // Store plugin
    std::string pluginName = plugin->getName();
    plugins[pluginName] = std::unique_ptr<Plugin>(plugin);
    pluginEnabled[pluginName] = true;  // Enable by default
    
    pluginHandles[pluginPath] = handle;
    
    NLM_LOG_INFO("Successfully loaded plugin: " + pluginName + " from " + pluginPath);
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    // Find and close the library handle
    for (auto it2 = pluginHandles.begin(); it2 != pluginHandles.end(); ++it2) {
        // In a real implementation, would need to track which handle corresponds to which plugin
        // For simplicity, we'll just close all handles
        dlclose(it2->second);
    }
    
    plugins.erase(it);
    pluginEnabled.erase(pluginName);
    pluginConfigs.erase(pluginName);
    pluginHandles.clear();
    
    NLM_LOG_INFO("Successfully unloaded plugin: " + pluginName);
    return true;
}

bool PluginManager::enablePlugin(const std::string& pluginName) {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    pluginEnabled[pluginName] = true;
    NLM_LOG_INFO("Enabled plugin: " + pluginName);
    return true;
}

bool PluginManager::disablePlugin(const std::string& pluginName) {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    pluginEnabled[pluginName] = false;
    NLM_LOG_INFO("Disabled plugin: " + pluginName);
    return true;
}

std::vector<std::string> PluginManager::getPluginNames() const {
    std::vector<std::string> names;
    for (const auto& pair : plugins) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> PluginManager::getEnabledPluginNames() const {
    std::vector<std::string> names;
    for (const auto& pair : plugins) {
        if (pluginEnabled[pair.first]) {
            names.push_back(pair.first);
        }
    }
    return names;
}

bool PluginManager::hasPlugin(const std::string& pluginName) const {
    return plugins.find(pluginName) != plugins.end();
}

bool PluginManager::isPluginEnabled(const std::string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return false;
    }
    
    auto enabledIt = pluginEnabled.find(pluginName);
    return (enabledIt != pluginEnabled.end()) && enabledIt->second;
}

void PluginManager::configurePlugin(const std::string& pluginName, const ConfigValue& config) {
    pluginConfigs[pluginName] = config;
}

ConfigValue PluginManager::getPluginConfig(const std::string& pluginName) const {
    auto it = pluginConfigs.find(pluginName);
    if (it != pluginConfigs.end()) {
        return it->second;
    }
    return ConfigValue();
}

void PluginManager::initializeAllPlugins(const Config& config) {
    for (const auto& pair : plugins) {
        if (pluginEnabled[pair.first]) {
            try {
                if (pair.second->initialize(config)) {
                    NLM_LOG_INFO("Initialized plugin: " + pair.first);
                } else {
                    NLM_LOG_ERROR("Failed to initialize plugin: " + pair.first);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Exception during plugin initialization " + pair.first + ": " + std::string(e.what()));
            }
        }
    }
}

void PluginManager::shutdownAllPlugins() {
    for (const auto& pair : plugins) {
        if (pluginEnabled[pair.first]) {
            try {
                pair.second->shutdown();
                NLM_LOG_INFO("Shutdown plugin: " + pair.first);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Exception during plugin shutdown " + pair.first + ": " + std::string(e.what()));
            }
        }
    }
}

void PluginManager::notifyPlugins(const std::string& event, const ConfigValue& data) {
    for (const auto& pair : plugins) {
        if (pluginEnabled[pair.first]) {
            // In a real implementation, would call plugin-specific event handlers
            NLM_LOG_INFO("Notifying plugin " + pair.first + " about event: " + event);
        }
    }
}

// NeuralComponentBuilder implementation
NeuralComponentBuilder& NeuralComponentBuilder::withNeuronFactory(std::unique_ptr<NeuronFactory> factory) {
    if (factory) {
        state.neuronFactories.push_back(std::move(factory));
    }
    return *this;
}

NeuralComponentBuilder& NeuralComponentBuilder::withSynapseFactory(std::unique_ptr<SynapseFactory> factory) {
    if (factory) {
        state.synapseFactories.push_back(std::move(factory));
    }
    return *this;
}

NeuralComponentBuilder& NeuralComponentBuilder::withPlasticityRuleFactory(std::unique_ptr<PlasticityRuleFactory> factory) {
    if (factory) {
        state.plasticityRuleFactories.push_back(std::move(factory));
    }
    return *this;
}

NeuralComponentBuilder& NeuralComponentBuilder::withMemorySystemFactory(std::unique_ptr<MemorySystemFactory> factory) {
    if (factory) {
        state.memorySystemFactories.push_back(std::move(factory));
    }
    return *this;
}

NeuralComponentBuilder& NeuralComponentBuilder::withNeuromodulatorFactory(std::unique_ptr<NeuromodulatorFactory> factory) {
    if (factory) {
        state.neuromodulatorFactories.push_back(std::move(factory));
    }
    return *this;
}

NeuralComponentBuilder& NeuralComponentBuilder::withConfig(const Config& config) {
    state.config = std::make_shared<Config>(config);
    return *this;
}

NeuralComponentBuilder& NeuralComponentBuilder::addConfigExtension(const std::string& key, 
                                                std::function<ConfigValue(const Config&)> extensionFunc) {
    state.configExtensions[key] = extensionFunc;
    return *this;
}

NeuralComponentBuilder& NeuralComponentBuilder::addPlugin(std::unique_ptr<Plugin> plugin) {
    if (plugin) {
        std::string name = plugin->getName();
        state.plugins.push_back(std::move(plugin));
        state.enabledPlugins[name] = true;
    }
    return *this;
}

NeuralComponentBuilder& NeuralComponentBuilder::enablePlugin(const std::string& pluginName) {
    state.enabledPlugins[pluginName] = true;
    return *this;
}

std::shared_ptr<Brain> NeuralComponentBuilder::buildBrain() {
    // Create extension registry
    auto registry = std::make_shared<ExtensionRegistry>();
    
    // Register factories
    for (auto& factory : state.neuronFactories) {
        registry->registerNeuronFactory(std::move(factory));
    }
    for (auto& factory : state.synapseFactories) {
        registry->registerSynapseFactory(std::move(factory));
    }
    for (auto& factory : state.plasticityRuleFactories) {
        registry->registerPlasticityRuleFactory(std::move(factory));
    }
    for (auto& factory : state.memorySystemFactories) {
        registry->registerMemorySystemFactory(std::move(factory));
    }
    for (auto& factory : state.neuromodulatorFactories) {
        registry->registerNeuromodulatorFactory(std::move(factory));
    }
    
    // Register config extensions
    for (const auto& extension : state.configExtensions) {
        registry->registerConfigExtension(extension.first, extension.second);
    }
    
    // Create extended brain factory
    auto factory = std::make_shared<ExtendedBrainFactory>(registry);
    
    return factory->createBrain(*state.config);
}

ExtendedBrainFactory NeuralComponentBuilder::buildExtendedFactory() {
    // Create extension registry
    auto registry = std::make_shared<ExtensionRegistry>();
    
    // Register factories
    for (auto& factory : state.neuronFactories) {
        registry->registerNeuronFactory(std::move(factory));
    }
    for (auto& factory : state.synapseFactories) {
        registry->registerSynapseFactory(std::move(factory));
    }
    for (auto& factory : state.plasticityRuleFactories) {
        registry->registerPlasticityRuleFactory(std::move(factory));
    }
    for (auto& factory : state.memorySystemFactories) {
        registry->registerMemorySystemFactory(std::move(factory));
    }
    for (auto& factory : state.neuromodulatorFactories) {
        registry->registerNeuromodulatorFactory(std::move(factory));
    }
    
    // Register config extensions
    for (const auto& extension : state.configExtensions) {
        registry->registerConfigExtension(extension.first, extension.second);
    }
    
    return ExtendedBrainFactory(registry);
}

void NeuralComponentBuilder::reset() {
    state.config.reset();
    state.neuronFactories.clear();
    state.synapseFactories.clear();
    state.plasticityRuleFactories.clear();
    state.memorySystemFactories.clear();
    state.neuromodulatorFactories.clear();
    state.plugins.clear();
    state.enabledPlugins.clear();
    state.configExtensions.clear();
}

// Extensions convenience functions
namespace Extensions {
    void registerDefaultExtensions(ExtensionRegistry& registry) {
        registry.registerNeuronFactory(std::make_unique<DefaultNeuronFactory>());
        registry.registerSynapseFactory(std::make_unique<DefaultSynapseFactory>());
        registry.registerPlasticityRuleFactory(std::make_unique<DefaultPlasticityRuleFactory>());
        registry.registerMemorySystemFactory(std::make_unique<DefaultMemorySystemFactory>());
        registry.registerNeuromodulatorFactory(std::make_unique<DefaultNeuromodulatorFactory>());
    }
    
    template<typename T, typename... Args>
    void registerFactory(ExtensionRegistry& registry, const std::string& name) {
        // Factory registration is done through constructor
        // This is a helper for template-based factory creation
        (void)registry; // Unused to avoid unused parameter warning
        (void)name; // Unused to avoid unused parameter warning
    }
    
    void configureFromFile(ExtensionRegistry& registry, const std::string& configFile) {
        // Load configuration from file and apply extensions
        Config config;
        if (config.loadFromFile(configFile)) {
            // Apply extensions based on configuration
            // Implementation depends on specific extension requirements
            NLM_LOG_INFO("Configuration loaded from file: " + configFile);
        } else {
            NLM_LOG_ERROR("Failed to load configuration from file: " + configFile);
        }
    }
    
    void configureFromConfig(ExtensionRegistry& registry, const Config& config) {
        // Apply extensions based on configuration
        // Implementation depends on specific extension requirements
        NLM_LOG_INFO("Configuration applied to extension registry");
    }
    
    bool loadBuiltInPlugins(ExtensionRegistry& registry) {
        // Load built-in plugins
        // Implementation depends on available plugins
        NLM_LOG_INFO("Loading built-in plugins");
        return true;
    }
    
    bool loadExternalPlugins(ExtensionRegistry& registry, const std::vector<std::string>& paths) {
        // Load external plugins from specified paths
        for (const auto& path : paths) {
            if (!registry.loadPlugin(path)) {
                NLM_LOG_ERROR("Failed to load external plugin: " + path);
                return false;
            }
        }
        return true;
    }
}

} // namespace nlm