// Integration Test Framework Header for NLM
// Comprehensive integration testing framework for NLM components
// Tests component interactions, data flow, and system integration

#pragma once

#include <string>

namespace nlm {
    class Config;
    class Brain;
    class NeuronId;
    enum class SimulationStep : uint64_t;
    enum class DevelopmentalStage;
    struct EpisodicMemoryItem;
    class SpikeSystem;
    class STDP;
    class Hebbian;
    class StructuralPlasticity;
    class DevelopmentSystem;
    class Dopamine;
    class Curiosity;
    class Novelty;
    class PredictionErrorSignal;
}

namespace test_integration {

struct IntegrationTestResult {
    std::string test_name;
    bool brain_component_tests;
    bool memory_system_tests;
    bool neuromodulation_tests;
    bool plasticity_tests;
    bool development_tests;
    
    int passed_tests;
    int total_tests;
    bool overall_pass;
};

class IntegrationTestFramework {
public:
    static bool testBrainComponentIntegration();
    static bool testMemorySystemIntegration();
    static bool testNeuromodulationIntegration();
    static bool testPlasticityIntegration();
    static bool testDevelopmentIntegration();
    
    static IntegrationTestResult runIntegrationTests();
};

} // namespace test_integration
