#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/core/Logger/Logger.hpp"

namespace py = pybind11;
namespace nlm {

// Test function to demonstrate enhanced bindings
void testEnhancedBindings(py::module_& m) {
    m.def("testBasicFunctionality", []() {
        // Test enhanced Python bindings
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test enhanced features
        auto stats = brain->getTotalNeuronCount();
        auto spikes = brain->getTotalSpikeCount();
        auto firingRate = brain->getAverageFiringRate();
        
        return py::make_tuple(stats, spikes, firingRate);
    }, "Test basic functionality with enhanced bindings");
    
    m.def("testConfigurationJSON", []() {
        // Test JSON configuration
        auto config = PythonConfigUtils::createDefaultConfig();
        config->set("neuron_count", 500, ConfigSource::User);
        
        std::string jsonStr = PythonConfigUtils::configToJSON(config);
        auto parsedConfig = PythonConfigUtils::loadFromJSON(jsonStr);
        
        return py::make_tuple(
            config->getOr<int64_t>("neuron_count", 0),
            parsedConfig->getOr<int64_t>("neuron_count", 0)
        );
    }, "Test JSON configuration support");
    
    m.def("testUtilityFunctions", []() {
        return py::make_tuple(
            utils::formatNeuronId(NeuronId{42}),
            utils::formatSynapseId(SynapseId{100}),
            utils::formatRegionId(RegionId{5}),
            utils::formatDevelopmentStage(DevelopmentalStage::CriticalPeriod),
            utils::formatNeuronType(NeuronType::Sensory),
            utils::formatActionType(ActionType::MoveForward),
            utils::formatWorldObjectType(WorldObjectType::Resource),
            utils::generateRandomSeed()
        );
    }, "Test utility functions");
    
    m.def("testContextManager", []() {
        // Test context manager functionality
        auto config = PythonConfigUtils::createDefaultConfig();
        config->set("neuron_count", 200, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run some steps
        for (int i = 0; i < 10; ++i) {
            brain->step(i);
        }
        
        auto finalSpikes = brain->getTotalSpikeCount();
        
        return py::make_tuple(finalSpikes, brain->getTotalNeuronCount());
    }, "Test context manager functionality");
    
    m.def("testPerformanceMonitoring", []() {
        auto config = PythonConfigUtils::createDefaultConfig();
        config->set("neuron_count", 300, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration<double>(end - start).count();
        auto stepsPerSecond = 100.0 / duration;
        auto totalSpikes = brain->getTotalSpikeCount();
        
        return py::make_tuple(
            duration,
            stepsPerSecond,
            totalSpikes,
            brain->getTotalNeuronCount()
        );
    }, "Test performance monitoring");
}

} // namespace nlm
