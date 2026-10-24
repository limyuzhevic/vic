#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>

// Forward declarations for the different NLM components
namespace nlm {
    class Config;
    class Brain;
    class SimpleWorld;
    class AgentBrain;
}

// Command base class
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual int execute(const std::vector<std::string>& args) = 0;
};

// Demo command - runs different NLM demos
class DemoCommand : public Command {
public:
    std::string getName() const override { return "demo"; }
    std::string getDescription() const override { 
        return "Run NLM demonstrations (phase2, phase3, phase4, phase6)"; 
    }
    std::string getUsage() const override {
        return "nlm demo <phase> [--episodes N] [--steps N] [--config file]";
    }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "NLM Demo Command" << std::endl;
        std::cout << "This command will run NLM demonstrations" << std::endl;
        std::cout << "Note: This is a framework - actual demo implementations exist in src/experiments/" << std::endl;
        
        if (args.size() < 1) {
            std::cout << "Available phases:" << std::endl;
            std::cout << "  phase2 - Basic neural computation (current main.cpp)" << std::endl;
            std::cout << "  phase3 - World interaction (src/experiments/Phase3Demo.cpp)" << std::endl;
            std::cout << "  phase4 - Advanced cognition (src/experiments/Phase4Demo.cpp)" << std::endl;
            std::cout << "  phase6 - Full integration test (src/experiments/Phase6Demo.cpp)" << std::endl;
            return 1;
        }
        
        std::string phase = args[0];
        int episodes = 5;
        int steps = 200;
        std::string configFile = "configs/default.cfg";
        
        // Parse additional arguments
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--episodes" && i + 1 < args.size()) {
                episodes = std::stoi(args[i + 1]);
                i++;
            } else if (args[i] == "--steps" && i + 1 < args.size()) {
                steps = std::stoi(args[i + 1]);
                i++;
            } else if (args[i] == "--config" && i + 1 < args.size()) {
                configFile = args[i + 1];
                i++;
            }
        }
        
        std::cout << "Running demo for phase: " << phase << std::endl;
        std::cout << "Episodes: " << episodes << ", Steps: " << steps << std::endl;
        std::cout << "Config: " << configFile << std::endl;
        
        // For now, just print what would be executed
        std::cout << "Note: Actual demo execution requires linking with experiment libraries" << std::endl;
        std::cout << "Available demo executables:" << std::endl;
        std::cout << "  ./nlm_phase2_demo - Basic neural computation" << std::endl;
        std::cout << "  ./nlm_phase3_demo - World interaction" << std::endl;
        std::cout << "  ./nlm_phase4_demo - Advanced cognition" << std::endl;
        std::cout << "  ./nlm_phase6_demo - Full integration test" << std::endl;
        
        return 0;
    }
};

// Test command - runs NLM tests
class TestCommand : public Command {
public:
    std::string getName() const override { return "test"; }
    std::string getDescription() const override { 
        return "Run NLM tests (unit tests, integration tests)"; 
    }
    std::string getUsage() const override {
        return "nlm test <type> [--verbose] [--output file]";
    }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "NLM Test Command" << std::endl;
        std::cout << "This command will run NLM tests" << std::endl;
        std::cout << "Note: This is a framework - actual tests exist in tests/" << std::endl;
        
        std::string testType = "all";
        bool verbose = false;
        std::string outputFile;
        
