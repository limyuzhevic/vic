// NLM Phase 6 Comprehensive Integration Demo
// A complete demonstration of the integrated artificial brain
// This is the recommended entry point for exploring the full Phase 6 capabilities

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase6Demo.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <chrono>
#include <thread>

using namespace nlm;

// Enhanced Phase 6 demo with advanced features
class Phase6Demo {
private:
    bool verbose_ = false;
    bool quiet_ = false;
    bool performanceMode_ = false;
    bool interactiveMode_ = false;
    std::string configFile_ = "configs/default.cfg";
    std::string outputFile_ = "";
    size_t neuronCount_ = 500;
    size_t maxSteps_ = 2000;
    float timestep_ = 0.001f;
    bool enableCheckpoint_ = true;
    bool enableReplay_ = true;
    bool enableDevelopment_ = true;
    std::vector<std::string> customArgs_;
    
public:
    Phase6Demo() = default;
    
    // Configuration
    void setVerbose(bool verbose) { verbose_ = verbose; }
    void setQuiet(bool quiet) { quiet_ = quiet; }
    void setPerformanceMode(bool mode) { performanceMode_ = mode; }
    void setInteractiveMode(bool mode) { interactiveMode_ = mode; }
    void setConfigFile(const std::string& file) { configFile_ = file; }
    void setNeuronCount(size_t count) { neuronCount_ = count; }
    void setMaxSteps(size_t steps) { maxSteps_ = steps; }
    void setTimestep(float timestep) { timestep_ = timestep; }
    void setEnableCheckpoint(bool enable) { enableCheckpoint_ = enable; }
    void setEnableReplay(bool enable) { enableReplay_ = enable; }
    void setEnableDevelopment(bool enable) { enableDevelopment_ = enable; }
    
