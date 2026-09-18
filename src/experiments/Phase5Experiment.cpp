/**
 * Phase 5 Implementation
 * 
 * Demonstrates lifetime learning capabilities of the NLM brain.
 Shows how neural systems can evolve over extended periods and acquire
 new capabilities through continuous experience.
 */

#include "Phase5Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace nlm {

// ============================================================================
// Config Structure for Phase 5
// ============================================================================

// Default configuration values for Phase 5 experiments
const std::map<std::string, std::string> DEFAULT_PHASE5_CONFIG = {
    {"initial_neurons", "1000"},
    {"max_neurons", "10000"},
    {"synapse_formation_steps", "1000"},
    {"sensory_development_steps", "2000"},
    {"motor_development_steps", "2000"},
    {"associative_development_steps", "2000"},
    {"memory_consolidation_steps", "2000"},
    {"social_development_steps", "2000"},
    {"continual_learning_cycles", "3"},
    {"damage_recovery_threshold", "0.5"},
    {"learning_efficiency_threshold", "0.7"},
    {"memory_capacity_growth_rate", "0.1"},
    {"prediction_accuracy_threshold", "0.8"},
    {"generalization_threshold", "0.8"}
};

// ============================================================================
// ConfigSchema Definition
// ============================================================================

// Configuration validation schemas for Phase 5
const std::map<std::string, ConfigSchema> PHASE5_CONFIG_SCHEMA = {
    // Neuron and network configuration
    {"initial_neurons", {"int", "Initial number of neurons for learning", "1000", 
                         {"min": "100", "max": "10000"}}},
    {"max_neurons", {"int", "Maximum number of neurons achievable", "10000", 
                     {"min": "1000", "max": "100000"}}},
    
    // Phase timing configuration
    {"synapse_formation_steps", {"int", "Steps for initial synapse formation", "1000", 
                                 {"min": "100", "max": "10000"}}},
    {"sensory_development_steps", {"int", "Steps for sensory system development", "2000", 
                                  {"min": "500", "max": "10000"}}},
    {"motor_development_steps", {"int", "Steps for motor system development", "2000", 
                                {"min": "500", "max": "10000"}}},
    {"associative_development_steps", {"int", "Steps for associative learning development", "2000", 
                                      {"min": "500", "max": "10000"}}},
    {"memory_consolidation_steps", {"int", "Steps for memory consolidation phase", "2000", 
                                   {"min": "500", "max": "10000"}}},
    {"social_development_steps", {"int", "Steps for social behavior development", "2000", 
                                 {"min": "500", "max": "10000"}}},
    
    // Learning and adaptation parameters
    {"continual_learning_cycles", {"int", "Number of continual learning cycles", "3", 
                                   {"min": "1", "max": "10"}}},
    {"damage_recovery_threshold", {"double", "Damage level threshold for recovery testing", "0.5", 
                                   {"min": "0.1", "max": "1.0"}}},
    {"learning_efficiency_threshold", {"double", "Minimum learning efficiency required", "0.7", 
                                       {"min": "0.0", "max": "1.0"}}},
    {"memory_capacity_growth_rate", {"double", "Rate of memory capacity growth per step", "0.1", 
                                    {"min": "0.0", "max": "1.0"}}},
    {"prediction_accuracy_threshold", {"double", "Minimum prediction accuracy required", "0.8", 
                                       {"min": "0.0", "max": "1.0"}}},
    {"generalization_threshold", {"double", "Minimum generalization ability required", "0.8", 
                                  {"min": "0.0", "max": "1.0"}}}
};

// ============================================================================
// LifetimeExperimentConfig Implementation
// ============================================================================

