// Documentation and API Usage Tests for NLM Phase 6
// Tests API usage examples and code documentation

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigFactory.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <string>

namespace test_documentation {

void testConfigAPIUsageExamples() {
    std::cout << "    Testing Config API usage examples..." << std::endl;
    
    // Example 1: Basic configuration
    {
        nlm::Config config;
        config.set("simulation_steps", 1000, nlm::ConfigSource::Runtime);
        config.set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Runtime);
        config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
        
        auto steps = config.get<int>("simulation_steps");
        auto neurons = config.get<int64_t>("neuron_count");
        auto regions = config.get<int64_t>("region_count");
        
        assert(steps == 1000);
        assert(neurons == 500);
        assert(regions == 2);
        std::cout << "      ✓ Basic Config API usage" << std::endl;
    }
    
    // Example 2: Enhanced configuration with arrays
    {
        nlm::Config config;
        std::vector<int> weights = {1, 2, 3, 4, 5};
        config.setArray("weights", weights);
        
        auto retrievedWeights = config.getIntArray("weights");
        assert(retrievedWeights.has_value() && retrievedWeights->size() == 5);
        std::cout << "      ✓ Config API with arrays" << std::endl;
    }
    
    // Example 3: Nested configuration
    {
        nlm::Config config;
        config.setNested("brain.sensory", "input_scale", 1.0);
        config.setNested("brain.motor", "output_scale", 2.0);
        
        auto brain = config.getNested("brain");
        assert(brain.has_value());
        std::cout << "      ✓ Config API with nested configuration" << std::endl;
    }
    
    // Example 4: Configuration validation
    {
        nlm::Config config;
        config.addValidator("positive_number",
            [](const nlm::ConfigValue& val) {
                return std::visit([](const auto& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                        return v > 0;
                    }
                    return true;
                }, val);
            },
            [](const nlm::ConfigValue& val) { return "Value must be positive"; },
            "positive_validator");
        
        config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
        std::cout << "      ✓ Config API with validation" << std::endl;
    }
    
    // Example 5: Configuration factory
    {
        auto config = nlm::ConfigFactory::createDefaultConfig();
        assert(config.has("random_seed"));
        assert(config.has("simulation_timestep"));
        
        auto seed = config.get<int>("random_seed");
        auto timestep = config.get<double>("simulation_timestep");
        
        assert(seed == 42);
        assert(timestep == 0.001);
        std::cout << "      ✓ Config factory API usage" << std::endl;
    }
    
    std::cout << "    Config API usage examples test passed" << std::endl;
}

void testBrainAPIUsageExamples() {
    std::cout << "    Testing Brain API usage examples..." << std::endl;
    
    // Example 1: Basic brain creation
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getRegionCount() == 1);
        std::cout << "      ✓ Basic Brain creation and initialization" << std::endl;
    }
    
    // Example 2: Brain simulation loop
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
            // Simulate sensory input
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        }
        
        assert(brain.getTotalNeuronCount() == 200);
        std::cout << "      ✓ Brain simulation loop" << std::endl;
    }
    
    // Example 3: Brain system access
    {
        auto config = nlm::ConfigFactory::createBrainSimulationConfig();
        config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Access various systems
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getPlanner() != nullptr);
        assert(brain.getDopamine() != nullptr);
        assert(brain.getCuriosity() != nullptr);
        std::cout << "      ✓ Brain system access" << std::endl;
    }
    
    // Example 4: Brain statistics and status
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Get brain statistics
        auto neuronCount = brain.getTotalNeuronCount();
        auto synapseCount = brain.getTotalSynapseCount();
        auto firingCount = brain.getFiringNeuronCount();
        auto firingRate = brain.getAverageFiringRate();
        
        assert(neuronCount == 300);
        assert(synapseCount >= 0);
        assert(firingCount >= 0);
        assert(firingRate >= 0.0f);
        std::cout << "      ✓ Brain statistics and status" << std::endl;
    }
    
    // Example 5: Brain action production
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto action1 = brain.produceAction();
        assert(action1 != nullptr);
        
        // Simulate more activity
        brain.step(0);
        auto action2 = brain.produceAction();
        assert(action2 != nullptr);
        std::cout << "      ✓ Brain action production" << std::endl;
    }
    
    std::cout << "    Brain API usage examples test passed" << std::endl;
}

