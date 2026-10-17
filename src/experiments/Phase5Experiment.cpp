// Refactored Phase5Experiment.cpp - Improved maintainability
// 
// This version addresses the maintainability issues in the original implementation:
// - Reduced function complexity
// - Better error handling and validation
// - Improved code organization
// - Enhanced documentation
// - Easier to test and maintain

#include "Phase5Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// ============================================================================
// LifetimeExperimentResult
// ============================================================================

std::string LifetimeExperimentResult::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"totalReward\": " << totalReward << ",\n";
    oss << "  \"avgRewardPerStep\": " << avgRewardPerStep << ",\n";
    oss << "  \"overallLearningEfficiency\": " << overallLearningEfficiency << ",\n";
    oss << "  \"finalPerformance\": " << finalPerformance << ",\n";
    oss << "  \"memoryCapacity\": " << memoryCapacity << ",\n";
    oss << "  \"predictionAccuracy\": " << predictionAccuracy << ",\n";
    oss << "  \"generalizationAbility\": " << generalizationAbility << ",\n";
    oss << "  \"phases\": [\n";
    
    for (size_t i = 0; i < phaseResults.size(); ++i) {
        const auto& phase = phaseResults[i];
        oss << "    {\n";
        oss << "      \"name\": \"" << escapeString(phase.phaseName) << "\",\n";
        oss << "      \"startStep\": " << phase.startStep << ",\n";
        oss << "      \"endStep\": " << phase.endStep << ",\n";
        oss << "      \"reward\": " << phase.totalReward << "\n";
        oss << "    }";
        if (i < phaseResults.size() - 1) oss << ",";
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}\n";
    return oss.str();
}

std::string LifetimeExperimentResult::summary() const {
    std::ostringstream oss;
    oss << "=== NLM Phase 5 Lifetime Experiment Results ===\n\n";
    oss << "Total Reward: " << std::fixed << std::setprecision(2) << totalReward << "\n";
    oss << "Avg Reward/Step: " << avgRewardPerStep << "\n";
    oss << "Learning Efficiency: " << overallLearningEfficiency << "\n\n";
    oss << "Final Capabilities:\n";
    oss << "  Performance: " << finalPerformance << "\n";
    oss << "  Memory: " << memoryCapacity << "\n";
    oss << "  Prediction: " << predictionAccuracy << "\n";
    oss << "  Generalization: " << generalizationAbility << "\n\n";
    oss << "Phases:\n";
    for (const auto& phase : phaseResults) {
        oss << "  " << phase.phaseName << ": steps " << phase.startStep 
            << "-" << phase.endStep << ", reward=" << std::fixed << std::setprecision(2) 
            << phase.totalReward << "\n";
    }
    return oss.str();
}

// ============================================================================
// Phase5IntegratedExperiment
// ============================================================================

Phase5IntegratedExperiment::Phase5IntegratedExperiment() {
    NLM_LOG_INFO("Initializing Phase 5 Integrated Experiment");
}

Phase5IntegratedExperiment::~Phase5IntegratedExperiment() = default;