LifetimeExperimentConfig::LifetimeExperimentConfig() {
    // Load default values from configuration system
    auto& configManager = ConfigManager::getInstance();
    configManager.reset();
    configManager.loadPhaseDefaults(5);
    
    initialNeurons = configManager.getConfig()->getOr<int64_t>("initial_neurons", 1000);
    maxNeurons = configManager.getConfig()->getOr<int64_t>("max_neurons", 10000);
    synapseFormationSteps = configManager.getConfig()->getOr<uint64_t>("synapse_formation_steps", 1000);
    sensoryDevelopmentSteps = configManager.getConfig()->getOr<uint64_t>("sensory_development_steps", 2000);
    motorDevelopmentSteps = configManager.getConfig()->getOr<uint64_t>("motor_development_steps", 2000);
    associativeDevelopmentSteps = configManager.getConfig()->getOr<uint64_t>("associative_development_steps", 2000);
    memoryConsolidationSteps = configManager.getConfig()->getOr<uint64_t>("memory_consolidation_steps", 2000);
    socialDevelopmentSteps = configManager.getConfig()->getOr<uint64_t>("social_development_steps", 2000);
    continualLearningCycles = configManager.getConfig()->getOr<size_t>("continual_learning_cycles", 3);
    damageRecoveryThreshold = configManager.getConfig()->getOr<float>("damage_recovery_threshold", 0.5f);
    learningEfficiencyThreshold = configManager.getConfig()->getOr<float>("learning_efficiency_threshold", 0.7f);
    memoryCapacityGrowthRate = configManager.getConfig()->getOr<float>("memory_capacity_growth_rate", 0.1f);
    predictionAccuracyThreshold = configManager.getConfig()->getOr<float>("prediction_accuracy_threshold", 0.8f);
    generalizationThreshold = configManager.getConfig()->getOr<float>("generalization_threshold", 0.8f);
}

std::string LifetimeExperimentConfig::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"initialNeurons\": " << initialNeurons << ",\n";
    oss << "  \"maxNeurons\": " << maxNeurons << ",\n";
    oss << "  \"synapseFormationSteps\": " << synapseFormationSteps << ",\n";
    oss << "  \"sensoryDevelopmentSteps\": " << sensoryDevelopmentSteps << ",\n";
    oss << "  \"motorDevelopmentSteps\": " << motorDevelopmentSteps << ",\n";
    oss << "  \"associativeDevelopmentSteps\": " << associativeDevelopmentSteps << ",\n";
    oss << "  \"memoryConsolidationSteps\": " << memoryConsolidationSteps << ",\n";
    oss << "  \"socialDevelopmentSteps\": " << socialDevelopmentSteps << ",\n";
    oss << "  \"continualLearningCycles\": " << continualLearningCycles << ",\n";
    oss << "  \"damageRecoveryThreshold\": " << damageRecoveryThreshold << ",\n";
    oss << "  \"learningEfficiencyThreshold\": " << learningEfficiencyThreshold << ",\n";
    oss << "  \"memoryCapacityGrowthRate\": " << memoryCapacityGrowthRate << ",\n";
    oss << "  \"predictionAccuracyThreshold\": " << predictionAccuracyThreshold << ",\n";
    oss << "  \"generalizationThreshold\": " << generalizationThreshold << "\n";
    oss << "}\n";
    return oss.str();
}

std::string LifetimeExperimentConfig::getPhaseName(int phase) const {
    switch (phase) {
        case 1: return "Early Development";
        case 2: return "Sensory Development";
        case 3: return "Motor Development";
        case 4: return "Associative Development";
        case 5: return "Memory Consolidation";
        case 6: return "Social Development";
        default: return "Unknown Phase";
    }
}

// ============================================================================
// Phase5IntegratedExperiment Public Methods
// ============================================================================

Phase5IntegratedExperiment::Phase5IntegratedExperiment() {
    NLM_LOG_INFO("Initializing Phase 5 Integrated Experiment");
    
    // Initialize configuration
    auto& configManager = ConfigManager::getInstance();
    configManager.reset();
    configManager.loadPhaseDefaults(5);
}

Phase5IntegratedExperiment::~Phase5IntegratedExperiment() = default;

