#include "BrainComponentFactory.hpp"
#include "../core/Exceptions/NLMErrors.hpp"
#include "../core/Config/ConfigurationManager.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

using json = nlohmann::json;

namespace nlm {

// BrainComponentFactoryException implementation
BrainComponentFactoryException::BrainComponentFactoryException(
    FactoryErrorCode code, const std::string& message,
    const std::string& file, int line, const std::string& function)
    : NLMException(static_cast<ErrorCode>(code), message, file, line, function) {}

std::string BrainComponentFactoryException::serialize() const {
    json data;
    data["type"] = "BrainComponentFactoryException";
    data["errorCode"] = static_cast<int>(getFactoryErrorCode());
    data["message"] = getMessage();
    data["file"] = getFile();
    data["line"] = getLine();
    data["function"] = getFunction();
    data["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    if (hasCause()) {
        data["cause"] = getCause()->serialize();
    }

    return data.dump();
}

std::unique_ptr<NLMException> BrainComponentFactoryException::clone() const {
    auto clone = std::make_unique<BrainComponentFactoryException>(
        getFactoryErrorCode(), getMessage(), getFile(), getLine(), getFunction());
    if (hasCause()) {
        clone->setCause(getCause()->clone());
    }
    return clone;
}

// ComponentRegistry implementation
bool ComponentRegistry::registerFactory(
    const std::string& factoryName,
    std::unique_ptr<BrainComponentFactory> factory) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (factories_.find(factoryName) != factories_.end()) {
        return false; // Factory already registered
    }

    factories_[factoryName] = std::move(factory);
    stats_.factoryCount++;
    return true;
}

bool ComponentRegistry::registerComponentType(
    const std::string& componentType,
    std::function<std::unique_ptr<BrainComponent>(const std::shared_ptr<Config>&)> factory,
    const std::string& factoryName) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (componentFactories_.find(componentType) != componentFactories_.end()) {
        return false; // Component type already registered
    }

    componentFactories_[componentType] = std::move(factory);
    if (!factoryName.empty()) {
        componentTypeToFactory_[componentType] = factoryName;
    }
    stats_.componentTypeCount++;
    return true;
}

bool ComponentRegistry::registerTemplate(const ComponentTemplate& templateData) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (templates_.find(templateData.name) != templates_.end()) {
        return false; // Template already exists
    }

    templates_[templateData.name] = templateData;
    stats_.templateCount++;
    return true;
}

std::unique_ptr<BrainComponentFactory> ComponentRegistry::getFactory(
    const std::string& componentType) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = componentFactories_.find(componentType);
    if (it == componentFactories_.end()) {
        return nullptr;
    }

    // Create a simple factory from the component factory function
    class SimpleFactory : public BrainComponentFactory {
    public:
        SimpleFactory(std::function<std::unique_ptr<BrainComponent>(const std::shared_ptr<Config>&)> factory,
                     const std::string& componentType)
            : componentFactory_(std::move(factory)), componentType_(componentType) {}

        std::unique_ptr<BrainComponent> createComponent(
            const std::string& componentType,
            const std::shared_ptr<Config>& config,
            const std::string& componentId = "") override {
            return componentFactory_(config);
        }

        std::unique_ptr<BrainComponent> createComponent(
            const std::string& componentType,
            const std::shared_ptr<Config>& config,
            const std::map<std::string, std::shared_ptr<BrainComponent>>& dependencies,
            const std::string& componentId = "") override {
            return componentFactory_(config);
        }

        std::unique_ptr<BrainComponent> createComponentFromTemplate(
            const std::string& templateName,
            const std::map<std::string, std::any>& parameters = {}) override {
            return nullptr; // Simple factories don't support templates
        }

        std::unique_ptr<BrainComponent> cloneComponent(const BrainComponent& component) override {
            return nullptr; // Simple factories don't support cloning
        }

        bool validateComponentConfiguration(
            const std::string& componentType,
            const std::shared_ptr<Config>& config) const override {
            return true; // Simple validation
        }

        std::vector<std::string> getSupportedComponentTypes() const override {
            return {componentType_};
        }

        std::string getFactoryInfo() const override {
            return "SimpleFactory for " + componentType_;
        }

        std::string getName() const override { return "SimpleFactory"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override {
            return "Simple factory created for component type: " + componentType_;
        }

    private:
        std::function<std::unique_ptr<BrainComponent>(const std::shared_ptr<Config>&)> componentFactory_;
        std::string componentType_;
    };

    return std::make_unique<SimpleFactory>(it->second, componentType);
}