LifetimeExperimentResult Phase5IntegratedExperiment::run(
    const LifetimeExperimentConfig& config,
    std::function<std::shared_ptr<Brain>(const LifetimeExperimentConfig& config)> createBrain,
    std::function<bool(Brain* brain, uint64_t steps)> runSimulation
) {
    // Validate configuration
    if (!validateConfig(config)) {
        throw std::invalid_argument("Invalid configuration provided");
    }
    
    NLM_LOG_INFO("Starting Phase 5 Lifetime Experiment");
    NLM_LOG_INFO("Neurons: " + std::to_string(config.initialNeurons) + 
                 " -> " + std::to_string(config.maxNeurons));
    
    // Initialize result
    LifetimeExperimentResult result = initializeResult(config);
    
    // Create brain
    std::shared_ptr<Brain> brain = createBrain(config);
    if (!brain) {
        result.failures.push_back("Failed to create brain");
        return result;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    uint64_t currentStep = 0;
    
    // Run all developmental phases
    result = runDevelopmentalPhases(
        brain.get(), result, config, currentStep
    );
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Compute overall metrics
    computeOverallMetrics(result, currentStep);
    
    NLM_LOG_INFO("Phase 5 Experiment Complete");
    NLM_LOG_INFO("Total steps: " + std::to_string(currentStep));
    NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
    
    return result;
}

LifetimeExperimentResult Phase5IntegratedExperiment::runDevelopmentalPhases(
    Brain* brain,
    LifetimeExperimentResult& result,
    const LifetimeExperimentConfig& config,
    uint64_t& currentStep
) {
    // Define the developmental phases with their durations and names
    const std::vector<std::pair<std::string, uint64_t>> phases = {
        {"Early Development", config.synapseFormationSteps},
        {"Sensory Development", config.sensoryDevelopmentSteps},
        {"Motor Development", config.motorDevelopmentSteps},
        {"Associative Development", config.associativeDevelopmentSteps},
        {"Memory Consolidation", config.memoryConsolidationSteps},
        {"Social Development", config.socialDevelopmentSteps}
    };
    
    // Run each developmental phase
    for (const auto& phase : phases) {
        NLM_LOG_INFO("Running " + phase.first + " phase");
        
        uint64_t phaseEnd = currentStep + phase.second;
        LifetimePhaseResult phaseResult = runPhase(
            brain, phase.first, currentStep, phaseEnd,
            [phase.second](uint64_t step) {
                if (step % 1000 == 0) {
                    NLM_LOG_DEBUG("  Step " + std::to_string(step) + "/" + std::to_string(phase.second));
                }
            }
        );
        
        result.phaseResults.push_back(phaseResult);
        currentStep = phaseEnd;
        
        NLM_LOG_INFO(phase.first + " phase completed: " + std::to_string(phase.second) + " steps, " + 
                     std::to_string(phaseResult.totalReward) + " reward");
    }
    
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
    
    try {
        // Validate input parameters
        if (!brain) {
            throw std::invalid_argument("Brain pointer is null");
        }
        if (endStep <= startStep) {
            throw std::invalid_argument("End step must be greater than start step");
        }
        
        // Run simulation steps
        NLM_LOG_DEBUG(phaseName + ": Starting simulation from step " + std::to_string(startStep) + 
                     " to " + std::to_string(endStep));
        
        for (uint64_t step = startStep; step < endStep; ++step) {
            // Collect metrics periodically
            if (step % 1000 == 0) {
                result.totalReward += brain->getAverageFiringRate() * 0.01f;
                result.avgFiringRate = brain->getAverageFiringRate();
            }
            
            // Update brain state
            brain->step(step, step * 0.001); // 1ms timestep
            
            // Call progress callback
            progressCallback(step);
        }
        
        auto phaseEnd = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration<double>(phaseEnd - phaseStart).count();
        
        NLM_LOG_DEBUG(phaseName + ": Phase completed in " + std::to_string(result.duration) + "s");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during " + phaseName + ": " + std::string(e.what()));
        result.failures.push_back("Phase " + phaseName + " failed: " + std::string(e.what()));
    }
    
    return result;
}

LifetimeExperimentResult Phase5IntegratedExperiment::runContinualLearning(
    std::shared_ptr<Brain> brain,
    const std::vector<ContinualTask>& tasks,
    size_t cycles
) {
    if (!brain) {
        throw std::invalid_argument("Brain pointer is null");
    }
    if (tasks.empty()) {
        throw std::invalid_argument("Task list cannot be empty");
    }
    if (cycles == 0) {
        throw std::invalid_argument("Cycles must be greater than 0");
    }
    
    LifetimeExperimentResult result;
    
    NLM_LOG_INFO("Starting Continual Learning Experiment");
    NLM_LOG_INFO("Tasks: " + std::to_string(tasks.size()) + ", Cycles: " + std::to_string(cycles));
    
    for (size_t cycle = 0; cycle < cycles; ++cycle) {
        NLM_LOG_INFO("Cycle " + std::to_string(cycle + 1) + "/" + std::to_string(cycles));
        
        for (const auto& task : tasks) {
            NLM_LOG_INFO("  Task: " + task.name + " - " + task.description);
            
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
            
            NLM_LOG_INFO("  Task completed: " + std::to_string(performance) + " performance score");
        }
    }
    
    NLM_LOG_INFO("Continual Learning Experiment completed");
    return result;
}

LifetimeExperimentResult Phase5IntegratedExperiment::runDamageRecovery(
    std::shared_ptr<Brain> brain,
    uint64_t damageStep,
    float damageFraction,
    const std::string& damageRegion
) {
    if (!brain) {
        throw std::invalid_argument("Brain pointer is null");
    }
    if (damageStep == 0) {
        throw std::invalid_argument("Damage step cannot be 0");
    }
    if (damageFraction < 0.0f || damageFraction > 1.0f) {
        throw std::invalid_argument("Damage fraction must be between 0.0 and 1.0");
    }
    
    LifetimeExperimentResult result;
    
    NLM_LOG_INFO("Starting Damage Recovery Experiment");
    NLM_LOG_INFO("Damage at step: " + std::to_string(damageStep));
    NLM_LOG_INFO("Damage fraction: " + std::to_string(damageFraction));
    NLM_LOG_INFO("Damage region: " + damageRegion);
    
    try {
        // Pre-damage baseline
        NLM_LOG_INFO("Collecting pre-damage baseline");
        auto baselineResult = runPhase(
            brain.get(), "Pre-Damage Baseline",
            0, damageStep,
            [](uint64_t step) {}
        );
        result.phaseResults.push_back(baselineResult);
        
        // Note: Actual damage application would be done through the brain interface
        // This is a placeholder for the experiment framework
        NLM_LOG_WARN("Damage application not implemented - this is a placeholder");
        
        // Post-damage recovery
        NLM_LOG_INFO("Starting post-damage recovery");
        auto recoveryResult = runPhase(
            brain.get(), "Recovery",
            damageStep, damageStep + 50000,
            [](uint64_t step) {}
        );
        result.phaseResults.push_back(recoveryResult);
        
        NLM_LOG_INFO("Damage Recovery Experiment completed");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Damage recovery experiment failed: " + std::string(e.what()));
        result.failures.push_back(std::string("Damage recovery failed: ") + e.what());
    }
    
    return result;
}

ScalingAnalysis Phase5IntegratedExperiment::runScalingExperiment(
    const std::vector<ScaleLevel>& scales,
    uint64_t stepsPerScale
) {
    if (scales.empty()) {
        throw std::invalid_argument("Scale levels cannot be empty");
    }
    if (stepsPerScale == 0) {
        throw std::invalid_argument("Steps per scale must be greater than 0");
    }
    
    ScalingAnalysis analysis;
    NLM_LOG_INFO("Starting Scaling Experiment");
    NLM_LOG_INFO("Testing " + std::to_string(scales.size()) + " scale levels");
    
    for (const auto& scale : scales) {
        NLM_LOG_INFO("Scale: " + scale.name + 
                     " (" + std::to_string(scale.neurons) + " neurons)");
        
        BenchmarkRun run;
        run.name = scale.name;
        run.scale = scale;
        run.randomSeed = 42;
        run.stepsRun = stepsPerScale;
        
        // Run benchmark (simplified - actual implementation would create brain and run)
        // This is a placeholder for actual scaling experiments
        NLM_LOG_DEBUG("Running scaling benchmark for " + scale.name);
        
        analysis.runs.push_back(run);
    }
    
    NLM_LOG_INFO("Scaling Experiment completed");
    return analysis;
}

std::vector<AblationResult> Phase5IntegratedExperiment::runAblationComparison(
    const std::string& task,
    const AblationExperiment& experiment
) {
    if (task.empty()) {
        throw std::invalid_argument("Task name cannot be empty");
    }
    if (experiment.ablations.empty()) {
        throw std::invalid_argument("Ablation configurations cannot be empty");
    }
    
    std::vector<AblationResult> results;
    NLM_LOG_INFO("Running Ablation Comparison for: " + task);
    
    for (const auto& config : experiment.ablations) {
        AblationResult result;
        result.name = task + "_ablated";
        result.config = config;
        
        // Actual ablation running would be implemented here
        // This is a placeholder for actual ablation experiments
        NLM_LOG_DEBUG("Running ablation: " + config.name);
        
        results.push_back(result);
    }
    
    NLM_LOG_INFO("Ablation comparison completed: " + std::to_string(results.size()) + " runs");
    return results;
}

LifetimeExperimentResult Phase5IntegratedExperiment::initializeResult(
    const LifetimeExperimentConfig& config
) {
    LifetimeExperimentResult result;
    result.config = config;
    result.startTime = time(nullptr);
    
    NLM_LOG_DEBUG("Initialized experiment result");
    return result;
}

bool Phase5IntegratedExperiment::validateConfig(const LifetimeExperimentConfig& config) {
    bool isValid = true;
    
    // Check basic constraints
    if (config.initialNeurons == 0) {
        NLM_LOG_ERROR("Initial neurons must be greater than 0");
        isValid = false;
    }
    
    if (config.maxNeurons < config.initialNeurons) {
        NLM_LOG_ERROR("Max neurons must be greater than or equal to initial neurons");
        isValid = false;
    }
    
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Connection probability must be between 0.0 and 1.0");
        isValid = false;
    }
    
    // Check phase step sums
    uint64_t totalPhaseSteps = config.synapseFormationSteps + config.sensoryDevelopmentSteps +
                               config.motorDevelopmentSteps + config.associativeDevelopmentSteps +
                               config.memoryConsolidationSteps + config.socialDevelopmentSteps;
    
    if (totalPhaseSteps == 0) {
        NLM_LOG_ERROR("Total phase steps cannot be 0");
        isValid = false;
    }
    
    if (isValid) {
        NLM_LOG_DEBUG("Configuration validation passed");
    }
    
    return isValid;
}

