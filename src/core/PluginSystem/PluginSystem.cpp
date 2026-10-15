#include "PluginSystem.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "environment/Environment.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <dlfcn.h>  // For dynamic loading

namespace nlm {

struct PluginManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<PluginBase>> plugins;
    std::unordered_map<std::string, PluginCapabilities> pluginCapabilities;
    std::unordered_map<std::string, void*> pluginHandles;
    std::vector<std::string> pluginSearchPaths;
    
    std::unordered_map<std::string, std::any> globalData;
    
    std::mutex pluginsMutex;
    bool initialized;
    
    Impl() : initialized(false) {
        // Add default search paths
        pluginSearchPaths.push_back("./plugins");
        pluginSearchPaths.push_back("./lib/plugins");
        pluginSearchPaths.push_back("${HOME}/.nlm/plugins");
    }
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}

PluginManager::~PluginManager() {
    shutdownAllPlugins();
    
    // Unload all plugins
    for (const auto& pair : pImpl->pluginHandles) {
        dlclose(pair.second);
    }
}

bool PluginManager::loadPlugin(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    std::string pluginName = std::filesystem::path(filepath).stem().string();
    
    if (pImpl->plugins.find(pluginName) != pImpl->plugins.end()) {
        NLM_LOG_WARNING("Plugin '" + pluginName + "' is already loaded");
        return false;
    }
    
    // Open the shared library
    void* handle = dlopen(filepath.c_str(), RTLD_LAZY);
    if (!handle) {
        NLM_LOG_ERROR("Failed to load plugin '" + pluginName + "': " + dlerror());
        return false;
    }
    
    // Get the plugin factory function
    std::string factoryName = "create_plugin_" + pluginName;
    void* symbol = dlsym(handle, factoryName.c_str());
    if (!symbol) {
        NLM_LOG_ERROR("Plugin '" + pluginName + "' does not export 'create_plugin_' function");
        dlclose(handle);
        return false;
    }
    
    PluginFactory factory = reinterpret_cast<PluginFactory>(symbol);
    
    // Create plugin instance
    auto plugin = factory(std::any());
    if (!plugin) {
        NLM_LOG_ERROR("Failed to create plugin instance for '" + pluginName + "'");
        dlclose(handle);
        return false;
    }
    
    // Get plugin capabilities
    auto capabilities = plugin->getCapabilities();
    
    // Check dependencies
    for (const auto& dep : capabilities.dependencies) {
        if (pImpl->plugins.find(dep) == pImpl->plugins.end()) {
            NLM_LOG_ERROR("Plugin '" + pluginName + "' depends on unloaded plugin '" + dep + "'");
            dlclose(handle);
            return false;
        }
    }
    
    // Store plugin
    pImpl->plugins[pluginName] = std::move(plugin);
    pImpl->pluginHandles[pluginName] = handle;
    pImpl->pluginCapabilities[pluginName] = capabilities;
    
    NLM_LOG_INFO("Plugin '" + pluginName + "' loaded successfully");
    return true;
}

bool PluginManager::loadPlugin(const std::string& name, PluginFactory factory,
                               const std::any& config) {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    if (pImpl->plugins.find(name) != pImpl->plugins.end()) {
        NLM_LOG_WARNING("Plugin '" + name + "' is already loaded");
        return false;
    }
    
    auto plugin = factory(config);
    if (!plugin) {
        NLM_LOG_ERROR("Failed to create plugin instance for '" + name + "'");
        return false;
    }
    
    auto capabilities = plugin->getCapabilities();
    
    // Check dependencies
    for (const auto& dep : capabilities.dependencies) {
        if (pImpl->plugins.find(dep) == pImpl->plugins.end()) {
            NLM_LOG_ERROR("Plugin '" + name + "' depends on unloaded plugin '" + dep + "'");
            return false;
        }
    }
    
    // Store plugin
    pImpl->plugins[name] = std::move(plugin);
    pImpl->pluginCapabilities[name] = capabilities;
    
    NLM_LOG_INFO("Plugin '" + name + "' loaded successfully");
    return true;
}

bool PluginManager::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_WARNING("Plugin '" + name + "' is not loaded");
        return false;
    }
    
    // Shutdown plugin
    it->second->shutdown();
    
    // Remove from maps
    pImpl->plugins.erase(it);
    
    auto handleIt = pImpl->pluginHandles.find(name);
    if (handleIt != pImpl->pluginHandles.end()) {
        dlclose(handleIt->second);
        pImpl->pluginHandles.erase(handleIt);
    }
    
    pImpl->pluginCapabilities.erase(name);
    
    NLM_LOG_INFO("Plugin '" + name + "' unloaded successfully");
    return true;
}

std::shared_ptr<PluginBase> PluginManager::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return nullptr;
    }
    
    return it->second;
}

