// Clean, modular NLM demo system
// Provides extensible CLI for running demos and tests

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <iomanip>

namespace nlm {

// Simple configuration parser for demo options
class DemoOptions {
public:
    struct Options {
        std::string demoType = "phase2";
        int numEpisodes = 5;
        int stepsPerEpisode = 1000;
        int neuronCount = 500;
        int regionCount = 1;
        float connectionProb = 0.15f;
        double timestep = 0.001;
        int64_t randomSeed = 42;
        bool verbose = false;
        bool help = false;
        bool version = false;
        std::string outputFile;
        
        // Parse command line arguments
        bool parse(int argc, char** argv) {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                
                if (arg == "--help" || arg == "-h") {
                    help = true;
                } else if (arg == "--version" || arg == "-v") {
                    version = true;
                } else if (arg.substr(0, 12) == "--demo=") {
                    demoType = arg.substr(12);
                } else if (arg.find("--episodes=") == 0) {
                    numEpisodes = std::stoi(arg.substr(11));
                } else if (arg.find("--steps=") == 0) {
                    stepsPerEpisode = std::stoi(arg.substr(8));
                } else if (arg.find("--neurons=") == 0) {
                    neuronCount = std::stoi(arg.substr(9));
                } else if (arg.find("--regions=") == 0) {
                    regionCount = std::stoi(arg.substr(10));
                } else if (arg.find("--connection=") == 0) {
                    connectionProb = std::stof(arg.substr(12));
                } else if (arg.find("--timestep=") == 0) {
                    timestep = std::stod(arg.substr(11));
                } else if (arg.find("--seed=") == 0) {
                    randomSeed = std::stoll(arg.substr(7));
                } else if (arg == "--verbose" || arg == "-V") {
                    verbose = true;
                } else if (arg == "--output" && i + 1 < argc) {
                    outputFile = argv[++i];
                }
            }
            return true;
        }
        
        void printHelp() {
            std::cout << "NLM Demo Options:" << std::endl;
            std::cout << "  --help, -h              Show this help" << std::endl;
            std::cout << "  --version, -v           Show version info" << std::endl;
            std::cout << "  --demo=<type>           Demo type: phase2, phase3, phase4, phase6, complete" << std::endl;
            std::cout << "  --episodes=<n>          Number of episodes (default: 5)" << std::endl;
            std::cout << "  --steps=<n>             Steps per episode (default: 1000)" << std::endl;
            std::cout << "  --neurons=<n>           Neuron count (default: 500)" << std::endl;
            std::cout << "  --regions=<n>           Region count (default: 1)" << std::endl;
            std::cout << "  --connection=<p>        Connection probability (default: 0.15)" << std::endl;
            std::cout << "  --timestep=<t>           Simulation timestep (default: 0.001)" << std::endl;
            std::cout << "  --seed=<s>              Random seed (default: 42)" << std::endl;
            std::cout << "  --verbose, -V           Enable verbose output" << std::endl;
            std::cout << "  --output <file>         Save output to file" << std::endl;
            std::cout << std::endl;
            std::cout << "Example: nlm --demo=phase2 --episodes=3 --steps=500 --verbose" << std::endl;
        }
        
        void printVersion() {
            std::cout << "NLM (Neural Learning Machine)" << std::endl;
            std::cout << "Version: 0.1.0" << std::endl;
            std::cout << "Build: Demo System Edition" << std::endl;
            std::cout << "Phase: 2 - Real Neural Computation" << std::endl;
        }
    };
    
    static Options parse(int argc, char** argv) {
        Options options;
        options.parse(argc, argv);
        return options;
    }
};

// Simple demo runner interface
class DemoRunner {
public:
    virtual ~DemoRunner() {}
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool run(const DemoOptions::Options& options) = 0;
    virtual bool supportsOption(const std::string& option) const = 0;
};

// Phase 2 demo implementation
class Phase2Demo : public DemoRunner {
public:
    std::string getName() const override { return "phase2"; }
    std::string getDescription() const override { 
        return "Phase 2: Basic Neural Computation - LIF neurons, STDP, Hebbian learning";
    }
    
