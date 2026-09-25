// Exception Tests
// Tests for the NLM exception hierarchy

#include "core/Exception.hpp"
#include <cassert>
#include <iostream>

namespace test_exception {

void testExceptionCreation() {
    // Test base exception
    nlm::NLMException baseEx("Base error message");
    assert(std::string(baseEx.what()) == "Base error message");
    
    std::cout << "    testExceptionCreation passed" << std::endl;
}

void testConfigException() {
    // Test config-specific exceptions
    nlm::ConfigException configEx("Config error");
    assert(std::string(configEx.what()).find("Configuration Error") != std::string::npos);
    
    nlm::ConfigParseException parseEx("Parse error", "test.cfg");
    assert(std::string(parseEx.what()).find("Configuration Error") != std::string::npos);
    assert(std::string(parseEx.what()).find("test.cfg") != std::string::npos);
    
    nlm::ConfigValidationException validationEx("Validation error");
    assert(std::string(validationEx.what()).find("Configuration validation failed") != std::string::npos);
    
    std::cout << "    testConfigException passed" << std::endl;
}

void testNeuralException() {
    // Test neural-specific exceptions
    nlm::NeuralException neuralEx("Neural error");
    assert(std::string(neuralEx.what()).find("Neural Network Error") != std::string::npos);
    
    nlm::NeuronException neuronEx("Neuron error");
    assert(std::string(neuronEx.what()).find("Neuron Error") != std::string::npos);
    
    nlm::SynapseException synapseEx("Synapse error");
    assert(std::string(synapseEx.what()).find("Synapse Error") != std::string::npos);
    
    std::cout << "    testNeuralException passed" << std::endl;
}

void testPlasticityException() {
    // Test plasticity-specific exceptions
    nlm::PlasticityException plasticityEx("Plasticity error");
    assert(std::string(plasticityEx.what()).find("Plasticity Error") != std::string::npos);
    
    nlm::PlasticityRuleError ruleEx("STDP", "Rule failed");
    assert(std::string(ruleEx.what()).find("Plasticity Error") != std::string::npos);
    assert(std::string(ruleEx.what()).find("STDP") != std::string::npos);
    
    std::cout << "    testPlasticityException passed" << std::endl;
}

void testMemoryException() {
    // Test memory-specific exceptions
    nlm::MemoryException memoryEx("Memory error");
    assert(std::string(memoryEx.what()).find("Memory System Error") != std::string::npos);
    
    nlm::MemoryAllocationError allocEx(1024, "neuron_creation");
    assert(std::string(allocEx.what()).find("Memory allocation failed") != std::string::npos);
    
    std::cout << "    testMemoryException passed" << std::endl;
}

void testSystemException() {
    // Test system-specific exceptions
    nlm::SystemException systemEx("System error");
    assert(std::string(systemEx.what()).find("System Error") != std::string::npos);
    
    nlm::InitializationException initEx("neural_network", "Failed to initialize");
    assert(std::string(initEx.what()).find("Failed to initialize neural_network") != std::string::npos);
    
    std::cout << "    testSystemException passed" << std::endl;
}

void testExceptionInheritance() {
    // Test that exceptions are properly inherited
    nlm::ConfigException configEx("test");
    nlm::NLMException& baseRef = configEx;
    assert(&baseRef != nullptr);  // Should be able to treat as base
    
    nlm::NeuronException neuronEx("test");
    nlm::NeuralException& neuralRef = neuronEx;
    assert(&neuralRef != nullptr);
    
    nlm::PlasticityRuleError ruleEx("test", "test");
    nlm::PlasticityException& plasticityRef = ruleEx;
    assert(&plasticityRef != nullptr);
    
    nlm::MemoryAllocationError allocEx(0, "test");
    nlm::MemoryException& memoryRef = allocEx;
    assert(&memoryRef != nullptr);
    
    std::cout << "    testExceptionInheritance passed" << std::endl;
}

void testExceptionFactory() {
    // Test exception factory functions
    auto configEx = nlm::createException("Config", "Test config error");
    assert(configEx != nullptr);
    assert(std::string(configEx->what()).find("Configuration Error") != std::string::npos);
    
    auto neuronEx = nlm::createException("Neuron", "Test neuron error");
    assert(neuronEx != nullptr);
    assert(std::string(neuronEx->what()).find("Neuron Error") != std::string::npos);
    
    auto unknownEx = nlm::createException("UnknownType", "Test unknown error");
    assert(unknownEx != nullptr);
    assert(std::string(unknownEx->what()).find("Test unknown error") != std::string::npos);
    
    std::cout << "    testExceptionFactory passed" << std::endl;
}

void runAll() {
    testExceptionCreation();
    testConfigException();
    testNeuralException();
    testPlasticityException();
    testMemoryException();
    testSystemException();
    testExceptionInheritance();
    testExceptionFactory();
}

} // namespace test_exception
