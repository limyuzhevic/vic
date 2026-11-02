// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase2Demo.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace nlm;

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

int main(int argc, char** argv) {
    // Parse command line arguments for demo configuration
    Phase2Demo::DemoConfig config;
    config.demoName = "NLM Phase 2 Demo";
    config.neuronCount = 500;
    config.timestepMultiplier = 1;
    config.verbose = true;
    config.showProgress = false;
    config.configFile = "configs/default.cfg";
    
    // Simple command line argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 2 Demo Help:" << std::endl;
            std::cout << "  --demo-name <name> - Set demo name" << std::endl;
            std::cout << "  --no-basic          - Skip basic connectivity test" << std::endl;
            std::cout << "  --no-plasticity     - Skip plasticity experiment" << std::endl;
            std::cout << "  --no-stdp           - Skip STDP verification" << std::endl;
            std::cout << "  --neurons <count>   - Set neuron count (default: 500)" << std::endl;
            std::cout << "  --multiplier <n>    - Multiply all steps by n" << std::endl;
            std::cout << "  --verbose            - Enable verbose output" << std::endl;
            std::cout << "  --progress           - Show progress during simulation" << std::endl;
            std::cout << "  --config <file>      - Configuration file (default: configs/default.cfg)" << std::endl;
            return 0;
        }
        
        if (arg.substr(0, 12) == "--demo-name=") {
            config.demoName = arg.substr(12);
        } else if (arg == "--no-basic") {
            config.enableBasicConnectivityTest = false;
        } else if (arg == "--no-plasticity") {
            config.enablePlasticityExperiment = false;
        } else if (arg == "--no-stdp") {
            config.enableStdpVerification = false;
        } else if (arg.substr(0, 11) == "--neurons=") {
            config.neuronCount = std::stoi(arg.substr(11));
        } else if (arg.substr(0, 13) == "--multiplier=") {
            config.timestepMultiplier = std::stoi(arg.substr(13));
        } else if (arg == "--verbose") {
            config.verbose = true;
        } else if (arg == "--progress") {
            config.showProgress = true;
        } else if (arg.substr(0, 10) == "--config=") {
            config.configFile = arg.substr(10);
        }
    }
    
    // Print banner
    printBanner();
    
    std::cout << "=== NLM Phase 2 Demo: " << config.demoName << " ===" << std::endl;
    std::cout << "Tests to run:" << std::endl;
    std::cout << "  Basic connectivity: " << (config.enableBasicConnectivityTest ? "YES" : "NO") << std::endl;
    std::cout << "  Plasticity experiment: " << (config.enablePlasticityExperiment ? "YES" : "NO") << std::endl;
    std::cout << "  STDP verification: " << (config.enableStdpVerification ? "YES" : "NO") << std::endl;
    std::cout << "Neuron count: " << config.neuronCount << std::endl;
    std::cout << "Timestep multiplier: " << config.timestepMultiplier << std::endl;
    std::cout << "Config file: " << config.configFile << std::endl;
    std::cout << std::endl;

    // Run the demo
    Phase2Demo demo(config);
    if (demo.run()) {
        std::cout << "=== Demo completed successfully ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Demo failed ===" << std::endl;
        return 1;
    }
}