    bool run(const DemoOptions::Options& options) override {
        std::cout << "============================================" << std::endl;
        std::cout << "NLM Phase 2 Demo" << std::endl;
        std::cout << "============================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Episodes: " << options.numEpisodes << std::endl;
        std::cout << "  Steps per episode: " << options.stepsPerEpisode << std::endl;
        std::cout << "  Neurons: " << options.neuronCount << std::endl;
        std::cout << "  Regions: " << options.regionCount << std::endl;
        std::cout << "  Connection probability: " << options.connectionProb << std::endl;
        std::cout << "  Timestep: " << options.timestep << std::endl;
        std::cout << "  Random seed: " << options.randomSeed << std::endl;
        std::cout << std::endl;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount));
        config->set("region_count", static_cast<int64_t>(options.regionCount));
        config->set("connection_probability", options.connectionProb);
        config->set("random_seed", options.randomSeed);
        config->set("simulation_timestep", options.timestep);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return false;
        }
        
        std::cout << "Brain initialized successfully" << std::endl;
        std::cout << std::endl;
        
        // Run Phase 2 tests
        return runPhase2Tests(brain, options);
    }
    
    bool supportsOption(const std::string& option) const override {
        return option == "episodes" || option == "steps" || option == "neurons" || 
               option == "regions" || option == "connection" || option == "timestep" ||
               option == "seed" || option == "verbose";
    }
    
private:
    bool runPhase2Tests(std::shared_ptr<Brain> brain, const DemoOptions::Options& options) {
        std::cout << "Running Phase 2 Tests:" << std::endl;
        std::cout << "=======================" << std::endl;
        std::cout << std::endl;
        
        // Test 1: Basic Connectivity
        std::cout << "Test 1: Basic Neural Connectivity" << std::endl;
        if (!runBasicConnectivityTest(brain)) {
            std::cout << "FAILED" << std::endl;
            return false;
        }
        std::cout << "PASSED" << std::endl;
        
        brain->reset();
        brain->initialize();
        
        std::cout << std::endl;
        std::cout << "Test 2: Plasticity Learning" << std::endl;
        if (!runPlasticityExperiment(brain)) {
            std::cout << "FAILED" << std::endl;
            return false;
        }
        std::cout << "PASSED" << std::endl;
        
        brain->reset();
        brain->initialize();
        
        std::cout << std::endl;
        std::cout << "Test 3: STDP Verification" << std::endl;
        if (!runStdpVerification(brain)) {
            std::cout << "FAILED" << std::endl;
            return false;
        }
        std::cout << "PASSED" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Phase 2 Demo completed successfully!" << std::endl;
        
        return true;
    }
    
    bool runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "  Result: " << spikes << " spikes generated" << std::endl;
        
        return spikes > 0;
    }
    
    bool runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        std::vector<float> initialWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < std::min(size_t(20), brain->getTotalNeuronCount() / 4); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        std::vector<float> finalWeights;
        bool learningOccurred = false;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
            
            size_t minSize = std::min(initialWeights.size(), finalWeights.size());
            for (size_t i = 0; i < minSize; ++i) {
                if (std::abs(finalWeights[i] - initialWeights[i]) > 0.01f) {
                    learningOccurred = true;
                    break;
                }
            }
        }
        
        std::cout << "  Result: " << (learningOccurred ? "LEARNING DETECTED" : "No significant change") << std::endl;
        
        return learningOccurred;
    }
    
    bool runStdpVerification(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 5) return false;
        
        auto& synapses = region->getSynapses();
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            for (int trial = 0; trial < 20; ++trial) {
                neurons[0]->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                neurons[1]->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        std::cout << "  Result: " << (totalDelta > 0.001f ? "LTP detected" : "No significant change") << std::endl;
        
        return totalDelta > 0.001f;
    }
};

// Phase 3 demo (simplified)
class Phase3Demo : public DemoRunner {
public:
    std::string getName() const override { return "phase3"; }
    std::string getDescription() const override { 
        return "Phase 3: World Interaction - Agent learning in environment";
    }
    
    bool run(const DemoOptions::Options& options) override {
        std::cout << "Phase 3 Demo" << std::endl;
        std::cout << "============" << std::endl;
        std::cout << "Note: Phase 3 demo is simplified in this modular system" << std::endl;
        std::cout << "Features implemented:" << std::endl;
        std::cout << "  - Agent brain interface" << std::endl;
        std::cout << "  - World simulation" << std::endl;
        std::cout << "  - Sensory processing" << std::endl;
        std::cout << "  - Motor control" << std::endl;
        std::cout << "  - Reward modulation" << std::endl;
        std::cout << std::endl;
        std::cout << "Phase 3 demo would run here with full implementation" << std::endl;
        return true;
    }
    
    bool supportsOption(const std::string& option) const override {
        return option == "episodes" || option == "steps" || option == "neurons" ||
               option == "regions" || option == "connection" || option == "timestep" ||
               option == "seed" || option == "verbose";
    }
};

// Phase 4 demo (simplified)
class Phase4Demo : public DemoRunner {
public:
    std::string getName() const override { return "phase4"; }
    std::string getDescription() const override { 
        return "Phase 4: Emerging Cognition - Prediction, memory, planning";
    }
    
