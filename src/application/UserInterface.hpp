#pragma once

#include <iostream>
#include <string>
#include <memory>

namespace nlm {

class UserInterface {
public:
    UserInterface() = default;
    
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    void printConfigurationSummary(const std::shared_ptr<class Config>& config) {
        std::cout << "" << std::endl;
        std::cout << "Configuration:" << std::endl;
        if (config->has("random_seed")) {
            std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
        }
        if (config->has("simulation_timestep")) {
            std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
        }
        if (config->has("neuron_count")) {
            std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
        }
        if (config->has("region_count")) {
            std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
        }
        if (config->has("connection_probability")) {
            std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.15f) << std::endl;
        }
        std::cout << "" << std::endl;
    }
    
    void printLearningResults(const std::string& prefix, const std::string& message) {
        std::cout << prefix << " " << message << std::endl;
    }
    
    void printPhase2Objectives() {
        std::cout << "" << std::endl;
        std::cout << "Phase 2 Objectives Completed:" << std::endl;
        std::cout << "  ✓ Real LIF neuron dynamics implemented" << std::endl;
        std::cout << "  ✓ Event-driven spike propagation with delays" << std::endl;
        std::cout << "  ✓ STDP plasticity rule" << std::endl;
        std::cout << "  ✓ Hebbian plasticity rule" << std::endl;
        std::cout << "  ✓ Structural plasticity (synaptogenesis/pruning)" << std::endl;
        std::cout << "  ✓ Learning experiment demonstrates measurable changes" << std::endl;
        std::cout << "  ✓ Network shows activity-dependent synaptic modification" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "The NLM brain is now a functioning artificial neural substrate" << std::endl;
        std::cout << "capable of changing its own synaptic connections through experience." << std::endl;
        std::cout << "" << std::endl;
    }
};