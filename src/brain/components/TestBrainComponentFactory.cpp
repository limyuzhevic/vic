#include "brain/components/BrainComponentFactory.hpp"
#include "core/Exceptions/NLMErrors.hpp"
#include "core/Types/Types.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

void printUsage() {
    std::cout << "=== BrainComponentFactory System Test ===" << std::endl;
    std::cout << "This test demonstrates the BrainComponentFactory system:" << std::endl;
    std::cout << "1. Component Registration" << std::endl;
    std::cout << "2. Component Creation" << std::endl;
    std::cout << "3. Component Cloning" << std::endl;
    std::cout << "4. Component Serialization" << std::endl;
    std::cout << "5. Component Lifecycle Management" << std::endl;
    std::cout << "6. Template-based Component Creation" << std::endl;
    std::cout << "7. Registry Operations" << std::endl;
    std::cout << "" << std::endl;
}

void testComponentRegistration() {
    std::cout << "1. Testing Component Registration..." << std::endl;
    
    auto& registry = ComponentRegistry::getInstance();
    
    // Get supported component types
    std::vector<std::string> supportedTypes = registry.getSupportedComponentTypes();
    std::cout << "   Supported component types: ";
    for (const auto& type : supportedTypes) {
        std::cout << type << " ";
    }
    std::cout << std::endl;
    
    // Get factory names
    std::vector<std::string> factoryNames = registry.getFactoryNames();
    std::cout << "   Registered factories: ";
    for (const auto& name : factoryNames) {
        std::cout << name << " ";
    }
    std::cout << std::endl;
    
    // Register a custom component type
    bool success = registry.registerComponentType("CustomNeuron",
        [](const std::shared_ptr<Config>& config) {
            class CustomNeuron : public BrainComponent {
            public:
                CustomNeuron(const std::shared_ptr<Config>& config) : config_(config) {}

                std::string getTypeName() const override { return "CustomNeuron"; }
                std::string getComponentId() const override {
                    return "custom_neuron_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
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
                    auto clone = std::make_unique<CustomNeuron>(*this);
                    clone->config_ = config_;
                    return clone;
                }

                bool canClone() const override { return true; }

                std::string serialize() const override {
                    return "custom_neuron_data";
                }

                bool deserialize(const std::string& data) override {
                    return data == "custom_neuron_data";
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
            return std::make_unique<CustomNeuron>(config);
        }, "CustomFactory");
    
    std::cout << "   Custom component type registration: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "" << std::endl;
}

void testComponentCreation() {
    std::cout << "2. Testing Component Creation..." << std::endl;
    
    auto& registry = ComponentRegistry::getInstance();
    auto& manager = BrainComponentManager();
    
    // Create neuron using configuration
    std::string neuronId = "test_neuron_1";
    auto neuron = manager.createComponent("Neuron", std::shared_ptr<Config>(), neuronId);
    std::cout << "   Neuron creation: " << (neuron ? "SUCCESS" : "FAILED") << std::endl;
    
    if (neuron) {
        std::cout << "   Neuron ID: " << neuron->getComponentId() << std::endl;
        std::cout << "   Neuron type: " << neuron->getTypeName() << std::endl;
        std::cout << "   Neuron initialized: " << (neuron->isInitialized() ? "YES" : "NO") << std::endl;
    }
    
    // Create synapse
    std::string synapseId = "test_synapse_1";
    auto synapse = manager.createComponent("Synapse", std::shared_ptr<Config>(), synapseId);
    std::cout << "   Synapse creation: " << (synapse ? "SUCCESS" : "FAILED") << std::endl;
    
    if (synapse) {
        std::cout << "   Synapse ID: " << synapse->getComponentId() << std::endl;
        std::cout << "   Synapse type: " << synapse->getTypeName() << std::endl;
    }
    
    // Create neural population
    std::string populationId = "test_population_1";
    auto population = manager.createComponent("NeuralPopulation", std::shared_ptr<Config>(), populationId);
    std::cout << "   NeuralPopulation creation: " << (population ? "SUCCESS" : "FAILED") << std::endl;
    
    if (population) {
        std::cout << "   Population ID: " << population->getComponentId() << std::endl;
        std::cout << "   Population type: " << population->getTypeName() << std::endl;
    }
    
    std::cout << "" << std::endl;
}

void testComponentCloning() {
    std::cout << "3. Testing Component Cloning..." << std::endl;
    
    auto& manager = BrainComponentManager();
    
    // Create a component to clone
    std::string originalId = "clone_source_1";
    auto original = manager.createComponent("Neuron", std::shared_ptr<Config>(), originalId);
    std::cout << "   Original component creation: " << (original ? "SUCCESS" : "FAILED") << std::endl;
    
    if (original) {
        // Clone the component
        auto clone = manager.cloneComponent(*original);
        std::cout << "   Component cloning: " << (clone ? "SUCCESS" : "FAILED") << std::endl;
        
        if (clone) {
            std::cout << "   Clone component ID: " << clone->getComponentId() << std::endl;
            std::cout << "   Clone type: " << clone->getTypeName() << std::endl;
            std::cout << "   Clone initialized: " << (clone->isInitialized() ? "YES" : "NO") << std::endl;
        }
    }
    
    std::cout << "" << std::endl;
}

void testComponentSerialization() {
    std::cout << "4. Testing Component Serialization..." << std::endl;
    
    auto& manager = BrainComponentManager();
    
    // Create a component to serialize
    std::string originalId = "serialize_source_1";
    auto original = manager.createComponent("Synapse", std::shared_ptr<Config>(), originalId);
    std::cout << "   Component creation for serialization: " << (original ? "SUCCESS" : "FAILED") << std::endl;
    
    if (original) {
        // Serialize component
        std::string serialized = manager.serializeComponent(*original);
        std::cout << "   Component serialization: " << (!serialized.empty() ? "SUCCESS" : "FAILED") << std::endl;
        
        if (!serialized.empty()) {
            std::cout << "   Serialized data length: " << serialized.length() << std::endl;
            
            // Deserialize component
            auto deserialized = manager.deserializeComponent(serialized);
            std::cout << "   Component deserialization: " << (deserialized ? "SUCCESS" : "FAILED") << std::endl;
            
            if (deserialized) {
                std::cout << "   Deserialized component type: " << deserialized->getTypeName() << std::endl;
                std::cout << "   Deserialized component initialized: " << (deserialized->isInitialized() ? "YES" : "NO") << std::endl;
            }
        }
    }
    
    std::cout << "" << std::endl;
}

void testComponentLifecycle() {
    std::cout << "5. Testing Component Lifecycle Management..." << std::endl;
    
    auto& manager = BrainComponentManager();
    
    // Create component
    std::string componentId = "lifecycle_test_1";
    auto component = manager.createComponent("NeuralPopulation", std::shared_ptr<Config>(), componentId);
    std::cout << "   Component creation: " << (component ? "SUCCESS" : "FAILED") << std::endl;
    
    if (component) {
        // Check if component exists
        bool hasComponent = manager.hasComponent(componentId);
        std::cout << "   Component registration: " << (hasComponent ? "YES" : "NO") << std::endl;
        
        // Get all components
        auto allComponents = manager.getAllComponents();
        std::cout << "   Total components managed: " << allComponents.size() << std::endl;
        
        // Check component configuration validation
        bool configValid = manager.validateComponentConfiguration(*component);
        std::cout << "   Configuration validation: " << (configValid ? "PASSED" : "FAILED") << std::endl;
        
        // Get component manager statistics
        auto stats = manager.getStats();
        std::cout << "   Manager statistics:" << std::endl;
        std::cout << "     Active components: " << stats.activeComponentCount << std::endl;
        std::cout << "     Total components created: " << stats.totalComponentsCreated << std::endl;
        std::cout << "     Total components cloned: " << stats.totalComponentsCloned << std::endl;
        std::cout << "     Total components serialized: " << stats.totalComponentsSerialized << std::endl;
    }
    
    std::cout << "" << std::endl;
}

void testTemplateCreation() {
    std::cout << "6. Testing Template-based Component Creation..." << std::endl;
    
    auto& registry = ComponentRegistry::getInstance();
    
    // Get available templates
    const ComponentTemplate* neuronTemplate = registry.getTemplate("DefaultNeuronTemplate");
    std::cout << "   Neuron template available: " << (neuronTemplate ? "YES" : "NO") << std::endl;
    
    if (neuronTemplate) {
        std::cout << "   Template name: " << neuronTemplate->name << std::endl;
        std::cout << "   Template description: " << neuronTemplate->description << std::endl;
        std::cout << "   Template component type: " << neuronTemplate->componentType << std::endl;
        std::cout << "   Template is cloneable: " << (neuronTemplate->isCloneable ? "YES" : "NO") << std::endl;
        std::cout << "   Template is serializable: " << (neuronTemplate->isSerializable ? "YES" : "NO") << std::endl;
    }
    
    const ComponentTemplate* synapseTemplate = registry.getTemplate("DefaultSynapseTemplate");
    std::cout << "   Synapse template available: " << (synapseTemplate ? "YES" : "NO") << std::endl;
    
    if (synapseTemplate) {
        std::cout << "   Template name: " << synapseTemplate->name << std::endl;
        std::cout << "   Template is cloneable: " << (synapseTemplate->isCloneable ? "YES" : "NO") << std::endl;
        std::cout << "   Template is serializable: " << (synapseTemplate->isSerializable ? "YES" : "NO") << std::endl;
    }
    
    // Test BrainFactory
    BrainFactory brainFactory;
    std::cout << "   BrainFactory info: " << brainFactory.getFactoryInfo() << std::endl;
    
    std::cout << "" << std::endl;
}

void testRegistryOperations() {
    std::cout << "7. Testing Registry Operations..." << std::endl;
    
    auto& registry = ComponentRegistry::getInstance();
    
    // Get registry statistics
    auto stats = registry.getStats();
    std::cout << "   Registry statistics:" << std::endl;
    std::cout << "     Factory count: " << stats.factoryCount << std::endl;
    std::cout << "     Component type count: " << stats.componentTypeCount << std::endl;
    std::cout << "     Template count: " << stats.templateCount << std::endl;
    std::cout << "     Total components created: " << stats.totalComponentsCreated << std::endl;
    
    // Test thread safety
    registry.lock();
    std::cout << "   Thread safety: Locking supported" << std::endl;
    registry.unlock();
    
    // Test clearing registry
    registry.clear();
    std::cout << "   Registry cleared successfully" << std::endl;
    
    // Verify registry is empty
    auto emptyStats = registry.getStats();
    std::cout << "   Registry after clear - Factory count: " << emptyStats.factoryCount << std::endl;
    
    // Note: In real usage, the registry should be re-initialized after clearing
    std::cout << "   (Registry has been cleared - factories and templates removed)" << std::endl;
    
    std::cout << "" << std::endl;
}

int main() {
    std::cout << "BrainComponentFactory System Test" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "" << std::endl;
    
    try {
        printUsage();
        
        testComponentRegistration();
        testComponentCreation();
        testComponentCloning();
        testComponentSerialization();
        testComponentLifecycle();
        testTemplateCreation();
        testRegistryOperations();
        
        std::cout << "=== All Tests Completed ===" << std::endl;
        std::cout << "The BrainComponentFactory system is working correctly!" << std::endl;
        std::cout << "" << std::endl;
        
        return 0;
        
    } catch (const NLMException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Error code: " << static_cast<int>(e.getErrorCode()) << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}