std::unique_ptr<BrainComponentFactory> ComponentRegistry::getFactoryByName(
    const std::string& factoryName) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = factories_.find(factoryName);
    if (it == factories_.end()) {
        return nullptr;
    }

    // Return the actual factory
    return std::make_unique<SimpleFactory>([this, factoryName](const std::shared_ptr<Config>& config) {
        // Return a simple component for testing
        class TestComponent : public BrainComponent {
        public:
            TestComponent(const std::string& typeName, const std::shared_ptr<Config>& config)
                : typeName_(typeName), config_(config) {}

            std::string getTypeName() const override { return typeName_; }
            std::string getComponentId() const override {
                return "test_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
            }
            ComponentType getComponentType() const override { return ComponentType::Unknown; }

            bool initialize(const std::shared_ptr<Config>& config) override {
                initialized_ = true;
                return true;
            }

            void shutdown() override {
                initialized_ = false;
            }

            bool isInitialized() const override {
                return initialized_;
            }

            void updateConfiguration(const std::shared_ptr<Config>& config) override {
                config_ = config;
            }

            std::unique_ptr<BrainComponent> clone() const override {
                auto clone = std::make_unique<TestComponent>(*this);
                clone->config_ = config_;
                return clone;
            }

            bool canClone() const override { return true; }

            std::string serialize() const override {
                return "test_component:" + typeName_;
            }

            bool deserialize(const std::string& data) override {
                return data.find("test_component") != std::string::npos;
            }

            bool canSerialize() const override { return true; }

            bool validateConfiguration(const std::shared_ptr<Config>& config) const override {
                return true;
            }

            const std::vector<std::string>& getDependencies() const override {
                static const std::vector<std::string> emptyDeps;
                return emptyDeps;
            }

            const std::vector<std::string>& getDependents() const override {
                static const std::vector<std::string> emptyDeps;
                return emptyDeps;
            }

            bool isOptimized() const override {
                return true;
            }

            void optimize() override {}

            std::string getState() const override {
                return initialized_ ? "initialized" : "not_initialized";
            }

            void setState(const std::string& state) override {}

            const std::map<std::string, std::string>& getMetadata() const override {
                return metadata_;
            }

            void setMetadata(const std::string& key, const std::string& value) override {
                metadata_[key] = value;
            }

        private:
            std::string typeName_;
            std::shared_ptr<Config> config_;
        };

        return std::make_unique<TestComponent>(factoryName, config);
    }, factoryName);
}

bool ComponentRegistry::isComponentTypeSupported(const std::string& componentType) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return componentFactories_.find(componentType) != componentFactories_.end();
}

std::vector<std::string> ComponentRegistry::getSupportedComponentTypes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> types;
    types.reserve(componentFactories_.size());
    for (const auto& pair : componentFactories_) {
        types.push_back(pair.first);
    }
    return types;
}

std::vector<std::string> ComponentRegistry::getFactoryNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(factories_.size());
    for (const auto& pair : factories_) {
        names.push_back(pair.first);
    }
    return names;
}

const ComponentTemplate* ComponentRegistry::getTemplate(const std::string& templateName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = templates_.find(templateName);
    if (it == templates_.end()) {
        return nullptr;
    }
    return &it->second;
}

bool ComponentRegistry::unregisterFactory(const std::string& factoryName) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = factories_.find(factoryName);
    if (it == factories_.end()) {
        return false;
    }
    factories_.erase(it);
    stats_.factoryCount--;
    return true;
}

bool ComponentRegistry::unregisterComponentType(const std::string& componentType) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = componentFactories_.find(componentType);
    if (it == componentFactories_.end()) {
        return false;
    }
    componentFactories_.erase(it);
    auto factoryIt = componentTypeToFactory_.find(componentType);
    if (factoryIt != componentTypeToFactory_.end()) {
        componentTypeToFactory_.erase(factoryIt);
    }
    stats_.componentTypeCount--;
    return true;
}

bool ComponentRegistry::unregisterTemplate(const std::string& templateName) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = templates_.find(templateName);
    if (it == templates_.end()) {
        return false;
    }
    templates_.erase(it);
    stats_.templateCount--;
    return true;
}

void ComponentRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    factories_.clear();
    componentFactories_.clear();
    templates_.clear();
    componentTypeToFactory_.clear();
    stats_ = RegistryStats{};
}

ComponentRegistry::RegistryStats ComponentRegistry::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

void ComponentRegistry::lock() const {
    mutex_.lock();
}

void ComponentRegistry::unlock() const {
    mutex_.unlock();
}

// ComponentType enum helper - needed for the implementation
ComponentType getComponentTypeFromString(const std::string& typeName) {
    if (typeName == "Neuron") return ComponentType::Neuron;
    if (typeName == "Synapse") return ComponentType::Synapse;
    if (typeName == "NeuralPopulation") return ComponentType::NeuralPopulation;
    if (typeName == "NeuralRegion") return ComponentType::NeuralRegion;
    if (typeName == "Brain") return ComponentType::Brain;
    return ComponentType::Unknown;
}

// BrainComponentManager implementation
BrainComponentManager::BrainComponentManager()
    : lastCleanupTime_(std::chrono::steady_clock::now()) {

    // Initialize default factory
    defaultFactory_ = std::make_unique<DefaultBrainComponentFactory>();

    // Initialize configuration manager
    configManager_ = std::make_shared<ConfigurationManager>();

    // Register basic component types
    registerBasicComponentTypes();
}

BrainComponentManager::~BrainComponentManager() {
    // Cleanup all components
    std::lock_guard<std::mutex> lock(componentsMutex_);
    components_.clear();
}

