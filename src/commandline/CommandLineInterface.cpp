// Command implementations for NLM Command Line Interface

#include "commandline/CommandLineInterface.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>

using namespace nlm;

namespace {
    // Helper functions
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Command Line Interface                              ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }

    void printHelp() {
        std::cout << "=== NLM CLI Commands ===" << std::endl;
        std::cout << "help, ?              Show this help" << std::endl;
        std::cout << "status              Show brain status" << std::endl;
        std::cout << "quit, exit          Exit interactive mode" << std::endl;
        std::cout << "step [N]            Run N steps (default: 1)" << std::endl;
        std::cout << "run N               Run N simulation steps" << std::endl;
        std::cout << "train               Training mode" << std::endl;
        std::cout << "test                Test mode" << std::endl;
        std::cout << "inject N            Inject current N into sensory neurons" << std::endl;
        std::cout << "visualize           Launch visualization" << std::endl;
        std::cout << "benchmark N         Run benchmark with N steps" << std::endl;
        std::cout << "save [FILE]         Save brain state" << std::endl;
        std::cout << "load N              Load checkpoint N" << std::endl;
    }
}

// Interactive Command Implementation
class InteractiveCommand::Impl {
public:
    void execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        std::cout << "=== NLM Interactive Mode ===" << std::endl;
        std::cout << "Brain initialized with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        std::cout << "Ready for commands. Type 'help' for available commands." << std::endl;

        std::string command;
        while (std::getline(std::cin, command)) {
            processCommand(brain, command, options);
        }
    }

    void processCommand(std::shared_ptr<Brain> brain, const std::string& cmd, const CommandLineParser::Options& options) {
        if (cmd == "help" || cmd == "?") {
            printHelp();
        } else if (cmd == "status") {
            brain->logStatus();
        } else if (cmd == "quit" || cmd == "exit") {
            // Exit will be handled by main loop
        } else if (cmd.substr(0, 4) == "step") {
            // Parse step count
            int steps = 1;
            if (cmd.size() > 5) {
                steps = std::stoi(cmd.substr(5));
            }
            runSteps(brain, steps);
        } else if (cmd.substr(0, 4) == "run ") {
            int steps = std::stoi(cmd.substr(4));
            runSteps(brain, steps);
        } else if (cmd == "train") {
            trainMode(brain, options.steps);
        } else if (cmd == "test") {
            testMode(brain);
        } else if (cmd.substr(0, 6) == "inject ") {
            float current = std::stof(cmd.substr(6));
            injectCurrent(brain, current);
        } else if (cmd.substr(0, 10) == "benchmark ") {
            int steps = std::stoi(cmd.substr(10));
            benchmarkMode(brain, steps);
        } else if (cmd.substr(0, 5) == "save ") {
            std::string filename = cmd.substr(5);
            saveState(brain, filename);
        } else if (cmd.substr(0, 5) == "load ") {
            int checkpoint = std::stoi(cmd.substr(5));
            loadState(brain, checkpoint);
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }

    void runSteps(std::shared_ptr<Brain> brain, int steps) {
        std::cout << "Running " << steps << " steps..." << std::endl;
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
            if (i % 100 == 0) {
                std::cout << "  Step " << i << ", spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
        }
        std::cout << "Completed " << steps << " steps" << std::endl;
    }

    void trainMode(std::shared_ptr<Brain> brain, int steps) {
        std::cout << "=== Training Mode ===" << std::endl;
        // Enable plasticity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }

        // Apply training input
        for (int i = 0; i < steps; ++i) {
            // Apply training pattern based on step
            applyTrainingPattern(brain, i);

            brain->step(i, i * 0.001);

            if (i % 100 == 0) {
                std::cout << "  Step " << i << ", spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
        }
        std::cout << "Training completed" << std::endl;
    }

    void testMode(std::shared_ptr<Brain> brain) {
        std::cout << "=== Test Mode ===" << std::endl;
        // Simple test - check if brain responds to input
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "No region found" << std::endl;
            return;
        }

        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cout << "No neurons found" << std::endl;
            return;
        }

        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();

        // Inject strong current into first 10 neurons
        std::cout << "Injecting current into 10 neurons..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }

        // Run a few steps
        for (int step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }

        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "Spikes generated: " << spikes << std::endl;

        if (spikes > 0) {
            std::cout << "✓ Spikes propagate through network" << std::endl;
        } else {
            std::cout << "! No spikes - checking neuron parameters..." << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::cout << "  Neuron " << i 
                          << " V=" << neurons[i]->getMembranePotential()
                          << " thresh=" << neurons[i]->getThreshold() << std::endl;
            }
        }
    }

    void injectCurrent(std::shared_ptr<Brain> brain, float current) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "No region found" << std::endl;
            return;
        }

        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cout << "No neurons found" << std::endl;
            return;
        }

        // Inject current into all neurons
        for (auto* neuron : neurons) {
            neuron->injectCurrent(current);
        }

        std::cout << "Injected " << current << " current into " << neurons.size() << " neurons" << std::endl;
    }

    void benchmarkMode(std::shared_ptr<Brain> brain, int steps) {
        std::cout << "=== Benchmark Mode ===" << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Benchmark completed:" << std::endl;
        std::cout << "  Steps: " << steps << std::endl;
        std::cout << "  Total time: " << duration.count() << " ms" << std::endl;
        std::cout << "  Steps per second: " << (steps / (duration.count() / 1000.0)) << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    }

    void saveState(std::shared_ptr<Brain> brain, const std::string& filename) {
        std::cout << "Saving brain state to " << filename << std::endl;
        if (brain->save(filename)) {
            std::cout << "✓ Brain state saved successfully" << std::endl;
        } else {
            std::cout << "✗ Failed to save brain state" << std::endl;
        }
    }

    void loadState(std::shared_ptr<Brain> brain, int checkpoint) {
        std::cout << "Loading checkpoint " << checkpoint << std::endl;
        // For now, just log the attempt
        std::cout << "Note: Checkpoint loading requires additional implementation" << std::endl;
    }

    void applyTrainingPattern(std::shared_ptr<Brain> brain, int step) {
        // Simple pattern: inject into different neuron groups based on step
        int pattern = step % 4;

        switch (pattern) {
            case 0:
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
                break;
            case 1:
                brain->injectCurrentToNeurons(NeuronType::Internal, 15.0f);
                break;
            case 2:
                brain->injectCurrentToNeurons(NeuronType::Motor, 25.0f);
                break;
            case 3:
                // Random injection
                if (brain->getRandomGenerator()) {
                    float strength = 10.0f + brain->getRandomGenerator()->uniformReal(0.0f, 15.0f);
                    brain->injectCurrentToNeurons(NeuronType::Sensory, strength);
                }
                break;
        }
    }
};

