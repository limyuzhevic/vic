#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <memory>
#include <map>

namespace nlm {

// Custom exception class for configuration errors
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // NEW: Extensibility components
    std::unique_ptr<DependencyInjector> dependencyInjector;
    std::unique_ptr<ComponentFactory> componentFactory;
    std::vector<std::unique_ptr<Observer>> observers;
    std::vector<std::unique_ptr<EventHandler>> eventHandlers;
    std::unique_ptr<Strategy> strategy;
    std::unique_ptr<NeuralModel> neuralModel;
    std::vector<std::unique_ptr<Neuromodulator>> neuromodulators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Basic file loading implementation
    std::ifstream file(filepath);
    if (!file) {
        return false;
    }
    
    // Simple key=value parsing (could be enhanced with JSON/XML support)
    std::string line;
    while (std::getline(file, line)) {
        size_t delimiter = line.find('=');
        if (delimiter != std::string::npos) {
            std::string key = trim(line.substr(0, delimiter));
            std::string value = trim(line.substr(delimiter + 1));
            set(key, value);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Basic command line argument parsing
    bool loaded = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.find("--") == 0) {
            // Remove -- prefix
            arg = arg.substr(2);
            size_t delimiter = arg.find('=');
            if (delimiter != std::string::npos) {
                std::string key = trim(arg.substr(0, delimiter));
                std::string value = trim(arg.substr(delimiter + 1));
                set(key, value);
                loaded = true;
            }
        }
    }
    return loaded;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << "=" << entry.value << std::endl;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            try {
                return std::get<T>(entry.value);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
    }
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? *val : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if key already exists
    bool exists = false;
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.value = value;
            entry.source = source;
            exists = true;
            break;
        }
    }
    
    if (!exists) {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Notify observers of state change
    notifyStateChange("set", key, value);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return true;
        }
    }
    return false;
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                            [&key](const ConfigEntry& entry) { return entry.key == key; });
    pImpl->entries.erase(it, pImpl->entries.end());
    
    // Notify observers of state change
    ConfigValue emptyValue;
    notifyStateChange("remove", key, emptyValue);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    
    // Notify observers of state change
    notifyStateChange("clear", "", ConfigValue());
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    ss << "Total entries: " << pImpl->entries.size() << std::endl;
    for (const auto& entry : pImpl->entries) {
        ss << "  " << entry.key << " = " << entry.value << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
    }
    return ss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// EXTENSIBILITY IMPLEMENTATION
void Config::attachObserver(std::unique_ptr<Observer> observer) {
    pImpl->observers.push_back(std::move(observer));
}

void Config::detachObserver(const std::string& observerName) {
    auto it = std::remove_if(pImpl->observers.begin(), pImpl->observers.end(),
                            [&observerName](const std::unique_ptr<Observer>& observer) {
                                return observer->getName() == observerName;
                            });
    pImpl->observers.erase(it, pImpl->observers.end());
}

void Config::notifyStateChange(const std::string& eventType, 
                              const std::string& key, 
                              const ConfigValue& value) {
    for (const auto& observer : pImpl->observers) {
        observer->onStateChange(eventType, key, value);
    }
}

void Config::registerEventHandler(std::unique_ptr<EventHandler> handler) {
    pImpl->eventHandlers.push_back(std::move(handler));
}

void Config::unregisterEventHandler(const std::string& handlerName) {
    auto it = std::remove_if(pImpl->eventHandlers.begin(), pImpl->eventHandlers.end(),
                            [&handlerName](const std::unique_ptr<EventHandler>& handler) {
                                return handler->getName() == handlerName;
                            });
    pImpl->eventHandlers.erase(it, pImpl->eventHandlers.end());
}

void Config::dispatchEvent(const std::string& eventType,
                          const std::map<std::string, ConfigValue>& data) {
    for (const auto& handler : pImpl->eventHandlers) {
        if (handler->canHandle(eventType)) {
            handler->handleEvent(eventType, data);
        }
    }
}

void Config::setStrategy(std::unique_ptr<Strategy> strategy) {
    pImpl->strategy = std::move(strategy);
}

Strategy* Config::getStrategy() const {
    return pImpl->strategy.get();
}

ConfigValue Config::executeStrategy(const std::vector<ConfigValue>& inputs) {
    if (pImpl->strategy) {
        return pImpl->strategy->execute(inputs);
    }
    return ConfigValue();
}

void Config::setNeuralModel(std::unique_ptr<NeuralModel> model) {
    pImpl->neuralModel = std::move(model);
}

NeuralModel* Config::getNeuralModel() const {
    return pImpl->neuralModel.get();
}

std::vector<float> Config::processNeuralModel(const std::vector<float>& input) {
    if (pImpl->neuralModel) {
        return pImpl->neuralModel->process(input);
    }
    return std::vector<float>();
}

void Config::addNeuromodulator(std::unique_ptr<Neuromodulator> modulator) {
    pImpl->neuromodulators.push_back(std::move(modulator));
}

void Config::removeNeuromodulator(const std::string& name) {
    auto it = std::remove_if(pImpl->neuromodulators.begin(), pImpl->neuromodulators.end(),
                            [&name](const std::unique_ptr<Neuromodulator>& modulator) {
                                return modulator->getName() == name;
                            });
    pImpl->neuromodulators.erase(it, pImpl->neuromodulators.end());
}

void Config::applyNeuromodulation(NeuronId neuronId, const std::string& modulatorName, float signal) {
    for (const auto& modulator : pImpl->neuromodulators) {
        if (modulator->getName() == modulatorName) {
            modulator->apply(neuronId, signal);
            break;
        }
    }
}

void Config::setDependencyInjector(std::unique_ptr<DependencyInjector> injector) {
    pImpl->dependencyInjector = std::move(injector);
}

DependencyInjector* Config::getDependencyInjector() const {
    return pImpl->dependencyInjector.get();
}

void Config::setComponentFactory(std::unique_ptr<ComponentFactory> factory) {
    pImpl->componentFactory = std::move(factory);
}

ComponentFactory* Config::getComponentFactory() const {
    return pImpl->componentFactory.get();
}

} // namespace nlm