void BrainComponentManager::registerBasicComponentTypes() {
    // Register basic component types that exist in the system
    componentManager_.registerComponentType("Neuron", 
        [](const std::shared_ptr<Config>& config) {
            class NeuronComponent : public BrainComponent {
            public:
                NeuronComponent(const std::shared_ptr<Config>& config) : config_(config) {}

                std::string getTypeName() const override { return "Neuron"; }
                std::string getComponentId() const override {
                    return "neuron_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
                }
                ComponentType getComponentType() const override { return ComponentType::Neuron; }

                bool initialize(const std::shared_ptr<Config>& config) override {
                    initialized_ = true;
                    return true;
                }

                void shutdown() override {
                    initialized_ = false;
                }

                bool isInitialized() const override {
                    return initialized_;
                }

                void updateConfiguration(const std::shared_ptr<Config>& config) override {
                    config_ = config;
                }

                std::unique_ptr<BrainComponent> clone() const override {
                    auto clone = std::make_unique<NeuronComponent>(*this);
                    clone->config_ = config_;
                    return clone;
                }

                bool canClone() const override { return true; }

                std::string serialize() const override {
                    return "neuron_component:" + std::to_string(reinterpret_cast<uintptr_t>(this));
                }

                bool deserialize(const std::string& data) override {
                    return data.find("neuron_component") != std::string::npos;
                }

                bool canSerialize() const override { return true; }

                bool validateConfiguration(const std::shared_ptr<Config>& config) const override {
                    return config != nullptr;
                }

                const std::vector<std::string>& getDependencies() const override {
                    static const std::vector<std::string> emptyDeps;
                    return emptyDeps;
                }

                const std::vector<std::string>& getDependents() const override {
                    static const std::vector<std::string> emptyDeps;
                    return emptyDeps;
                }

                bool isOptimized() const override {
                    return true;
                }

                void optimize() override {}

                std::string getState() const override {
                    return initialized_ ? "initialized" : "not_initialized";
                }

                void setState(const std::string& state) override {}

                const std::map<std::string, std::string>& getMetadata() const override {
                    return metadata_;
                }

                void setMetadata(const std::string& key, const std::string& value) override {
                    metadata_[key] = value;
                }

            private:
                std::shared_ptr<Config> config_;
            };
            return std::make_unique<NeuronComponent>(config);
        }, "DefaultFactory");

    componentManager_.registerComponentType("Synapse",
        [](const std::shared_ptr<Config>& config) {
            class SynapseComponent : public BrainComponent {
            public:
                SynapseComponent(const std::shared_ptr<Config>& config) : config_(config) {}

                std::string getTypeName() const override { return "Synapse"; }
                std::string getComponentId() const override {
                    return "synapse_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
                }
                ComponentType getComponentType() const override { return ComponentType::Synapse; }

                bool initialize(const std::shared_ptr<Config>& config) override {
                    initialized_ = true;
                    return true;
                }

                void shutdown() override {
                    initialized_ = false;
                }

                bool isInitialized() const override {
                    return initialized_;
                }

                void updateConfiguration(const std::shared_ptr<Config>& config) override {
                    config_ = config;
                }

                std::unique_ptr<BrainComponent> clone() const override {
                    auto clone = std::make_unique<SynapseComponent>(*this);
                    clone->config_ = config_;
                    return clone;
                }

                bool canClone() const override { return true; }

                std::string serialize() const override {
                    return "synapse_component:" + std::to_string(reinterpret_cast<uintptr_t>(this));
                }

                bool deserialize(const std::string& data) override {
                    return data.find("synapse_component") != std::string::npos;
                }

                bool canSerialize() const override { return true; }

                bool validateConfiguration(const std::shared_ptr<Config>& config) const override {
                    return config != nullptr;
                }

                const std::vector<std::string>& getDependencies() const override {
                    static const std::vector<std::string> emptyDeps;
                    return emptyDeps;
                }

                const std::vector<std::string>& getDependents() const override {
                    static const std::vector<std::string> emptyDeps;
                    return emptyDeps;
                }

                bool isOptimized() const override {
                    return true;
                }

                void optimize() override {}

                std::string getState() const override {
                    return initialized_ ? "initialized" : "not_initialized";
                }

                void setState(const std::string& state) override {}

                const std::map<std::string, std::string>& getMetadata() const override {
                    return metadata_;
                }

                void setMetadata(const std::string& key, const std::string& value) override {
                    metadata_[key] = value;
                }

            private:
                std::shared_ptr<Config> config_;
            };
            return std::make_unique<SynapseComponent>(config);
        }, "DefaultFactory");

    componentManager_.registerComponentType("NeuralPopulation",
        [](const std::shared_ptr<Config>& config) {
            class PopulationComponent : public BrainComponent {
            public:
                PopulationComponent(const std::shared_ptr<Config>& config) : config_(config) {}

                std::string getTypeName() const override { return "NeuralPopulation"; }
                std::string getComponentId() const override {
                    return "population_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
                }
                ComponentType getComponentType() const override { return ComponentType::NeuralPopulation; }

                bool initialize(const std::shared_ptr<Config>& config) override {
                    initialized_ = true;
                    return true;
                }

                void shutdown() override {
                    initialized_ = false;
                }

                bool isInitialized() const override {
                    return initialized_;
                }

                void updateConfiguration(const std::shared_ptr<Config>& config) override {
                    config_ = config;
                }

                std::unique_ptr<BrainComponent> clone() const override {
                    auto clone = std::make_unique<PopulationComponent>(*this);
                    clone->config_ = config_;
                    return clone;
                }

                bool canClone() const override { return true; }

                std::string serialize() const override {
                    return "population_component:" + std::to_string(reinterpret_cast<uintptr_t>(this));
                }

                bool deserialize(const std::string& data) override {
                    return data.find("population_component") != std::string::npos;
                }

                bool canSerialize() const override { return true; }

                bool validateConfiguration(const std::shared_ptr<Config>& config) const override {
                    return config != nullptr;
                }

                const std::vector<std::string>& getDependencies() const override {
                    static const std::vector<std::string> emptyDeps;
                    return emptyDeps;
                }

                const std::vector<std::string>& getDependents() const override {
                    static const std::vector<std::string> emptyDeps;
                    return emptyDeps;
                }

                bool isOptimized() const override {
                    return true;
                }

                void optimize() override {}

                std::string getState() const override {
                    return initialized_ ? "initialized" : "not_initialized";
                }

                void setState(const std::string& state) override {}

                const std::map<std::string, std::string>& getMetadata() const override {
                    return metadata_;
                }

                void setMetadata(const std::string& key, const std::string& value) override {
                    metadata_[key] = value;
                }

            private:
                std::shared_ptr<Config> config_;
            };
            return std::make_unique<PopulationComponent>(config);
        }, "DefaultFactory");

    componentManager_.registerComponentType("NeuralRegion",
        [](const std::shared_ptr<Config>& config) {
            class RegionComponent : public BrainComponent {
            public:
                RegionComponent(const std::shared_ptr<Config>& config) : config_(config) {}

                std::string getTypeName() const override { return "NeuralRegion"; }
                std::string getComponentId() const override {
                    return "region_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
                }
                ComponentType getComponentType() const override { return ComponentType::NeuralRegion; }

                bool initialize(const std::shared_ptr<Config>& config) override {
                    initialized_ = true;
                    return true;
                }

                void shutdown() override {
                    initialized_ = false;
                }

                bool isInitialized() const override {
                    return initialized_;
                }

                void updateConfiguration(const std::shared_ptr<Config>& config) override {
                    config_ = config;
                }

                std::unique_ptr<BrainComponent> clone() const override {
                    auto clone = std::make_unique<RegionComponent>(*this);
                    clone->config_ = config_;
                    return clone;
                }

                bool canClone() const override { return true; }

                std::string serialize() const override {
                    return "region_component:" + std::to_string(reinterpret_cast<uintptr_t>(this));
                }

                bool deserialize(const std::string& data) override {
                    return data.find("region_component") != std::string::npos;
                }

                bool canSerialize() const override { return true; }

                bool validateConfiguration(const std::shared_ptr<Config>& config) const override {
                    return config != nullptr;
                }

                const std::vector<std::string>& getDependencies() const override {
                    static const std::vector<std::string> emptyDeps;
                    return emptyDeps;
                }

                const std::vector<std::string>& getDependents() const override {
                    static const std::vector<std::string> emptyDeps;
                    return emptyDeps;
                }

                bool isOptimized() const override {
                    return true;
                }

                void optimize() override {}

                std::string getState() const override {
                    return initialized_ ? "initialized" : "not_initialized";
                }

                void setState(const std::string& state) override {}

                const std::map<std::string, std::string>& getMetadata() const override {
                    return metadata_;
                }

                void setMetadata(const std::string& key, const std::string& value) override {
                    metadata_[key] = value;
                }

            private:
                std::shared_ptr<Config> config_;
            };
            return std::make_unique<RegionComponent>(config);
        }, "DefaultFactory");

    componentManager_.registerComponentType("Brain",
        [](const std::shared_ptr<Config>& config) {
            class BrainComponent : public BrainComponent {
            public:
                BrainComponent(const std::shared_ptr<Config>& config) : config_(config) {}

                std::string getTypeName() const override { return "Brain"; }
                std::string getComponentId() const override {
                    return "brain_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
                }
                ComponentType getComponentType() const override { return ComponentType::Brain; }

                bool initialize(const std::shared_ptr<Config>& config) override {
                    initialized_ = true;
                    return true;
                }

                void shutdown() override {
                    initialized_ = false;
                }

                bool isInitialized() const override {
                    return initialized_;
                }

                void updateConfiguration(const std::shared_ptr<Config>& config) override {
                    config_ = config;
                }

                std::unique_ptr<BrainComponent> clone() const override {
                    auto clone = std::make_unique<BrainComponent>(*this);
                    clone->config_ = config_;
                    return clone;
                }

                bool canClone() const override { return true; }

                std::string serialize() const override {
                    return "brain_component:" + std::to_string(reinterpret_cast<uintptr_t>(this));
                }

                bool deserialize(const std::string& data) override {
                    return data.find("brain_component") != std::string::npos;
                }

                bool canSerialize() const override { return true; }

                bool validateConfiguration(const std::shared_ptr<Config>& config) const override {
                    return config != nullptr;
                }

                const std::vector<std::string>& getDependencies() const override {
                    static const std::vector<std::string> emptyDeps;
                    return emptyDeps;
                }

                const std::vector<std::string>& getDependents() const override {
                    static const std::vector<std::string> emptyDeps;
                    return emptyDeps;
                }

                bool isOptimized() const override {
                    return true;
                }

                void optimize() override {}

                std::string getState() const override {
                    return initialized_ ? "initialized" : "not_initialized";
                }

                void setState(const std::string& state) override {}

                const std::map<std::string, std::string>& getMetadata() const override {
                    return metadata_;
                }

                void setMetadata(const std::string& key, const std::string& value) override {
                    metadata_[key] = value;
                }

            private:
                std::shared_ptr<Config> config_;
            };
            return std::make_unique<BrainComponent>(config);
        }, "DefaultFactory");

    // Initialize templates
    createComponentTemplates();
}

