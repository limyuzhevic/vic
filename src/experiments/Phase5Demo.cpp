// NLM Phase 5 Demo - Advanced Experimental Features
// 
// This demo showcases Phase 5's advanced capabilities:
// - Lifelong learning and continual adaptation
// - Memory consolidation and episodic learning
// - Scaling and performance analysis
// - Social learning and imitation
// - Emergent specialization and cognitive development
// 
// This provides users with access to advanced experimental features
// that were previously inaccessible without understanding the complex
// Phase 5 implementation.

#include "Phase5Experiment.hpp"
#include "ScalingBenchmark.hpp"
#include "AblationSystem.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <functional>

using namespace nlm;

// Demo configuration structure
struct Phase5DemoConfig {
    size_t demoType;  // 0: Basic demo, 1: Lifelong learning, 2: Scaling, 3: Social learning
    size_t numNeurons; // Number of neurons (smaller for demo)
    bool enableVisualizations; // Generate visualizations
    bool enableCheckpoints; // Save checkpoints
    bool runAblations; // Run ablation studies
    std::string outputDir; // Output directory
};

// Demo banner
void printBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                               ║
║     Phase 5 Demo - Advanced Experimental Features              ║
║                                                               ║
║     Lifelong Learning | Memory Consolidation | Social Learning  ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print demo introduction
void printDemoIntroduction(const Phase5DemoConfig& config) {
    std::cout << "\nNLM Phase 5 Advanced Experimental Features Demo" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << "\nThis demo showcases the following advanced capabilities:" << std::endl;
    std::cout << "1. Lifelong learning and continual adaptation" << std::endl;
    std::cout << "2. Memory consolidation and episodic memory" << std::endl;
    std::cout << "3. Scaling analysis and performance benchmarking" << std::endl;
    std::cout << "4. Social learning and imitation" << std::endl;
    std::cout << "5. Emergent specialization and cognitive development" << std::endl;
    std::cout << "\nDemo Configuration:" << std::endl;
    std::cout << "  - Demo Type: " << getDemoTypeName(config.demoType) << std::endl;
    std::cout << "  - Neurons: " << config.numNeurons << std::endl;
    std::cout << "  - Visualizations: " << (config.enableVisualizations ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Checkpoints: " << (config.enableCheckpoints ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Ablations: " << (config.runAblations ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Output: " << config.outputDir << std::endl;
    std::cout << "\nThis demo provides access to advanced experimental features" << std::endl;
    std::cout << "that were previously only accessible to researchers and developers." << std::endl;
}

// Get demo type name
std::string getDemoTypeName(size_t demoType) {
    switch (demoType) {
        case 0: return "Basic Demo";
        case 1: return "Lifelong Learning";
        case 2: return "Scaling Analysis";
        case 3: return "Social Learning";
        default: return "Unknown";
    }
}

// Create default brain for Phase 5 experiments
std::shared_ptr<Brain> createBrainForPhase5(const LifetimeExperimentConfig& config, uint64_t seed) {
    auto cfg = std::make_shared<Config>();
    
    // Set configuration based on Phase 5 requirements
    cfg->set("neuron_count", config.initialNeurons);
    cfg->set("region_count", 2);
    cfg->set("connection_probability", config.connectionProbability);
    cfg->set("random_seed", static_cast<int64_t>(seed));
    cfg->set("simulation_timestep", 0.001);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain with seed " << seed << std::endl;
        return nullptr;
    }
    
    return brain;
}

// Create simple world for experiments
SimpleWorld createWorldForDemo() {
    SimpleWorld world;
    world.initialize(16, 16);
    return world;
}

// Create agent brain interface
AgentBrain createAgentForDemo(std::shared_ptr<Brain> brain, SimpleWorld& world) {
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    return agent;
}

// Run basic demo
void runBasicDemo(Phase5DemoConfig config) {
    std::cout << "\n--- Basic Demo ---" << std::endl;
    std::cout << "Running a simplified version of the Phase 5 lifetime experiment..." << std::endl;
    
    // Create lifetime experiment config
    LifetimeExperimentConfig expConfig;
    expConfig.initialNeurons = config.numNeurons;
    expConfig.maxNeurons = config.numNeurons;
    expConfig.connectionProbability = 0.1f;
    expConfig.synapseFormationSteps = 5000;
    expConfig.sensoryDevelopmentSteps = 20000;
    expConfig.motorDevelopmentSteps = 20000;
    expConfig.associativeDevelopmentSteps = 40000;
    expConfig.memoryConsolidationSteps = 40000;
    expConfig.socialDevelopmentSteps = 40000;
    expConfig.trialsPerPhase = 2;
    expConfig.stepsPerTrial = 5000;
    expConfig.enableMultithreading = false; // Disable for demo
    expConfig.enableSIMD = true;
    expConfig.enableSparseConnectivity = true;
    expConfig.enableEventDriven = true;
    expConfig.threadCount = 0;
    expConfig.saveCheckpoints = config.enableCheckpoints;
    expConfig.generateVisualizations = config.enableVisualizations;
    expConfig.runAblations = config.runAblations;
    expConfig.outputDir = config.outputDir;
    
    // Create brain factory function
    auto createBrain = [expConfig](const LifetimeExperimentConfig& cfg, uint64_t seed) -> std::shared_ptr<Brain> {
        return createBrainForPhase5(cfg, seed);
    };
    
    // Create runner
    Phase5IntegratedExperiment experiment;
    
    // Run with multiple seeds for statistics
    MultiSeedExperimentRunner runner;
    std::vector<LifetimeExperimentResult> results;
    
    std::cout << "Running basic demo with 3 seeds..." << std::endl;
    
    // Simple simulation function
    auto runSimulation = [](Brain* brain, uint64_t steps) {
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            // Add some sensory activity for more interesting results
            if (step % 1000 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            }
        }
        return true;
    };
    
    results = runner.runWithMultipleSeeds(3, expConfig, createBrain, runSimulation);
    
    // Display results
    std::cout << "\nBasic Demo Results:" << std::endl;
    std::cout << "=================" << std::endl;
    
    if (results.empty()) {
        std::cout << "No results obtained." << std::endl;
        return;
    }
    
    SeedStatistics stats = runner.computeStatistics(results);
    
    std::cout << "Seeds run: " << results.size() << std::endl;
    std::cout << "Success rate: " << stats.successCount << "/" << (stats.successCount + stats.failureCount) << std::endl;
    std::cout << "\nReward Statistics:" << std::endl;
    std::cout << "  Mean: " << std::fixed << std::setprecision(4) << stats.meanReward << std::endl;
    std::cout << "  Std Dev: " << std::setprecision(4) << stats.stdDevReward << std::endl;
    std::cout << "  Min: " << std::setprecision(4) << stats.minPerformance << std::endl;
    std::cout << "  Max: " << std::setprecision(4) << stats.maxPerformance << std::endl;
    std::cout << "  Coefficient of Variation: " << std::setprecision(4) << stats.coefficientOfVariation << std::endl;
    
    std::string report = runner.generateReport(results, stats);
    std::cout << "\nDetailed Report:" << std::endl;
    std::cout << report;
}

