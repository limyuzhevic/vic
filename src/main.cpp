// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase integrates all existing systems into a coherent artificial brain.
// Phase 6 focuses on:
// - All memory systems connected to neural processing
// - Neuromodulation integration with plasticity
// - Prediction system as central organizing principle
// - Development affecting plasticity rates
// - Checkpoint save/load working
// - Replay and consolidation functional
// - Complete brain loop integration

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "ProgressMonitor.hpp"
#include "BrainFactory.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates all systems:                        ║
    ║     - Memory systems (working, episodic, associative)        ║
    ║     - Neuromodulation (dopamine, curiosity, novelty)          ║
    ║     - Prediction and cognitive systems                        ║
    ║     - Development and structural plasticity                    ║
    ║     - Checkpointing and replay                                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printProgress(const std::string& message, int step, int totalSteps) {
    static int lastStep = -1;
    if (step != lastStep) {
        float progress = static_cast<float>(step) / totalSteps * 100.0f;
        std::cout << "\r[" << std::setw(3) << static_cast<int>(progress) << "%] " 
                  << message << " (Step " << step << "/" << totalSteps << ")..."; 
        std::cout.flush();
        lastStep = step;
    }
}

void printCompletion() {
    std::cout << std::endl;
    std::cout.flush();
}

void printHelp() {
    std::cout << "NLM Phase 6 - Advanced Neural Learning Machine CLI" << std::endl;
    std::cout << "=============================================" << std::endl << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm [--help] [--version] [mode] [options]" << std::endl << std::endl;
    
    std::cout << "MODES:" << std::endl;
    std::cout << "  demo         Run Phase 6 integration demo (default)" << std::endl;
    std::cout << "  experiment   Run custom experiment with specified configuration" << std::endl;
    std::cout << "  benchmark    Run performance benchmark" << std::endl;
    std::cout << "  debug        Run with debug output and extended logging" << std::endl;
    std::cout << "  validate     Validate brain configuration and system integrity" << std::endl;
    std::cout << "  benchmark    Run performance benchmark tests" << std::endl;
    std::cout << "  compare      Compare results between different runs" << std::endl;
    std::cout << "  restore      Restore from checkpoint" << std::endl;
    std::cout << "  replay       Replay stored episodes" << std::endl;
    
    std::cout << std::endl << "GLOBAL OPTIONS:" << std::endl;
    std::cout << "  --help, -h       Show this help message" << std::endl;
    std::cout << "  --version        Show version information" << std::endl;
    std::cout << "  --verbose, -v    Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q      Reduce output to essential information" << std::endl;
    std::cout << "  --config <file>  Load configuration from file (JSON format)" << std::endl;
    std::cout << "  --output <dir>   Set output directory for results and logs" << std::endl;
    std::cout << "  --threads <n>    Number of threads for parallel processing" << std::endl;
    std::cout << "  --disable <feat> Disable specific feature (memory, neuromod, prediction)" << std::endl;
    std::cout << "  --enable <feat>  Enable specific feature (all features enabled by default)" << std::endl;
    std::cout << "  --seed <n>       Set random seed for reproducible experiments" << std::endl;
    
    std::cout << std::endl << "EXPERIMENT-SPECIFIC OPTIONS:" << std::endl;
    std::cout << "  --neurons <n>    Number of neurons in the brain" << std::endl;
    std::cout << "  --steps <n>      Maximum simulation steps" << std::endl;
    std::cout << "  --regions <n>    Number of brain regions" << std::endl;
    std::cout << "  --density <f>    Connection probability (0.0-1.0)" << std::endl;
    std::cout << "  --checkpoint <path> Path for saving/loading checkpoints" << std::endl;
    std::cout << "  --replay         Enable episodic memory replay" << std::endl;
    std::cout << "  --noreplay        Disable episodic memory replay" << std::endl;
    std::cout << "  --development     Enable developmental stages" << std::endl;
    std::cout << "  --nodevelopment   Disable developmental stages" << std::endl;
    std::cout << "  --learning        Enable learning mechanisms (STDP, Hebbian)" << std::endl;
    std::cout << "  --nolearning     Disable learning mechanisms" << std::endl;
    
    std::cout << std::endl << "ADVANCED OPTIONS:" << std::endl;
    std::cout << "  --experiment <name>  Set experiment name for result tracking" << std::endl;
    std::cout << "  --metrics <file>      File to save performance metrics" << std::endl;
    std::cout << "  --validation <level>  Validation level: basic, thorough, exhaustive" << std::endl;
    std::cout << "  --performance <mode>  Performance mode: standard, optimized, aggressive" << std::endl;
    std::cout << "  --timing              Record detailed timing information" << std::endl;
    std::cout << "  --memory              Monitor memory usage" << std::endl;
    std::cout << "  --network <spec>      Network topology specification" << std::endl;
    std::cout << "  --plasticity <spec>   Plasticity configuration specification" << std::endl;
    
    std::cout << std::endl << "PREDEFINED EXPERIMENTS:" << std::endl;
    std::cout << "  --predefined <name>   Run predefined experiment (small, medium, large, full)" << std::endl;
    std::cout << "                       small: 100 neurons, 1000 steps" << std::endl;
    std::cout << "                       medium: 500 neurons, 5000 steps" << std::endl;
    std::cout << "                       large: 2000 neurons, 20000 steps" << std::endl;
    std::cout << "                       full: 10000 neurons, 100000 steps" << std::endl;
    
    std::cout << std::endl << "PHASE 4 COMPATIBILITY (for backward compatibility):" << std::endl;
    std::cout << "  --phase2             Run Phase 2-style basic neural computation test" << std::endl;
    std::cout << "  --phase3             Run Phase 3-style world interaction test" << std::endl;
    std::cout << "  --phase4             Run Phase 4-style cognitive emergence test" << std::endl;
    std::cout << "  --phase5             Run Phase 5-style performance optimization test" << std::endl;
}

void printVersion() {
    std::cout << "NLM Phase 6 - Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Built on Phase 6: Final Integration" << std::endl;
    std::cout << "Architecture: Modular brain systems with factory initialization" << std::endl;
    std::cout << "Features: Memory systems, neuromodulation, prediction, development" << std::endl;
    std::cout << "Author: NLM Research Team" << std::endl;
    std::cout << "License: MIT" << std::endl;
}

void printExperimentInfo(const Phase6Config& config) {
    std::cout << "Experiment Configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Max Steps: " << config.maxSteps << std::endl;
    std::cout << "  Connection Density: " << config.connectionProbability << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << "  Checkpoint Path: " << config.checkpointPath << std::endl;
    std::cout << std::endl;
}

void printSystemStatus(const std::shared_ptr<Brain>& brain, const std::string& title) {
    std::cout << "=== " << title << " ===" << std::endl;
    if (brain) {
        brain->logStatus();
    } else {
        std::cout << "Brain not initialized!" << std::endl;
    }
    std::cout << std::endl;
}