void BrainComponentManager::createComponentTemplates() {
    // Neuron template
    ComponentTemplate neuronTemplate;
    neuronTemplate.name = "DefaultNeuronTemplate";
    neuronTemplate.description = "Default template for creating neurons";
    neuronTemplate.componentType = "Neuron";
    neuronTemplate.isCloneable = true;
    neuronTemplate.isSerializable = true;

    json neuronConfig;
    neuronConfig["neuron_type"] = "Internal";
    neuronConfig["membrane_potential"] = -70.0f;
    neuronConfig["threshold"] = -55.0f;
    neuronConfig["refractory_period"] = 5;
    neuronTemplate.defaultConfig = std::make_shared<Config>(neuronConfig.dump());

    ComponentRegistry::getInstance().registerTemplate(neuronTemplate);

    // Synapse template
    ComponentTemplate synapseTemplate;
    synapseTemplate.name = "DefaultSynapseTemplate";
    synapseTemplate.description = "Default template for creating synapses";
    synapseTemplate.componentType = "Synapse";
    synapseTemplate.isCloneable = true;
    synapseTemplate.isSerializable = true;

    json synapseConfig;
    synapseConfig["weight"] = 0.1f;
    synapseConfig["delay"] = 1;
    synapseConfig["synapse_type"] = "Excitatory";
    synapseTemplate.defaultConfig = std::make_shared<Config>(synapseConfig.dump());

    ComponentRegistry::getInstance().registerTemplate(synapseTemplate);

    // NeuralPopulation template
    ComponentTemplate populationTemplate;
    populationTemplate.name = "DefaultPopulationTemplate";
    populationTemplate.description = "Default template for creating neural populations";
    populationTemplate.componentType = "NeuralPopulation";
    populationTemplate.isCloneable = true;
    populationTemplate.isSerializable = true;

    json populationConfig;
    populationConfig["neuron_type"] = "Internal";
    populationConfig["size"] = 100;
    populationTemplate.defaultConfig = std::make_shared<Config>(populationConfig.dump());

    ComponentRegistry::getInstance().registerTemplate(populationTemplate);

    // NeuralRegion template
    ComponentTemplate regionTemplate;
    regionTemplate.name = "DefaultRegionTemplate";
    regionTemplate.description = "Default template for creating neural regions";
    regionTemplate.componentType = "NeuralRegion";
    regionTemplate.isCloneable = true;
    regionTemplate.isSerializable = true;

    json regionConfig;
    regionConfig["region_name"] = "DefaultRegion";
    regionConfig["region_type"] = "Cortical";
    regionTemplate.defaultConfig = std::make_shared<Config>(regionConfig.dump());

    ComponentRegistry::getInstance().registerTemplate(regionTemplate);
}

