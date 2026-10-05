// Factory Pattern Implementation for NLM C++ Framework
// 
// This document outlines the factory pattern implementation for creating
// NLM components with proper validation and error handling.

## Factory Pattern Overview

The NLM C++ framework implements a comprehensive factory pattern to:
- Encapsulate object creation logic
- Provide validation and error checking
- Enable dependency injection and loose coupling
- Support component pooling and reuse
- Ensure thread-safe object creation

## Factory Classes

### 1. Component Factory

```cpp
class ComponentFactory {
public:
    // Configuration factory
    static std::shared_ptr<Config> createDefaultConfig() {
        return std::make_shared<Config>();
    }
    
    static std::shared_ptr<Config> createConfig(const std::string& filename) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filename);
        return config;
    }
    
    // Brain factory with validation
    static std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        // Validate config before creating brain
        validateConfig(*config);
        
        return std::make_shared<Brain>(config);
    }
    
    // Brain factory with default config
    static std::shared_ptr<Brain> createBrain() {
        return createBrain(createDefaultConfig());
    }
    
    // World factory
    static std::shared_ptr<SimpleWorld> createWorld() {
        return std::make_shared<SimpleWorld>();
    }
    
    // World factory with configuration
    static std::shared_ptr<SimpleWorld> createWorld(
        size_t width, size_t height,
        size_t visionWidth, size_t visionHeight) {
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }
    
    // AgentBrain factory
    static std::shared_ptr<AgentBrain> createAgentBrain(
        std::shared_ptr<Brain> brain) {
        
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null");
        }
        
        return std::make_shared<AgentBrain>(brain);
    }
    
    // Neural component factories
    static std::shared_ptr<Neuron> createNeuron(
        const NeuronConfig& config) {
        validateNeuronConfig(config);
        return std::make_shared<Neuron>(config);
    }
    
    static std::shared_ptr<Synapse> createSynapse(
        std::shared_ptr<Neuron> pre_neuron,
        std::shared_ptr<Neuron> post_neuron,
        const SynapseConfig& config) {
        
        if (!pre_neuron || !post_neuron) {
            throw std::invalid_argument("Neurons cannot be null");
        }
        
        validateSynapseConfig(config);
        return std::make_shared<Synapse>(pre_neuron, post_neuron, config);
    }
    
    // Region factory
    static std::shared_ptr<NeuralRegion> createRegion(
        const std::string& name,
        const RegionConfig& config) {
        
        validateRegionConfig(config);
        return std::make_shared<NeuralRegion>(name, config);
    }
    
    // Population factory
    static std::shared_ptr<NeuralPopulation> createPopulation(
        const std::string& name,
        const PopulationConfig& config,
        std::shared_ptr<NeuralRegion> region) {
        
        if (!region) {
            throw std::invalid_argument("Region cannot be null");
        }
        
        validatePopulationConfig(config);
        return std::make_shared<NeuralPopulation>(name, config, region);
    }
    
    // Sensory input factories
    static std::shared_ptr<Vision> createVision(
        size_t width, size_t height, size_t channels = 3) {
        
        return std::make_shared<Vision>(width, height, channels);
    }
    
    static std::shared_ptr<Audio> createAudio(
        size_t sampleRate, size_t numSamples) {
        
        return std::make_shared<Audio>(sampleRate, numSamples);
    }
    
    static std::shared_ptr<InternalSignals> createInternalSignals() {
        return std::make_shared<InternalSignals>();
    }
    
    // Action factories
    static std::shared_ptr<Action> createAction(
        ActionType type,
        const std::vector<float>& parameters = {}) {
        
        return std::make_shared<Action>(type, parameters);
    }
    
    // World object factories
    static std::shared_ptr<WorldObject> createWorldObject(
        float x, float y,
        WorldObjectType type,
        float value = 0.0f,
        float radius = 0.5f) {
        
        return std::make_shared<WorldObject>(x, y, type, value, radius);
    }
    
    // Agent body factory
    static std::shared_ptr<AgentBody> createAgentBody() {
        return std::make_shared<AgentBody>();
    }
    
    // Action result factory
    static std::shared_ptr<ActionResult> createActionResult(
        float reward, bool success,
        const std::string& message = "") {
        
        return std::make_shared<ActionResult>(reward, success, message);
    }
    
    // Sensory percept factory
    static std::shared_ptr<SensoryPercept> createSensoryPercept() {
        return std::make_shared<SensoryPercept>();
    }
};
```