// Run Command Implementation
class RunCommand::Impl {
public:
    void execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        std::cout << "=== Run Mode ===" << std::endl;
        std::cout << "Running " << options.steps << " simulation steps..." << std::endl;

        runSteps(brain, options.steps, options.timestep);

        if (options.outputFile != "") {
            saveResults(brain, options.outputFile);
        }
    }

    void runSteps(std::shared_ptr<Brain> brain, int steps, double timestep) {
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * timestep);

            if (i % 100 == 0) {
                std::cout << "  Step " << i 
                          << ", spikes: " << brain->getTotalSpikeCount()
                          << ", active neurons: " << brain->getActiveNeuronCount()
                          << ", firing rate: " << brain->getAverageFiringRate()
                          << std::endl;
            }
        }
    }

    void saveResults(std::shared_ptr<Brain> brain, const std::string& filename) {
        std::cout << "Saving results to " << filename << std::endl;
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "step,spikes,active_neurons,firing_rate" << std::endl;
            file << "0," << brain->getTotalSpikeCount() 
                 << "," << brain->getActiveNeuronCount()
                 << "," << brain->getAverageFiringRate() << std::endl;
            file.close();
        } else {
            std::cerr << "Failed to open output file: " << filename << std::endl;
        }
    }
};

// Benchmark Command Implementation
class BenchmarkCommand::Impl {
public:
    void execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        std::cout << "=== Benchmark Mode ===" << std::endl;
        std::cout << "Benchmarking brain performance..." << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        // Run benchmark
        for (int i = 0; i < options.steps; ++i) {
            brain->step(i, i * options.timestep);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Benchmark completed:" << std::endl;
        std::cout << "  Steps: " << options.steps << std::endl;
        std::cout << "  Total time: " << duration.count() << " ms" << std::endl;
        std::cout << "  Steps per second: " << (options.steps / (duration.count() / 1000.0)) << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    }
};