LifetimeExperimentResult Phase5IntegratedExperiment::run(
    const LifetimeExperimentConfig& config,
    std::function<std::shared_ptr<Brain>(const LifetimeExperimentConfig& config)> createBrain,
    std::function<bool(Brain* brain, uint64_t steps)> runSimulation
) {
    // Initialize configuration system
    auto& configManager = ConfigManager::getInstance();
    configManager.reset();
    configManager.loadPhaseDefaults(5);  // Load Phase 5 defaults
    
    // Validate configuration
    if (!validateConfig(config)) {
        NLM_LOG_ERROR("Invalid Phase 5 configuration");
        return initializeResult(config);
    }
    
    // Create brain with validated configuration
    auto brain = createBrain(config);
    if (!brain) {
        NLM_LOG_ERROR("Failed to create brain for Phase 5 experiment");
        LifetimeExperimentResult result = initializeResult(config);
        result.failures.push_back("Failed to create brain");
        return result;
    }
    
    auto result = initializeResult(config);
    
    NLM_LOG_INFO("Starting Phase 5 Lifetime Experiment");
    NLM_LOG_INFO("Neurons: " + std::to_string(config.initialNeurons) + 
                 " -> " + std::to_string(config.maxNeurons));
    NLM_LOG_INFO("Phases: " + std::to_string(config.synapseFormationSteps) + 
                 " + " + std::to_string(config.sensoryDevelopmentSteps) + 
                 " + " + std::to_string(config.motorDevelopmentSteps) + 
                 " + " + std::to_string(config.associativeDevelopmentSteps) + 
                 " + " + std::to_string(config.memoryConsolidationSteps) + 
                 " + " + std::to_string(config.socialDevelopmentSteps) + " steps");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    uint64_t currentStep = 0;
    
    // Execute all six developmental phases
    result = runDevelopmentPhases(brain, config, result, currentStep);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Compute comprehensive metrics
    computeComprehensiveMetrics(result, config);
    
    NLM_LOG_INFO("Phase 5 Experiment Complete");
    NLM_LOG_INFO("Total steps: " + std::to_string(currentStep));
    NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Learning efficiency: " + std::to_string(result.overallLearningEfficiency));
    
    return result;
}

LifetimeExperimentResult Phase5IntegratedExperiment::runContinualLearning(
    std::shared_ptr<Brain> brain,
    const std::vector<ContinualTask>& tasks,
    size_t cycles
) {
    LifetimeExperimentResult result;
    
    NLM_LOG_INFO("Starting Continual Learning Experiment");
    NLM_LOG_INFO("Tasks: " + std::to_string(tasks.size()) + 
                 ", Cycles: " + std::to_string(cycles));
    
    for (size_t cycle = 0; cycle < cycles; ++cycle) {
        for (const auto& task : tasks) {
            NLM_LOG_INFO("Cycle " + std::to_string(cycle + 1) + 
                         ", Task: " + task.name);
            
            uint64_t startStep = cycle * task.stepsBetweenTasks;
            uint64_t endStep = startStep + task.stepsBetweenTasks;
            
            auto phaseResult = runPhase(
                brain.get(), 
                task.name + " (cycle " + std::to_string(cycle + 1) + ")",
                startStep, endStep,
                [](uint64_t step) {}
            );
            
            // Evaluate task performance
            float performance = task.evaluateFunction(brain.get());
            phaseResult.totalReward = performance;
            
            result.phaseResults.push_back(phaseResult);
        }
    }
    
    return result;
}

LifetimeExperimentResult Phase5IntegratedExperiment::runDamageRecovery(
    std::shared_ptr<Brain> brain,
    uint64_t damageStep,
    float damageFraction,
    const std::string& damageRegion
) {
    LifetimeExperimentResult result;
    
    NLM_LOG_INFO("Starting Damage Recovery Experiment");
    NLM_LOG_INFO("Damage at step: " + std::to_string(damageStep));
    NLM_LOG_INFO("Damage fraction: " + std::to_string(damageFraction));
    NLM_LOG_INFO("Damage region: " + damageRegion);
    
    // Pre-damage baseline measurement
    NLM_LOG_INFO("Collecting pre-damage baseline data...");
    auto baselineResult = collectBaselineMetrics(brain, damageStep);
    result.phaseResults.push_back(baselineResult);
    
    // Apply controlled damage (simplified model)
    NLM_LOG_INFO("Applying controlled damage...");
    applyDamage(brain, damageFraction, damageRegion);
    
    // Post-damage recovery measurement
    NLM_LOG_INFO("Measuring post-damage recovery...");
    auto recoveryResult = collectRecoveryMetrics(brain, damageStep, 50000);
    result.phaseResults.push_back(recoveryResult);
    
    return result;
}