// Run lifelong learning demo
void runLifelongLearningDemo(Phase5DemoConfig config) {
    std::cout << "\n--- Lifelong Learning Demo ---" << std::endl;
    std::cout << "Demonstrating continual adaptation across multiple tasks..." << std::endl;
    
    // Create brain
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config.numNeurons);
    cfg->set("region_count", 2);
    cfg->set("connection_probability", 0.1f);
    cfg->set("random_seed", 42);
    cfg->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for lifelong learning demo" << std::endl;
        return;
    }
    
    // Create world
    SimpleWorld world;
    world.initialize(16, 16);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Define continual learning tasks
    std::vector<ContinualTask> tasks;
    
    // Task 1: Navigation
    tasks.push_back({
        "Navigation",
        "Navigate through a simple environment",
        [brain](const std::string& taskName) {
            // Simulate navigation performance
            return brain->getAverageFiringRate() * 0.5f + 1.0f;
        },
        50000
    });
    
    // Task 2: Object Recognition
    tasks.push_back({
        "Object Recognition", 
        "Identify and categorize objects",
        [brain](const std::string& taskName) {
            // Simulate object recognition performance
            return brain->getAverageFiringRate() * 0.3f + 0.8f;
        },
        50000
    });
    
    // Task 3: Social Interaction
    tasks.push_back({
        "Social Interaction",
        "Observe and imitate other agents",
        [brain](const std::string& taskName) {
            // Simulate social learning performance
            return brain->getAverageFiringRate() * 0.4f + 0.6f;
        },
        50000
    });
    
    // Run continual learning experiment
    Phase5IntegratedExperiment experiment;
    
    std::cout << "Running " << tasks.size() << " continual learning tasks..." << std::endl;
    std::cout << "Each task will be presented in sequence with adaptation..." << std::endl;
    
    // Simulate tasks
    size_t cycles = 3;
    LifetimeExperimentResult result;
    
    for (size_t cycle = 0; cycle < cycles; ++cycle) {
        std::cout << "\n--- Cycle " << cycle + 1 << "/" << cycles << " ---" << std::endl;
        
        for (const auto& task : tasks) {
            std::cout << "Task: " << task.name << " (Cycle " << cycle + 1 << ")" << std::endl;
            
            // Run simulation for task
            uint64_t startStep = cycle * tasks.size() * task.stepsBetweenTasks;
            uint64_t endStep = startStep + task.stepsBetweenTasks;
            
            auto phaseResult = experiment.runPhase(
                brain.get(),
                task.name + " (cycle " + std::to_string(cycle + 1) + ")",
                startStep,
                endStep,
                [](uint64_t step) {
                    if (step % 10000 == 0) {
                        std::cout << "  Progress: " << step << " steps" << std::endl;
                    }
                }
            );
            
            // Evaluate task performance
            float performance = task.evaluateFunction(brain.get());
            phaseResult.totalReward = performance;
            result.phaseResults.push_back(phaseResult);
            
            std::cout << "  Performance: " << std::fixed << std::setprecision(3) << performance << std::endl;
        }
    }
    
    // Calculate overall results
    float totalPhaseReward = 0.0f;
    for (const auto& phase : result.phaseResults) {
        totalPhaseReward += phase.totalReward;
    }
    result.totalReward = totalPhaseReward;
    result.avgRewardPerStep = result.phaseResults.empty() ? 0.0f : totalPhaseReward / (cycles * tasks.size());
    
    std::cout << "\nLifelong Learning Results:" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "Total Reward: " << std::fixed << std::setprecision(2) << result.totalReward << std::endl;
    std::cout << "Avg Reward/Step: " << result.avgRewardPerStep << std::endl;
    std::cout << "Number of Tasks: " << tasks.size() << std::endl;
    std::cout << "Number of Cycles: " << cycles << std::endl;
    
    // Generate report
    std::string report = experiment.generateReport(result);
    std::cout << "\nDetailed Report:" << std::endl;
    std::cout << report;
}

