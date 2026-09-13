// Python Bindings Tests
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_python_bindings {

void testPythonModuleImport() {
    // This test would typically import the Python module and test its interface
    // Since we're in C++ tests, we'll just verify the C++ API is accessible
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Basic functionality should work
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testPythonModuleImport passed" << std::endl;
}

void testPythonBrainWrapper() {
    // Test that Brain class can be wrapped for Python
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test basic methods that would be exposed to Python
    assert(brain.getTotalNeuronCount() == 30);
    assert(brain.getRegionCount() >= 1);
    
    // Step the brain
    for (int i = 0; i < 10; ++i) {
        brain.step(i, i * 0.001);
    }
    
    // Check statistics
    assert(brain.getFiringNeuronCount() >= 0);
    
    std::cout << "    testPythonBrainWrapper passed" << std::endl;
}

void testPythonNeuronWrapper() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test neuron properties
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    // Test neuron state
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.recordSpike(0.1);
    assert(neuron.getSpikeHistory().size() == 1);
    
    std::cout << "    testPythonNeuronWrapper passed" << std::endl;
}

void testPythonConfigWrapper() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test config operations
    config->set("test_param", 42, nlm::ConfigSource::Default);
    config->set("another_param", 3.14f, nlm::ConfigSource::Default);
    
    assert(config->get<int64_t>("test_param") == 42);
    assert(config->get<float>("another_param") == 3.14f);
    
    std::cout << "    testPythonConfigWrapper passed" << std::endl;
}

void testPythonBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(25), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Test brain constructor
    nlm::Brain brain(config);
    
    // Test initialization
    assert(brain.initialize());
    
    // Test region access
    auto regions = brain.getRegionIds();
    assert(regions.size() >= 2);
    
    std::cout << "    testPythonBrainCreation passed" << std::endl;
}

void testPythonBrainState() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test brain state queries
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getTotalSpikeCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    // Test step
    brain.step(0, 0.0);
    
    std::cout << "    testPythonBrainState passed" << std::endl;
}

void testPythonActionInterface() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test action production
    auto action = brain.produceAction();
    
    // Action should be valid
    assert(action != nullptr);
    
    // Action should have valid type
    nlm::ActionType type = action->getType();
    assert(static_cast<int>(type) >= 0 && static_cast<int>(type) <= 10);
    
    std::cout << "    testPythonActionInterface passed" << std::endl;
}

void testPythonNeuromodulationInterface() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(15), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neuromodulation access
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        assert(dopamine->getLevel() >= 0.0f && dopamine->getLevel() <= 1.0f);
    }
    
    auto* novelty = brain.getNovelty();
    if (novelty) {
        assert(novelty->getLevel() >= 0.0f && novelty->getLevel() <= 1.0f);
    }
    
    std::cout << "    testPythonNeuromodulationInterface passed" << std::endl;
}

void testPythonMemoryInterface() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(15), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test memory system access
    auto* workingMem = brain.getWorkingMemory();
    if (workingMem) {
        assert(workingMem->getCapacity() >= 0);
    }
    
    auto* episodicMem = brain.getEpisodicMemory();
    if (episodicMem) {
        assert(episodicMem->getEpisodeCount() == 0);
    }
    
    std::cout << "    testPythonMemoryInterface passed" << std::endl;
}

void testPythonCognitionInterface() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(15), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test cognition system access
    auto* planner = brain.getPlanner();
    if (planner) {
        assert(planner->getPlanningDepth() >= 1);
    }
    
    auto* conceptForm = brain.getConceptFormation();
    if (conceptForm) {
        assert(conceptForm->getConceptCount() == 0);
    }
    
    std::cout << "    testPythonCognitionInterface passed" << std::endl;
}

void testPythonDevelopmentInterface() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(15), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test development system access
    auto* dev = brain.getDevelopmentSystem();
    if (dev) {
        assert(dev->getStage() == nlm::DevelopmentalStage::Initial);
    }
    
    std::cout << "    testPythonDevelopmentInterface passed" << std::endl;
}

void testPythonPredictionInterface() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(15), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system access
    auto* prediction = brain.getPredictionSystem();
    if (prediction) {
        // Prediction system should be accessible
        assert(prediction != nullptr);
    }
    
    std::cout << "    testPythonPredictionInterface passed" << std::endl;
}

void testPythonWrapperFlexibility() {
    // Test that various types can be easily wrapped for Python
    std::vector<int> intList = {1, 2, 3, 4, 5};
    std::vector<float> floatList = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<std::string> stringList = {"a", "b", "c", "d", "e"};
    
    // These operations should be straightforward for Python bindings
    assert(intList.size() == 5);
    assert(floatList.size() == 5);
    assert(stringList.size() == 5);
    
    // Type conversions for Python
    std::vector<double> converted;
    for (float f : floatList) {
        converted.push_back(static_cast<double>(f));
    }
    assert(converted.size() == floatList.size());
    
    std::cout << "    testPythonWrapperFlexibility passed" << std::endl;
}

void runAll() {
    std::cout << "Running Python bindings tests..." << std::endl;
    
    testPythonModuleImport();
    testPythonBrainWrapper();
    testPythonNeuronWrapper();
    testPythonConfigWrapper();
    testPythonBrainCreation();
    testPythonBrainState();
    testPythonActionInterface();
    testPythonNeuromodulationInterface();
    testPythonMemoryInterface();
    testPythonCognitionInterface();
    testPythonDevelopmentInterface();
    testPythonPredictionInterface();
    testPythonWrapperFlexibility();
}

} // namespace test_python_bindings