        // Parse arguments
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--verbose") {
                verbose = true;
            } else if (args[i] == "--output" && i + 1 < args.size()) {
                outputFile = args[i + 1];
                i++;
            } else if (testType == "all") {
                testType = args[i];
            }
        }
        
        std::cout << "Running tests for type: " << testType << std::endl;
        std::cout << "Verbose: " << (verbose ? "yes" : "no") << std::endl;
        std::cout << "Output: " << (outputFile.empty() ? "stdout" : outputFile) << std::endl;
        
        // Available test types
        std::map<std::string, std::string> availableTests = {
            {"types", "Data types tests"},
            {"config", "Configuration tests"},
            {"random", "Random number generation tests"},
            {"clock", "Clock/simulation time tests"},
            {"neuron", "Neuron functionality tests"},
            {"synapse", "Synapse functionality tests"},
            {"stdp", "STDP plasticity tests"},
            {"brain", "Brain integration tests"},
            {"all", "All tests"}
        };
        
        std::cout << "Available test types:" << std::endl;
        for (const auto& pair : availableTests) {
            std::cout << "  " << pair.first << " - " << pair.second << std::endl;
        }
        
        std::cout << "Note: Actual test execution requires linking with test executables" << std::endl;
        std::cout << "Available test executables:" << std::endl;
        std::cout << "  ./nlm_test - Full test suite (Phase 2 tests)" << std::endl;
        
        return 0;
    }
};

// Benchmark command - runs NLM benchmarks
class BenchmarkCommand : public Command {
public:
    std::string getName() const override { return "benchmark"; }
    std::string getDescription() const override { 
        return "Run NLM performance benchmarks"; 
    }
    std::string getUsage() const override {
        return "nlm benchmark <neuron_counts> [--steps N] [--output file]";
    }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "NLM Benchmark Command" << std::endl;
        std::cout << "This command runs NLM performance benchmarks" << std::endl;
        std::cout << "Note: This is a framework - actual benchmark exists in src/benchmark.cpp" << std::endl;
        
        std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
        size_t steps = 1000;
        std::string outputFile;
        
        // Parse arguments
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--steps" && i + 1 < args.size()) {
                steps = std::stoul(args[i + 1]);
                i++;
            } else if (args[i] == "--output" && i + 1 < args.size()) {
                outputFile = args[i + 1];
                i++;
            } else {
                try {
                    neuronCounts.clear();
                    neuronCounts.push_back(std::stoul(args[i]));
                } catch (...) {
                    // Try to parse as comma-separated list
                    std::stringstream ss(args[i]);
                    std::string item;
                    while (std::getline(ss, item, ',')) {
                        try {
                            neuronCounts.push_back(std::stoul(item));
                        } catch (...) {
                            std::cout << "Warning: Invalid neuron count: " << item << std::endl;
                        }
                    }
                }
            }
        }
        
        std::cout << "Running benchmarks for neuron counts: ";
        for (size_t n : neuronCounts) {
            std::cout << n << " ";
        }
        std::cout << std::endl;
        std::cout << "Steps per benchmark: " << steps << std::endl;
        std::cout << "Output: " << (outputFile.empty() ? "stdout" : outputFile) << std::endl;
        
        std::cout << "Note: Actual benchmark execution requires linking with benchmark executable" << std::endl;
        std::cout << "Available benchmark executable:" << std::endl;
        std::cout << "  ./benchmark - Performance benchmark (src/benchmark.cpp)" << std::endl;
        
        return 0;
    }
};

// Run command - run a single simulation
class RunCommand : public Command {
public:
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { 
        return "Run a single NLM simulation"; 
    }
    std::string getUsage() const override {
        return "nlm run [--config file] [--steps N] [--output file]";
    }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "NLM Run Command" << std::endl;
        std::cout << "This command runs a single NLM simulation" << std::endl;
        std::cout << "Note: This is a framework - requires full NLM integration" << std::endl;
        
        std::string configFile = "configs/default.cfg";
        size_t steps = 1000;
        std::string outputFile;
        
        // Parse arguments
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--config" && i + 1 < args.size()) {
                configFile = args[i + 1];
                i++;
            } else if (args[i] == "--steps" && i + 1 < args.size()) {
                steps = std::stoul(args[i + 1]);
                i++;
            } else if (args[i] == "--output" && i + 1 < args.size()) {
                outputFile = args[i + 1];
                i++;
            }
        }
        
        std::cout << "Running simulation with config: " << configFile << std::endl;
        std::cout << "Steps: " << steps << std::endl;
        std::cout << "Output: " << (outputFile.empty() ? "stdout" : outputFile) << std::endl;
        
        std::cout << "Note: Full simulation requires complete NLM integration (Phase 2-6)" << std::endl;
        std::cout << "The main demo (nlm) currently implements Phase 2: Real Neural Computation" << std::endl;
        
        return 0;
    }
};