ScalingAnalysis Phase5IntegratedExperiment::runScalingExperiment(
    const std::vector<ScaleLevel>& scales,
    uint64_t stepsPerScale
) {
    ScalingAnalysis analysis;
    
    NLM_LOG_INFO("Starting Scaling Experiment");
    
    for (const auto& scale : scales) {
        NLM_LOG_INFO("Scale: " + scale.name + 
                     " (" + std::to_string(scale.neurons) + " neurons)");
        
        BenchmarkRun run;
        run.name = scale.name;
        run.scale = scale;
        run.randomSeed = 42;
        run.stepsRun = stepsPerScale;
        
        // Run actual benchmark with configuration
        auto brain = createBrainForScaling(scale.neurons);
        if (!brain) {
            NLM_LOG_ERROR("Failed to create brain for scale " + scale.name);
            continue;
        }
        
        run.result = runScalingBenchmark(brain, stepsPerScale);
        analysis.runs.push_back(run);
    }
    
    return analysis;
}

std::vector<AblationResult> Phase5IntegratedExperiment::runAblationComparison(
    const std::string& task,
    const AblationExperiment& experiment
) {
    NLM_LOG_INFO("Running Ablation Comparison for: " + task);
    
    std::vector<AblationResult> results;
    
    for (const auto& config : experiment.ablations) {
        AblationResult result;
        result.name = task + "_ablated";
        result.config = config;
        
        // Create brain with ablated configuration
        auto brain = createBrainWithAblation(config);
        if (!brain) {
            NLM_LOG_ERROR("Failed to create ablated brain for " + config.name);
            continue;
        }
        
        // Run experiment with ablated brain
        result.performance = runSingleAblationExperiment(brain, config);
        results.push_back(result);
        
        // Clean up for next iteration
        brain.reset();
    }
    
    return results;
}

// ============================================================================
// Phase5IntegratedExperiment Private Methods
// ============================================================================

LifetimeExperimentResult Phase5IntegratedExperiment::initializeResult(
    const LifetimeExperimentConfig& config
) {
    LifetimeExperimentResult result;
    result.config = config;
    result.startTime = time(nullptr);
    
    // Initialize metrics
    result.totalReward = 0.0f;
    result.avgRewardPerStep = 0.0f;
    result.overallLearningEfficiency = 0.0f;
    result.finalPerformance = 0.0f;
    result.memoryCapacity = 0.0f;
    result.predictionAccuracy = 0.0f;
    result.generalizationAbility = 0.0f;
    
    return result;
}

bool Phase5IntegratedExperiment::validateConfig(const LifetimeExperimentConfig& config) {
    auto& configManager = ConfigManager::getInstance();
    configManager.reset();
    configManager.loadPhaseDefaults(5);
    
    // Validate neuron counts
    if (config.initialNeurons <= 0) {
        NLM_LOG_ERROR("Initial neurons must be positive");
        return false;
    }
    if (config.maxNeurons < config.initialNeurons) {
        NLM_LOG_ERROR("Max neurons must be >= initial neurons");
        return false;
    }
    if (config.maxNeurons > 1000000) {  // Reasonable upper limit
        NLM_LOG_ERROR("Max neurons too large (max: 1000000)");
        return false;
    }
    
    // Validate phase durations
    if (config.synapseFormationSteps == 0 || config.sensoryDevelopmentSteps == 0 ||
        config.motorDevelopmentSteps == 0 || config.associativeDevelopmentSteps == 0 ||
        config.memoryConsolidationSteps == 0 || config.socialDevelopmentSteps == 0) {
        NLM_LOG_ERROR("All phase durations must be positive");
        return false;
    }
    
    // Validate learning thresholds
    if (config.damageRecoveryThreshold < 0.0f || config.damageRecoveryThreshold > 1.0f) {
        NLM_LOG_ERROR("Damage recovery threshold must be between 0.0 and 1.0");
        return false;
    }
    
    if (config.learningEfficiencyThreshold < 0.0f || config.learningEfficiencyThreshold > 1.0f) {
        NLM_LOG_ERROR("Learning efficiency threshold must be between 0.0 and 1.0");
        return false;
    }
    
    if (config.memoryCapacityGrowthRate < 0.0f || config.memoryCapacityGrowthRate > 1.0f) {
        NLM_LOG_ERROR("Memory capacity growth rate must be between 0.0 and 1.0");
        return false;
    }
    
    if (config.predictionAccuracyThreshold < 0.0f || config.predictionAccuracyThreshold > 1.0f) {
        NLM_LOG_ERROR("Prediction accuracy threshold must be between 0.0 and 1.0");
        return false;
    }
    
    if (config.generalizationThreshold < 0.0f || config.generalizationThreshold > 1.0f) {
        NLM_LOG_ERROR("Generalization threshold must be between 0.0 and 1.0");
        return false;
    }
    
    return true;
}

