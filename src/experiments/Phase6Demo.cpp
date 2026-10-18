/**
 * Phase 6 Demo - Enhanced Integration Test
 * 
 * Enhanced version with advanced features:
 * - Performance monitoring and timing
 * - Detailed integration analysis
 * - Advanced checkpointing tests
 * - Replay verification with statistics
 * - System health monitoring
 * - Configuration validation
 * - Extensible experiment framework
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <map>

using namespace nlm;

// Enhanced configuration with advanced options
struct EnhancedPhase6Config {
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool enableDetailedLogging = true;
    bool measurePerformance = true;
    std::string checkpointDir = "checkpoints";
    size_t replayRatio = 3;  // Replay stored experiences 3x
    float curiosityThreshold = 0.5f;
};

// Performance measurement utilities
class PerformanceMonitor {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> stageStartTimes;
    std::map<std::string, double> stageDurations;
    
public:
    void startStage(const std::string& stageName) {
        stageStartTimes[stageName] = std::chrono::high_resolution_clock::now();
    }
    
    void endStage(const std::string& stageName) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - stageStartTimes[stageName]).count();
        stageDurations[stageName] = duration;
    }
    
    void startOverall() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double getOverallDuration() {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime).count();
    }
    
    void printReport() {
        std::cout << "\n=== PERFORMANCE REPORT ===\n";
        for (const auto& [stage, duration] : stageDurations) {
            std::cout << std::left << std::setw(25) << stage << ": " 
                      << std::fixed << std::setprecision(3) << duration << "s\n";
        }
    }
};

// Enhanced integration verification with detailed analysis
class EnhancedIntegrationVerifier {
private:
    Phase6IntegratedExperiment& experiment;
    EnhancedPhase6Config config;
    
public:
    EnhancedIntegrationVerifier(Phase6IntegratedExperiment& exp, const EnhancedPhase6Config& cfg) 
        : experiment(exp), config(cfg) {}
    
    struct SystemHealthReport {
        bool workingMemoryIntegrated = false;
        bool episodicMemoryIntegrated = false;
        bool neuromodulationIntegrated = false;
        bool predictionIntegrated = false;
        bool developmentIntegrated = false;
        bool planningIntegrated = false;
        bool curiosityIntegrated = false;
        
        float systemHealthScore = 0.0f;
        std::string overallStatus = "UNKNOWN";
    };
    
    SystemHealthReport verifyIntegration() {
        SystemHealthReport report;
        
        std::cout << "Enhanced Integration Verification:" << std::endl;
        std::cout << "================================" << std::endl;
        
        // Memory Systems
        std::cout << "\n1. MEMORY SYSTEMS:" << std::endl;
        experiment.testMemoryIntegration();
        report.workingMemoryIntegrated = experiment.getWorkingMemoryHealth();
        report.episodicMemoryIntegrated = experiment.getEpisodicMemoryHealth();
        report.memoryHealthScore = calculateMemoryHealth(report);
        
        // Neuromodulation
        std::cout << "\n2. NEUROMODULATION SYSTEMS:" << std::endl;
        experiment.testNeuromodulationIntegration();
        report.neuromodulationIntegrated = experiment.getNeuromodulationHealth();
        report.dopamineHealthScore = experiment.getDopamineLevel();
        
        // Prediction System
        std::cout << "\n3. PREDICTION SYSTEM:" << std::endl;
        experiment.testPredictionIntegration();
        report.predictionIntegrated = experiment.getPredictionHealth();
        
        // Development System
        std::cout << "\n4. DEVELOPMENT SYSTEM:" << std::endl;
        experiment.testDevelopmentIntegration();
        report.developmentIntegrated = experiment.getDevelopmentHealth();
        
        // Planning System
        std::cout << "\n5. PLANNING SYSTEM:" << std::endl;
        experiment.testPlanningIntegration();
        report.planningIntegrated = experiment.getPlanningHealth();
        
        // Curiosity System
        std::cout << "\n6. CURIOSITY SYSTEM:" << std::endl;
        report.curiosityIntegrated = experiment.getCuriosityHealth();
        
        // Calculate overall health score
        int integratedCount = countTrue(report.workingMemoryIntegrated, report.episodicMemoryIntegrated,
                                       report.neuromodulationIntegrated, report.predictionIntegrated,
                                       report.developmentIntegrated, report.planningIntegrated,
                                       report.curiosityIntegrated);
        
        report.systemHealthScore = (static_cast<float>(integratedCount) / 7.0f) * 100.0f;
        
        // Determine overall status
        if (report.systemHealthScore >= 85.0f) {
            report.overallStatus = "EXCELLENT";
        } else if (report.systemHealthScore >= 70.0f) {
            report.overallStatus = "GOOD";
        } else if (report.systemHealthScore >= 55.0f) {
            report.overallStatus = "ACCEPTABLE";
        } else {
            report.overallStatus = "REQUIRES ATTENTION";
        }
        
        printHealthReport(report);
        return report;
    }
    
private:
    float calculateMemoryHealth(const SystemHealthReport& report) {
        float score = 0.0f;
        if (report.workingMemoryIntegrated) score += 25.0f;
        if (report.episodicMemoryIntegrated) score += 25.0f;
        if (report.memoryHealthScore > 0) score += 50.0f; // Additional check
        return score;
    }
    
    int countTrue(bool a, bool b, bool c, bool d, bool e, bool f, bool g) {
        return (a + b + c + d + e + f + g);
    }
    
    void printHealthReport(const SystemHealthReport& report) {
        std::cout << "\n=== SYSTEM HEALTH REPORT ===" << std::endl;
        std::cout << std::left << std::setw(25) << "Working Memory:" 
                  << (report.workingMemoryIntegrated ? "✓" : "✗") << std::endl;
        std::cout << std::left << std::setw(25) << "Episodic Memory:" 
                  << (report.episodicMemoryIntegrated ? "✓" : "✗") << std::endl;
        std::cout << std::left << std::setw(25) << "Neuromodulation:" 
                  << (report.neuromodulationIntegrated ? "✓" : "✗") << std::endl;
        std::cout << std::left << std::setw(25) << "Prediction System:" 
                  << (report.predictionIntegrated ? "✓" : "✗") << std::endl;
        std::cout << std::left << std::setw(25) << "Development:" 
                  << (report.developmentIntegrated ? "✓" : "✗") << std::endl;
        std::cout << std::left << std::setw(25) << "Planning:" 
                  << (report.planningIntegrated ? "✓" : "✗") << std::endl;
        std::cout << std::left << std::setw(25) << "Curiosity:" 
                  << (report.curiosityIntegrated ? "✓" : "✗") << std::endl;
        
        std::cout << std::endl;
        std::cout << std::left << std::setw(25) << "Health Score:" 
                  << report.systemHealthScore << "%" << std::endl;
        std::cout << std::left << std::setw(25) << "Overall Status:" 
                  << report.overallStatus << std::endl;
    }
};

// Enhanced checkpoint testing
void advancedCheckpointTest(Phase6IntegratedExperiment& experiment, const EnhancedPhase6Config& config) {
    std::cout << "\n=== ADVANCED CHECKPOINT TESTING ===" << std::endl;
    
    // Test 1: Multiple checkpoints
    std::cout << "1. Testing multiple checkpoints...";
    experiment.testCheckpointing();
    std::cout << " Done." << std::endl;
    
    // Test 2: Checkpoint restoration
    std::cout << "2. Testing checkpoint restoration...";
    bool restoreSuccess = experiment.testCheckpointRestoration(config.checkpointDir);
    std::cout << " " << (restoreSuccess ? "✓ SUCCESS" : "✗ FAILED") << std::endl;
    
    // Test 3: Checkpoint performance
    std::cout << "3. Testing checkpoint performance...";
    double checkpointTime = experiment.testCheckpointPerformance(10);
    std::cout << " Average checkpoint time: " << std::fixed << std::setprecision(3) 
              << checkpointTime << "s" << std::endl;
    
    // Test 4: Checkpoint integrity
    std::cout << "4. Testing checkpoint integrity...";
    bool integrityCheck = experiment.testCheckpointIntegrity();
    std::cout << " " << (integrityCheck ? "✓ PASSED" : "✗ FAILED") << std::endl;
}

// Enhanced replay testing
void advancedReplayTest(Phase6IntegratedExperiment& experiment, const EnhancedPhase6Config& config) {
    std::cout << "\n=== ADVANCED REPLAY TESTING ===" << std::endl;
    
    // Test 1: Replay accuracy
    std::cout << "1. Testing replay accuracy...";
    bool accuracyTest = experiment.testReplayAccuracy(config.replayRatio);
    std::cout << " " << (accuracyTest ? "✓ PASSED" : "✗ FAILED") << std::endl;
    
    // Test 2: Replay with variations
    std::cout << "2. Testing replay with variations...";
    bool variationTest = experiment.testReplayVariations();
    std::cout << " " << (variationTest ? "✓ PASSED" : "✗ FAILED") << std::endl;
    
    // Test 3: Replay performance impact
    std::cout << "3. Testing performance impact of replay...";
    double performanceImpact = experiment.testReplayPerformanceImpact();
    std::cout << " Performance change: " << std::fixed << std::setprecision(2) 
              << (performanceImpact > 0 ? '+' : '-') << performanceImpact * 100 << "%" << std::endl;
}

// System monitoring and health checking
void systemMonitoring(Phase6IntegratedExperiment& experiment, const EnhancedPhase6Config& config) {
    std::cout << "\n=== SYSTEM MONITORING ===" << std::endl;
    
    PerformanceMonitor monitor;
    monitor.startOverall();
    
    // Monitor memory usage
    monitor.startStage("Memory Monitoring");
    size_t memoryUsage = experiment.getMemoryUsage();
    double memoryUsageMB = memoryUsage / (1024.0 * 1024.0);
    std::cout << "Current memory usage: " << std::fixed << std::setprecision(2) 
              << memoryUsageMB << " MB" << std::endl;
    monitor.endStage("Memory Monitoring");
    
    // Monitor spike rates
    monitor.startStage("Activity Monitoring");
    size_t spikeRate = experiment.getAverageSpikeRate();
    std::cout << "Average spike rate: " << spikeRate << " spikes/second" << std::endl;
    monitor.endStage("Activity Monitoring");
    
    // Monitor system resources
    monitor.startStage("Resource Monitoring");
    std::cout << "CPU Usage: N/A (platform dependent)" << std::endl;
    std::cout << "Memory Limit: Checking..." << std::endl;
    monitor.endStage("Resource Monitoring");
    
    // Print performance report
    monitor.printReport();
    std::cout << "Total experiment time: " << std::fixed << std::setprecision(3)
              << monitor.getOverallDuration() << "s" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== ENHANCED NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the advanced integrated artificial brain..." << std::endl;
    std::cout << std::endl;
    
    // Parse command line arguments for enhanced configuration
    EnhancedPhase6Config config;
    
    if (argc > 1) {
        // Parse enhanced arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--detailed") {
                config.enableDetailedLogging = true;
            } else if (arg == "--performance") {
                config.measurePerformance = true;
            } else if (arg == "--quick") {
                config.neuronCount = 200;
                config.maxSteps = 500;
            } else if (arg == "--comprehensive") {
                config.neuronCount = 1000;
                config.maxSteps = 5000;
            }
        }
    }
    
    // Initialize logging with enhanced level
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment and verifier
    Phase6IntegratedExperiment experiment;
    EnhancedIntegrationVerifier verifier(experiment, config);
    
    PerformanceMonitor overallMonitor;
    overallMonitor.startOverall();
    
    // Phase 1: Enhanced integration verification
    std::cout << "=== PHASE 1: ENHANCED INTEGRATION VERIFICATION ===" << std::endl;
    EnhancedIntegrationVerifier::SystemHealthReport healthReport = verifier.verifyIntegration();
    
    // Check if integration is acceptable
    if (healthReport.systemHealthScore < 70.0f) {
        std::cerr << "\nERROR: Integration health score is below threshold (" 
                  << healthReport.systemHealthScore << "% < 70%)." << std::endl;
        return 1;
    }
    
    // Phase 2: Enhanced checkpoint testing
    std::cout << "\n=== PHASE 2: ADVANCED CHECKPOINT TESTING ===" << std::endl;
    advancedCheckpointTest(experiment, config);
    
    // Phase 3: Enhanced replay testing
    std::cout << "\n=== PHASE 3: ADVANCED REPLAY TESTING ===" << std::endl;
    advancedReplayTest(experiment, config);
    
    // Phase 4: System monitoring
    if (config.measurePerformance) {
        std::cout << "\n=== PHASE 4: SYSTEM MONITORING ===" << std::endl;
        systemMonitoring(experiment, config);
    }
    
    // Phase 5: Full experiment
    std::cout << "\n=== PHASE 5: FULL INTEGRATION EXPERIMENT ===" << std::endl;
    overallMonitor.startStage("Full Experiment");
    
    Phase6Config phase6Config;
    phase6Config.neuronCount = config.neuronCount;
    phase6Config.maxSteps = config.maxSteps;
    phase6Config.enableCheckpointing = config.enableCheckpointing;
    phase6Config.enableReplay = config.enableReplay;
    phase6Config.enableDevelopment = config.enableDevelopment;
    
    auto result = experiment.run(phase6Config);
    overallMonitor.endStage("Full Experiment");
    
    // Print enhanced results
    std::cout << std::endl;
    std::cout << "=== ENHANCED FINAL RESULTS ===" << std::endl;
    std::cout << std::left << std::setw(30) << "Total Reward:" << result.totalReward << std::endl;
    std::cout << std::left << std::setw(30) << "Avg Firing Rate:" << result.avgFiringRate << " Hz" << std::endl;
    std::cout << std::left << std::setw(30) << "Episodes Stored:" << result.memoryEpisodesStored << std::endl;
    std::cout << std::left << std::setw(30) << "Dopamine Level:" << result.dopamineLevel << std::endl;
    std::cout << std::left << std::setw(30) << "Curiosity Level:" << result.curiosityLevel << std::endl;
    std::cout << std::left << std::setw(30) << "Prediction Error:" << result.predictionError << std::endl;
    std::cout << std::left << std::setw(30) << "Planning Confidence:" << result.planningConfidence << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== ENHANCED INTEGRATION STATUS ===" << std::endl;
    std::cout << std::left << std::setw(30) << "Working Memory:" 
              << (result.memoryWorkingMemoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << std::left << std::setw(30) << "Episodic Memory:" 
              << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << std::left << std::setw(30) << "Neuromodulation:" 
              << (result.neuromodulationIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << std::left << std::setw(30) << "Prediction:" 
              << (result.predictionIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << std::left << std::setw(30) << "Development:" 
              << (result.developmentIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << std::left << std::setw(30) << "Checkpointing:" 
              << (result.checkpointingWorks ? "WORKING ✓" : "NOT WORKING ✗") << std::endl;
    
    // Calculate enhanced integration metrics
    float integrationScore = calculateIntegrationScore(result);
    std::cout << std::left << std::setw(30) << "Integration Health:" 
              << integrationScore << "%" << std::endl;
    std::cout << std::left << std::setw(30) << "Experiment Status:" 
              << getStatusString(integrationScore) << std::endl;
    
    // Performance timing
    double totalTime = overallMonitor.getOverallDuration();
    std::cout << std::endl;
    std::cout << "Wall clock time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
    std::cout << "Simulation efficiency: " << std::fixed << std::setprecision(2) 
              << (result.maxSteps / totalTime) << " steps/s" << std::endl;
    
    // Final verdict
    std::cout << std::endl;
    std::cout << "=== FINAL VERDICT ===" << std::endl;
    if (integrationScore >= 85.0f) {
        std::cout << "✓ EXCELLENT: The integrated brain system is functioning optimally!" << std::endl;
        std::cout << "  All major subsystems are properly connected and working in harmony." << std::endl;
    } else if (integrationScore >= 70.0f) {
        std::cout << "✓ GOOD: The integrated brain system shows strong functionality." << std::endl;
        std::cout << "  Most subsystems are working well, with minor areas for improvement." << std::endl;
    } else if (integrationScore >= 55.0f) {
        std::cout << "⚠ ACCEPTABLE: The integrated brain system is functional." << std::endl;
        std::cout << "  Core systems work, but several areas need attention for optimal performance." << std::endl;
    } else {
        std::cout << "✗ NEEDS IMPROVEMENT: The integrated brain system has significant issues." << std::endl;
        std::cout << "  Multiple subsystems require attention and may not be fully functional." << std::endl;
        return 1;
    }
    
    return 0;
}

// Helper functions
float calculateIntegrationScore(const Phase6ExperimentResult& result) {
    float score = 0.0f;
    
    // Memory integration (30%)
    if (result.memoryWorkingMemoryIntegrated) score += 15.0f;
    if (result.memoryEpisodicMemoryIntegrated) score += 15.0f;
    
    // Neuromodulation (20%)
    if (result.neuromodulationIntegrated) score += 20.0f;
    
    // Prediction system (15%)
    if (result.predictionIntegrated) score += 15.0f;
    
    // Development system (15%)
    if (result.developmentIntegrated) score += 15.0f;
    
    // Checkpointing (10%)
    if (result.checkpointingWorks) score += 10.0f;
    
    // Curiosity level bonus (5%)
    if (result.curiosityLevel > 0.5f) score += 5.0f;
    
    // Planning confidence bonus (5%)
    if (result.planningConfidence > 0.3f) score += 5.0f;
    
    return score;
}

std::string getStatusString(float score) {
    if (score >= 90.0f) return "OPTIMAL";
    if (score >= 80.0f) return "EXCELLENT";
    if (score >= 70.0f) return "GOOD";
    if (score >= 60.0f) return "ACCEPTABLE";
    return "REQUIRES IMPROVEMENT";
}