### 2. Neural Network Factory

```cpp
class NeuralNetworkFactory {
public:
    // Factory method to create a complete neural network
    static std::shared_ptr<Brain> createNeuralNetwork(
        const std::string& name,
        const NetworkConfig& config,
        const NeuralNetworkParameters& params) {
        
        // Validate parameters
        validateNetworkParameters(params);
        
        // Create configuration
        auto brain_config = createNetworkConfig(name, config, params);
        
        // Create brain
        auto brain = ComponentFactory::createBrain(brain_config);
        brain->initialize();
        
        // Create and connect regions
        auto regions = createRegions(brain, params);
        
        // Create and connect populations
        auto populations = createPopulations(regions, params);
        
        // Create and connect neurons
        auto neurons = createNeurons(populations, params);
        
        // Create and connect synapses
        auto synapses = createSynapses(neurons, params);
        
        // Configure connectivity
        configureConnectivity(synapses, params);
        
        return brain;
    }
    
    // Factory method to create a simple brain
    static std::shared_ptr<Brain> createSimpleBrain(
        size_t neuron_count,
        float connection_probability = 0.1f,
        NeuronType neuron_type = NeuronType::Excitatory) {
        
        NetworkConfig config;
        config.name = "SimpleBrain";
        config.neuron_count = neuron_count;
        config.connection_probability = connection_probability;
        config.neuron_type = neuron_type;
        
        NeuralNetworkParameters params;
        params.random_seed = std::random_device{}();
        
        return createNeuralNetwork(config.name, config, params);
    }
    
    // Factory method to create a recurrent network
    static std::shared_ptr<Brain> createRecurrentNetwork(
        size_t input_size,
        size_t hidden_size,
        size_t output_size,
        float feedback_strength = 0.1f) {
        
        NetworkConfig config;
        config.name = "RecurrentNetwork";
        config.neuron_count = input_size + hidden_size + output_size;
        config.connection_probability = 0.2f;
        config.recurrent_connections = true;
        
        NeuralNetworkParameters params;
        params.random_seed = std::random_device{}();
        params.recurrent_weight = feedback_strength;
        
        return createNeuralNetwork(config.name, config, params);
    }
    
private:
    // Helper methods for network creation
    static std::shared_ptr<Config> createNetworkConfig(
        const std::string& name,
        const NetworkConfig& config,
        const NeuralNetworkParameters& params) {
        
        auto brain_config = ComponentFactory::createDefaultConfig();
        
        // Set brain parameters
        brain_config->set("brain.name", name);
        brain_config->set("brain.neuron_count", static_cast<int64_t>(config.neuron_count));
        brain_config->set("brain.synapse_density", config.synapse_density);
        brain_config->set("brain.connection_probability", config.connection_probability);
        brain_config->set("brain.random_seed", static_cast<int64_t>(params.random_seed));
        
        // Set region parameters
        brain_config->set("region.count", static_cast<int64_t>(config.region_count));
        brain_config->set("region.recurrent_connections", config.recurrent_connections);
        
        // Set plasticity parameters
        brain_config->set("plasticity.stdp.enable", config.stdp_enabled);
        brain_config->set("plasticity.hebbian.enable", config.hebbian_enabled);
        brain_config->set("plasticity.structural.enable", config.structural_enabled);
        
        // Set neuromodulation parameters
        brain_config->set("neuromod.dopamine.scale", config.dopamine_scale);
        brain_config->set("neuromod.curiosity.enable", config.curiosity_enabled);
        
        return brain_config;
    }
    
    static std::vector<std::shared_ptr<NeuralRegion>> createRegions(
        std::shared_ptr<Brain> brain,
        const NeuralNetworkParameters& params) {
        
        std::vector<std::shared_ptr<NeuralRegion>> regions;
        
        // Create input region
        RegionConfig input_region_config;
        input_region_config.name = "InputRegion";
        input_region_config.region_type = RegionType::Input;
        
        auto input_region = ComponentFactory::createRegion(
            input_region_config.name, input_region_config);
        
        // Create hidden region
        RegionConfig hidden_region_config;
        hidden_region_config.name = "HiddenRegion";
        hidden_region_config.region_type = RegionType::Hidden;
        
        auto hidden_region = ComponentFactory::createRegion(
            hidden_region_config.name, hidden_region_config);
        
        // Create output region
        RegionConfig output_region_config;
        output_region_config.name = "OutputRegion";
        output_region_config.region_type = RegionType::Output;
        
        auto output_region = ComponentFactory::createRegion(
            output_region_config.name, output_region_config);
        
        // Add regions to brain
        brain->addRegion(input_region_config.name);
        brain->addRegion(hidden_region_config.name);
        brain->addRegion(output_region_config.name);
        
        regions = {input_region, hidden_region, output_region};
        return regions;
    }
    
    static std::vector<std::shared_ptr<NeuralPopulation>> createPopulations(
        const std::vector<std::shared_ptr<NeuralRegion>>& regions,
        const NeuralNetworkParameters& params) {
        
        std::vector<std::shared_ptr<NeuralPopulation>> populations;
        
        // Create input population
        PopulationConfig input_pop_config;
        input_pop_config.name = "InputPopulation";
        input_pop_config.population_type = PopulationType::Input;
        input_pop_config.neuron_count = params.input_size;
        
        auto input_pop = ComponentFactory::createPopulation(
            input_pop_config.name, input_pop_config, regions[0]);
        
        // Create hidden population
        PopulationConfig hidden_pop_config;
        hidden_pop_config.name = "HiddenPopulation";
        hidden_pop_config.population_type = PopulationType::Hidden;
        hidden_pop_config.neuron_count = params.hidden_size;
        
        auto hidden_pop = ComponentFactory::createPopulation(
            hidden_pop_config.name, hidden_pop_config, regions[1]);
        
        // Create output population
        PopulationConfig output_pop_config;
        output_pop_config.name = "OutputPopulation";
        output_pop_config.population_type = PopulationType::Output;
        output_pop_config.neuron_count = params.output_size;
        
        auto output_pop = ComponentFactory::createPopulation(
            output_pop_config.name, output_pop_config, regions[2]);
        
        populations = {input_pop, hidden_pop, output_pop};
        return populations;
    }
    
    static std::vector<std::vector<std::shared_ptr<Neuron>>> createNeurons(
        const std::vector<std::shared_ptr<NeuralPopulation>>& populations,
        const NeuralNetworkParameters& params) {
        
        std::vector<std::vector<std::shared_ptr<Neuron>>> neurons;
        
        for (auto& population : populations) {
            std::vector<std::shared_ptr<Neuron>> population_neurons;
            
            for (size_t i = 0; i < population->getNeuronCount(); ++i) {
                NeuronConfig neuron_config;
                neuron_config.type = params.neuron_type;
                neuron_config.excitability = 1.0f;
                neuron_config.threshold = -50.0f;
                neuron_config.rest_potential = -70.0f;
                
                auto neuron = ComponentFactory::createNeuron(neuron_config);
                population_neurons.push_back(neuron);
            }
            
            neurons.push_back(population_neurons);
        }
        
        return neurons;
    }
    
    static std::vector<std::vector<std::vector<std::shared_ptr<Synapse>>>> createSynapses(
        const std::vector<std::vector<std::shared_ptr<Neuron>>>& neurons,
        const NeuralNetworkParameters& params) {
        
        std::vector<std::vector<std::vector<std::shared_ptr<Synapse>>>> synapses;
        
        // Create synapses between populations
        for (size_t pre_idx = 0; pre_idx < neurons.size(); ++pre_idx) {
            std::vector<std::vector<std::shared_ptr<Synapse>>> pre_synapses;
            
            for (size_t post_idx = 0; post_idx < neurons.size(); ++post_idx) {
                std::vector<std::shared_ptr<Synapse>> post_synapses;
                
                // Create connections based on probability
                for (auto& pre_neuron : neurons[pre_idx]) {
                    for (auto& post_neuron : neurons[post_idx]) {
                        if (std::generate_canonical<double, 10>(rng_) < params.connection_probability) {
                            // Skip self-connections for recurrent networks
                            if (!params.recurrent_connections && pre_idx == post_idx && pre_neuron == post_neuron) {
                                continue;
                            }
                            
                            SynapseConfig synapse_config;
                            synapse_config.weight = std::normal_distribution<float>(1.0f, 0.1f)(rng_);
                            synapse_config.delay = 1;
                            synapse_config.type = SynapseType::Excitatory;
                            
                            auto synapse = ComponentFactory::createSynapse(
                                pre_neuron, post_neuron, synapse_config);
                            
                            post_synapses.push_back(synapse);
                        }
                    }
                }
                
                pre_synapses.push_back(post_synapses);
            }
            
            synapses.push_back(pre_synapses);
        }
        
        return synapses;
    }
    
    static void configureConnectivity(
        const std::vector<std::vector<std::vector<std::shared_ptr<Synapse>>>>& synapses,
        const NeuralNetworkParameters& params) {
        
        // Configure recurrent connections
        if (params.recurrent_connections) {
            for (size_t i = 0; i < synapses.size(); ++i) {
                for (size_t j = 0; j < synapses[i].size(); ++j) {
                    for (auto& synapse : synapses[i][j]) {
                        if (i == j) {  // Recurrent connection
                            synapse->setWeight(synapse->getWeight() * params.recurrent_weight);
                        }
                    }
                }
            }
        }
        
        // Set initial synaptic weights based on connection probability
        for (size_t i = 0; i < synapses.size(); ++i) {
            for (size_t j = 0; j < synapses[i].size(); ++j) {
                float total_weight = 0.0f;
                size_t connection_count = 0;
                
                for (auto& synapse : synapses[i][j]) {
                    total_weight += synapse->getWeight();
                    connection_count++;
                }
                
                if (connection_count > 0 && params.normalize_weights) {
                    float normalization_factor = 1.0f / static_cast<float>(connection_count);
                    for (auto& synapse : synapses[i][j]) {
                        synapse->setWeight(synapse->getWeight() * normalization_factor);
                    }
                }
            }
        }
    }
    
    // Validation methods
    static void validateConfig(const Config& config) {
        // Validate configuration parameters
        if (config.has("brain.neuron_count")) {
            int64_t neuron_count = config.getInt("brain.neuron_count");
            if (neuron_count <= 0) {
                throw std::invalid_argument("Neuron count must be positive");
            }
        }
        
        if (config.has("brain.synapse_density")) {
            float density = config.getFloat("brain.synapse_density");
            if (density < 0.0f || density > 1.0f) {
                throw std::invalid_argument("Synapse density must be between 0 and 1");
            }
        }
    }
    
    static void validateNeuronConfig(const NeuronConfig& config) {
        // Validate neuron configuration
        if (config.type == NeuronType::Invalid) {
            throw std::invalid_argument("Invalid neuron type");
        }
    }
    
    static void validateSynapseConfig(const SynapseConfig& config) {
        // Validate synapse configuration
        if (config.type == SynapseType::Invalid) {
            throw std::invalid_argument("Invalid synapse type");
        }
        
        if (config.weight < 0.0f) {
            throw std::invalid_argument("Synaptic weight cannot be negative");
        }
    }
    
    static void validateRegionConfig(const RegionConfig& config) {
        // Validate region configuration
        if (config.region_type == RegionType::Invalid) {
            throw std::invalid_argument("Invalid region type");
        }
    }
    
    static void validatePopulationConfig(const PopulationConfig& config) {
        // Validate population configuration
        if (config.population_type == PopulationType::Invalid) {
            throw std::invalid_argument("Invalid population type");
        }
        
        if (config.neuron_count <= 0) {
            throw std::invalid_argument("Neuron count must be positive");
        }
    }
    
    static void validateNetworkParameters(
        const NeuralNetworkParameters& params) {
        
        if (params.input_size <= 0) {
            throw std::invalid_argument("Input size must be positive");
        }
        
        if (params.hidden_size <= 0) {
            throw std::invalid_argument("Hidden size must be positive");
        }
        
        if (params.output_size <= 0) {
            throw std::invalid_argument("Output size must be positive");
        }
        
        if (params.connection_probability < 0.0f || params.connection_probability > 1.0f) {
            throw std::invalid_argument("Connection probability must be between 0 and 1");
        }
        
        if (params.recurrent_weight < 0.0f || params.recurrent_weight > 1.0f) {
            throw std::invalid_argument("Recurrent weight must be between 0 and 1");
        }
    }
    
    static std::mt19937 rng_;
};
```