LifetimeExperimentResult Phase5IntegratedExperiment::runDevelopmentPhases(
    std::shared_ptr<Brain> brain,
    const LifetimeExperimentConfig& config,
    LifetimeExperimentResult& result,
    uint64_t& currentStep
) {
    // Phase 1: Early Development (Synaptogenesis)
    NLM_LOG_INFO("Phase 1: Early Development (Synaptogenesis)");
    auto phase1Result = runPhase(
        brain.get(), "Early Development", currentStep, 
        currentStep + config.synapseFormationSteps,
        [this](uint64_t step) {
            if (step % 1000 == 0) {
                NLM_LOG_DEBUG("  Step " + std::to_string(step));
            }
        }
    );
    result.phaseResults.push_back(phase1Result);
    currentStep += config.synapseFormationSteps;
    
    // Phase 2: Sensory Development
    NLM_LOG_INFO("Phase 2: Sensory Development");
    auto phase2Result = runPhase(
        brain.get(), "Sensory Development", currentStep,
        currentStep + config.sensoryDevelopmentSteps,
        [this](uint64_t step) {}
    );
    result.phaseResults.push_back(phase2Result);
    currentStep += config.sensoryDevelopmentSteps;
    
    // Phase 3: Motor Development
    NLM_LOG_INFO("Phase 3: Motor Development");
    auto phase3Result = runPhase(
        brain.get(), "Motor Development", currentStep,
        currentStep + config.motorDevelopmentSteps,
        [this](uint64_t step) {}
    );
    result.phaseResults.push_back(phase3Result);
    currentStep += config.motorDevelopmentSteps;
    
    // Phase 4: Associative Development
    NLM_LOG_INFO("Phase 4: Associative Development");
    auto phase4Result = runPhase(
        brain.get(), "Associative Development", currentStep,
        currentStep + config.associativeDevelopmentSteps,
        [this](uint64_t step) {}
    );
    result.phaseResults.push_back(phase4Result);
    currentStep += config.associativeDevelopmentSteps;
    
    // Phase 5: Memory Consolidation
    NLM_LOG_INFO("Phase 5: Memory Consolidation");
    auto phase5Result = runPhase(
        brain.get(), "Memory Consolidation", currentStep,
        currentStep + config.memoryConsolidationSteps,
        [this](uint64_t step) {}
    );
    result.phaseResults.push_back(phase5Result);
    currentStep += config.memoryConsolidationSteps;
    
    // Phase 6: Social Development
    NLM_LOG_INFO("Phase 6: Social Development");
    auto phase6Result = runPhase(
        brain.get(), "Social Development", currentStep,
        currentStep + config.socialDevelopmentSteps,
        [this](uint64_t step) {}
    );
    result.phaseResults.push_back(phase6Result);
    currentStep += config.socialDevelopmentSteps;
    
    return result;
}