// Info command - display NLM information
class InfoCommand : public Command {
public:
    std::string getName() const override { return "info"; }
    std::string getDescription() const override { 
        return "Display NLM system information"; 
    }
    std::string getUsage() const override {
        return "nlm info [--verbose]";
    }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                     NLM — 熙然 (Neural Learning Machine)                     ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║                     Phase 2: Real Neural Computation             ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║     An experimental artificial developmental brain.            ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║                      Implementation Status                       ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        
        bool verbose = false;
        for (const std::string& arg : args) {
            if (arg == "--verbose") {
                verbose = true;
            }
        }
        
        std::cout << "║     Core Neural Dynamics:     ✓ LEaky Integrate-and-Fire (LIF)      ║" << std::endl;
        std::cout << "║     Event-Driven Processing:   ✓ Spike propagation with delays      ║" << std::endl;
        std::cout << "║     Plasticity Mechanisms:     ✓ STDP and Hebbian learning        ║" << std::endl;
        std::cout << "║     Structural Plasticity:     ✓ Synaptogenesis and pruning       ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        
        if (verbose) {
            std::cout << "║                    Available Components:                           ║" << std::endl;
            std::cout << "║                                                               ║" << std::endl;
            std::cout << "║  - Neural Elements: Neuron, Synapse, NeuralPopulation           ║" << std::endl;
            std::cout << "║  - Brain Architecture: NeuralRegion, Brain                       ║" << std::endl;
            std::cout << "║  - Dynamics: NeuralDynamics, SpikeSystem                        ║" << std::endl;
            std::cout << "║  - Plasticity: STDP, Hebbian, StructuralPlasticity              ║" << std::endl;
            std::cout << "║  - Development: DevelopmentSystem, Synaptogenesis, Pruning       ║" << std::endl;
            std::cout << "║  - Neuromodulation: Reward, PredictionError, Novelty, Curiosity ║" << std::endl;
            std::cout << "║  - Memory: WorkingMemory, EpisodicMemory, SemanticMemory       ║" << std::endl;
            std::cout << "║  - Cognition: ConceptFormation, NeuralPlanner                   ║" << std::endl;
            std::cout << "║  - Sensory: Vision, Audio, InternalSignals                      ║" << std::endl;
            std::cout << "║  - Motor: MotorSystem, Action                                    ║" << std::endl;
            std::cout << "║  - Environment: SimpleWorld, Environment                         ║" << std::endl;
            std::cout << "║  - Agent: AgentBrain, SensoryPercept                            ║" << std::endl;
            std::cout << "║                                                               ║" << std::endl;
            std::cout << "║                    Available Demos:                              ║" << std::endl;
            std::cout << "║                                                               ║" << std::endl;
            std::cout << "║  - phase2 (nlm): Basic neural computation test                 ║" << std::endl;
            std::cout << "║  - phase3 (nlm_phase3_demo): World interaction                  ║" << std::endl;
            std::cout << "║  - phase4 (nlm_phase4_demo): Advanced cognition                ║" << std::endl;
            std::cout << "║  - phase6 (nlm_phase6_demo): Full integration test             ║" << std::endl;
            std::cout << "║                                                               ║" << std::endl;
            std::cout << "║                    Available Tests:                              ║" << std::endl;
            std::cout << "║                                                               ║" << std::endl;
            std::cout << "║  - nlm_test: Unit test suite (Phase 2)                         ║" << std::endl;
            std::cout << "║                                                               ║" << std::endl;
            std::cout << "║                    Python Bindings:                              ║" << std::endl;
            std::cout << "║                                                               ║" << std::endl;
            std::cout << "║  - pynlm: Python bindings for NLM (pybind11)                   ║" << std::endl;
            std::cout << "║                                                               ║" << std::endl;
        }
        
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║                 Usage Examples:                                   ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  nlm demo phase2           # Run Phase 2 neural computation demo     ║" << std::endl;
        std::cout << "║  nlm test neuron           # Run neuron unit tests                 ║" << std::endl;
        std::cout << "║  nlm benchmark 100,500     # Run benchmarks for 100 and 500 neurons ║" << std::endl;
        std::cout << "║  nlm info --verbose        # Show detailed system information     ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        return 0;
    }
};