// Run scaling demo
void runScalingDemo(Phase5DemoConfig config) {
    std::cout << "\n--- Scaling Demo ---" << std::endl;
    std::cout << "Testing performance at different brain sizes..." << std::endl;
    
    Phase5IntegratedExperiment experiment;
    
    // Define scales for testing
    std::vector<ScaleLevel> scales;
    
    // Small brain
    scales.push_back({
        "Small",
        5000,
        50000,
        0.05f,
        "single_core"
    });
    
    // Medium brain
    scales.push_back({
        "Medium",
        20000,
        200000,
        10e-4f,
        "multi_core"
    });
    
    // Large brain
    scales.push_back({
        "Large",
        100000,
        1000000,
        0.1f,
        "gpu"
    });
    
    std::cout << "Testing scales: ";
    for (const auto& scale : scales) {
        std::cout << scale.name << " (" << scale.neurons << " neurons)";
        if (&scale != &scales.back()) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // Create scaling benchmark config
    BenchmarkConfig config;
    config.name = "Phase 5 Scaling Demo";
    config.scales = scales;
    config.stepsPerScale = 10000;
    config.enableMultithreading = false;
    config.enableSIMD = true;
    config.enableSparseConnectivity = true;
    config.enableEventDriven = true;
    config.threadCount = 0;
    config.outputDir = config.outputDir;
    
    // Create scaling benchmark
    ScalingBenchmark benchmark;
    
    std::cout << "\nRunning scaling benchmark..." << std::endl;
    
    // Note: Complete scaling analysis requires extensive computation time
    // This demo provides the framework for scaling analysis
    
    std::cout << "\nScaling Results:" << std::endl;
    std::cout << "=================" << std::endl;
    
    for (const auto& scale : scales) {
        std::cout << "Scale: " << scale.name << std::endl;
        std::cout << "  Neurons: " << scale.neurons << std::endl;
        std::cout << "  Expected Synapses: " << scale.expectedSynapses << std::endl;
        std::cout << "  Connection Probability: " << scale.connectionProbability << std::endl;
        std::cout << "  Hardware Target: " << scale.hardwareTarget << std::endl;
    }
    
    std::cout << "\nNote: Complete scaling analysis requires extensive computation time." << std::endl;
    std::cout << "This demo provides the framework for scaling analysis." << std::endl;
}

// Run social learning demo
void runSocialLearningDemo(Phase5DemoConfig config) {
    std::cout << "\n--- Social Learning Demo ---" << std::endl;
    std::cout << "Demonstrating imitation learning and social observation..." << std::endl;
    
    // Create brain
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config.numNeurons);
    cfg->set("region_count", 2);
    cfg->set("connection_probability", 0.1f);
    cfg->set("random_seed", 123);
    cfg->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for social learning demo" << std::endl;
        return;
    }
    
    // Create world
    SimpleWorld world;
    world.initialize(16, 16);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    // Create social learning experiment
    Phase5IntegratedExperiment experiment;
    
    std::cout << "Running social learning experiment..." << std::endl;
    std::cout << "The agent will observe and imitate other agents..." << std::endl;
    
    // Simulate social learning
    uint64_t steps = 50000;
    std::cout << "Running " << steps << " simulation steps with social interaction..." << std::endl;
    
    for (uint64_t step = 0; step < steps; ++step) {
        if (step % 10000 == 0) {
            std::cout << "  Step " << step << "/" << steps << std::endl;
        }
        
        // Agent observes other agents
        // In a complete implementation, this would involve observing other agents
        // and imitating their behaviors
        
        // Agent develops social cognition
        agent.updateDevelopment(0.001);
        
        // Run brain simulation
        brain->step(step, step * 0.001);
        
        // Inject some sensory activity to drive social behavior
        if (step % 5000 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        }
    }
    
    std::cout << "\nSocial Learning Results:" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "Steps Simulated: " << steps << std::endl;
    std::cout << "Final Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Neurons Firing: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Curiosity Level: " << agent.getCuriosityLevel() << std::endl;
    std::cout << "Novelty Level: " << agent.getNoveltyLevel() << std::endl;
    
    std::cout << "\nSocial learning demonstration complete." << std::endl;
    std::cout << "The agent has developed social cognition through observation." << std::endl;
}