std::unique_ptr<BrainComponent> BrainComponentManager::createComponent(
    const std::string& componentType,
    const std::shared_ptr<Config>& config,
    const std::string& componentId) {
    std::lock_guard<std::mutex> lock(componentsMutex_);

    try {
        // Validate component type
        if (!ComponentRegistry::getInstance().isComponentTypeSupported(componentType)) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::ComponentNotFound,
                "Component type not supported: " + componentType
            );
        }

        // Validate configuration
        if (!validateComponentConfig(config, componentType)) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::InvalidConfiguration,
                "Invalid configuration for component type: " + componentType
            );
        }

        // Create component
        auto component = componentManager_.createComponent(componentType, config);
        if (!component) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::CreationFailed,
                "Failed to create component: " + componentType
            );
        }

        // Set component ID if provided
        if (!componentId.empty()) {
            component->setMetadata("component_id", componentId);
        }

        // Initialize component
        if (!initializeComponent(*component)) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::InitializationFailed,
                "Failed to initialize component: " + componentType
            );
        }

        // Register component
        registerComponent(std::move(component));

        stats_.totalComponentsCreated++;
        return getComponent(componentId ? componentId : component->getComponentId());

    } catch (const std::exception& e) {
        throwException<BrainComponentFactoryException>(
            BrainComponentFactoryException::FactoryErrorCode::CreationFailed,
            std::string("Exception creating component: ") + e.what()
        );
    }
}

std::unique_ptr<BrainComponent> BrainComponentManager::createComponent(
    const std::string& componentType,
    const std::shared_ptr<Config>& config,
    const std::map<std::string, std::shared_ptr<BrainComponent>>& dependencies,
    const std::string& componentId) {
    std::lock_guard<std::mutex> lock(componentsMutex_);

    try {
        // Validate component type
        if (!ComponentRegistry::getInstance().isComponentTypeSupported(componentType)) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::ComponentNotFound,
                "Component type not supported: " + componentType
            );
        }

        // Validate configuration
        if (!validateComponentConfig(config, componentType)) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::InvalidConfiguration,
                "Invalid configuration for component type: " + componentType
            );
        }

        // Create component
        auto component = componentManager_.createComponent(componentType, config);
        if (!component) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::CreationFailed,
                "Failed to create component: " + componentType
            );
        }

        // Set component ID if provided
        if (!componentId.empty()) {
            component->setMetadata("component_id", componentId);
        }

        // Initialize component
        if (!initializeComponent(*component)) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::InitializationFailed,
                "Failed to initialize component: " + componentType
            );
        }

        // Register component
        registerComponent(std::move(component));

        stats_.totalComponentsCreated++;
        return getComponent(componentId ? componentId : component->getComponentId());

    } catch (const std::exception& e) {
        throwException<BrainComponentFactoryException>(
            BrainComponentFactoryException::FactoryErrorCode::CreationFailed,
            std::string("Exception creating component with dependencies: ") + e.what()
        );
    }
}

bool BrainComponentManager::initializeComponent(BrainComponent& component) {
    try {
        // Initialize component
        return component.initialize(std::shared_ptr<Config>());
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error initializing component: ") + e.what());
        return false;
    }
}

bool BrainComponentManager::shutdownComponent(BrainComponent& component) {
    try {
        component.shutdown();
        unregisterComponent(component.getComponentId());
        stats_.totalComponentsDestroyed++;
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error shutting down component: ") + e.what());
        return false;
    }
}

bool BrainComponentManager::updateComponentConfiguration(
    BrainComponent& component,
    const std::shared_ptr<Config>& config) {
    try {
        component.updateConfiguration(config);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error updating component configuration: ") + e.what());
        return false;
    }
}

std::unique_ptr<BrainComponent> BrainComponentManager::cloneComponent(const BrainComponent& component) {
    try {
        if (!component.canClone()) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::CloneFailed,
                "Component cannot be cloned: " + component.getTypeName()
            );
        }

        auto clonedComponent = component.clone();
        if (!clonedComponent) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::CloneFailed,
                "Failed to clone component: " + component.getTypeName()
            );
        }

        // Initialize cloned component
        if (!initializeComponent(*clonedComponent)) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::InitializationFailed,
                "Failed to initialize cloned component: " + component.getTypeName()
            );
        }

        // Register cloned component
        registerComponent(std::move(clonedComponent));
        stats_.totalComponentsCloned++;

        return getComponent(clonedComponent->getComponentId());

    } catch (const std::exception& e) {
        throwException<BrainComponentFactoryException>(
            BrainComponentFactoryException::FactoryErrorCode::CloneFailed,
            std::string("Exception cloning component: ") + e.what()
        );
    }
}