// Help command - display help information
class HelpCommand : public Command {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { 
        return "Display help information"; 
    }
    std::string getUsage() const override {
        return "nlm help [command]";
    }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "NLM — 熙然 (Neural Learning Machine) Command Line Interface" << std::endl;
        std::cout << std::endl;
        
        if (args.empty()) {
            std::cout << "Available commands:" << std::endl;
            std::cout << std::endl;
            
            // List all commands
            std::vector<Command*> commands = {
                new DemoCommand(),
                new TestCommand(),
                new BenchmarkCommand(),
                new RunCommand(),
                new InfoCommand(),
                new HelpCommand()
            };
            
            for (Command* cmd : commands) {
                std::cout << "  " << cmd->getName() << std::endl;
                std::cout << "    " << cmd->getDescription() << std::endl;
                std::cout << "    Usage: " << cmd->getUsage() << std::endl;
                std::cout << std::endl;
                delete cmd;
            }
        } else {
            std::string cmdName = args[0];
            std::vector<Command*> commands = {
                new DemoCommand(),
                new TestCommand(),
                new BenchmarkCommand(),
                new RunCommand(),
                new InfoCommand(),
                new HelpCommand()
            };
            
            for (Command* cmd : commands) {
                if (cmd->getName() == cmdName) {
                    std::cout << "Command: " << cmd->getName() << std::endl;
                    std::cout << "Description: " << cmd->getDescription() << std::endl;
                    std::cout << "Usage: " << cmd->getUsage() << std::endl;
                    delete cmd;
                    return 0;
                }
                delete cmd;
            }
            
            std::cout << "Unknown command: " << cmdName << std::endl;
            return 1;
        }
        
        return 0;
    }
};

// Main CLI class
class NLMCLI {
private:
    std::map<std::string, Command*> commands;
    std::string programName;
    
public:
    NLMCLI(const std::string& name) : programName(name) {
        // Register all commands
        commands["demo"] = new DemoCommand();
        commands["test"] = new TestCommand();
        commands["benchmark"] = new BenchmarkCommand();
        commands["run"] = new RunCommand();
        commands["info"] = new InfoCommand();
        commands["help"] = new HelpCommand();
    }
    
    ~NLMCLI() {
        for (auto& pair : commands) {
            delete pair.second;
        }
    }
    
    int run(int argc, char* argv[]) {
        if (argc < 2) {
            printUsage();
            return 1;
        }
        
        std::string commandName = argv[1];
        std::vector<std::string> args;
        
        // Convert remaining arguments to strings
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        // Find and execute the command
        auto it = commands.find(commandName);
        if (it == commands.end()) {
            std::cerr << "Error: Unknown command: " << commandName << std::endl;
            std::cerr << "Use 'nlm help' for available commands." << std::endl;
            printUsage();
            return 1;
        }
        
        return it->second->execute(args);
    }
    
    void printUsage() const {
        std::cout << "Usage: " << programName << " <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Available commands:" << std::endl;
        for (const auto& pair : commands) {
            std::cout << "  " << pair.first << " - " << pair.second->getDescription() << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Use 'nlm help <command>' for detailed information about a specific command." << std::endl;
    }
};

// Global CLI instance
NLMCLI cli("nlm");

int main(int argc, char* argv[]) {
    std::cout << "NLM — 熙然 (Neural Learning Machine) CLI" << std::endl;
    std::cout << "Version 0.1.0 - Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Type 'nlm help' for available commands" << std::endl;
    std::cout << std::endl;
    
    return cli.run(argc, argv);
}