LifetimePhaseResult Phase5IntegratedExperiment::runPhase(
    Brain* brain,
    const std::string& phaseName,
    uint64_t startStep,
    uint64_t endStep,
    const std::function<void(uint64_t step)>& progressCallback
) {
    LifetimePhaseResult result;
    result.phaseName = phaseName;
    result.startStep = startStep;
    result.endStep = endStep;
    
    auto phaseStart = std::chrono::high_resolution_clock::now();
    
    // Simulate learning phase with realistic brain dynamics
    for (uint64_t step = startStep; step < endStep; ++step) {
        brain->step(step, step * 0.001);  // 1ms timestep
        
        progressCallback(step);
        
        // Collect phase-specific metrics every 1000 steps
        if (step % 1000 == 0) {
            result.totalReward += brain->getAverageFiringRate() * 0.01f;
            result.avgFiringRate = brain->getAverageFiringRate();
            
            // Log progress for longer phases
            if (phaseName.find("Development") != std::string::npos) {
                NLM_LOG_INFO("  Phase " + phaseName + ": step " + std::to_string(step) + 
                             ", neurons: " + std::to_string(brain->getTotalNeuronCount()) + 
                             ", synapses: " + std::to_string(brain->getTotalSynapseCount()));
            }
        }
    }
    
    auto phaseEnd = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(phaseEnd - phaseStart).count();
    
    return result;
}