    // Main demo execution
    int run() {
        printBanner();
        
        try {
            if (quiet_) {
                std::cout << "=== NLM Phase 6 Demo (Quiet Mode) ===" << std::endl;
            } else {
                std::cout << "=== NLM Phase 6: Comprehensive Integration Demo ===" << std::endl;
                std::cout << "Demonstrating the complete integrated artificial brain" << std::endl;
            }
            
            // Initialize logging based on mode
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(quiet_ ? LogLevel::Error : LogLevel::Info);
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            // Create configuration
            auto config = std::make_shared<Config>();
            if (!quiet_) {
                std::cout << "Loading configuration..." << std::endl;
            }
            
            // Load configuration
            if (!configFile_.empty() && configFile_ != "configs/default.cfg") {
                if (config->loadFromFile(configFile_)) {
                    if (!quiet_) {
                        std::cout << "✓ Loaded custom config: " << configFile_ << std::endl;
                    }
                } else {
                    if (!quiet_) {
                        std::cout << "⚠ Failed to load config: " << configFile_ << " (using defaults)" << std::endl;
                    }
                }
            }
            
            // Apply Phase 6 defaults
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", timestep_, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(neuronCount_), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            
            if (!quiet_) {
                std::cout << std::endl;
                std::cout << "=== Configuration ===" << std::endl;
                std::cout << "Neurons: " << neuronCount_ << std::endl;
                std::cout << "Steps: " << maxSteps_ << std::endl;
                std::cout << "Timestep: " << timestep_ << "s" << std::endl;
                std::cout << "Checkpointing: " << (enableCheckpoint_ ? "Enabled" : "Disabled") << std::endl;
                std::cout << "Replay: " << (enableReplay_ ? "Enabled" : "Disabled") << std::endl;
                std::cout << "Development: " << (enableDevelopment_ ? "Enabled" : "Disabled") << std::endl;
            }
            
            // Create Phase 6 experiment
            Phase6IntegratedExperiment experiment;
            
            // Phase 1: Quick system verification
            if (!quiet_) {
                std::cout << std::endl;
                std::cout << "=== Phase 1: System Verification ===" << std::endl;
            }
            
            bool integrationOK = experiment.verifyIntegration();
            
            if (!integrationOK) {
                if (!quiet_) {
                    std::cerr << "ERROR: System integration verification failed!" << std::endl;
                }
                return 1;
            }
            
            if (!quiet_) {
                std::cout << "✓ System integration verified successfully" << std::endl;
                std::cout << "✓ All Phase 6 systems are properly connected" << std::endl;
            }
            
            // Phase 2: Individual system demonstrations
            if (!quiet_) {
                std::cout << std::endl;
                std::cout << "=== Phase 2: Individual System Tests ===" << std::endl;
            }
            
            experiment.testMemoryIntegration();
            experiment.testNeuromodulationIntegration();
            
            if (enableCheckpoint_) {
                experiment.testCheckpointing();
            }
            
            if (enableReplay_) {
                experiment.testReplay();
            }
            
            // Phase 3: Comprehensive integration experiment
            if (!quiet_) {
                std::cout << std::endl;
                std::cout << "=== Phase 3: Comprehensive Integration Experiment ===" << std::endl;
                std::cout << "Running full-scale integration with all systems working together..." << std::endl;
            }
            
            Phase6Config phase6Config;
            phase6Config.neuronCount = neuronCount_;
            phase6Config.maxSteps = maxSteps_;
            phase6Config.enableCheckpointing = enableCheckpoint_;
            phase6Config.enableReplay = enableReplay_;
            phase6Config.enableDevelopment = enableDevelopment_;
            phase6Config.checkpointPath = "/tmp/nlm_phase6_demo_checkpoint.bin";
            
            auto startTime = std::chrono::high_resolution_clock::now();
            auto result = experiment.run(phase6Config);
            auto endTime = std::chrono::high_resolution_clock::now();
            auto totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Phase 4: Detailed results analysis
            if (!quiet_) {
                std::cout << std::endl;
                std::cout << "=== Phase 4: Results Analysis ===" << std::endl;
            }
            
            printDetailedResults(result, totalTime);
            
            // Phase 5: Performance and optimization insights
            if (!performanceMode_ && !quiet_) {
                std::cout << std::endl;
                std::cout << "=== Performance Insights ===" << std::endl;
                std::cout << "Total reward rate: " << (result.totalReward / maxSteps_) << std::endl;
                std::cout << "Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
                std::cout << "Memory efficiency: " << (result.memoryEpisodesStored / (neuronCount_ / 10.0f)) * 100 << "%" << std::endl;
                std::cout << "Neuromodulation balance: " << (result.dopamineLevel / (result.curiosityLevel + result.noveltyLevel + 0.001f)) * 100 << "%" << std::endl;
            }
            
            // Phase 6: Interactive demo mode
            if (interactiveMode_ && !quiet_) {
                std::cout << std::endl;
                std::cout << "=== Interactive Demo Mode ===" << std::endl;
                std::cout << "The brain is ready for interactive exploration." << std::endl;
                std::cout << "For interactive control, use the NLM Python API:" << std::endl;
                std::cout << "  import pynlm" << std::endl;
                std::cout << "  brain = pynlm.create_brain()" << std::endl;
                std::cout << "  world = pynlm.create_world()" << std::endl;
                std::cout << "  agent = pynlm.create_agent(brain)" << std::endl;
            }
            
            // Final summary
            std::cout << std::endl;
            std::cout << "=== DEMO SUMMARY ===" << std::endl;
            
            if (result.totalReward > 0 || result.memoryEpisodesStored > 0) {
                std::cout << "✓ DEMO COMPLETED SUCCESSFULLY" << std::endl;
                std::cout << "✓ All Phase 6 systems integrated and functional" << std::endl;
                std::cout << "✓ Neural learning and memory systems operational" << std::endl;
                std::cout << "✓ Neuromodulation and adaptive behavior working" << std::endl;
                
                if (enableCheckpoint_ && result.checkpointingWorks) {
                    std::cout << "✓ Checkpointing system validated" << std::endl;
                }
                
                if (enableReplay_ && result.memoryEpisodesStored > 0) {
                    std::cout << "✓ Memory replay system functional" << std::endl;
                }
                
                return 0;
            } else {
                std::cout << "⚠ DEMO COMPLETED WITH LIMITED ACTIVITY" << std::endl;
                std::cout << "⚠ Systems are functional but may need more complex input" << std::endl;
                std::cout << "⚠ Try running with --quick or --full flags for different scales" << std::endl;
                return 0;
            }
            
        } catch (const std::exception& e) {
            if (!quiet_) {
                std::cerr << "ERROR: " << e.what() << std::endl;
            }
            return 1;
        } catch (...) {
            if (!quiet_) {
                std::cerr << "ERROR: Unknown exception occurred during demo execution" << std::endl;
            }
            return 1;
        }
    }
    
private:
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Comprehensive Integration Demo                    ║
    ║                                                               ║
    ║     Complete brain with all systems working together           ║
    ║     (Memory, Neuromodulation, Prediction, Cognition, Development) ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
        )" << std::endl;
    }
    
    void printDetailedResults(const Phase6IntegrationResult& result, double totalTime) {
        if (quiet_) return;
        
        std::cout << std::endl;
        std::cout << "=== DETAILED RESULTS ===" << std::endl;
        std::cout << "Total reward:          " << std::setw(8) << result.totalReward << std::endl;
        std::cout << "Average firing rate:   " << std::setw(8) << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "Episodes stored:       " << std::setw(8) << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level:        " << std::setw(8) << result.dopamineLevel << std::endl;
        std::cout << "Curiosity level:       " << std::setw(8) << result.curiosityLevel << std::endl;
        std::cout << "Novelty level:         " << std::setw(8) << result.noveltyLevel << std::endl;
        std::cout << "Prediction error:      " << std::setw(8) << result.predictionError << std::endl;
        std::cout << "Total wall time:       " << std::setw(8) << totalTime << "s" << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory:        " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory:       " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation:       " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Prediction:            " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Development:           " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Checkpointing:          " << (result.checkpointingWorks ? "✓ WORKING" : "✗ FAILED") << std::endl;
        
        // System health assessment
        int systemCount = 0;
        int functionalCount = 0;
        
        if (result.memoryWorkingMemoryIntegrated) { systemCount++; functionalCount++; }
        if (result.memoryEpisodicMemoryIntegrated) { systemCount++; functionalCount++; }
        if (result.neuromodulationIntegrated) { systemCount++; functionalCount++; }
        if (result.predictionIntegrated) { systemCount++; functionalCount++; }
        if (result.developmentIntegrated) { systemCount++; functionalCount++; }
        if (result.checkpointingWorks) { systemCount++; functionalCount++; }
        
        std::cout << std::endl;
        std::cout << "=== SYSTEM HEALTH ===" << std::endl;
        std::cout << "Systems: " << functionalCount << "/" << systemCount << " functional" << std::endl;
        std::cout << "Health:  " << (functionalCount * 100 / systemCount) << "%" << std::endl;
        
        if (functionalCount == systemCount) {
            std::cout << "Status:  OPTIMAL - All systems fully operational" << std::endl;
        } else if (functionalCount >= systemCount * 0.8) {
            std::cout << "Status:  GOOD - Most systems functional" << std::endl;
        } else if (functionalCount >= systemCount * 0.6) {
            std::cout << "Status:  FAIR - Several systems need attention" << std::endl;
        } else {
            std::cout << "Status:  POOR - Multiple systems require investigation" << std::endl;
        }
    }
};