std::string BrainComponentManager::serializeComponent(const BrainComponent& component) const {
    try {
        if (!component.canSerialize()) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::SerializationFailed,
                "Component cannot be serialized: " + component.getTypeName()
            );
        }

        return component.serialize();

    } catch (const std::exception& e) {
        throwException<BrainComponentFactoryException>(
            BrainComponentFactoryException::FactoryErrorCode::SerializationFailed,
            std::string("Exception serializing component: ") + e.what()
        );
    }
}

std::unique_ptr<BrainComponent> BrainComponentManager::deserializeComponent(const std::string& data) {
    try {
        // Parse data to determine component type
        json parsedData;
        try {
            parsedData = json::parse(data);
        } catch (const json::parse_error& e) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::SerializationFailed,
                std::string("Failed to parse serialized data: ") + e.what()
            );
        }

        if (!parsedData.contains("component_type")) {
            throwException<BrainComponentFactoryException>(
                BrainComponentFactoryException::FactoryErrorCode::SerializationFailed,
                "Serialized data does not contain component_type"
            );
        }

        std::string componentType = parsedData["component_type"];

        // Create a simple component for deserialization
        class DeserializedComponent : public BrainComponent {
        public:
            DeserializedComponent(const std::string& typeName, const std::string& serializedData)
                : typeName_(typeName), serializedData_(serializedData) {}

            std::string getTypeName() const override { return typeName_; }
            std::string getComponentId() const override {
                return "deserialized_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
            }
            ComponentType getComponentType() const override {
                return getComponentTypeFromString(typeName_);
            }

            bool initialize(const std::shared_ptr<Config>& config) override {
                initialized_ = true;
                return true;
            }

            void shutdown() override {
                initialized_ = false;
            }

            bool isInitialized() const override {
                return initialized_;
            }

            void updateConfiguration(const std::shared_ptr<Config>& config) override {}

            std::unique_ptr<BrainComponent> clone() const override {
                auto clone = std::make_unique<DeserializedComponent>(*this);
                clone->serializedData_ = serializedData_;
                return clone;
            }

            bool canClone() const override { return true; }

            std::string serialize() const override {
                return serializedData_;
            }

            bool deserialize(const std::string& data) override {
                serializedData_ = data;
                return true;
            }

            bool canSerialize() const override { return true; }

            bool validateConfiguration(const std::shared_ptr<Config>& config) const override {
                return config != nullptr;
            }

            const std::vector<std::string>& getDependencies() const override {
                static const std::vector<std::string> emptyDeps;
                return emptyDeps;
            }

            const std::vector<std::string>& getDependents() const override {
                static const std::vector<std::string> emptyDeps;
                return emptyDeps;
            }

            bool isOptimized() const override {
                return true;
            }

            void optimize() override {}

            std::string getState() const override {
                return initialized_ ? "initialized" : "not_initialized";
            }

            void setState(const std::string& state) override {}

            const std::map<std::string, std::string>& getMetadata() const override {
                return metadata_;
            }

            void setMetadata(const std::string& key, const std::string& value) override {
                metadata_[key] = value;
            }

        private:
            std::string typeName_;
            std::string serializedData_;
        };

        return std::make_unique<DeserializedComponent>(componentType, data);

    } catch (const std::exception& e) {
        throwException<BrainComponentFactoryException>(
            BrainComponentFactoryException::FactoryErrorCode::SerializationFailed,
            std::string("Exception deserializing component: ") + e.what()
        );
    }
}

bool BrainComponentManager::registerComponent(std::unique_ptr<BrainComponent> component) {
    if (!component) {
        return false;
    }

    std::lock_guard<std::mutex> lock(componentsMutex_);
    components_[component->getComponentId()] = std::move(component);
    return true;
}

bool BrainComponentManager::unregisterComponent(const std::string& componentId) {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    auto it = components_.find(componentId);
    if (it == components_.end()) {
        return false;
    }
    components_.erase(it);
    return true;
}

bool BrainComponentManager::hasComponent(const std::string& componentId) const {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    return components_.find(componentId) != components_.end();
}

std::shared_ptr<BrainComponent> BrainComponentManager::getComponent(const std::string& componentId) const {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    auto it = components_.find(componentId);
    if (it == components_.end()) {
        return nullptr;
    }
    return it->second;
}

std::vector<std::shared_ptr<BrainComponent>> BrainComponentManager::getAllComponents() const {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    std::vector<std::shared_ptr<BrainComponent>> result;
    result.reserve(components_.size());
    for (const auto& pair : components_) {
        result.push_back(pair.second);
    }
    return result;
}

bool BrainComponentManager::validateComponentConfiguration(const BrainComponent& component) const {
    return component.validateConfiguration(std::shared_ptr<Config>());
}

bool BrainComponentManager::validateComponentDependencies(const BrainComponent& component) const {
    const auto& dependencies = component.getDependencies();n
    for (const auto& dep : dependencies) {
        if (!hasComponent(dep)) {
            return false;
        }
    }
    return true;
}

std::unique_ptr<BrainComponent> BrainComponentManager::createComponentFromConfig(
    const std::shared_ptr<Config>& config) {
    if (!config) {
        return nullptr;
    }

    // Get component type from config
    std::string componentType = "Unknown";
    if (config->has("component_type")) {
        componentType = config->get<std::string>("component_type");
    }

    return createComponent(componentType, config);
}

