// Main Tests
// Tests for main functionality and integration

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>
#include <string>

namespace test_main {

void testIncludesAndForwardDeclarations() {
    // Test that all headers can be included without errors
    // and that forward declarations work
    
    // This is a compile-time test - if there are issues with forward
    // declarations or missing includes, the compilation would fail
    
    std::cout << "    testIncludesAndForwardDeclarations passed" << std::endl;
}

void testConfigHeaderOnly() {
    // Test that Config.hpp can be used without implementation
    // Just testing that the interface exists
    
    nlm::Config config;
    assert(config.has("test_key") == false);
    
    // Test setting and getting different types
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    
    assert(config.get<int>("int_value").has_value());
    assert(*config.get<int>("int_value") == 42);
    
    assert(config.get<double>("double_value").has_value());
    assert(std::abs(*config.get<double>("double_value") - 3.14) < 0.01);
    
    assert(config.get<std::string>("string_value").has_value());
    assert(*config.get<std::string>("string_value") == "hello");
    
    assert(config.get<bool>("bool_value").has_value());
    assert(*config.get<bool>("bool_value") == true);
    
    std::cout << "    testConfigHeaderOnly passed" << std::endl;
}

void testNeuronHeaderOnly() {
    // Test that Neuron.hpp interface is correct
    nlm::NeuronId id(100);
    nlm::Neuron neuron(id);
    
    assert(neuron.getId().value == 100);
    
    // Test that all public methods exist and can be called
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setMembranePotential(-60.0f);
    neuron.setThreshold(-50.0f);
    neuron.setFiringState(nlm::FiringState::Active);
    
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    assert(neuron.getMembranePotential() == -60.0f);
    assert(neuron.getThreshold() == -50.0f);
    assert(neuron.isFiring() == true);
    
    std::cout << "    testNeuronHeaderOnly passed" << std::endl;
}

void testBrainHeaderOnly() {
    // Test that Brain.hpp interface is correct
    auto config = std::make_shared<nlm::Config>();
    
    // Brain constructor requires Config
    nlm::Brain brain(config);
    
    // Test that all public methods exist
    brain.getTotalNeuronCount();
    brain.getTotalSynapseCount();
    brain.getActiveNeuronCount();
    brain.getFiringNeuronCount();
    brain.getAverageFiringRate();
    brain.getExcitationInhibitionRatio();
    brain.getTotalSpikeCount();
    brain.getPendingSpikeEventCount();
    
    std::cout << "    testBrainHeaderOnly passed" << std::endl;
}

void testLoggerHeaderOnly() {
    // Test that Logger.hpp interface is correct
    nlm::Logger logger;
    
    // Test all logging methods exist
    logger.debug("Debug");
    logger.info("Info");
    logger.warning("Warning");
    logger.error("Error");
    logger.critical("Critical");
    
    // Test level checking
    logger.isEnabled(nlm::LogLevel::Info);
    logger.isEnabled(nlm::LogLevel::Error);
    
    // Test global logger access
    nlm::Logger& global = nlm::Logger::getGlobal();
    
    std::cout << "    testLoggerHeaderOnly passed" << std::endl;
}

void testTypesHeaderOnly() {
    // Test that all type definitions are available
    
    nlm::NeuronId nid(42);
    nlm::SynapseId sid(99);
    nlm::RegionId rid(1);
    nlm::PopulationId pid(5);
    
    nlm::NeuronType nt = nlm::NeuronType::Excitatory;
    nlm::SynapseType st = nlm::SynapseType::Excitatory;
    nlm::DevelopmentalStage ds = nlm::DevelopmentalStage::Adult;
    nlm::FiringState fs = nlm::FiringState::Active;
    nlm::ActionType at = nlm::ActionType::MoveForward;
    nlm::MotorCommand mc = nlm::MotorCommand::MoveForward;
    
    assert(nid.index() == 42);
    assert(sid.index() == 99);
    assert(rid.index() == 1);
    assert(pid.index() == 5);
    
    std::cout << "    testTypesHeaderOnly passed" << std::endl;
}

void testConstantsAndInvalidIDs() {
    // Test that invalid ID constants exist
    
    assert(nlm::INVALID_NEURON_ID.value == UINT64_MAX);
    assert(nlm::INVALID_SYNAPSE_ID.value == UINT64_MAX);
    assert(nlm::INVALID_REGION_ID.value == UINT64_MAX);
    assert(nlm::INVALID_POPULATION_ID.value == UINT64_MAX);
    
    std::cout << "    testConstantsAndInvalidIDs passed" << std::endl;
}

void testNamespaceComponents() {
    // Test that all major components are in the nlm namespace
    
    // These should all be accessible without qualification if properly declared
    
    // Example: We can't test everything here since this is mostly a compilation test
    // but we can verify some basic functionality
    
    nlm::ConfigValue cv = "test string";
    
    // Test type alias
    nlm::MembranePotential vp = 0.0f;
    nlm::Timestamp time = 1.0;
    nlm::SimulationStep step = 42;
    
    assert(vp == 0.0f);
    assert(time == 1.0);
    assert(step == 42);
    
    std::cout << "    testNamespaceComponents passed" << std::endl;
}

void testBuildSystemIntegration() {
    // Test that the build system integration is working
    // This is mainly a verification that all the right files are included
    
    // If we're compiling this test, it means the build system
    // is correctly configured to include all necessary files
    
    // Test that we can create and use the main objects
    auto config = std::make_shared<nlm::Config>();
    config->set("test_param", 123);
    
    nlm::RandomGenerator rng(42);
    
    nlm::SimulationClock clock(0.001);
    
    std::cout << "    testBuildSystemIntegration passed" << std::endl;
}

void testMemoryManagement() {
    // Test that smart pointers and memory management work correctly
    
    auto config = std::make_shared<nlm::Config>();
    
    // Config should be reference-counted
    auto config2 = config;
    assert(config.use_count() == 2);
    
    // After reassigning, count should be 1
    config2 = nullptr;
    assert(config.use_count() == 1);
    
    std::cout << "    testMemoryManagement passed" << std::endl;
}

void runAll() {
    std::cout << "=== Main Tests ===" << std::endl;
    testIncludesAndForwardDeclarations();
    testConfigHeaderOnly();
    testNeuronHeaderOnly();
    testBrainHeaderOnly();
    testLoggerHeaderOnly();
    testTypesHeaderOnly();
    testConstantsAndInvalidIDs();
    testNamespaceComponents();
    testBuildSystemIntegration();
    testMemoryManagement();
    std::cout << "All main tests passed!" << std::endl << std::endl;
}

} // namespace test_main