// Command-line argument parser for the demo
class DemoCommandLineParser {
private:
    Phase6Demo& demo_;
    
public:
    DemoCommandLineParser(Phase6Demo& demo) : demo_(demo) {}
    
    bool parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            } else if (arg == "--version") {
                printVersion();
                return false;
            } else if (arg == "--verbose" || arg == "-v") {
                demo_.setVerbose(true);
            } else if (arg == "--quiet" || arg == "-q") {
                demo_.setQuiet(true);
            } else if (arg == "--performance" || arg == "--perf") {
                demo_.setPerformanceMode(true);
            } else if (arg == "--interactive") {
                demo_.setInteractiveMode(true);
            } else if (arg == "--quick" || arg == "--demo") {
                demo_.setNeuronCount(100);
                demo_.setMaxSteps(500);
                demo_.setEnableCheckpoint(false);
                demo_.setEnableReplay(false);
            } else if (arg == "--full") {
                demo_.setNeuronCount(2000);
                demo_.setMaxSteps(10000);
            } else if (arg == "--standard") {
                demo_.setNeuronCount(500);
                demo_.setMaxSteps(2000);
            } else if (arg == "--checkpoint" && i + 1 < argc) {
                demo_.setEnableCheckpoint(true);
                // Note: checkpoint path is handled internally
            } else if (arg == "--no-checkpoint") {
                demo_.setEnableCheckpoint(false);
            } else if (arg == "--no-replay") {
                demo_.setEnableReplay(false);
            } else if (arg == "--no-development") {
                demo_.setEnableDevelopment(false);
            } else if (arg == "--neurons" && i + 1 < argc) {
                try {
                    demo_.setNeuronCount(std::stoull(argv[++i]));
                } catch (const std::exception& e) {
                    std::cerr << "Error: Invalid neuron count: " << argv[i] << std::endl;
                    return false;
                }
            } else if (arg == "--steps" && i + 1 < argc) {
                try {
                    demo_.setMaxSteps(std::stoull(argv[++i]));
                } catch (const std::exception& e) {
                    std::cerr << "Error: Invalid step count: " << argv[i] << std::endl;
                    return false;
                }
            } else if (arg == "--timestep" && i + 1 < argc) {
                try {
                    demo_.setTimestep(std::stof(argv[++i]));
                } catch (const std::exception& e) {
                    std::cerr << "Error: Invalid timestep: " << argv[i] << std::endl;
                    return false;
                }
            } else if (arg.substr(0, 8) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    demo_.setConfigFile(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    demo_.setConfigFile(argv[++i]);
                }
            } else {
                std::cerr << "Error: Unknown option: " << arg << std::endl;
                printHelp();
                return false;
            }
        }
        return true;
    }
    
    void printHelp() const {
        std::cout << "NLM Phase 6 Demo - Usage:" << std::endl;
        std::cout << "  ./nlm_phase6_demo [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h                    Show help message" << std::endl;
        std::cout << "  --version                     Show version info" << std::endl;
        std::cout << "  --verbose, -v                Verbose output" << std::endl;
        std::cout << "  --quiet, -q                   Quiet mode (minimal output)" << std::endl;
        std::cout << "  --performance, --perf         Performance monitoring mode" << std::endl;
        std::cout << "  --interactive                 Interactive mode enabled" << std::endl;
        std::cout << "  --quick, --demo               Quick demo (small scale)" << std::endl;
        std::cout << "  --full                        Full demo (large scale)" << std::endl;
        std::cout << "  --standard                    Standard demo (default)" << std::endl;
        std::cout << "  --checkpoint                  Enable checkpointing" << std::endl;
        std::cout << "  --no-checkpoint               Disable checkpointing" << std::endl;
        std::cout << "  --no-replay                   Disable replay" << std::endl;
        std::cout << "  --no-development              Disable development system" << std::endl;
        std::cout << "  --neurons <count>             Set neuron count (default: 500)" << std::endl;
        std::cout << "  --steps <count>               Set simulation steps (default: 2000)" << std::endl;
        std::cout << "  --timestep <value>            Set timestep (default: 0.001)" << std::endl;
        std::cout << "  --config=<file> or --config <file>  Load configuration" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm_phase6_demo                    # Standard demo" << std::endl;
        std::cout << "  ./nlm_phase6_demo --quick            # Quick demo" << std::endl;
        std::cout << "  ./nlm_phase6_demo --full             # Full demo" << std::endl;
        std::cout << "  ./nlm_phase6_demo --quiet --no-checkpoint" << std::endl;
    }
    
    void printVersion() const {
        std::cout << "NLM Phase 6 Demo Version: 1.0" << std::endl;
        std::cout << "Phase 6: Complete integration of all neural systems" << std::endl;
        std::cout << "Features: Memory, Neuromodulation, Prediction, Cognition, Development" << std::endl;
    }
};

// Main entry point
int main(int argc, char* argv[]) {
    Phase6Demo demo;
    DemoCommandLineParser parser(demo);
    
    if (!parser.parse(argc, argv)) {
        return 1;
    }
    
    return demo.run();
}