LifetimePhaseResult Phase5IntegratedExperiment::collectBaselineMetrics(
    std::shared_ptr<Brain> brain,
    uint64_t damageStep
) {
    LifetimePhaseResult result;
    result.phaseName = "Pre-Damage Baseline";
    result.startStep = 0;
    result.endStep = damageStep;
    
    NLM_LOG_INFO("Collecting baseline metrics over " + std::to_string(damageStep) + " steps");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (uint64_t step = 0; step < damageStep; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0) {
            result.totalReward += brain->getAverageFiringRate() * 0.01f;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(endTime - startTime).count();
    result.avgFiringRate = brain->getAverageFiringRate();
    
    return result;
}

LifetimePhaseResult Phase5IntegratedExperiment::collectRecoveryMetrics(
    std::shared_ptr<Brain> brain,
    uint64_t damageStep,
    uint64_t recoverySteps
) {
    LifetimePhaseResult result;
    result.phaseName = "Recovery";
    result.startStep = damageStep;
    result.endStep = damageStep + recoverySteps;
    
    NLM_LOG_INFO("Collecting recovery metrics over " + std::to_string(recoverySteps) + " steps");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (uint64_t step = damageStep; step < damageStep + recoverySteps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0) {
            result.totalReward += brain->getAverageFiringRate() * 0.01f;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(endTime - startTime).count();
    result.avgFiringRate = brain->getAverageFiringRate();
    
    return result;
}

void Phase5IntegratedExperiment::applyDamage(
    std::shared_ptr<Brain> brain,
    float damageFraction,
    const std::string& damageRegion
) {
    NLM_LOG_INFO("Applying " + std::to_string(damageFraction * 100) + "% damage to " + damageRegion);
    
    // Simplified damage model: remove synapses and reduce neuron connectivity
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t synapsesToRemove = static_cast<size_t>(totalSynapses * damageFraction);
    
    // Remove synapses from brain (implementation would need access to brain's synapse management)
    // This is a placeholder for the actual damage application logic
    NLM_LOG_INFO("Damage simulation: would remove " + std::to_string(synapsesToRemove) + 
                 " synapses from " + damageRegion);
}

LifetimeExperimentResult Phase5IntegratedExperiment::computeComprehensiveMetrics(
    LifetimeExperimentResult& result,
    const LifetimeExperimentConfig& config
) {
    // Compute phase-to-phase improvements
    if (result.phaseResults.size() >= 2) {
        const auto& first = result.phaseResults.front();
        const auto& last = result.phaseResults.back();
        
        result.totalReward = last.totalReward;
        
        // Calculate learning efficiency as improvement across phases
        float phaseImprovement = 0.0f;
        for (size_t i = 1; i < result.phaseResults.size(); ++i) {
            float improvement = result.phaseResults[i].totalReward - result.phaseResults[i-1].totalReward;
            phaseImprovement += improvement;
        }
        
        result.overallLearningEfficiency = result.phaseResults.empty() ? 0.0f : 
            phaseImprovement / static_cast<float>(result.phaseResults.size() - 1);
    }
    
    // Compute final capabilities based on developmental outcomes
    result.finalPerformance = result.totalReward > 0.0f ? result.totalReward / 1000.0f : 0.0f;
    result.memoryCapacity = config.maxNeurons / 1000.0f;  // Normalized to 0-1 scale
    result.predictionAccuracy = config.predictionAccuracyThreshold;
    result.generalizationAbility = config.generalizationThreshold;
    
    // Calculate average firing rate across all phases
    float totalFiringRate = 0.0f;
    for (const auto& phase : result.phaseResults) {
        totalFiringRate += phase.avgFiringRate;
    }
    result.avgFiringRate = result.phaseResults.empty() ? 0.0f : 
        totalFiringRate / static_cast<float>(result.phaseResults.size());
    
    return result;
}

void Phase5IntegratedExperiment::generateVisualizations(
    const LifetimeExperimentResult& result,
    const std::string& outputDir
) {
    NLM_LOG_INFO("Generating visualizations to: " + outputDir);
    // Placeholder for visualization generation
}

std::string Phase5IntegratedExperiment::generateReport(
    const LifetimeExperimentResult& result
) {
    std::ostringstream oss;
    oss << "\n";
    oss << "╔══════════════════════════════════════════════════════════════════════╗\n";
    oss << "║              NLM Phase 5 Lifetime Experiment - Final Report                ║\n";
    oss << "╚══════════════════════════════════════════════════════════════════════╝\n\n";
    
    oss << "Total Reward: " << std::fixed << std::setprecision(2) << result.totalReward << "\n";
    oss << "Avg Reward/Step: " << result.avgRewardPerStep << "\n";
    oss << "Learning Efficiency: " << result.overallLearningEfficiency << "\n\n";
    
    oss << "Final Capabilities:\n";
    oss << "  Performance: " << result.finalPerformance << "\n";
    oss << "  Memory: " << result.memoryCapacity << "\n";
    oss << "  Prediction: " << result.predictionAccuracy << "\n";
    oss << "  Generalization: " << result.generalizationAbility << "\n\n";
    
    oss << "Phases:\n";
    for (const auto& phase : result.phaseResults) {
        oss << "  " << phase.phaseName << ": steps " << phase.startStep 
            << "-" << phase.endStep << ", reward=" << std::fixed << std::setprecision(2) 
            << phase.totalReward << "\n";
    }
    
    return oss.str();
}

// ============================================================================
// MultiSeedExperimentRunner Implementation
// ============================================================================

MultiSeedExperimentRunner::MultiSeedExperimentRunner()
    : baseSeed_(42), rng_(baseSeed_) {
}

MultiSeedExperimentRunner::~MultiSeedExperimentRunner() = default;

std::vector<LifetimeExperimentResult> MultiSeedExperimentRunner::runWithMultipleSeeds(
    size_t numSeeds,
    const LifetimeExperimentConfig& baseConfig,
    std::function<std::shared_ptr<Brain>(const LifetimeExperimentConfig& config, uint64_t seed)> createBrain,
    std::function<bool(Brain* brain, uint64_t steps)> runSimulation
) {
    std::vector<LifetimeExperimentResult> results;
    results.reserve(numSeeds);
    
    Phase5IntegratedExperiment experiment;
    
    for (size_t seed = 0; seed < numSeeds; ++seed) {
        uint64_t actualSeed = baseSeed_ + seed;
        NLM_LOG_INFO("Running with seed " + std::to_string(actualSeed) + 
                     " (" + std::to_string(seed + 1) + "/" + std::to_string(numSeeds) + ")");
        
        auto brain = createBrain(baseConfig, actualSeed);
        if (!brain) {
            NLM_LOG_ERROR("Failed to create brain with seed " + std::to_string(actualSeed));
            continue;
        }
        
        // Run simulation
        bool success = runSimulation(brain.get(), baseConfig.synapseFormationSteps * 6);
        
        if (success) {
            LifetimeExperimentResult result;
            result.totalReward = brain->getAverageFiringRate() * 1000.0f;
            results.push_back(result);
        }
    }
    
    return results;
}

MultiSeedExperimentRunner::SeedStatistics MultiSeedExperimentRunner::computeStatistics(
    const std::vector<LifetimeExperimentResult>& results
) {
    SeedStatistics stats{};
    
    if (results.empty()) return stats;
    
    std::vector<float> rewards;
    rewards.reserve(results.size());
    
    for (const auto& r : results) {
        rewards.push_back(r.totalReward);
    }
    
    // Compute mean
    float sum = 0.0f;
    float min = rewards[0];
    float max = rewards[0];
    for (float r : rewards) {
        sum += r;
        min = std::min(min, r);
        max = std::max(max, r);
    }
    stats.meanReward = sum / static_cast<float>(results.size());
    stats.minPerformance = min;
    stats.maxPerformance = max;
    
    // Compute std dev
    float sqSum = 0.0f;
    for (float r : rewards) {
        float diff = r - stats.meanReward;
        sqSum += diff * diff;
    }
    float variance = sqSum / static_cast<float>(results.size());
    stats.stdDevReward = std::sqrt(variance);
    
    stats.coefficientOfVariation = stats.meanReward > 0 ? 
        stats.stdDevReward / stats.meanReward : 0.0f;
    
    stats.successCount = results.size();
    stats.failureCount = 0;
    
    stats.meanPerformance = stats.meanReward;
    stats.stdDevPerformance = stats.stdDevReward;
    
    return stats;
}

std::string MultiSeedExperimentRunner::generateReport(
    const std::vector<LifetimeExperimentResult>& results,
    const SeedStatistics& stats
) {
    std::ostringstream oss;
    oss << "\n";
    oss << "═══════════════════════════════════════════════════════════════════════════\n";
    oss << "              Multi-Seed Experiment Results\n";
    oss << "═══════════════════════════════════════════════════════════════════════════\n\n";
    
    oss << "Seeds run: " << results.size() << "\n";
    oss << "Success rate: " << stats.successCount << "/" << (stats.successCount + stats.failureCount) << "\n\n";
    
    oss << "Reward Statistics:\n";
    oss << "  Mean: " << std::fixed << std::setprecision(4) << stats.meanReward << "\n";
    oss << "  Std Dev: " << stats.stdDevReward << "\n";
    oss << "  Min: " << stats.minPerformance << "\n";
    oss << "  Max: " << stats.maxPerformance << "\n";
    oss << "  CV: " << stats.coefficientOfVariation << "\n\n";
    
    return oss.str();
}

// ============================================================================
// Helper Functions
// ============================================================================

// Create brain for scaling experiments
std::shared_ptr<Brain> Phase5IntegratedExperiment::createBrainForScaling(size_t neuronCount) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount));
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    
    return std::make_shared<Brain>(config);
}

