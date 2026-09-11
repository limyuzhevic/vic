// Phase 6 Diagnostic Integration - Command Line Interface Commands
// Advanced diagnostic and analysis system for NLM Phase 6 integration

#include "Phase6Diagnostic.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

// Global diagnostic instance
Phase6Diagnostic gDiagnostic;

// Diagnostic command implementations
void runDiagnosticCommand(const std::vector<std::string>& args) {
    std::cout << "=== Phase 6 Diagnostic Analysis ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    // Create config from args if provided
    if (args.size() > 0) {
        try {
            config.neuronCount = std::stoull(args[0]);
        } catch (...) {
            config.neuronCount = 1000;
        }
    }
    
    if (args.size() > 1) {
        try {
            config.maxSteps = std::stoull(args[1]);
        } catch (...) {
            config.maxSteps = 10000;
        }
    }
    
    // Run diagnostic
    Phase6DiagnosticResult result = gDiagnostic.analyze(experiment, config);
    
    // Generate and output report
    std::string report = gDiagnostic.generateReport(result);
    std::cout << report;
    
    std::cout << std::endl << "Diagnostic analysis completed successfully!" << std::endl;
}

void validateIntegrationCommand(const std::vector<std::string>& args) {
    std::cout << "=== Phase 6 Integration Validation ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    // Validate integration
    std::string validationMessage;
    bool isValid = gDiagnostic.validateIntegration(experiment, config, validationMessage);
    
    std::cout << validationMessage << std::endl;
    std::cout << (isValid ? "✓ VALIDATION PASSED" : "✗ VALIDATION FAILED") << std::endl;
}

void exportDiagnosticCommand(const std::vector<std::string>& args) {
    std::cout << "=== Diagnostic Data Export ===" << std::endl;
    
    if (args.size() < 1) {
        std::cout << "Usage: diagnostic-export <format> [filepath]" << std::endl;
        std::cout << "Formats: json, csv, text" << std::endl;
        return;
    }
    
    Phase6Diagnostic::ExportFormat format;
    if (args[0] == "json") format = Phase6Diagnostic::ExportFormat::JSON;
    else if (args[0] == "csv") format = Phase6Diagnostic::ExportFormat::CSV;
    else format = Phase6Diagnostic::ExportFormat::TEXT;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    // Get diagnostic result
    Phase6DiagnosticResult result = gDiagnostic.analyze(experiment, config);
    
    // Set filepath
    std::string filepath = args.size() > 1 ? args[1] : "diagnostic_export.json";
    
    // Export data
    bool success = gDiagnostic.exportDiagnosticData(result, filepath, format);
    
    if (success) {
        std::cout << "Diagnostic data exported to: " << filepath << std::endl;
    } else {
        std::cout << "Failed to export diagnostic data!" << std::endl;
    }
}

void logDiagnosticCommand(const std::vector<std::string>& args) {
    std::cout << "=== Log Diagnostic Results ===" << std::endl;
    
    if (args.size() < 1) {
        std::cout << "Usage: diagnostic-log <filepath>" << std::endl;
        return;
    }
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    // Get diagnostic result and report
    Phase6DiagnosticResult result = gDiagnostic.analyze(experiment, config);
    std::string report = gDiagnostic.generateReport(result);
    
    // Log to file
    gDiagnostic.logDiagnosticResults(report, args[0]);
    
    std::cout << "Diagnostic results logged to: " << args[0] << std::endl;
}

void analyzeMemoryRetentionCommand(const std::vector<std::string>& args) {
    std::cout << "=== Memory Retention Analysis ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    Phase6DiagnosticResult result = gDiagnostic.analyze(experiment, config);
    
    // Analyze memory retention
    std::string analysis;
    gDiagnostic.analyzeMemoryRetention(result, analysis);
    std::cout << analysis;
}

void analyzePredictionAccuracyCommand(const std::vector<std::string>& args) {
    std::cout << "=== Prediction Accuracy Analysis ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    Phase6DiagnosticResult result = gDiagnostic.analyze(experiment, config);
    
    // Analyze prediction accuracy
    std::string analysis;
    gDiagnostic.analyzePredictionAccuracy(result, analysis);
    std::cout << analysis;
}

void analyzeContinualLearningCommand(const std::vector<std::string>& args) {
    std::cout << "=== Continual Learning Analysis ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    Phase6DiagnosticResult result = gDiagnostic.analyze(experiment, config);
    
    // Analyze continual learning
    std::string analysis;
    gDiagnostic.analyzeContinualLearning(result, analysis);
    std::cout << analysis;
}

void analyzeMultiSystemInteractionCommand(const std::vector<std::string>& args) {
    std::cout << "=== Multi-System Interaction Analysis ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    Phase6DiagnosticResult result = gDiagnostic.analyze(experiment, config);
    
    // Analyze multi-system interaction
    std::string analysis;
    gDiagnostic.analyzeMultiSystemInteraction(result, analysis);
    std::cout << analysis;
}

