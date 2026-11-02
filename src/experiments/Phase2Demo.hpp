// Phase 2 Demo Header - Configurable demo system for NLM Phase 2
#pragma once

#include <string>
#include <memory>

// Forward declarations
namespace nlm {
    class Brain;
    class RegionId;
    class NeuronId;
    class Synapse;
    using SimulationStep = uint64_t;
    using Timestamp = double;
    class Config;
    
    class Phase2Demo {
    public:
        struct DemoConfig {
            std::string demoName = "NLM Phase 2 Demo";
            bool enableBasicConnectivityTest = true;
            bool enablePlasticityExperiment = true;
            bool enableStdpVerification = true;
            int timestepMultiplier = 1;
            int neuronCount = 500;
            float connectionProbability = 0.15f;
            std::string configFile = "configs/default.cfg";
            bool verbose = true;
            bool showProgress = false;
        };

        Phase2Demo(const DemoConfig& config);
        ~Phase2Demo() = default;
        
        bool run();
        
    private:
        void runBasicConnectivityTest(std::shared_ptr<Brain> brain, int multiplier);
        void runPlasticityExperiment(std::shared_ptr<Brain> brain, int multiplier);
        void runStdpVerification(std::shared_ptr<Brain> brain, int multiplier);
        
        DemoConfig config;
    };
}

// Command line parsing utility
namespace nlm {
    bool parseCommandLine(int argc, char** argv, Phase2Demo::DemoConfig& config);
}