std::vector<std::shared_ptr<PluginBase>> PluginManager::getPluginsByType(PluginType type) const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    std::vector<std::shared_ptr<PluginBase>> plugins;
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getCapabilities().type == type) {
            plugins.push_back(pair.second);
        }
    }
    
    return plugins;
}

std::vector<std::shared_ptr<PluginBase>> PluginManager::getAllPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    std::vector<std::shared_ptr<PluginBase>> plugins;
    plugins.reserve(pImpl->plugins.size());
    
    for (const auto& pair : pImpl->plugins) {
        plugins.push_back(pair.second);
    }
    
    return plugins;
}

bool PluginManager::initializeAllPlugins(const std::unordered_map<std::string, std::any>& configs) {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    if (pImpl->initialized) {
        NLM_LOG_WARNING("Plugins are already initialized");
        return false;
    }
    
    // Sort plugins by dependencies (simple topological sort)
    std::vector<std::string> pluginOrder;
    std::unordered_map<std::string, int> dependencyCount;
    
    for (const auto& pair : pImpl->plugins) {
        const auto& name = pair.first;
        const auto& capabilities = pImpl->pluginCapabilities[name];
        
        dependencyCount[name] = capabilities.dependencies.size();
        if (dependencyCount[name] == 0) {
            pluginOrder.push_back(name);
        }
    }
    
    // Initialize plugins in dependency order
    for (const auto& name : pluginOrder) {
        auto it = pImpl->plugins.find(name);
        if (it == pImpl->plugins.end()) continue;
        
        auto configIt = configs.find(name);
        std::any config = configIt != configs.end() ? configIt->second : std::any();
        
        if (!it->second->initialize(config)) {
            NLM_LOG_ERROR("Failed to initialize plugin '" + name + "'");
            return false;
        }
        
        NLM_LOG_INFO("Plugin '" + name + "' initialized successfully");
    }
    
    pImpl->initialized = true;
    NLM_LOG_INFO("All plugins initialized successfully");
    return true;
}

void PluginManager::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    // Shutdown plugins in reverse order
    for (auto it = pImpl->plugins.rbegin(); it != pImpl->plugins.rend(); ++it) {
        const auto& name = it->first;
        it->second->shutdown();
        NLM_LOG_INFO("Plugin '" + name + "' shutdown successfully");
    }
    
    pImpl->initialized = false;
    NLM_LOG_INFO("All plugins shutdown successfully");
}

bool PluginManager::isPluginLoaded(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    return pImpl->plugins.find(name) != pImpl->plugins.end();
}

PluginCapabilities PluginManager::getPluginCapabilities(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    auto it = pImpl->pluginCapabilities.find(name);
    if (it == pImpl->pluginCapabilities.end()) {
        return PluginCapabilities();
    }
    
    return it->second;
}

std::vector<std::string> PluginManager::getPluginNames() const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    std::vector<std::string> names;
    names.reserve(pImpl->plugins.size());
    
    for (const auto& pair : pImpl->plugins) {
        names.push_back(pair.first);
    }
    
    return names;
}

void PluginManager::setPluginSearchPath(const std::vector<std::string>& paths) {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    pImpl->pluginSearchPaths = paths;
}

std::vector<std::string> PluginManager::discoverPlugins(const std::string& directory) {
    std::vector<std::string> plugins;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                if (extension == ".so" || extension == ".dylib" || extension == ".dll") {
                    plugins.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        NLM_LOG_ERROR("Failed to discover plugins in directory '" + directory + "': " + e.what());
    }
    
    return plugins;
}

void PluginManager::onBrainInitialized(class Brain* brain) {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getCapabilities().requiresBrain) {
            pair.second->onBrainInitialized(brain);
        }
    }
}

void PluginManager::onWorldInitialized(class Environment* world) {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->getCapabilities().requiresWorld) {
            pair.second->onWorldInitialized(world);
        }
    }
}

void PluginManager::onUpdate(double timestep) {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    for (const auto& pair : pImpl->plugins) {
        if (pair.second->isEnabled()) {
            pair.second->update(timestep);
        }
    }
}

void PluginManager::setGlobalData(const std::string& key, const std::any& value) {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    pImpl->globalData[key] = value;
}

std::any PluginManager::getGlobalData(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->pluginsMutex);
    
    auto it = pImpl->globalData.find(key);
    if (it == pImpl->globalData.end()) {
        return std::any();
    }
    
    return it->second;
}

void PluginManager::registerBuiltinPlugin(const std::string& name, PluginFactory factory,
                                         const PluginCapabilities& capabilities) {
    // Register builtin plugins for easy loading
    static std::unordered_map<std::string, PluginFactory> builtinPlugins;
    static std::unordered_map<std::string, PluginCapabilities> builtinCapabilities;
    
    builtinPlugins[name] = factory;
    builtinCapabilities[name] = capabilities;
    
    NLM_LOG_INFO("Registered builtin plugin: '" + name + "'");
}