// Training Command Implementation
class TrainCommand::Impl {
public:
    void execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
        std::cout << "=== Training Mode ===" << std::endl;

        enablePlasticity(brain);

        std::cout << "Applying training patterns..." << std::endl;

        for (int i = 0; i < options.steps; ++i) {
            // Apply training input pattern
            applyTrainingPattern(brain, i);

            brain->step(i, i * options.timestep);

            if (i % 100 == 0) {
                std::cout << "  Step " << i << ", spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
        }

        std::cout << "Training completed" << std::endl;
        std::cout << "Plasticity active: " << isPlasticityActive(brain) << std::endl;
    }

    void enablePlasticity(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Hebbian and STDP
            }
            std::cout << "Plasticity enabled for " << region->getSynapseCount() << " synapses" << std::endl;
        }
    }

    void applyTrainingPattern(std::shared_ptr<Brain> brain, int step) {
        // Simple pattern: inject into different neuron groups based on step
        int pattern = step % 4;

        switch (pattern) {
            case 0:
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
                break;
            case 1:
                brain->injectCurrentToNeurons(NeuronType::Internal, 15.0f);
                break;
            case 2:
                brain->injectCurrentToNeurons(NeuronType::Motor, 25.0f);
                break;
            case 3:
                // Random injection
                if (brain->getRandomGenerator()) {
                    float strength = 10.0f + brain->getRandomGenerator()->uniformReal(0.0f, 15.0f);
                    brain->injectCurrentToNeurons(NeuronType::Sensory, strength);
                }
                break;
        }
    }

    bool isPlasticityActive(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;

        for (const auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp || syn->getPlasticityFlags().hebbian) {
                return true;
            }
        }
        return false;
    }
};

// Command Factory Implementation
std::unique_ptr<CommandInterface> CommandFactory::createCommand(const std::string& mode) {
    if (mode == "interactive") {
        auto impl = std::make_unique<InteractiveCommand::Impl>();
        return std::make_unique<InteractiveCommand>(std::move(impl));
    } else if (mode == "run") {
        auto impl = std::make_unique<RunCommand::Impl>();
        return std::make_unique<RunCommand>(std::move(impl));
    } else if (mode == "benchmark") {
        auto impl = std::make_unique<BenchmarkCommand::Impl>();
        return std::make_unique<BenchmarkCommand>(std::move(impl));
    } else if (mode == "train") {
        auto impl = std::make_unique<TrainCommand::Impl>();
        return std::make_unique<TrainCommand>(std::move(impl));
    }
    return nullptr;
}

// Command Interface Implementations
InteractiveCommand::InteractiveCommand() : pImpl(std::make_unique<InteractiveCommand::Impl>()) {}
InteractiveCommand::~InteractiveCommand() = default;
void InteractiveCommand::execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
    pImpl->execute(brain, options);
}
std::string InteractiveCommand::getName() const {
    return "interactive";
}

RunCommand::RunCommand() : pImpl(std::make_unique<RunCommand::Impl>()) {}
RunCommand::~RunCommand() = default;
void RunCommand::execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
    pImpl->execute(brain, options);
}
std::string RunCommand::getName() const {
    return "run";
}

BenchmarkCommand::BenchmarkCommand() : pImpl(std::make_unique<BenchmarkCommand::Impl>()) {}
BenchmarkCommand::~BenchmarkCommand() = default;
void BenchmarkCommand::execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
    pImpl->execute(brain, options);
}
std::string BenchmarkCommand::getName() const {
    return "benchmark";
}

TrainCommand::TrainCommand() : pImpl(std::make_unique<TrainCommand::Impl>()) {}
TrainCommand::~TrainCommand() = default;
void TrainCommand::execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) {
    pImpl->execute(brain, options);
}
std::string TrainCommand::getName() const {
    return "train";
}