    bool run(const DemoOptions::Options& options) override {
        std::cout << "Phase 4 Demo" << std::endl;
        std::cout << "=============" << std::endl;
        std::cout << "Note: Phase 4 demo is simplified in this modular system" << std::endl;
        std::cout << "Features implemented:" << std::endl;
        std::cout << "  - Neural prediction" << std::endl;
        std::cout << "  - Working memory" << std::endl;
        std::cout << "  - Episodic memory" << std::endl;
        std::cout << "  - Concept formation" << std::endl;
        std::cout << "  - Neural attention" << std::endl;
        std::cout << "  - Planning" << std::endl;
        std::cout << "  - Self-model" << std::endl;
        std::cout << "  - Social learning" << std::endl;
        std::cout << std::endl;
        std::cout << "Phase 4 demo would run here with full implementation" << std::endl;
        return true;
    }
    
    bool supportsOption(const std::string& option) const override {
        return option == "episodes" || option == "steps" || option == "neurons" ||
               option == "regions" || option == "connection" || option == "timestep" ||
               option == "seed" || option == "verbose";
    }
};

// Demo system manager
class DemoSystem {
public:
    DemoSystem() {
        registerDemos();
    }
    
    void registerDemos() {
        demos["phase2"] = std::make_unique<Phase2Demo>();
        demos["phase3"] = std::make_unique<Phase3Demo>();
        demos["phase4"] = std::make_unique<Phase4Demo>();
    }
    
    bool runDemo(const std::string& demoName, const DemoOptions::Options& options) {
        auto it = demos.find(demoName);
        if (it == demos.end()) {
            std::cerr << "Error: Unknown demo: " << demoName << std::endl;
            return false;
        }
        
        DemoRunner* demo = it->second.get();
        std::cout << "Running demo: " << demo->getName() << std::endl;
        std::cout << "Description: " << demo->getDescription() << std::endl;
        std::cout << std::endl;
        
        return demo->run(options);
    }
    
    void listDemos() {
        std::cout << "Available Demos:" << std::endl;
        for (const auto& pair : demos) {
            DemoRunner* demo = pair.second.get();
            std::cout << "  " << pair.first << ": " << demo->getDescription() << std::endl;
        }
    }
    
    bool isDemoAvailable(const std::string& demoName) {
        return demos.find(demoName) != demos.end();
    }
    
    void showHelp() {
        std::cout << "NLM Demo System" << std::endl;
        std::cout << "================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Available demos:" << std::endl;
        listDemos();
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --demo=<name>           Specify demo to run (default: phase2)" << std::endl;
        std::cout << "  --episodes=<n>          Number of episodes (default: 5)" << std::endl;
        std::cout << "  --steps=<n>             Steps per episode (default: 1000)" << std::endl;
        std::cout << "  --neurons=<n>           Neuron count (default: 500)" << std::endl;
        std::cout << "  --regions=<n>           Region count (default: 1)" << std::endl;
        std::cout << "  --connection=<p>        Connection probability (default: 0.15)" << std::endl;
        std::cout << "  --timestep=<t>           Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --seed=<s>              Random seed (default: 42)" << std::endl;
        std::cout << "  --verbose, -V           Enable verbose output" << std::endl;
        std::cout << "  --help, -h              Show this help" << std::endl;
        std::cout << "  --version, -v           Show version information" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm --demo=phase2 --episodes=3 --steps=500 --verbose" << std::endl;
        std::cout << "  nlm --demo=phase3 --neurons=1000 --seed=123" << std::endl;
        std::cout << "  nlm --demo=phase4 --connection=0.05 --timestep=0.01" << std::endl;
    }
    
private:
    std::unordered_map<std::string, std::unique_ptr<DemoRunner>> demos;
};

// Global demo system instance
DemoSystem globalDemoSystem;

// Main demo entry point
int runDemoSystem(int argc, char** argv) {
    DemoOptions::Options options = DemoOptions::parse(argc, argv);
    
    if (options.help) {
        globalDemoSystem.showHelp();
        return 0;
    }
    
    if (options.version) {
        options.printVersion();
        return 0;
    }
    
    std::cout << "NLM (Neural Learning Machine) Demo System" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;
    
    if (!globalDemoSystem.isDemoAvailable(options.demoType)) {
        std::cerr << "Error: Demo not available: " << options.demoType << std::endl;
        std::cout << "Available demos:" << std::endl;
        globalDemoSystem.listDemos();
        return 1;
    }
    
    try {
        bool success = globalDemoSystem.runDemo(options.demoType, options);
        if (success) {
            std::cout << std::endl;
            std::cout << "Demo completed successfully!" << std::endl;
            return 0;
        } else {
            std::cerr << std::endl;
            std::cerr << "Demo failed." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

} // namespace nlm

// Main entry point
int main(int argc, char** argv) {
    try {
        return nlm::runDemoSystem(argc, argv);
    } catch (...) {
        std::cerr << "Fatal error occurred" << std::endl;
        return 1;
    }
}