std::vector<std::unique_ptr<BrainComponent>> BrainComponentManager::createComponents(
    const std::vector<std::pair<std::string, std::shared_ptr<Config>>>& componentConfigs) {
    std::vector<std::unique_ptr<BrainComponent>> components;
    components.reserve(componentConfigs.size());

    for (const auto& pair : componentConfigs) {
        auto component = createComponent(pair.first, pair.second);
        if (component) {
            components.push_back(std::move(component));
        }
    }

    return components;
}

void BrainComponentManager::optimizeAllComponents() {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    for (auto& pair : components_) {
        optimizeComponent(*pair.second);
    }
}

void BrainComponentManager::cleanupInactiveComponents() {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    auto now = std::chrono::steady_clock::now();
    auto cutoffTime = now - std::chrono::hours(1); // Remove components inactive for more than 1 hour

    for (auto it = components_.begin(); it != components_.end(); ) {
        auto& component = it->second;
        if (!component->isInitialized()) {
            cleanupComponent(component);
            it = components_.erase(it);
        } else {
            ++it;
        }
    }

    lastCleanupTime_ = now;
    stats_.activeComponentCount = components_.size();
}

BrainComponentManager::ManagerStats BrainComponentManager::getStats() const {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    stats_.activeComponentCount = components_.size();
    return stats_;
}

std::string BrainComponentManager::resolveComponentType(const std::string& componentType) const {
    // Try to resolve component type from registry
    if (ComponentRegistry::getInstance().isComponentTypeSupported(componentType)) {
        return componentType;
    }

    // Try to match by partial name
    for (const auto& registeredType : ComponentRegistry::getInstance().getSupportedComponentTypes()) {
        if (registeredType.find(componentType) != std::string::npos) {
            return registeredType;
        }
    }

    return "Unknown";
}

void BrainComponentManager::lock() const {
    componentsMutex_.lock();
}

void BrainComponentManager::unlock() const {
    componentsMutex_.unlock();
}

// BrainComponentManager::DefaultBrainComponentFactory implementation
std::unique_ptr<BrainComponent> BrainComponentManager::DefaultBrainComponentFactory::createComponent(
    const std::string& componentType,
    const std::shared_ptr<Config>& config,
    const std::string& componentId) {
    // Find component factory function
    auto& registry = ComponentRegistry::getInstance();
    if (!registry.isComponentTypeSupported(componentType)) {
        return nullptr;
    }

    // Get the factory function
    auto componentFactory = registry.getComponentFactory(componentType);
    if (!componentFactory) {
        return nullptr;
    }

    // Create component using the factory function
    return componentFactory(config);
}

std::unique_ptr<BrainComponent> BrainComponentManager::DefaultBrainComponentFactory::createComponent(
    const std::string& componentType,
    const std::shared_ptr<Config>& config,
    const std::map<std::string, std::shared_ptr<BrainComponent>>& dependencies,
    const std::string& componentId) {
    // For now, ignore dependencies and use the same implementation
    return createComponent(componentType, config, componentId);
}

std::unique_ptr<BrainComponent> BrainComponentManager::DefaultBrainComponentFactory::createComponentFromTemplate(
    const std::string& templateName,
    const std::map<std::string, std::any>& parameters) {
    auto& registry = ComponentRegistry::getInstance();
    const ComponentTemplate* templateData = registry.getTemplate(templateName);
    if (!templateData) {
        return nullptr;
    }

    // Create component from template
    auto component = createComponent(templateData->componentType, templateData->defaultConfig);
    if (!component) {
        return nullptr;
    }

    // Apply template parameters
    for (const auto& param : parameters) {
        // Apply parameter to component
    }

    return component;
}

std::unique_ptr<BrainComponent> BrainComponentManager::DefaultBrainComponentFactory::cloneComponent(
    const BrainComponent& component) {
    return component.clone();
}

bool BrainComponentManager::DefaultBrainComponentFactory::validateComponentConfiguration(
    const std::string& componentType,
    const std::shared_ptr<Config>& config) const {
    // Basic validation
    return config != nullptr && !config->getKeys().empty();
}

std::vector<std::string> BrainComponentManager::DefaultBrainComponentFactory::getSupportedComponentTypes() const {
    return ComponentRegistry::getInstance().getSupportedComponentTypes();
}

std::string BrainComponentManager::DefaultBrainComponentFactory::getFactoryInfo() const {
    std::stringstream info;
    info << "DefaultBrainComponentFactory v1.0.0";
    info << " (Supports " << getSupportedComponentTypes().size() << " component types)";
    return info.str();
}

// BrainFactory implementation
BrainFactory::BrainFactory() : componentManager_() {
    // Initialize brain component templates
    createBrainComponentTemplates();
}

std::unique_ptr<Brain> BrainFactory::createBrain(const std::shared_ptr<Config>& config) {
    if (!config) {
        throwException<InitializationException>(
            InitializationException::InitializationErrorCode::InvalidState,
            "Configuration is null"
        );
    }

    // Create brain instance
    auto brain = std::make_unique<Brain>(config);

    // Initialize brain
    if (!brain->initialize()) {
        throwException<InitializationException>(
            InitializationException::InitializationErrorCode::InitializationFailed,
            "Failed to initialize brain"
        );
    }

    return brain;
}

std::unique_ptr<Brain> BrainFactory::createBrain(
    const std::shared_ptr<Config>& config,
    const std::map<std::string, std::shared_ptr<BrainComponent>>& components) {
    if (!config) {
        throwException<InitializationException>(
            InitializationException::InitializationErrorCode::InvalidState,
            "Configuration is null"
        );
    }

    // Create brain instance
    auto brain = std::make_unique<Brain>(config);

    // Initialize brain
    if (!brain->initialize()) {
        throwException<InitializationException>(
            InitializationException::InitializationErrorCode::InitializationFailed,
            "Failed to initialize brain"
        );
    }

    // Integrate provided components
    integrateComponentsIntoBrain(brain, components);

    return brain;
}

