// NLM Phase 2 Legacy Demo Header
// 
// This header defines the Phase2LegacyDemo class for backward compatibility
// with the original NLM Phase 2 demonstrations from main.cpp

#ifndef NLM_PHASE2_LEGACY_DEMO_HPP
#define NLM_PHASE2_LEGACY_DEMO_HPP

#include <memory>
#include <vector>
#include <string>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

namespace nlm {

// Forward declarations
class Neuron;
class Synapse;
class SimulationStep;
class RegionId;
class NeuronId;

class Phase2LegacyDemo {
public:
    Phase2LegacyDemo();
    ~Phase2LegacyDemo();
    
    // Run specific demo types:
    // 1 = Basic connectivity test
    // 2 = Plasticity learning experiment
    // 3 = STDP verification test
    // 4 = Complete suite (default)
    void runDemo(int demoType = 4);
    
    // Individual demo methods
    void runBasicConnectivityDemo();
    void runPlasticityDemo();
    void runStdpDemo();
    void runAllDemos();

private:
    void configureDefaultSettings();
    
    std::shared_ptr<Config> config_;
};

} // namespace nlm

#endif // NLM_PHASE2_LEGACY_DEMO_HPP