// Initialize static registration of built-in plugins
namespace {
    // Neural network plugin
    void registerNeuralNetworkPlugin() {
        PluginManager::registerBuiltinPlugin(
            "simple_neural_network",
            [](const std::any& config) -> std::unique_ptr<PluginBase> {
                return std::make_unique<SimpleNeuralNetworkPlugin>();
            },
            PluginCapabilities{PluginType::NeuralNetwork, "Simple Neural Network", "1.0.0", 
                             "Simple neural network implementation for demonstration",
                             {}, false, false, true}
        );
    }
    
    // Learning rule plugin
    void registerLearningRulePlugin() {
        PluginManager::registerBuiltinPlugin(
            "simple_stdp",
            [](const std::any& config) -> std::unique_ptr<PluginBase> {
                return std::make_unique<SimpleSTDPPlugin>();
            },
            PluginCapabilities{PluginType::LearningRule, "Simple STDP", "1.0.0",
                             "Simple Spike-Timing-Dependent Plasticity implementation",
                             {"simple_neural_network"}, true, false, true}
        );
    }
    
    // Visualization plugin
    void registerVisualizationPlugin() {
        PluginManager::registerBuiltinPlugin(
            "simple_visualization",
            [](const std::any& config) -> std::unique_ptr<PluginBase> {
                return std::make_unique<SimpleVisualizationPlugin>();
            },
            PluginCapabilities{PluginType::Visualization, "Simple Visualization", "1.0.0",
                             "Simple visualization interface for brain activity",
                             {}, true, false, false}
        );
    }
    
    // Register all built-in plugins
    struct BuiltInPluginRegistrar {
        BuiltInPluginRegistrar() {
            registerNeuralNetworkPlugin();
            registerLearningRulePlugin();
            registerVisualizationPlugin();
        }
    } builtin_registrar;
} // anonymous namespace

// SimpleNeuralNetworkPlugin implementation
bool SimpleNeuralNetworkPlugin::initialize(const std::any& config) {
    NLM_LOG_INFO("SimpleNeuralNetworkPlugin initialized");
    return true;
}

void SimpleNeuralNetworkPlugin::shutdown() {
    NLM_LOG_INFO("SimpleNeuralNetworkPlugin shutdown");
    weights_.clear();
    architecture_.clear();
}

PluginCapabilities SimpleNeuralNetworkPlugin::getCapabilities() const {
    PluginCapabilities caps;
    caps.type = PluginType::NeuralNetwork;
    caps.name = "Simple Neural Network";
    caps.version = "1.0.0";
    caps.description = "Simple neural network implementation for demonstration";
    caps.dependencies = {};
    caps.requiresBrain = true;
    caps.requiresWorld = false;
    caps.isSingleton = true;
    return caps;
}

void SimpleNeuralNetworkPlugin::initializeNetwork(const std::string& architecture, const std::any& params) {
    architecture_ = architecture;
    // Simple initialization - just store the architecture
    NLM_LOG_INFO("SimpleNeuralNetworkPlugin initialized network with architecture: " + architecture);
}

void SimpleNeuralNetworkPlugin::forward(class Neuron* neuron, double input) {
    // Simple forward pass - just store input and output as weight * input
    if (neuron) {
        neuron->addToMembranePotential(static_cast<MembranePotential>(input));
    }
}

double SimpleNeuralNetworkPlugin::getOutput(class Neuron* neuron) const {
    if (neuron) {
        return neuron->getMembranePotential();
    }
    return 0.0;
}

void SimpleNeuralNetworkPlugin::reset() {
    weights_.clear();
    NLM_LOG_INFO("SimpleNeuralNetworkPlugin reset");
}

std::vector<float> SimpleNeuralNetworkPlugin::getWeights() const {
    return weights_;
}

void SimpleNeuralNetworkPlugin::setWeights(const std::vector<float>& weights) {
    weights_ = weights;
}

size_t SimpleNeuralNetworkPlugin::getParameterCount() const {
    return weights_.size();
}

void SimpleNeuralNetworkPlugin::update(double timestep) {
    // Simple update - just log
    if (!weights_.empty()) {
        NLM_LOG_INFO("SimpleNeuralNetworkPlugin updating with " + std::to_string(weights_.size()) + " weights");
    }
}

// SimpleSTDPPlugin implementation
bool SimpleSTDPPlugin::initialize(const std::any& config) {
    NLM_LOG_INFO("SimpleSTDPPlugin initialized with learning rate: " + std::to_string(learningRate_));
    return true;
}