### 3. Component Pooling Factory

```cpp
class ComponentPoolFactory {
public:
    // Neuron pool management
    class NeuronPool {
    public:
        std::shared_ptr<Neuron> getNeuron(const NeuronConfig& config) {
            if (!neuron_pool_.empty()) {
                auto neuron = neuron_pool_.front();
                neuron_pool_.pop_front();
                neuron->reset(config);
                return neuron;
            }
            return ComponentFactory::createNeuron(config);
        }
        
        void returnNeuron(std::shared_ptr<Neuron> neuron) {
            if (neuron && neuron->isAvailable()) {
                neuron_pool_.push_back(neuron);
            }
        }
        
        void clear() {
            neuron_pool_.clear();
        }
        
    private:
        std::list<std::shared_ptr<Neuron>> neuron_pool_;
    };
    
    // Synapse pool management
    class SynapsePool {
    public:
        std::shared_ptr<Synapse> getSynapse(
            std::shared_ptr<Neuron> pre_neuron,
            std::shared_ptr<Neuron> post_neuron,
            const SynapseConfig& config) {
            
            // Find available synapse or create new one
            for (auto it = synapse_pool_.begin(); it != synapse_pool_.end(); ++it) {
                if ((*it)->isAvailable() &&
                    (*it)->getPreNeuron() == pre_neuron &&
                    (*it)->getPostNeuron() == post_neuron) {
                    
                    (*it)->reset(config);
                    auto synapse = *it;
                    synapse_pool_.erase(it);
                    return synapse;
                }
            }
            
            return ComponentFactory::createSynapse(pre_neuron, post_neuron, config);
        }
        
        void returnSynapse(std::shared_ptr<Synapse> synapse) {
            if (synapse && synapse->isAvailable()) {
                synapse_pool_.push_back(synapse);
            }
        }
        
        void clear() {
            synapse_pool_.clear();
        }
        
    private:
        std::list<std::shared_ptr<Synapse>> synapse_pool_;
    };
    
    // Factory methods for pooled components
    static std::shared_ptr<Neuron> createNeuron(
        const NeuronConfig& config,
        NeuronPool& pool) {
        
        auto neuron = pool.getNeuron(config);
        return neuron;
    }
    
    static std::shared_ptr<Synapse> createSynapse(
        std::shared_ptr<Neuron> pre_neuron,
        std::shared_ptr<Neuron> post_neuron,
        const SynapseConfig& config,
        SynapsePool& pool) {
        
        auto synapse = pool.getSynapse(pre_neuron, post_neuron, config);
        return synapse;
    }
    
    // Pool management
    static void cleanupPools() {
        // Request cleanup of all pools
        // Implementation depends on specific pool management
    }
    
    static void clearAllPools() {
        // Clear all pools
        // Implementation depends on specific pool management
    }
};
```