void Phase5IntegratedExperiment::computeOverallMetrics(
    LifetimeExperimentResult& result,
    uint64_t currentStep
) {
    // Compute overall metrics
    float totalPhaseReward = 0.0f;
    for (const auto& phase : result.phaseResults) {
        totalPhaseReward += phase.totalReward;
    }
    result.totalReward = totalPhaseReward;
    result.avgRewardPerStep = result.phaseResults.empty() ? 0.0f : 
        totalPhaseReward / static_cast<float>(currentStep);
    
    // Compute learning efficiency
    if (result.phaseResults.size() > 1) {
        float rewardVariances = 0.0f;
        for (size_t i = 1; i < result.phaseResults.size(); ++i) {
            rewardVariances += std::abs(result.phaseResults[i].totalReward - 
                                      result.phaseResults[i-1].totalReward);
        }
        result.overallLearningEfficiency = rewardVariances / (result.phaseResults.size() - 1);
    }
    
    // Compute final capabilities
    if (!result.phaseResults.empty()) {
        const auto& lastPhase = result.phaseResults.back();
        result.finalPerformance = lastPhase.avgFiringRate;
        result.memoryCapacity = std::min(1.0f, lastPhase.memoryRetention * 10.0f); // Scale to [0,1]
        result.predictionAccuracy = std::max(0.0f, lastPhase.predictionAccuracy - 0.5f) * 2.0f; // Scale to [0,1]
        result.generalizationAbility = lastPhase.adaptationSpeed;
    }
}