void SimpleSTDPPlugin::shutdown() {
    NLM_LOG_INFO("SimpleSTDPPlugin shutdown");
}

PluginCapabilities SimpleSTDPPlugin::getCapabilities() const {
    PluginCapabilities caps;
    caps.type = PluginType::LearningRule;
    caps.name = "Simple STDP";
    caps.version = "1.0.0";
    caps.description = "Simple Spike-Timing-Dependent Plasticity implementation";
    caps.dependencies = {"simple_neural_network"};
    caps.requiresBrain = true;
    caps.requiresWorld = false;
    caps.isSingleton = true;
    return caps;
}

void SimpleSTDPPlugin::computeWeightChange(class Synapse* synapse, 
                                         class Neuron* preNeuron, 
                                         class Neuron* postNeuron,
                                         double reward) {
    // Simple STDP rule implementation
    // Long-term potentiation (LTP) for pre before post
    // Long-term depression (LTD) for post before pre
    if (preNeuron && postNeuron && synapse) {
        // Simple model: if pre fires before post, potentiate
        // This is a simplified version of STDP
        float delta = learningRate_ * (1.0f - std::abs(preNeuron->getMembranePotential() - postNeuron->getMembranePotential()));
        synapse->addToWeight(delta);
    }
}

void SimpleSTDPPlugin::applyWeightChange(class Synapse* synapse, float delta) {
    if (synapse) {
        synapse->addToWeight(delta);
    }
}

float SimpleSTDPPlugin::getLearningRate() const {
    return learningRate_;
}

void SimpleSTDPPlugin::setLearningRate(float rate) {
    learningRate_ = rate;
    NLM_LOG_INFO("SimpleSTDPPlugin learning rate set to: " + std::to_string(learningRate_));
}

float SimpleSTDPPlugin::getMomentum() const {
    return momentum_;
}

void SimpleSTDPPlugin::setMomentum(float momentum) {
    momentum_ = momentum;
}

float SimpleSTDPPlugin::getL1Decay() const {
    return l1Decay_;
}

void SimpleSTDPPlugin::setL1Decay(float decay) {
    l1Decay_ = decay;
}

void SimpleSTDPPlugin::update(double timestep) {
    // Simple update - log STDP state
    NLM_LOG_INFO("SimpleSTDPPlugin updating: LR=" + std::to_string(learningRate_) + 
                 ", momentum=" + std::to_string(momentum_) + 
                 ", L1 decay=" + std::to_string(l1Decay_));
}

// SimpleVisualizationPlugin implementation
bool SimpleVisualizationPlugin::initialize(const std::any& config) {
    NLM_LOG_INFO("SimpleVisualizationPlugin initialized");
    return true;
}

void SimpleVisualizationPlugin::shutdown() {
    NLM_LOG_INFO("SimpleVisualizationPlugin shutdown");
}

PluginCapabilities SimpleVisualizationPlugin::getCapabilities() const {
    PluginCapabilities caps;
    caps.type = PluginType::Visualization;
    caps.name = "Simple Visualization";
    caps.version = "1.0.0";
    caps.description = "Simple visualization interface for brain activity";
    caps.dependencies = {};
    caps.requiresBrain = true;
    caps.requiresWorld = false;
    caps.isSingleton = false;
    return caps;
}

void SimpleVisualizationPlugin::render(class Brain* brain, double timestamp) {
    if (brain) {
        NLM_LOG_INFO("SimpleVisualizationPlugin rendering brain at time " + std::to_string(timestamp) +
                     ": " + std::to_string(brain->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(brain->getTotalSpikeCount()) + " spikes");
    }
}

void SimpleVisualizationPlugin::setOutputMode(const std::string& mode) {
    outputMode_ = mode;
    NLM_LOG_INFO("SimpleVisualizationPlugin output mode set to: " + mode);
}

std::string SimpleVisualizationPlugin::getOutputMode() const {
    return outputMode_;
}

void SimpleVisualizationPlugin::saveVisualization(const std::string& filepath) {
    NLM_LOG_INFO("SimpleVisualizationPlugin saving visualization to: " + filepath);
}

void SimpleVisualizationPlugin::setStyle(const std::string& style) {
    style_ = style;
}

std::string SimpleVisualizationPlugin::getStyle() const {
    return style_;
}

void SimpleVisualizationPlugin::setColorMap(const std::vector<float>& colormap) {
    colormap_ = colormap;
}

std::vector<float> SimpleVisualizationPlugin::getColorMap() const {
    return colormap_;
}

void SimpleVisualizationPlugin::update(double timestep) {
    // Simple update - log visualization state
    NLM_LOG_INFO("SimpleVisualizationPlugin updating: mode=" + outputMode_ + ", style=" + style_);
}

} // namespace nlm