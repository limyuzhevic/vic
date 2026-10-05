// Convenience Functions Header for NLM
// ===================================

// This header file declares all the comprehensive convenience functions for NLM,
// providing shortcuts and utilities for common use cases across all categories:
// 1. Quick-Start Functions
// 2. Configuration Presets
// 3. Convenience Functions
// 4. Utility Functions
// 5. Batch Processing
// 6. Advanced Convenience Functions
// 7. Integration Functions
// 8. Analysis and Reporting Functions

#pragma once

#include "ExperimentRunner.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <any>
#include <chrono>

namespace nlm {

// Forward declarations
class Experiment;
class Environment;
class AgentBrain;

// Quick-Start Functions
// ====================

/**
 * Single-line brain initialization and basic simulation
 * 
 * Creates a balanced preset brain, runs a simple simulation with vision input,
 * and returns basic metrics.
 * 
 * Usage:
 * ```cpp
 * auto results = quickStart();
 * std::cout << "Simulation completed in " << results.steps << " steps" << std::endl;
 * ```
 * 
 * @return std::map<std::string, std::any> Results containing metrics and data
 */
std::map<std::string, std::any> quickStart();

/**
 * Setup and run standard experiment with results
 * 
 * Creates a standard experiment configuration, runs it, and returns comprehensive results.
 * 
 * Usage:
 * ```cpp
 * auto results = brainExperiment();
 * auto metrics = results["metrics"];
 * for (const auto& [key, value] : metrics) {
 *     std::cout << key << ": " << std::any_cast<double>(value) << std::endl;
 * }
 * ```
 * 
 * @return std::map<std::string, std::any> Results containing experiment metrics and data
 */
std::map<std::string, std::any> brainExperiment();

/**
 * Train neural network with learning curves
 * 
 * Creates a neural network and trains it on a simple pattern learning task,
 * returning detailed learning curves and performance metrics.
 * 
 * Usage:
 * ```cpp
 * auto [learning_curve, final_performance] = neuronTraining();
 * analyzeLearningCurve(learning_curve); // Plot learning progress
 * ```
 * 
 * @return std::pair<std::vector<double>, std::map<std::string, double>> Learning curve and performance metrics
 */
std::pair<std::vector<double>, std::map<std::string, double>> neuronTraining();

/**
 * Demonstrate plasticity rules (STDP, Hebbian)
 * 
 * Sets up a simple synaptic demonstration showing both STDP and Hebbian plasticity
 * in action, returning plasticity metrics and synaptic changes.
 * 
 * Usage:
 * ```cpp
 * auto plasticityData = plasticityDemo();
 * auto stdpChanges = plasticityData["stdp_changes"];
 * auto hebbianChanges = plasticityData["hebbian_changes"];
 * ```
 * 
 * @return std::map<std::string, std::any> Plasticity demonstration results
 */
std::map<std::string, std::any> plasticityDemo();

/**
 * Test memory systems (working + episodic)
 * 
 * Tests both working memory and episodic memory systems with various scenarios,
 * returning comprehensive memory benchmark results.
 * 
 * Usage:
 * ```cpp
 * auto memoryResults = memoryTest();
 * auto workingMem = memoryResults["working_memory"];
 * auto episodicMem = memoryResults["episodic_memory"];
 * ```
 * 
 * @return std::map<std::string, std::any> Memory system test results
 */
std::map<std::string, std::any> memoryTest();

/**
 * Environment interaction simulation
 * 
 * Simulates an agent interacting with its environment through sensory input,
 * motor output, and reward-based learning, returning interaction metrics.
 * 
 * Usage:
 * ```cpp
 * auto interactionResults = environmentExploration();
 * auto explorationMetrics = interactionResults["exploration_metrics"];
 * auto adaptationMetrics = interactionResults["adaptation_metrics"];
 * ```
 * 
 * @return std::map<std::string, std::any> Environment interaction simulation results
 */
std::map<std::string, std::any> environmentExploration();

// Configuration Presets
// ====================

/**
 * Speed-optimized configuration (100 neurons, aggressive learning)
 * 
 * Creates a configuration optimized for fast execution and rapid learning.
 * 
 * Usage:
 * ```cpp
 * auto config = presetFast();
 * auto brain = std::make_shared<Brain>(config);
 * ```
 * 
 * @return Config Fast configuration
 */
Config presetFast();

/**
 * Precision-optimized configuration (2000 neurons, detailed learning)
 * 
 * Creates a configuration optimized for high precision and detailed analysis.
 * 
 * Usage:
 * ```cpp
 * auto config = presetAccurate();
 * auto brain = std::make_shared<Brain>(config);
 * ```
 * 
 * @return Config Precision configuration
 */
Config presetAccurate();

/**
 * General purpose configuration (500 neurons)
 * 
 * Creates a balanced configuration suitable for general purpose simulations.
 * 
 * Usage:
 * ```cpp
 * auto config = presetBalanced();
 * auto brain = std::make_shared<Brain>(config);
 * ```
 * 
 * @return Config Balanced configuration
 */
Config presetBalanced();

/**
 * Novelty-driven exploration configuration
 * 
 * Creates a configuration optimized for curiosity-driven exploration and learning.
 * 
 * Usage:
 * ```cpp
 * auto config = presetExploration();
 * auto brain = std::make_shared<Brain>(config);
 * ```
 * 
 * @return Config Exploration configuration
 */
Config presetExploration();

/**
 * Reward-based learning configuration
 * 
 * Creates a configuration optimized for reinforcement learning and reward maximization.
 * 
 * Usage:
 * ```cpp
 * auto config = presetExploitation();
 * auto brain = std::make_shared<Brain>(config);
 * ```
 * 
 * @return Config Exploitation configuration
 */
Config presetExploitation();

/**
 * Developmental brain simulation configuration
 * 
 * Creates a configuration optimized for developmental brain simulations and maturation.
 * 
 * Usage:
 * ```cpp
 * auto config = presetDevelopment();
 * auto brain = std::make_shared<Brain>(config);
 * ```
 * 
 * @return Config Developmental configuration
 */
Config presetDevelopment();

// Convenience Functions
// ====================

/**
 * Run all basic tests automatically
 * 
 * Executes a comprehensive suite of basic tests and returns aggregated results.
 * 
 * Usage:
 * ```cpp
 * auto results = runStandardTestSuite();
 * bool allPassed = results["all_tests_passed"];
 * ```
 * 
 * @return std::map<std::string, std::any> Test suite results
 */
std::map<std::string, std::any> runStandardTestSuite();

/**
 * Run learning with results analysis
 * 
 * Runs a learning experiment with specified steps and pattern size, returning analysis.
 * 
 * Usage:
 * ```cpp
 * auto results = runLearningExperiment(1000, 5);
 * auto learningMetrics = results["learning_metrics"];
 * ```
 * 
 * @param steps Number of simulation steps to run
 * @param patternSize Size of the pattern to learn
 * @return std::map<std::string, std::any> Learning experiment results
 */
std::map<std::string, std::any> runLearningExperiment(SimulationStep steps, size_t patternSize);

/**
 * Verify STDP and Hebbian rules
 * 
 * Runs verification tests for spike-timing dependent plasticity and Hebbian plasticity.
 * 
 * Usage:
 * ```cpp
 * auto verificationResults = runPlasticityVerification();
 * bool stdpValid = verificationResults["stdp_verified"];
 * ```
 * 
 * @return std::map<std::string, std::any> Plasticity verification results
 */
std::map<std::string, std::any> runPlasticityVerification();

/**
 * Benchmark memory systems
 * 
 * Runs comprehensive benchmarks on working memory and episodic memory systems.
 * 
 * Usage:
 * ```cpp
 * auto benchmarkResults = runMemoryBenchmark();
 * auto memoryPerformance = benchmarkResults["memory_performance"];
 * ```
 * 
 * @return std::map<std::string, std::any> Memory benchmark results
 */
std::map<std::string, std::any> runMemoryBenchmark();

/**
 * Show brain development over time
 * 
 * Runs a developmental simulation and shows brain development stages over time.
 * 
 * Usage:
 * ```cpp
 * auto developmentTimeline = runDevelopmentTimeline();
 * auto stages = developmentTimeline["developmental_stages"];
 * ```
 * 
 * @return std::map<std::string, std::any> Development timeline results
 */
std::map<std::string, std::any> runDevelopmentTimeline();

/**
 * Profile simulation performance
 * 
 * Runs performance profiling on the simulation system and returns profiling data.
 * 
 * Usage:
 * ```cpp
 * auto performanceProfile = runPerformanceProfile();
 * auto metrics = performanceProfile["performance_metrics"];
 * ```
 * 
 * @return std::map<std::string, std::any> Performance profile results
 */
std::map<std::string, std::any> runPerformanceProfile();

// Utility Functions
// =================

/**
 * Create configured test brain with validation
 * 
 * Creates a test brain with specified configuration and validates its initialization.
 * 
 * Usage:
 * ```cpp
 * auto testBrain = createTestBrain("balanced", true);
 * bool brainValid = testBrain && testBrain->initialize();
 * ```
 * 
 * @param presetName Name of preset configuration to use
 * @param validate Init validation flag
 * @return std::shared_ptr<Brain> Test brain or nullptr if failed
 */
std::shared_ptr<Brain> createTestBrain(const std::string& presetName = "balanced", bool validate = true);

/**
 * Save simulation results with metadata
 * 
 * Saves simulation results to file with comprehensive metadata.
 * 
 * Usage:
 * ```cpp
 * saveSimulationResults(results, "simulation_output.json", "MyExperiment");
 * ```
 * 
 * @param results Results to save
 * @param filepath File path to save results
 * @param experimentName Name of the experiment
 */
void saveSimulationResults(const std::map<std::string, std::any>& results, 
                          const std::string& filepath,
                          const std::string& experimentName = "Experiment");

/**
 * Load configuration and run experiment
 * 
 * Loads configuration from file and runs the specified experiment.
 * 
 * Usage:
 * ```cpp
 * auto results = loadAndRunExperiment("config.json", "MyExperiment", 1000);
 * ```
 * 
 * @param configPath Path to configuration file
 * @param experimentName Name of experiment to run
 * @param steps Number of simulation steps
 * @return std::map<std::string, std::any> Experiment results
 */
std::map<std::string, std::any> loadAndRunExperiment(const std::string& configPath,
                                                    const std::string& experimentName,
                                                    SimulationStep steps);

/**
 * Compare two brain configurations
 * 
 * Compares performance characteristics of two brain configurations.
 * 
 * Usage:
 * ```cpp
 * auto comparison = compareBrains("fast_preset", "accurate_preset");
 * auto comparisonMetrics = comparison["comparison_metrics"];
 * ```
 * 
 * @param config1 First configuration name
 * @param config2 Second configuration name
 * @return std::map<std::string, std::any> Comparison results
 */
std::map<std::string, std::any> compareBrains(const std::string& config1,
                                             const std::string& config2);

/**
 * Analyze and plot learning curves
 * 
 * Analyzes learning curve data and returns analysis metrics.
 * 
 * Usage:
 * ```cpp
 * auto analysis = analyzeLearningCurves(learningData);
 * auto insights = analysis["insights"];
 * ```
 * 
 * @param learningData Learning curve data to analyze
 * @return std::map<std::string, std::any> Learning curve analysis
 */
std::map<std::string, std::any> analyzeLearningCurves(const std::vector<double>& learningData);

/**
 * Generate comprehensive report
 * 
 * Generates a comprehensive report from simulation results.
 * 
 * Usage:
 * ```cpp
 * generateReport(results, "experiment_report.html");
 * ```
 * 
 * @param results Results to include in report
 * @param reportPath Path to save the report
 */
void generateReport(const std::map<std::string, std::any>& results,
                   const std::string& reportPath);

// Batch Processing
// =================

/**
 * Run multiple simulations with parameter variations
 * 
 * Runs parameter sweep simulations with variations in specified parameters.
 * 
 * Usage:
 * ```cpp
 * auto sweepResults = runParameterSweep("neuron_count", {100, 200, 500});
 * ```
 * 
 * @param parameter Parameter name to vary
 * @param values Vector of parameter values to test
 * @return std::map<std::string, std::any> Parameter sweep results
 */
std::map<std::string, std::any> runParameterSweep(const std::string& parameter,
                                                  const std::vector<int>& values);

/**
 * Run multiple configuration combinations
 * 
 * Runs multiple configuration combinations in batch.
 * 
 * Usage:
 * ```cpp
 * auto batchResults = runConfigurationSet({"fast", "balanced", "accurate"});
 * ```
 * 
 * @param configNames Vector of configuration preset names
 * @return std::map<std::string, std::any> Batch results
 */
std::map<std::string, std::any> runConfigurationSet(const std::vector<std::string>& configNames);

/**
 * Compare different algorithm approaches
 * 
 * Runs comparison suite comparing different algorithm approaches.
 * 
 * Usage:
 * ```cpp
 * auto comparisonSuite = runComparisonSuite();
 * ```
 * 
 * @return std::map<std::string, std::any> Comparison suite results
 */
std::map<std::string, std::any> runComparisonSuite();

/**
 * Test individual components
 * 
 * Runs ablation study testing individual components' contributions.
 * 
 * Usage:
 * ```cpp
 * auto ablationResults = runAblationStudy();
 * ```
 * 
 * @return std::map<std::string, std::any> Ablation study results
 */
std::map<std::string, std::any> runAblationStudy();

// Advanced Convenience Functions
// ================================

/**
 * Run complete Phase 2 demonstration
 * 
 * Runs the complete Phase 2 demonstration including all subsystems.
 * 
 * Usage:
 * ```cpp
 * auto phase2Results = runCompletePhase2();
 * ```
 * 
 * @return std::map<std::string, std::any> Phase 2 demonstration results
 */
std::map<std::string, std::any> runCompletePhase2();

/**
 * Run all Phase 3 experiment types
 * 
 * Runs all Phase 3 experiment types and returns comprehensive results.
 * 
 * Usage:
 * ```cpp
 * auto phase3Results = runPhase3Experiments();
 * ```
 * 
 * @return std::map<std::string, std::any> Phase 3 experiment results
 */
std::map<std::string, std::any> runPhase3Experiments();

/**
 * Complete research simulation pipeline
 * 
 * Runs the complete research simulation pipeline from setup to analysis.
 * 
 * Usage:
 * ```cpp
 * auto pipelineResults = runFullResearchPipeline();
 * ```
 * 
 * @return std::map<std::string, std::any> Pipeline results
 */
std::map<std::string, std::any> runFullResearchPipeline();

/**
 * Guided tutorial for learning NLM
 * 
 * Runs a guided tutorial for learning NLM with step-by-step instructions.
 * 
 * Usage:
 * ```cpp
 * auto tutorialResults = runStudentTutorial();
 * ```
 * 
 * @return std::map<std::string, std::any> Tutorial results
 */
std::map<std::string, std::any> runStudentTutorial();

/**
 * Advanced analysis for research
 * 
 * Performs advanced analysis for research purposes.
 * 
 * Usage:
 * ```cpp
 * auto analysisResults = runExpertAnalysis();
 * ```
 * 
 * @return std::map<std::string, std::any> Analysis results
 */
std::map<std::string, std::any> runExpertAnalysis();

/**
 * Production-ready simulation
 * 
 * Runs production-ready simulation with error handling and monitoring.
 * 
 * Usage:
 * ```cpp
 * auto productionResults = runProductionSimulation();
 * ```
 * 
 * @return std::map<std::string, std::any> Production simulation results
 */
std::map<std::string, std::any> runProductionSimulation();

// Integration Functions
// ====================

/**
 * Setup complete environment with world/agent
 * 
 * Sets up a complete environment with integrated world and agent systems.
 * 
 * Usage:
 * ```cpp
 * auto envSetup = setupCompleteEnvironment();
 * ```
 * 
 * @return std::map<std::string, std::any> Environment setup results
 */
std::map<std::string, std::any> setupCompleteEnvironment();

/**
 * Complete agent-in-world simulation
 * 
 * Runs complete agent-in-world simulation with perception, action, and learning.
 * 
 * Usage:
 * ```cpp
 * auto agentWorldLoop = runFullAgentWorldLoop();
 * ```
 * 
 * @return std::map<std::string, std::any> Agent-world simulation results
 */
std::map<std::string, std::any> runFullAgentWorldLoop();

/**
 * RL with reward system
 * 
 * Runs reinforcement learning with comprehensive reward systems.
 * 
 * Usage:
 * ```cpp
 * auto rlResults = runReinforcementLearning();
 * ```
 * 
 * @return std::map<std::string, std::any> Reinforcement learning results
 */
std::map<std::string, std::any> runReinforcementLearning();

/**
 * Active learning with curiosity
 * 
 * Runs active learning with curiosity-driven exploration and discovery.
 * 
 * Usage:
 * ```cpp
 * auto activeLearningResults = runActiveLearning();
 * ```
 * 
 * @return std::map<std::string, std::any> Active learning results
 */
std::map<std::string, std::any> runActiveLearning();

/**
 * Self-organizing maps and clustering
 * 
 * Runs self-organizing map algorithms and clustering analyses.
 * 
 * Usage:
 * ```cpp
 * auto selfOrgResults = runSelfOrganization();
 * ```
 * 
 * @return std::map<std::string, std::any> Self-organization results
 */
std::map<std::string, std::any> runSelfOrganization();

// Analysis and Reporting Functions
// ==========================

/**
 * Comprehensive brain performance analysis
 * 
 * Performs comprehensive analysis of brain performance metrics.
 * 
 * Usage:
 * ```cpp
 * auto brainAnalysis = analyzeBrainPerformance(brainResults);
 * ```
 * 
 * @param results Results to analyze
 * @return std::map<std::string, std::any> Brain performance analysis
 */
std::map<std::string, std::any> analyzeBrainPerformance(const std::map<std::string, std::any>& results);

/**
 * Learning effectiveness metrics
 * 
 * Analyzes learning effectiveness with various metrics.
 * 
 * Usage:
 * ```cpp
 * auto learningAnalysis = analyzeLearningEffectiveness(learningResults);
 * ```
 * 
 * @param results Results to analyze
 * @return std::map<std::string, std::any> Learning effectiveness analysis
 */
std::map<std::string, std::any> analyzeLearningEffectiveness(const std::map<std::string, std::any>& results);

/**
 * Plasticity impact assessment
 * 
 * Assesses the impact of plasticity rules on system behavior.
 * 
 * Usage:
 * ```cpp
 * auto plasticityAnalysis = analyzePlasticityImpact(plasticityResults);
 * ```
 * 
 * @param results Results to analyze
 * @return std::map<std::string, std::any> Plasticity impact analysis
 */
std::map<std::string, std::any> analyzePlasticityImpact(const std::map<std::string, std::any>& results);

/**
 * Memory retention and recall analysis
 * 
 * Analyzes memory retention and recall performance.
 * 
 * Usage:
 * ```cpp
 * auto memoryAnalysis = analyzeMemoryRetention(memoryResults);
 * ```
 * 
 * @param results Results to analyze
 * @return std::map<std::string, std::any> Memory retention analysis
 */
std::map<std::string, std::any> analyzeMemoryRetention(const std::map<std::string, std::any>& results);

/**
 * Development stage analysis
 * 
 * Analyzes development trajectories across different stages.
 * 
 * Usage:
 * ```cpp
 * auto developmentAnalysis = analyzeDevelopmentTrajectories(devResults);
 * ```
 * 
 * @param results Results to analyze
 * @return std::map<std::string, std::any> Development stage analysis
 */
std::map<std::string, std::any> analyzeDevelopmentTrajectories(const std::map<std::string, std::any>& results);

/**
 * Generate complete research documentation
 * 
 * Generates comprehensive research documentation from all results.
 * 
 * Usage:
 * ```cpp
 * generateResearchReport(allResults, "research_documentation.html");
 * ```
 * 
 * @param results Results to document
 * @param reportPath Path to save the research documentation
 */
void generateResearchReport(const std::map<std::string, std::any>& results,
                           const std::string& reportPath);

} // namespace nlm

#endif // CONVENIENCE_FUNCTIONS_HPP