void Phase5IntegratedExperiment::savePhaseCheckpoint(
    Brain* brain,
    const std::string& phase,
    uint64_t step
) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot save checkpoint: brain is null");
        return;
    }
    
    std::string filename = "checkpoint_" + phase + "_step_" + std::to_string(step) + ".bin";
    NLM_LOG_INFO("Saving checkpoint: " + filename);
    
    if (brain->save(filename)) {
        NLM_LOG_INFO("Checkpoint saved successfully");
    } else {
        NLM_LOG_ERROR("Failed to save checkpoint: " + filename);
    }
}

LifetimePhaseResult Phase5IntegratedExperiment::collectPhaseResults(
    Brain* brain,
    const std::string& phaseName,
    uint64_t startStep,
    uint64_t endStep
) {
    return runPhase(brain, phaseName, startStep, endStep, [](uint64_t) {});
}

void Phase5IntegratedExperiment::generateVisualizations(
    const LifetimeExperimentResult& result,
    const std::string& outputDir
) {
    NLM_LOG_INFO("Generating visualizations to: " + outputDir);
    
    // Placeholder for visualization generation
    // In a real implementation, this would generate plots, charts, and graphs
    NLM_LOG_DEBUG("Visualization generation not yet implemented");
}

std::string Phase5IntegratedExperiment::generateReport(
    const LifetimeExperimentResult& result
) {
    std::ostringstream oss;
    oss << "\n";
    oss << "╔══════════════════════════════════════════════════════════════════╗\n";
    oss << "║          NLM Phase 5 Lifetime Experiment - Final Report           ║\n";
    oss << "╚══════════════════════════════════════════════════════════════════╝\n\n";
    
    oss << result.summary();
    
    return oss.str();
}