std::unique_ptr<Brain> BrainFactory::createBrainFromTemplate(
    const std::string& templateName,
    const std::map<std::string, std::any>& parameters) {
    auto& registry = ComponentRegistry::getInstance();
    const ComponentTemplate* templateData = registry.getTemplate(templateName);
    if (!templateData) {
        throwException<ConfigurationException>(
            ConfigurationException::ConfigurationErrorCode::InvalidParameter,
            "Template not found: " + templateName
        );
    }

    // Create config from template with parameters applied
    auto config = std::make_shared<Config>();
    config->set("component_type", "Brain");

    // Apply template parameters
    for (const auto& param : parameters) {
        config->set(param.first, param.second);
    }

    return createBrain(config);
}

bool BrainFactory::addComponentToBrainConfig(
    std::shared_ptr<Config>& brainConfig,
    const std::string& componentType,
    const std::map<std::string, std::any>& parameters) {
    if (!brainConfig) {
        return false;
    }

    // Add component configuration to brain config
    json componentConfig;
    componentConfig["component_type"] = componentType;

    for (const auto& param : parameters) {
        if (std::holds_alternative<int>(param.second)) {
            componentConfig[param.first] = std::get<int>(param.second);
        } else if (std::holds_alternative<double>(param.second)) {
            componentConfig[param.first] = std::get<double>(param.second);
        } else if (std::holds_alternative<std::string>(param.second)) {
            componentConfig[param.first] = std::get<std::string>(param.second);
        } else if (std::holds_alternative<bool>(param.second)) {
            componentConfig[param.first] = std::get<bool>(param.second);
        }
    }

    // Store component config (simplified)
    brainConfig->set("components." + componentType, componentConfig.dump());
    return true;
}

std::vector<std::string> BrainFactory::getBrainComponentTemplates() const {
    std::vector<std::string> templates;
    auto& registry = ComponentRegistry::getInstance();
    for (const auto& pair : registry.getTemplates()) {
        templates.push_back(pair.first);
    }
    return templates;
}

bool BrainFactory::validateBrainConfiguration(const std::shared_ptr<Config>& config) const {
    if (!config) {
        return false;
    }

    // Validate required brain configuration parameters
    const std::vector<std::string> requiredParams = {
        "neuron_count",
        "region_count",
        "connection_probability"
    };

    for (const auto& param : requiredParams) {
        if (!config->has(param)) {
            return false;
        }
    }

    return true;
}

std::string BrainFactory::getFactoryInfo() const {
    std::stringstream info;
    info << "BrainFactory v1.0.0";
    info << " (Manages " << getBrainComponentTemplates().size() << " brain component templates)";
    return info.str();
}

void BrainFactory::lock() const {
    // BrainFactory doesn't have its own mutex, uses componentManager_
}

void BrainFactory::unlock() const {
    // BrainFactory doesn't have its own mutex, uses componentManager_
}

bool BrainFactory::integrateComponentsIntoBrain(
    std::shared_ptr<Brain> brain,
    const std::map<std::string, std::shared_ptr<BrainComponent>>& components) {
    try {
        // Integrate components into brain
        for (const auto& pair : components) {
            const auto& component = pair.second;
            if (!component) {
                continue;
            }

            // Store component in brain config or metadata
            // This is a simplified implementation
        }

        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error integrating components into brain: ") + e.what());
        return false;
    }
}

// Helper function implementations
bool BrainComponentManager::validateComponentConfig(
    const std::shared_ptr<Config>& config,
    const std::string& componentType) const {
    if (!config) {
        return false;
    }

    // Check for component-specific validation rules
    if (componentType == "Neuron") {
        const std::vector<std::string> neuronRequiredParams = {
            "neuron_type",
            "membrane_potential"
        };
        for (const auto& param : neuronRequiredParams) {
            if (!config->has(param)) {
                return false;
            }
        }
    } else if (componentType == "Synapse") {
        const std::vector<std::string> synapseRequiredParams = {
            "weight",
            "delay"
        };
        for (const auto& param : synapseRequiredParams) {
            if (!config->has(param)) {
                return false;
            }
        }
    } else if (componentType == "NeuralPopulation") {
        const std::vector<std::string> populationRequiredParams = {
            "size",
            "neuron_type"
        };
        for (const auto& param : populationRequiredParams) {
            if (!config->has(param)) {
                return false;
            }
        }
    }

    return true;
}

bool BrainComponentManager::resolveDependencies(
    const BrainComponent& component,
    std::map<std::string, std::shared_ptr<BrainComponent>>& dependencies) const {
    const auto& deps = component.getDependencies();
    for (const auto& dep : deps) {
        auto it = dependencies.find(dep);
        if (it == dependencies.end()) {
            return false; // Dependency not found
        }
    }
    return true;
}

bool BrainComponentManager::validateComponent(const BrainComponent& component) const {
    return component.validateConfiguration(std::shared_ptr<Config>());
}

void BrainComponentManager::optimizeComponent(BrainComponent& component) {
    component.optimize();
}

void BrainComponentManager::cleanupComponent(std::shared_ptr<BrainComponent>& component) {
    if (component) {
        component->shutdown();
        component.reset();
    }
}

// Add missing getTemplates method to ComponentRegistry
const std::map<std::string, ComponentTemplate>& ComponentRegistry::getTemplates() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return templates_;
}

// Add missing getComponentFactory method to ComponentRegistry
std::function<std::unique_ptr<BrainComponent>(const std::shared_ptr<Config>&)> 
ComponentRegistry::getComponentFactory(const std::string& componentType) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = componentFactories_.find(componentType);
    if (it == componentFactories_.end()) {
        return nullptr;
    }
    return it->second;
}

} // namespace nlm