### 4. Advanced Factory with Dependency Injection

```cpp
class AdvancedFactory {
public:
    struct FactoryDependencies {
        std::shared_ptr<Config> config;
        std::shared_ptr<RandomNumberGenerator> rng;
        std::shared_ptr<EventSystem> event_system;
        std::shared_ptr<ComponentPoolFactory> pool_factory;
        std::shared_ptr<ValidationSystem> validation;
    };
    
    // Set global dependencies
    static void setDependencies(const FactoryDependencies& dependencies) {
        global_dependencies_ = dependencies;
    }
    
    // Create brain with dependency injection
    static std::shared_ptr<Brain> createBrain(
        const std::string& name,
        const NetworkConfig& config) {
        
        // Validate dependencies
        if (!global_dependencies_.config) {
            throw std::runtime_error("Config dependency not set");
        }
        
        // Create brain with injected dependencies
        auto brain = std::make_shared<Brain>(
            global_dependencies_.config,
            global_dependencies_.rng,
            global_dependencies_.event_system,
            global_dependencies_.pool_factory,
            global_dependencies_.validation
        );
        
        brain->initialize();
        return brain;
    }
    
    // Create component with dependency injection
    template<typename TComponent, typename... TArgs>
    static std::shared_ptr<TComponent> createComponent(
        TArgs&&... args) {
        
        // Validate component parameters
        if (!global_dependencies_.validation) {
            throw std::runtime_error("Validation dependency not set");
        }
        
        // Create component with injected dependencies
        return std::make_shared<TComponent>(
            std::forward<TArgs>(args)...,
            global_dependencies_)
    }
    
private:
    static FactoryDependencies global_dependencies_;
};
```

