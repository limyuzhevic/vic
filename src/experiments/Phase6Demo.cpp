/**
 * Phase 6 Demo - Enhanced Integration Test
 *
 * This enhanced demo runs the Phase 6 integration experiment with advanced features,
 * including real-time monitoring, performance profiling, and detailed analysis.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== Enhanced NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain with advanced analysis..." << std::endl << std::endl;
    
    // Initialize enhanced logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create enhanced experiment
    EnhancedPhase6IntegratedExperiment experiment;
    
    // Configure advanced experiment
    EnhancedPhase6Config config;
    config.baseConfig.maxSteps = 2000;
    config.baseConfig.neuronCount = 500;
    config.baseConfig.regionCount = 1;
    config.baseConfig.connectionProbability = 0.1f;
    config.baseConfig.enableCheckpointing = true;
    config.baseConfig.enableReplay = true;
    config.baseConfig.enableDevelopment = true;
    
    // Enable advanced features
    config.enableRealTimeMonitoring = true;
    config.enablePerformanceProfiling = true;
    config.enableDetailedLogging = true;
    config.enableVisualizationExport = false; // Set to true for detailed output
    config.enableAblationStudies = false;
    config.enableCustomScenarios = false;
    
    // Configure analysis
    config.analysisMetrics = {"spike_rate", "firing_patterns", "novelty", "curiosity", "dopamine"};
    config.analysisInterval = 0.5;
    config.analysisOutputFormat = "json";
    
    // Configure learning
    config.learningStrategy = "adaptive";
    config.learningRate = 0.02;
    config.explorationRate = 0.15;
    config.enableMetaLearning = true;
    config.enableSelfTuning = true;
    config.enableTransferLearning = false;
    
    // Configure memory
    config.enableMemoryProfiling = true;
    config.memorySampleSize = 2000;
    config.enableMemoryAnalysis = true;
    config.enableReplayAnalysis = true;
    
    // Configure neural analysis
    config.enableNeuralProfiling = true;
    config.enableSynapticAnalysis = true;
    config.enableCircuitAnalysis = true;
    
    std::cout << "--- Advanced Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual advanced systems
    std::cout << "--- Advanced Memory Integration Test ---" << std::endl;
    bool memoryIntegrated = experiment.testMemoryIntegration();
    std::cout << "Memory integration: " << (memoryIntegrated ? "PASSED" : "FAILED") << std::endl << std::endl;
    
    std::cout << "--- Advanced Neuromodulation Integration Test ---" << std::endl;
    bool neuromodulationIntegrated = experiment.testNeuromodulationIntegration();
    std::cout << "Neuromodulation integration: " << (neuromodulationIntegrated ? "PASSED" : "FAILED") << std::endl << std::endl;
    
    std::cout << "--- Advanced Checkpoint Test ---" << std::endl;
    bool checkpointingWorks = experiment.testCheckpointing();
    std::cout << "Checkpointing: " << (checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl << std::endl;
    
    std::cout << "--- Advanced Replay Test ---" << std::endl;
    bool replayWorks = experiment.testReplay();
    std::cout << "Replay: " << (replayWorks ? "WORKING" : "NOT WORKING") << std::endl << std::endl;
    
    // Run enhanced full experiment
    std::cout << "--- Enhanced Full Integration Experiment ---" << std::endl;
    auto result = experiment.runAdvanced(config);
    
    std::cout << std::endl;
    std::cout << "=== ADVANCED FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.baseResult.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.baseResult.avgFiringRate << std::endl;
    std::cout << "Avg synaptic weight: " << result.baseResult.avgSynapticWeight << std::endl;
    std::cout << "Memory episodes stored: " << result.baseResult.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.baseResult.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.baseResult.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.baseResult.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    // Enhanced analysis results
    std::cout << "=== ADVANCED ANALYSIS RESULTS ===" << std::endl;
    std::cout << "Learning progress score: " << result.learningProgressScore << std::endl;
    std::cout << "Behavioral adaptation score: " << result.behavioralAdaptationScore << std::endl;
    std::cout << "Neural efficiency score: " << result.neuralEfficiencyScore << std::endl;
    std::cout << "Memory retention score: " << result.memoryRetentionScore << std::endl;
    std::cout << "Prediction accuracy score: " << result.predictionAccuracyScore << std::endl;
    std::cout << "Survival fitness score: " << result.survivalFitnessScore << std::endl;
    std::cout << std::endl;
    
    std::cout << "Synaptic stability: " << result.synapticStability << std::endl;
    std::cout << "Memory consolidation quality: " << result.memoryConsolidationQuality << std::endl;
    std::cout << "Attention allocation: " << result.attentionAllocation << std::endl;
    std::cout << "Reward prediction accuracy: " << result.rewardPredictionAccuracy << std::endl;
    std::cout << "Exploration-exploitation balance: " << result.explorationExploitationBalance << std::endl;
    std::cout << "Developmental trajectory score: " << result.developmentalTrajectoryScore << std::endl;
    std::cout << std::endl;
    
    // Advanced integration status
    std::cout << "=== ADVANCED INTEGRATION STATUS ===" << std::endl;
    std::cout << "Meta-learning: " << (result.metaLearningEnabled ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Self-repair: " << (result.selfRepairEnabled ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Advanced neuromodulation: " << (result.advancedNeuromodulation ? "ACTIVE" : "INACTIVE") << std::endl;
    std::cout << "Complex memory architecture: " << (result.complexMemoryArchitecture ? "DEPLOYED" : "NOT PRESENT") << std::endl;
    std::cout << "Adaptive plasticity: " << (result.adaptivePlasticity ? "ACTIVE" : "NOT ACTIVE") << std::endl;
    std::cout << "Autonomous exploration: " << (result.autonomousExploration ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Continuous learning: " << (result.continuousLearning ? "ACTIVE" : "NOT ACTIVE") << std::endl;
    std::cout << "Life-long development: " << (result.lifeLongDevelopment ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << std::endl;
    
    // Performance metrics
    std::cout << "=== PERFORMANCE METRICS ===" << std::endl;
    std::cout << "Total execution time: " << result.performanceMetrics.totalExecutionTime << "s" << std::endl;
    std::cout << "CPU usage: " << result.performanceMetrics.cpuUsage << "%" << std::endl;
    std::cout << "Memory usage: " << result.performanceMetrics.memoryUsage << " MB" << std::endl;
    std::cout << "Spikes per second: " << result.performanceMetrics.spikesPerSecond << std::endl;
    std::cout << "Actions per second: " << result.performanceMetrics.actionsPerSecond << std::endl;
    std::cout << std::endl;
    
    // Quality scores
    std::cout << "=== OVERALL QUALITY ASSESSMENT ===" << std::endl;
    float totalScore = result.learningProgressScore + result.behavioralAdaptationScore +
                     result.neuralEfficiencyScore + result.memoryRetentionScore +
                     result.predictionAccuracyScore + result.survivalFitnessScore;
    float avgScore = totalScore / 6.0f;
    
    std::cout << "Overall integration score: " << avgScore << "/10" << std::endl;
    std::cout << "System stability: " << (result.performanceMetrics.simulationStability * 10.0f) << "/10" << std::endl;
    std::cout << "Learning efficiency: " << (result.performanceMetrics.learningEfficiency * 10.0f) << "/10" << std::endl;
    std::cout << std::endl;
    
    if (avgScore >= 8.0f) {
        std::cout << "🎉 SUCCESS: Advanced integration achieved high quality!" << std::endl;
    } else if (avgScore >= 6.0f) {
        std::cout << "✅ SUCCESS: Advanced integration achieved good quality." << std::endl;
    } else if (avgScore >= 4.0f) {
        std::cout << "⚠️  WARNING: Advanced integration needs improvement." << std::endl;
    } else {
        std::cout << "❌ FAILURE: Advanced integration failed to achieve basic quality." << std::endl;
        return 1;
    }
    
    // Export results if requested
    if (config.enableVisualizationExport) {
        std::cout << "--- Exporting Results ---" << std::endl;
        std::string timestamp = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        
        // Export analysis
        if (experiment.exportAnalysis("analysis_" + timestamp + ".json", result.detailedAnalysis)) {
            std::cout << "Analysis exported successfully!" << std::endl;
        }
        
        // Export results
        if (experiment.exportResults("results_" + timestamp + ".json", result)) {
            std::cout << "Results exported successfully!" << std::endl;
        }
        
        // Export performance metrics
        if (experiment.exportResults("performance_" + timestamp + ".json", 
                                    reinterpret_cast<AdvancedPhase6IntegrationResult&>(result.performanceMetrics))) {
            std::cout << "Performance metrics exported successfully!" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    // Show real-time monitoring data if enabled
    if (config.enableRealTimeMonitoring) {
        std::cout << "--- Real-time Monitoring Data ---" << std::endl;
        auto realTimeData = experiment.getRealTimeData();
        PerformanceMetrics currentPerf = experiment.getCurrentPerformanceMetrics();
        
        std::cout << "Latest analysis entries: " << realTimeData.size() << std::endl;
        std::cout << "Current performance metrics available." << std::endl;
        std::cout << "Real-time monitoring enabled: " << (config.enableRealTimeMonitoring ? "YES" : "NO") << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "=== ENHANCED PHASE 6 DEMO COMPLETED ===" << std::endl;
    
    return 0;
}