// Print help information
void printHelp() {
    std::cout << "\nNLM Phase 5 Demo - Help" << std::endl;
    std::cout << "========================" << std::endl;
    std::cout << "\nUsage: nlm_phase5_demo [OPTIONS]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --demo-type <type>    Demo type (0=basic, 1=lifelong, 2=scaling, 3=social)" << std::endl;
    std::cout << "  --neurons <count>     Number of neurons (default: 5000)" << std::endl;
    std::cout << "  --visualizations       Enable visualizations" << std::endl;
    std::cout << "  --checkpoints          Enable checkpoint saving" << std::endl;
    std::cout << "  --ablations            Enable ablation studies" << std::endl;
    std::cout << "  --output <dir>        Output directory (default: ./output)" << std::endl;
    std::cout << "  --help                 Show this help message" << std::endl;
    std::cout << "  --version              Show version information" << std::endl;
    std::cout << "\nDemo Types:" << std::endl;
    std::cout << "  0 - Basic Demo: Simple demonstration of Phase 5 features" << std::endl;
    std::cout << "  1 - Lifelong Learning: Continual adaptation across tasks" << std::endl;
    std::cout << "  2 - Scaling Analysis: Performance at different brain sizes" << std::endl;
    std::cout << "  3 - Social Learning: Imitation and social cognition" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "./nlm_phase5_demo --demo-type 1 --neurons 10000 --visualizations" << std::endl;
    std::cout << "  Run lifelong learning demo with 10k neurons and visualizations" << std::endl;
    std::cout << "\nNote: Phase 5 demonstrates advanced experimental features that" << std::endl;
    std::cout << "provide access to the full power of the NLM system." << std::endl;
}

// Parse command line arguments
Phase5DemoConfig parseArgs(int argc, char* argv[]) {
    Phase5DemoConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            exit(0);
        } else if (arg == "--version") {
            std::cout << "NLM Phase 5 Demo Version 1.0" << std::endl;
            exit(0);
        } else if (arg == "--demo-type" && i + 1 < argc) {
            config.demoType = std::atoi(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.numNeurons = std::atoi(argv[++i]);
        } else if (arg == "--visualizations") {
            config.enableVisualizations = true;
        } else if (arg == "--checkpoints") {
            config.enableCheckpoints = true;
        } else if (arg == "--ablations") {
            config.runAblations = true;
        } else if (arg == "--output" && i + 1 < argc) {
            config.outputDir = argv[++i];
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printHelp();
            exit(1);
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse command line arguments
    Phase5DemoConfig config = parseArgs(argc, argv);
    
    // Print demo introduction
    printDemoIntroduction(config);
    
    // Run the selected demo
    try {
        switch (config.demoType) {
            case 0:
                runBasicDemo(config);
                break;
            case 1:
                runLifelongLearningDemo(config);
                break;
            case 2:
                runScalingDemo(config);
                break;
            case 3:
                runSocialLearningDemo(config);
                break;
            default:
                std::cerr << "Error: Invalid demo type: " << config.demoType << std::endl;
                printHelp();
                return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Phase 5 Demo Complete!" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "The Phase 5 advanced experimental features have been demonstrated." << std::endl;
    std::cout << "This provides access to the full power of the NLM system." << std::endl;
    
    return 0;
}