// ============================================================================
// MultiSeedExperimentRunner
// ============================================================================

MultiSeedExperimentRunner::MultiSeedExperimentRunner()
    : baseSeed_(42), rng_(baseSeed_) {
    NLM_LOG_INFO("Initializing Multi-Seed Experiment Runner");
}

MultiSeedExperimentRunner::~MultiSeedExperimentRunner() = default;

std::vector<LifetimeExperimentResult> MultiSeedExperimentRunner::runWithMultipleSeeds(
    size_t numSeeds,
    const LifetimeExperimentConfig& baseConfig,
    std::function<std::shared_ptr<Brain>(const LifetimeExperimentConfig& config, uint64_t seed)> createBrain,
    std::function<bool(Brain* brain, uint64_t steps)> runSimulation
) {
    if (numSeeds == 0) {
        throw std::invalid_argument("Number of seeds must be greater than 0");
    }
    
    std::vector<LifetimeExperimentResult> results;
    results.reserve(numSeeds);
    
    Phase5IntegratedExperiment experiment;
    
    NLM_LOG_INFO("Running experiment with " + std::to_string(numSeeds) + " different seeds");
    
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
    
    NLM_LOG_INFO("Multi-seed experiment completed: " + std::to_string(results.size()) + "/" + 
                 std::to_string(numSeeds) + " seeds successful");
    return results;
}

MultiSeedExperimentRunner::SeedStatistics MultiSeedExperimentRunner::computeStatistics(
    const std::vector<LifetimeExperimentResult>& results
) {
    SeedStatistics stats{};
    
    if (results.empty()) {
        NLM_LOG_WARN("Cannot compute statistics from empty results");
        return stats;
    }
    
    std::vector<float> rewards;
    rewards.reserve(results.size());
    
    for (const auto& r : results) {
        rewards.push_back(r.totalReward);
    }
    
    // Compute statistics
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
    
    // Compute standard deviation
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
    
    NLM_LOG_INFO("Computed statistics: mean=" + std::to_string(stats.meanReward) + 
                 ", stddev=" + std::to_string(stats.stdDevReward) + 
                 ", min=" + std::to_string(stats.minPerformance) + 
                 ", max=" + std::to_string(stats.maxPerformance));
    
    return stats;
}

std::string MultiSeedExperimentRunner::generateReport(
    const std::vector<LifetimeExperimentResult>& results,
    const SeedStatistics& stats
) {
    std::ostringstream oss;
    oss << "\n";
    oss << "═══════════════════════════════════════════════════════════════════\n";
    oss << "              Multi-Seed Experiment Results\n";
    oss << "═══════════════════════════════════════════════════════════════════\n\n";
    
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

// Helper function for escaping strings in JSON
inline std::string Phase5IntegratedExperiment::escapeString(const std::string& str) {
    std::string escaped = str;
    // Escape quotes and backslashes
    size_t pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\");
        pos += 2;
    }
    pos = 0;
    while ((pos = escaped.find('\\', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\\\");
        pos += 2;
    }
    return escaped;
}

} // namespace nlm