## Factory Pattern Benefits

### 1. Loose Coupling
- Components depend on abstractions, not concrete implementations
- Easy to swap implementations
- Better testability

### 2. Single Responsibility
- Each factory has a single responsibility
- Easier to maintain and extend
- Clear separation of concerns

### 3. Encapsulation
- Object creation logic is encapsulated
- Changes to creation logic don't affect client code
- Better control over object lifecycle

### 4. Extensibility
- Easy to add new factories
- Easy to modify existing factories
- Support for pluggable architectures

### 5. Validation and Error Handling
- Centralized validation logic
- Consistent error handling
- Better debugging support

## Factory Pattern Implementation

### Basic Usage

```cpp
// Basic usage with ComponentFactory
#include "ComponentFactory.hpp"

int main() {
    // Create configuration
    auto config = ComponentFactory::createDefaultConfig();
    config->set("brain.neuron_count", 100);
    
    // Create brain
    auto brain = ComponentFactory::createBrain(config);
    brain->initialize();
    
    // Create world
    auto world = ComponentFactory::createWorld(20, 20, 8, 8);
    
    // Create agent
    auto agent = ComponentFactory::createAgentBrain(brain);
    agent->initialize(world);
    
    // Run simulation
    for (int i = 0; i < 100; ++i) {
        world->update(0.1f);
        agent->processSensoryInput(world->getSensoryPercept());
        brain->step(i);
        world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
    }
    
    return 0;
}
```

