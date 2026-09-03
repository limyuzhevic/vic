#include "Phase5Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>

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
        oss << "      \"name\": \"" << phase.phaseName << "\",\n";
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
    auto result = initializeResult(config);
    
    NLM_LOG_INFO("Starting Phase 5 Lifetime Experiment");
    NLM_LOG_INFO("Neurons: " + std::to_string(config.initialNeurons) + 
                 " -> " + std::to_string(config.maxNeurons));
    
    // Create brain
    auto brain = createBrain(config);
    if (!brain) {
        result.failures.push_back("Failed to create brain");
        return result;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    uint64_t currentStep = 0;
    
    // Phase 1: Early Development
    {
        NLM_LOG_INFO("Phase 1: Early Development");
        uint64_t phaseEnd = currentStep + config.synapseFormationSteps;
        auto phaseResult = runPhase(
            brain.get(), "Early Development", currentStep, phaseEnd,
            [](uint64_t step) {
                if (step % 1000 == 0) {
                    NLM_LOG_DEBUG("  Step " + std::to_string(step));
                }
            }
        );
        result.phaseResults.push_back(phaseResult);
        currentStep = phaseEnd;
    }
    
    // Phase 2: Sensory Development
    {
        NLM_LOG_INFO("Phase 2: Sensory Development");
        uint64_t phaseEnd = currentStep + config.sensoryDevelopmentSteps;
        auto phaseResult = runPhase(
            brain.get(), "Sensory Development", currentStep, phaseEnd,
            [](uint64_t step) {}
        );
        result.phaseResults.push_back(phaseResult);
        currentStep = phaseEnd;
    }
    
    // Phase 3: Motor Development
    {
        NLM_LOG_INFO("Phase 3: Motor Development");
        uint64_t phaseEnd = currentStep + config.motorDevelopmentSteps;
        auto phaseResult = runPhase(
            brain.get(), "Motor Development", currentStep, phaseEnd,
            [](uint64_t step) {}
        );
        result.phaseResults.push_back(phaseResult);
        currentStep = phaseEnd;
    }
    
    // Phase 4: Associative Development
    {
        NLM_LOG_INFO("Phase 4: Associative Development");
        uint64_t phaseEnd = currentStep + config.associativeDevelopmentSteps;
        auto phaseResult = runPhase(
            brain.get(), "Associative Development", currentStep, phaseEnd,
            [](uint64_t step) {}
        );
        result.phaseResults.push_back(phaseResult);
        currentStep = phaseEnd;
    }
    
    // Phase 5: Memory Consolidation
    {
        NLM_LOG_INFO("Phase 5: Memory Consolidation");
        uint64_t phaseEnd = currentStep + config.memoryConsolidationSteps;
        auto phaseResult = runPhase(
            brain.get(), "Memory Consolidation", currentStep, phaseEnd,
            [](uint64_t step) {}
        );
        result.phaseResults.push_back(phaseResult);
        currentStep = phaseEnd;
    }
    
    // Phase 6: Social Development
    {
        NLM_LOG_INFO("Phase 6: Social Development");
        uint64_t phaseEnd = currentStep + config.socialDevelopmentSteps;
        auto phaseResult = runPhase(
            brain.get(), "Social Development", currentStep, phaseEnd,
            [](uint64_t step) {}
        );
        result.phaseResults.push_back(phaseResult);
        currentStep = phaseEnd;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Compute overall metrics
    float totalPhaseReward = 0.0f;
    for (const auto& phase : result.phaseResults) {
        totalPhaseReward += phase.totalReward;
    }
    result.totalReward = totalPhaseReward;
    result.avgRewardPerStep = result.phaseResults.empty() ? 0.0f : 
        totalPhaseReward / static_cast<float>(currentStep);
    
    NLM_LOG_INFO("Phase 5 Experiment Complete");
    NLM_LOG_INFO("Total steps: " + std::to_string(currentStep));
    NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
    
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
    
    // Pre-damage baseline
    {
        auto phaseResult = runPhase(
            brain.get(), "Pre-Damage Baseline",
            0, damageStep,
            [](uint64_t step) {}
        );
        result.phaseResults.push_back(phaseResult);
    }
    
    // Note: Actual damage application would be done through the brain interface
    // This is a placeholder for the experiment framework
    
    // Post-damage recovery
    {
        auto phaseResult = runPhase(
            brain.get(), "Recovery",
            damageStep, damageStep + 50000,
            [](uint64_t step) {}
        );
        result.phaseResults.push_back(phaseResult);
    }
    
    return result;
}

ScalingAnalysis Phase5IntegratedExperiment::runScalingExperiment(
    const std::vector<ScaleLevel>& scales,
    uint64_t stepsPerScale
) {
    NLM_LOG_INFO("Starting Scaling Experiment");
    
    ScalingBenchmark benchmark;
    BenchmarkConfig config;
    config.stepsPerScale = stepsPerScale;
    config.enableMultithreading = true;
    config.enableSIMD = true;
    
    ScalingAnalysis analysis;
    
    for (const auto& scale : scales) {
        NLM_LOG_INFO("Scale: " + scale.name + 
                     " (" + std::to_string(scale.neurons) + " neurons)");
        
        BenchmarkRun run;
        run.name = scale.name;
        run.scale = scale;
        run.randomSeed = 42;
        run.stepsRun = stepsPerScale;
        
        // Run benchmark (simplified - actual implementation would create brain and run)
        // This is a placeholder
        
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
        // Actual ablation running would be implemented here
        results.push_back(result);
    }
    
    return results;
}

LifetimeExperimentResult Phase5IntegratedExperiment::initializeResult(
    const LifetimeExperimentConfig& config
) {
    LifetimeExperimentResult result;
    result.config = config;
    result.startTime = time(nullptr);
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
    
    // Simulate the phase
    for (uint64_t step = startStep; step < endStep; ++step) {
        brain->step(step, step * 0.001);  // 1ms timestep
        
        progressCallback(step);
        
        if (step % 1000 == 0) {
            // Collect basic metrics
            result.totalReward += brain->getAverageFiringRate() * 0.01f;
            result.avgFiringRate = brain->getAverageFiringRate();
        }
    }
    
    auto phaseEnd = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(phaseEnd - phaseStart).count();
    
    return result;
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
    : baseSeed_(42), rng_(baseSeed_)
{}

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

} // namespace nlm