void analyzeDevelopmentalProgressionCommand(const std::vector<std::string>& args) {
    std::cout << "=== Developmental Progression Analysis ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    Phase6DiagnosticResult result = gDiagnostic.analyze(experiment, config);
    
    // Analyze developmental progression
    std::string analysis;
    gDiagnostic.analyzeDevelopmentalProgression(result, analysis);
    std::cout << analysis;
}

void advancedDebugCommand(const std::vector<std::string>& args) {
    std::cout << "=== Advanced Debugging ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    Phase6DiagnosticResult result = gDiagnostic.analyze(experiment, config);
    
    // Create a temporary brain for debugging
    auto cfg = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Analyze advanced debugging
    std::string analysis;
    gDiagnostic.analyzeAdvancedDebugging(brain, result, analysis);
    std::cout << analysis;
}

void generatePerformanceReportCommand(const std::vector<std::string>& args) {
    std::cout << "=== Performance Report Generation ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    Phase6DiagnosticResult result = gDiagnostic.analyze(experiment, config);
    
    // Generate performance report
    std::string report;
    gDiagnostic.generatePerformanceReport(result, report);
    std::cout << report;
}

void timeSeriesAnalysisCommand(const std::vector<std::string>& args) {
    std::cout << "=== Time Series Analysis ===" << std::endl;
    
    if (args.size() < 1) {
        std::cout << "Usage: time-series-analysis <duration_steps> [sample_interval]" << std::endl;
        std::cout << "Example: time-series-analysis 1000 100" << std::endl;
        return;
    }
    
    size_t durationSteps;
    try {
        durationSteps = std::stoull(args[0]);
    } catch (...) {
        std::cout << "Error: Invalid duration steps" << std::endl;
        return;
    }
    
    size_t sampleInterval = args.size() > 1 ? std::stoull(args[1]) : 100;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    // Collect time series data
    std::vector<Phase6DiagnosticResult> timeSeries = 
        gDiagnostic.collectTimeSeriesData(experiment, config, durationSteps, sampleInterval);
    
    std::cout << "Collected " << timeSeries.size() << " data points over " << durationSteps 
              << " steps with sample interval " << sampleInterval << std::endl;
    
    // Analyze time series
    if (!timeSeries.empty()) {
        Phase6DiagnosticResult aggregated = gDiagnostic.analyzeTimeSeries(timeSeries);
        Phase6DiagnosticResult trend = gDiagnostic.calculateTrend(timeSeries);
        
        std::cout << "\n=== Time Series Analysis Results ===" << std::endl;
        std::cout << "Average Reward: " << aggregated.totalReward << std::endl;
        std::cout << "Average Firing Rate: " << aggregated.averageFiringRate << " Hz" << std::endl;
        std::cout << "Reward Trend: " << trend.totalReward << " per step" << std::endl;
        std::cout << "Firing Rate Trend: " << trend.averageFiringRate << " per step" << std::endl;
        
        std::cout << "\nIntegration Status Over Time:" << std::endl;
        std::cout << "  Working Memory: " << (aggregated.memoryWorkingMemoryIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        std::cout << "  Episodic Memory: " << (aggregated.memoryEpisodicMemoryIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        std::cout << "  Neuromodulation: " << (aggregated.neuromodulationIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        std::cout << "  Prediction System: " << (aggregated.predictionIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        std::cout << "  Cognition Systems: " << (aggregated.cognitionIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        std::cout << "  Development System: " << (aggregated.developmentIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
    }
}

void helpCommand(const std::vector<std::string>& args) {
    std::cout << "=== Phase 6 Diagnostic Command Line Interface ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Commands:" << std::endl;
    std::cout << "  diagnostic [neuron_count] [max_steps] - Run Phase 6 diagnostic analysis" << std::endl;
    std::cout << "  diagnostic-validate - Validate Phase 6 integration" << std::endl;
    std::cout << "  diagnostic-export <format> [filepath] - Export diagnostic results (json, csv, text)" << std::endl;
    std::cout << "  diagnostic-log <filepath> - Log diagnostic results to file" << std::endl;
    std::cout << "  memory-retention-analysis - Analyze memory retention capabilities" << std::endl;
    std::cout << "  prediction-accuracy-analysis - Analyze prediction accuracy" << std::endl;
    std::cout << "  continual-learning-analysis - Analyze continual learning capabilities" << std::endl;
    std::cout << "  multi-system-interaction-analysis - Analyze multi-system integration" << std::endl;
    std::cout << "  developmental-progression-analysis - Analyze developmental progression" << std::endl;
    std::cout << "  advanced-debug - Perform advanced debugging analysis" << std::endl;
    std::cout << "  performance-report - Generate performance report" << std::endl;
    std::cout << "  time-series-analysis <duration> [interval] - Analyze time series data" << std::endl;
    std::cout << "  help - Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  diagnostic 500 2000" << std::endl;
    std::cout << "  diagnostic-export json diagnostics.json" << std::endl;
    std::cout << "  time-series-analysis 1000 100" << std::endl;
}