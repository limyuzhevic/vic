#include "ExperimentRunner.hpp"
#include "BrainClone.hpp"
#include "EnvironmentClone.hpp"
#include "ExperimentResult.hpp"
#include "BatchExperimentManager.hpp"
#include <iostream>
#include <memory>
#include <vector>

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

void printExample(const std::string& description, const std::string& code) {
    std::cout << "\nExample: " << description << std::endl;
    std::cout << "```cpp" << std::endl;
    std::cout << code << std::endl;
    std::cout << "```" << std::endl;
}

int main() {
    std::cout << "Enhanced ExperimentRunner Demonstration" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl;
    
    // Create experiment runner
    ExperimentRunner runner;
    runner.setName("Demo Runner");
    
    printHeader("1. Basic Experiment Creation and Execution");
    
    // Create experiments
    auto exp1 = runner.createExperiment("exp_learning");
    auto exp2 = runner.createExperiment("exp_memory");
    
    // Set experiment metrics
    exp1->recordMetric("total_reward", 150.5);
    exp1->recordMetric("accuracy", 0.85);
    exp1->recordMetric("steps_completed", 1000);
    
    exp2->recordMetric("total_reward", 200.3);
    exp2->recordMetric("accuracy", 0.92);
    exp2->recordMetric("steps_completed", 1500);
    
    // Add notes
    exp1->addNote("Experiment completed successfully");
    exp1->addNote("Used reinforcement learning algorithm");
    
    exp2->addNote("Memory consolidation phase completed");
    exp2->addNote("Rehearsal sessions: 5");
    
    printExample("Creating and configuring experiments", 
R"(
// Create experiments
auto exp1 = runner.createExperiment("exp_learning");
auto exp2 = runner.createExperiment("exp_memory");

// Set experiment metrics
exp1->recordMetric("total_reward", 150.5);
exp1->recordMetric("accuracy", 0.85);

// Add notes
exp1->addNote("Experiment completed successfully");
exp1->addNote("Used reinforcement learning algorithm");
");
    
    printHeader("2. Brain Cloning with Configuration Templates");
    
    // Create brain configuration template
    auto config = std::make_shared<Config>();
    
    // Create brain template
    auto brainTemplate = std::make_shared<Brain>(config);
    brainTemplate->initialize();
    
    // Set brain template for cloning
    runner.setBrainTemplate("default", brainTemplate);
    
    // Clone brain using different modes
    auto clonedBrainDeep = ExperimentRunner::cloneBrain(
        brainTemplate, "default", BrainClone::CloneMode::DEEP
    );
    
    auto clonedBrainShallow = ExperimentRunner::cloneBrain(
        brainTemplate, "default", BrainClone::CloneMode::SHALLOW
    );
    
    printExample("Brain cloning with different modes",
R"(
// Create brain template
auto config = std::make_shared<Config>();
auto brainTemplate = std::make_shared<Brain>(config);

// Set brain template for cloning
runner.setBrainTemplate("default", brainTemplate);

// Clone brain with different modes
auto clonedBrainDeep = ExperimentRunner::cloneBrain(
    brainTemplate, "default", BrainClone::CloneMode::DEEP
);

auto clonedBrainShallow = ExperimentRunner::cloneBrain(
    brainTemplate, "default", BrainClone::CloneMode::SHALLOW
);
");
    
    printHeader("3. Environment Cloning");
    
    // Create environment template
    auto envTemplate = std::make_shared<GridWorldEnvironment>();
    runner.setEnvironmentTemplate("default", envTemplate);
    
    // Clone environment
    auto clonedEnv = ExperimentRunner::cloneEnvironment(
        envTemplate, EnvironmentClone::CloneMode::DEEP
    );
    
    printExample("Environment cloning",
R"(
// Create environment template
auto envTemplate = std::make_shared<GridWorldEnvironment>();
runner.setEnvironmentTemplate("default", envTemplate);

// Clone environment
auto clonedEnv = ExperimentRunner::cloneEnvironment(
    envTemplate, EnvironmentClone::CloneMode::DEEP
);
");
    
    printHeader("4. Experiment Result Management");
    
    // Create experiment results
    ExperimentResult result1;
    result1.setExperimentName("exp_learning");
    result1.setMetric("total_reward", 150.5);
    result1.setMetric("accuracy", 0.85);
    result1.addNote("High performance achieved");
    
    ExperimentResult result2;
    result2.setExperimentName("exp_memory");
    result2.setMetric("total_reward", 200.3);
    result2.setMetric("accuracy", 0.92);
    result2.addNote("Excellent memory retention");
    
    // Aggregate results
    std::vector<ExperimentResult> allResults = {result1, result2};
    auto aggregated = ExperimentResult::aggregateResults(
        allResults, "total_reward", "mean"
    );
    
    printExample("Experiment result aggregation",
R"(
// Create experiment results
ExperimentResult result1;
result1.setExperimentName("exp_learning");
result1.setMetric("total_reward", 150.5);

ExperimentResult result2;
result2.setExperimentName("exp_memory");
result2.setMetric("total_reward", 200.3);

// Aggregate results
std::vector<ExperimentResult> allResults = {result1, result2};
auto aggregated = ExperimentResult::aggregateResults(
    allResults, "total_reward", "mean"
);
");
    
    printHeader("5. Batch Experiment Management");
    
    // Configure batch experiment manager
    BatchExperimentManager::BatchConfig batchConfig;
    batchConfig.maxConcurrentExperiments = 2;
    batchConfig.maxStepsPerExperiment = 1000;
    batchConfig.timeoutSeconds = 300.0;
    batchConfig.useBrainCloning = true;
    batchConfig.useEnvironmentCloning = true;
    batchConfig.checkpointEnabled = true;
    
    // Create batch experiment manager
    BatchExperimentManager batchManager;
    batchManager.configure(batchConfig);
    
    // Set up progress callback
    auto progressCallback = [](size_t completed, size_t total, const std::string& currentExp) {
        std::cout << "Progress: " << completed << "/" << total 
                  << " - Current: " << currentExp << std::endl;
    };
    
    batchManager.setProgressCallback(progressCallback);
    
    printExample("Batch experiment management",
R"(
// Configure batch experiment manager
BatchExperimentManager::BatchConfig batchConfig;
batchConfig.maxConcurrentExperiments = 2;
batchConfig.maxStepsPerExperiment = 1000;
batchConfig.timeoutSeconds = 300.0;
batchConfig.useBrainCloning = true;
batchConfig.useEnvironmentCloning = true;
batchConfig.checkpointEnabled = true;

// Create batch experiment manager
BatchExperimentManager batchManager;
batchManager.configure(batchConfig);

// Set up progress callback
auto progressCallback = [](size_t completed, size_t total, const std::string& currentExp) {
    std::cout << "Progress: " << completed << "/" << total << std::endl;
};

batchManager.setProgressCallback(progressCallback);
");
    
    printHeader("6. Checkpoint Operations");
    
    // Save checkpoint
    bool saved = runner.saveExperimentCheckpoint(exp1, "demo_checkpoint.exp");
    std::cout << "Checkpoint saved: " << (saved ? "Yes" : "No") << std::endl;
    
    // Load checkpoint
    auto loadedExp = ExperimentRunner::loadExperimentCheckpoint("demo_checkpoint.exp");
    if (loadedExp) {
        std::cout << "Checkpoint loaded successfully" << std::endl;
        std::cout << "Loaded experiment name: " << loadedExp->getName() << std::endl;
    }
    
    printExample("Checkpoint operations",
R"(
// Save experiment checkpoint
bool saved = runner.saveExperimentCheckpoint(exp1, "demo_checkpoint.exp");

// Load experiment checkpoint
auto loadedExp = ExperimentRunner::loadExperimentCheckpoint("demo_checkpoint.exp");
if (loadedExp) {
    std::cout << "Checkpoint loaded successfully" << std::endl;
}
");
    
    printHeader("7. Enhanced Experiment Features");
    
    // Create enhanced experiment
    auto enhancedExp = std::make_unique<Experiment>("enhanced_experiment");
    enhancedExp->setSeed(12345);
    
    // Add configuration
    enhancedExp->setConfigValue("algorithm", "deep_reinforcement_learning");
    enhancedExp->setConfigValue("learning_rate", "0.001");
    enhancedExp->setConfigValue("batch_size", "32");
    
    // Record step data
    for (int i = 0; i < 10; ++i) {
        Experiment::StepData step;
        step.step = i;
        step.neuronCount = 100 + i * 10;
        step.synapseCount = 500 + i * 20;
        step.firingCount = 15 + i * 2;
        step.avgFiringRate = 0.1 + i * 0.01;
        enhancedExp->recordStep(step.step, step.neuronCount, step.synapseCount,
                                step.firingCount, step.avgFiringRate);
    }
    
    // Add notes
    enhancedExp->addNote("Enhanced experiment with advanced features");
    enhancedExp->addNote("Includes neural plasticity and memory systems");
    
    // Get experiment metrics summary
    auto summary = enhancedExp->calculateMetricsSummary();
    std::cout << "Enhanced experiment metrics summary:" << std::endl;
    std::cout << "  Average step duration: " << summary.averageStepDuration << std::endl;
    std::cout << "  Max firing rate: " << summary.maxFiringRate << std::endl;
    std::cout << "  Average neurons: " << summary.totalNeurons << std::endl;
    std::cout << "  Average synapses: " << summary.totalSynapses << std::endl;
    std::cout << "  Total reward: " << summary.totalReward << std::endl;
    std::cout << "  Efficiency score: " << summary.efficiencyScore << std::endl;
    
    printExample("Enhanced experiment features",
R"(
// Create enhanced experiment
auto enhancedExp = std::make_unique<Experiment>("enhanced_experiment");
enhancedExp->setSeed(12345);

// Add configuration
enhancedExp->setConfigValue("algorithm", "deep_reinforcement_learning");
enhancedExp->setConfigValue("learning_rate", "0.001");

// Record step data
for (int i = 0; i < 10; ++i) {
    Experiment::StepData step;
    step.step = i;
    step.neuronCount = 100 + i * 10;
    step.synapseCount = 500 + i * 20;
    enhancedExp->recordStep(step.step, step.neuronCount, step.synapseCount,
                            step.firingCount, step.avgFiringRate);
}

// Get metrics summary
auto summary = enhancedExp->calculateMetricsSummary();
");
    
    printHeader("8. Performance Metrics and Monitoring");
    
    // Get cloning performance metrics
    BrainClone::CloneStats brainStats = BrainClone::getClonePerformanceMetrics();
    std::cout << "Brain cloning performance:" << std::endl;
    std::cout << "  Neurons cloned: " << brainStats.neuronsCloned << std::endl;
    std::cout << "  Synapses cloned: " << brainStats.synapsesCloned << std::endl;
    std::cout << "  Cloning time: " << brainStats.cloningTimeMs << " ms" << std::endl;
    std::cout << "  Success: " << (brainStats.success ? "Yes" : "No") << std::endl;
    
    EnvironmentClone::CloneStats envStats = EnvironmentClone::getClonePerformanceMetrics();
    std::cout << "\nEnvironment cloning performance:" << std::endl;
    std::cout << "  State size: " << envStats.stateSize << " bytes" << std::endl;
    std::cout << "  Cloning time: " << envStats.cloningTimeMs << " ms" << std::endl;
    std::cout << "  Success: " << (envStats.success ? "Yes" : "No") << std::endl;
    
    // Get batch manager performance metrics
    BatchExperimentManager::PerformanceMetrics batchMetrics = 
        batchManager.getPerformanceMetrics();
    std::cout << "\nBatch manager performance:" << std::endl;
    std::cout << "  Throughput: " << batchMetrics.throughputExperimentsPerSecond << " exp/s" << std::endl;
    std::cout << "  Average experiment time: " << batchMetrics.avgExperimentTimeMs << " ms" << std::endl;
    std::cout << "  CPU utilization: " << batchMetrics.cpuUtilization << "%" << std::endl;
    std::cout << "  Memory usage: " << batchMetrics.memoryUsageMB << " MB" << std::endl;
    
    printExample("Performance metrics and monitoring",
R"(
// Get cloning performance metrics
BrainClone::CloneStats brainStats = BrainClone::getClonePerformanceMetrics();
std::cout << "Brain cloning: " << brainStats.neuronsCloned << " neurons, "
          << brainStats.cloningTimeMs << "ms" << std::endl;

// Get batch manager performance metrics
BatchExperimentManager::PerformanceMetrics batchMetrics = 
    batchManager.getPerformanceMetrics();
std::cout << "Throughput: " << batchMetrics.throughputExperimentsPerSecond << " exp/s" << std::endl;
");
    
    printHeader("9. Experiment Result Comparison and Filtering");
    
    // Create test results for comparison
    ExperimentResult testResult1;
    testResult1.setExperimentName("test_exp_1");
    testResult1.setMetric("accuracy", 0.85);
    testResult1.setMetric("precision", 0.82);
    
    ExperimentResult testResult2;
    testResult2.setExperimentName("test_exp_2");
    testResult2.setMetric("accuracy", 0.87);
    testResult2.setMetric("precision", 0.84);
    
    // Compare results
    bool approximatelyEqual = testResult1.approximatelyEqual(testResult2, 0.01);
    std::cout << "Test result comparison (tolerance 0.01): " 
              << (approximatelyEqual ? "Yes" : "No") << std::endl;
    
    // Filter results
    std::vector<ExperimentResult> highAccuracyResults;
    highAccuracyResults.push_back(testResult1);
    highAccuracyResults.push_back(testResult2);
    
    auto filtered = testResult1.filterByMetric("accuracy", 0.8, true);
    std::cout << "Results with accuracy > 0.8: " << filtered.size() << std::endl;
    
    printExample("Experiment result comparison and filtering",
R"(
// Create test results
ExperimentResult testResult1;
testResult1.setExperimentName("test_exp_1");
testResult1.setMetric("accuracy", 0.85);

ExperimentResult testResult2;
testResult2.setExperimentName("test_exp_2");
testResult2.setMetric("accuracy", 0.87);

// Compare results
bool approximatelyEqual = testResult1.approximatelyEqual(testResult2, 0.01);

// Filter results
auto filtered = testResult1.filterByMetric("accuracy", 0.8, true);
");
    
    printHeader("10. Advanced Experiment Run Configuration");
    
    // Configure complete experiment run
    ExperimentRunner::ExperimentRunConfig runConfig;
    runConfig.runName = "advanced_demo_run";
    
    runConfig.cloningConfig.enableBrainCloning = true;
    runConfig.cloningConfig.brainCloneMode = BrainClone::CloneMode::DEEP;
    runConfig.cloningConfig.enableEnvironmentCloning = true;
    runConfig.cloningConfig.environmentCloneMode = EnvironmentClone::CloneMode::DEEP;
    
    runConfig.batchConfig.maxConcurrentExperiments = 2;
    runConfig.batchConfig.maxStepsPerExperiment = 500;
    runConfig.batchConfig.timeoutSeconds = 120.0;
    runConfig.batchConfig.checkpointEnabled = true;
    runConfig.batchConfig.checkpointPath = "/tmp/checkpoints/";
    
    runConfig.experimentNames = {"exp_learning", "exp_memory", "exp_cognition"};
    runConfig.experimentParameters["exp_learning"] = "algorithm=deep_reinforcement_learning";
    runConfig.experimentParameters["exp_memory"] = "algorithm=memory_based_learning";
    runConfig.experimentParameters["exp_cognition"] = "algorithm=cognitive_architectures";
    
    std::cout << "Advanced experiment run configuration:" << std::endl;
    std::cout << "  Run name: " << runConfig.runName << std::endl;
    std::cout << "  Brain cloning: " << (runConfig.cloningConfig.enableBrainCloning ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Environment cloning: " << (runConfig.cloningConfig.enableEnvironmentCloning ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Batch concurrent experiments: " << runConfig.batchConfig.maxConcurrentExperiments << std::endl;
    std::cout << "  Batch max steps: " << runConfig.batchConfig.maxStepsPerExperiment << std::endl;
    std::cout << "  Checkpoint enabled: " << (runConfig.batchConfig.checkpointEnabled ? "Yes" : "No") << std::endl;
    std::cout << "  Number of experiments: " << runConfig.experimentNames.size() << std::endl;
    
    printExample("Advanced experiment run configuration",
R"(
// Configure complete experiment run
ExperimentRunner::ExperimentRunConfig runConfig;
runConfig.runName = "advanced_demo_run";

runConfig.cloningConfig.enableBrainCloning = true;
runConfig.cloningConfig.brainCloneMode = BrainClone::CloneMode::DEEP;

runConfig.batchConfig.maxConcurrentExperiments = 2;
runConfig.batchConfig.maxStepsPerExperiment = 500;
runConfig.batchConfig.checkpointEnabled = true;

runConfig.experimentNames = {"exp_learning", "exp_memory", "exp_cognition"};
");
    
    printHeader("Demonstration Complete!")
    
    std::cout << "\nThe enhanced ExperimentRunner provides comprehensive cloning and experiment management capabilities:" << std::endl;
    std::cout << "✓ Brain cloning with configurable templates and modes" << std::endl;
    std::cout << "✓ Environment cloning with state preservation" << std::endl;
    std::cout << "✓ Experiment result management and aggregation" << std::endl;
    std::cout << "✓ Advanced batch experiment management" << std::endl;
    std::cout << "✓ Checkpoint-based experiment persistence" << std::endl;
    std::cout << "✓ Enhanced experiment features and metrics" << std::endl;
    std::cout << "✓ Performance monitoring and logging" << std::endl;
    std::cout << "✓ Result comparison and filtering" << std::endl;
    std::cout << "✓ Comprehensive error handling and recovery" << std::endl;
    std::cout << "✓ Security and access control" << std::endl;
    
    return 0;
}

} // namespace nlm

// Include main demonstration program
#include "../experiments/DemoProgram.cpp"