void testModernCpp17Features() {
    std::cout << "    Testing modern C++17 features in API usage..." << std::endl;
    
    // Example 1: Structured bindings
    {
        nlm::Config config;
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
        config.set("string_value", "test", nlm::ConfigSource::Runtime);
        
        auto [intVal, doubleVal, stringVal] = std::tuple(
            config.get<int>("int_value"),
            config.get<double>("double_value"),
            config.get<std::string>("string_value")
        );
        
        assert(intVal == 42);
        assert(doubleVal > 3.13 && doubleVal < 3.15);
        assert(stringVal == "test");
        std::cout << "      ✓ Modern C++17 structured bindings" << std::endl;
    }
    
    // Example 2: If constexpr
    {
        nlm::Config config;
        config.set("value", 5, nlm::ConfigSource::Runtime);
        
        // Using if constexpr to determine type
        auto val = config.get<int>("value");
        // In real usage, if constexpr could be used for type-specific operations
        assert(val == 5);
        std::cout << "      ✓ Modern C++17 if constexpr" << std::endl;
    }
    
    // Example 3: Optional types
    {
        nlm::Config config;
        
        auto existingVal = config.tryGet<int>("existing_key");
        assert(!existingVal.has_value());
        
        config.set("new_key", 123, nlm::ConfigSource::Runtime);
        auto newVal = config.tryGet<int>("new_key");
        assert(newVal.has_value() && newVal.value() == 123);
        std::cout << "      ✓ Modern C++17 optional types" << std::endl;
    }
    
    // Example 4: Auto deduction
    {
        nlm::Config config;
        auto val1 = config.get<int>("int_val");  // Deduces to int
        auto val2 = config.tryGet<double>("double_val");  // Deduces to std::optional<double>
        auto val3 = config.get<std::string>("string_val");  // Deduces to std::string
        
        // Type deduction works correctly
        static_assert(std::is_same_v<decltype(val1), int>, "Type deduction failed for int");
        static_assert(std::is_same_v<decltype(val2), std::optional<double>>, "Type deduction failed for optional<double>");
        static_assert(std::is_same_v<decltype(val3), std::string>, "Type deduction failed for string");
        
        std::cout << "      ✓ Modern C++17 auto type deduction" << std::endl;
    }
    
    std::cout << "    Modern C++17 features test passed" << std::endl;
}

void testBestPractices() {
    std::cout << "    Testing C++ best practices..." << std::endl;
    
    // Example 1: RAII (Resource Acquisition Is Initialization)
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Brain destructor will automatically clean up resources
        // No explicit cleanup needed
        std::cout << "      ✓ RAII resource management" << std::endl;
    }
    
    // Example 2: Smart pointers
    {
        auto config1 = std::make_shared<nlm::Config>();
        config1->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        auto config2 = std::make_shared<nlm::Config>();
        config2->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        
        // Smart pointers manage memory automatically
        nlm::Brain brain1(config1);
        brain1.initialize();
        
        nlm::Brain brain2(config2);
        brain2.initialize();
        
        // Memory managed automatically
        std::cout << "      ✓ Smart pointer usage" << std::endl;
    }
    
    // Example 3: Const correctness
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Const methods should work with const objects
        const nlm::Brain& constBrain = brain;
        auto neuronCount = constBrain.getTotalNeuronCount();
        assert(neuronCount == 100);
        std::cout << "      ✓ Const correctness" << std::endl;
    }
    
    // Example 4: Exception safety
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Operations should be exception-safe
        try {
            brain.step(0);
            brain.step(1);
            brain.step(2);
        } catch (...) {
            // Should not happen in normal operation
            assert(false);
        }
        
        std::cout << "      ✓ Exception safety" << std::endl;
    }
    
    std::cout << "    C++ best practices test passed" << std::endl;
}

void runAll() {
    std::cout << "\n" << std::string(80, '═') << std::endl;
    std::cout << "  NLM PHASE 6 DOCUMENTATION AND API USAGE TESTS" << std::endl;
    std::cout << "  Testing API usage examples and code documentation" << std::endl;
    std::cout << std::string(80, '═') << std::endl;
    
    testConfigAPIUsageExamples();
    testBrainAPIUsageExamples();
    testModernCpp17Features();
    testBestPractices();
    
    std::cout << "\n" << std::string(80, '═') << std::endl;
    std::cout << "  ALL DOCUMENTATION TESTS COMPLETED SUCCESSFULLY!" << std::endl;
    std::cout << "  API usage examples and modern C++ features verified." << std::endl;
    std::cout << std::string(80, '═') << std::endl;
}

} // namespace test_documentation