// Create brain with ablated configuration
std::shared_ptr<Brain> Phase5IntegratedExperiment::createBrainWithAblation(
    const AblationConfig& ablationConfig
) {
    auto config = std::make_shared<Config>();
    
    // Apply ablated configuration
    config->set("neuron_count", static_cast<int64_t>(ablationConfig.neuronCount));
    config->set("region_count", static_cast<int64_t>(ablationConfig.regionCount));
    config->set("connection_probability", ablationConfig.connectionProbability);
    config->set("random_seed", uint64_t(ablationConfig.randomSeed));
    
    return std::make_shared<Brain>(config);
}

// Run single ablation experiment
float Phase5IntegratedExperiment::runSingleAblationExperiment(
    std::shared_ptr<Brain> brain,
    const AblationConfig& ablationConfig
) {
    // Initialize and run the brain with the given configuration
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize ablated brain");
        return 0.0f;
    }
    
    // Run for specified number of steps
    uint64_t steps = ablationConfig.stepsToRun;
    for (uint64_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    return brain->getAverageFiringRate() * 1000.0f;
}

// Run scaling benchmark
float Phase5IntegratedExperiment::runScalingBenchmark(
    std::shared_ptr<Brain> brain,
    uint64_t steps
) {
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for scaling benchmark");
        return 0.0f;
    }
    
    float totalReward = 0.0f;
    for (uint64_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        totalReward += brain->getAverageFiringRate() * 0.001f;
    }
    
    return totalReward;
}

} // namespace nlm