### Advanced Usage

```cpp
// Advanced usage with NeuralNetworkFactory
#include "NeuralNetworkFactory.hpp"

int main() {
    // Create a recurrent network
    auto brain = NeuralNetworkFactory::createRecurrentNetwork(
        10,  // Input size
        20,  // Hidden size
        5,   // Output size
        0.5f // Recurrent weight
    );
    
    // Create a simple brain
    auto simple_brain = NeuralNetworkFactory::createSimpleBrain(
        100,     // Neuron count
        0.1f,     // Connection probability
        NeuronType::Excitatory  // Neuron type
    );
    
    return 0;
}
```

### Component Pooling

```cpp
// Usage with component pooling
#include "ComponentPoolFactory.hpp"

int main() {
    ComponentPoolFactory::NeuronPool neuron_pool;
    ComponentPoolFactory::SynapsePool synapse_pool;
    
    // Create neurons using pool
    auto neuron1 = ComponentPoolFactory::createNeuron(
        NeuronConfig{NeuronType::Excitatory},
        neuron_pool
    );
    
    auto neuron2 = ComponentPoolFactory::createNeuron(
        NeuronConfig{NeuronType::Inhibitory},
        neuron_pool
    );
    
    // Create synapses using pool
    auto synapse = ComponentPoolFactory::createSynapse(
        neuron1, neuron2,
        SynapseConfig{SynapseType::Excitatory, 1.0f},
        synapse_pool
    );
    
    return 0;
}
```

## Conclusion

The factory pattern implementation in NLM provides:

1. **Centralized Object Creation**: All object creation is centralized in factory classes
2. **Validation and Error Handling**: Consistent validation and error handling across all components
3. **Memory Management**: Efficient memory management through pooling and smart pointers
4. **Extensibility**: Easy to extend with new components and factories
5. **Testability**: Better testability through dependency injection and loose coupling
6. **Performance**: Improved performance through component pooling and optimized allocation

This factory pattern implementation significantly improves the code quality, maintainability, and performance of the NLM framework.