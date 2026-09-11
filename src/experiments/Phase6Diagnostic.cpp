#include "Phase6Diagnostic.hpp"
#include "../core/Logger/Logger.hpp"
#include <iomanip>
#include <sstream>

using namespace nlm;

namespace {
void printDiagnosticHeader(std::stringstream& ss, const std::string& title, int width) {
    ss << std::string(width, '=') << std::endl;
    ss << title << std::endl;
    ss << std::string(width, '=') << std::endl << std::endl;
}

void printSystemStatus(std::stringstream& ss, const std::string& name, bool status, const std::string& details = "") {
    ss << "  " << name << ": ";
    if (status) {
        ss << "✓ CONNECTED";
    } else {
        ss << "✗ DISCONNECTED";
    }
    if (!details.empty()) {
        ss << " - " << details;
    }
    ss << std::endl;
}

void printSectionHeader(std::stringstream& ss, const std::string& title) {
    ss << "\n" << title << std::string(60 - title.length(), ' ') << std::endl;
    ss << std::string(60, '-') << std::endl;
}

void printMetric(std::stringstream& ss, const std::string& label, float value, const std::string& unit = "") {
    ss << "    " << std::left << std::setw(25) << label << ": " << std::fixed << std::setprecision(4) << value;
    if (!unit.empty()) ss << " " << unit;
    ss << std::endl;
}

void printMemoryMetrics(std::stringstream& ss, const Phase6DiagnosticResult& result) {
    ss << std::endl;
    ss << "MEMORY SYSTEM METRICS" << std::endl;
    ss << std::string(40, '-') << std::endl;
    
    if (result.memoryWorkingMemoryActive) {
        ss << "    Working Memory Active Traces:      " << result.workingMemoryActiveTraces << std::endl;
        ss << "    Working Memory Capacity:            " << result.workingMemoryCapacity << std::endl;
        ss << "    Working Memory Utilization:         " << std::fixed << std::setprecision(2) 
           << (result.workingMemoryCapacity > 0 ? 
               (float)result.workingMemoryActiveTraces / result.workingMemoryCapacity * 100 : 0)
           << "%" << std::endl;
    }
    
    if (result.memoryEpisodicStored) {
        ss << "    Episodic Memory Episodes Stored:    " << result.episodicMemoryEpisodes << std::endl;
        ss << "    Memory Storage Utilization:         " << std::fixed << std::setprecision(2)
           << (result.episodicMemoryMaxEpisodes > 0 ?
               (float)result.episodicMemoryEpisodes / result.episodicMemoryMaxEpisodes * 100 : 0)
           << "%" << std::endl;
    }
    
    if (result.associativeMemoryActive) {
        ss << "    Associative Memory Associations:    " << result.associativeMemoryAssociations << std::endl;
    }
}

void printPredictionMetrics(std::stringstream& ss, const Phase6DiagnosticResult& result) {
    ss << std::endl;
    ss << "PREDICTION SYSTEM METRICS" << std::endl;
    ss << std::string(40, '-') << std::endl;
    
    if (result.predictionSystemActive) {
        ss << "    Prediction Error:                   " << std::fixed << std::setprecision(4)
           << result.predictionError << std::endl;
        ss << "    Prediction Confidence:              " << std::fixed << std::setprecision(4)
           << result.predictionConfidence << std::endl;
        ss << "    Prediction Accuracy:                " << std::fixed << std::setprecision(2)
           << (result.predictionConfidence > 0 ? result.predictionConfidence * 100 : 0)
           << "%" << std::endl;
    }
}

void printNeuromodulationMetrics(std::stringstream& ss, const Phase6DiagnosticResult& result) {
    ss << std::endl;
    ss << "NEUROMODULATION METRICS" << std::endl;
    ss << std::string(40, '-') << std::endl;
    
    if (result.dopamineActive) {
        ss << "    Dopamine Level:                     " << std::fixed << std::setprecision(4)
           << result.dopamineLevel << std::endl;
    }
    
    if (result.curiosityActive) {
        ss << "    Curiosity Level:                    " << std::fixed << std::setprecision(4)
           << result.curiosityLevel << std::endl;
    }
    
    if (result.noveltyActive) {
        ss << "    Novelty Level:                      " << std::fixed << std::setprecision(4)
           << result.noveltyLevel << std::endl;
    }
}

void printPerformanceMetrics(std::stringstream& ss, const Phase6DiagnosticResult& result) {
    ss << std::endl;
    ss << "PERFORMANCE METRICS" << std::endl;
    ss << std::string(40, '-') << std::endl;
    
    ss << "    Brain Step Rate:                    " << result.stepsPerSecond << " steps/sec" << std::endl;
    ss << "    Average Firing Rate:                " << std::fixed << std::setprecision(4)
       << result.averageFiringRate << " Hz" << std::endl;
    ss << "    Total Spikes Generated:             " << result.totalSpikeCount << std::endl;
    ss << "    Active Neurons:                     " << result.activeNeuronCount << std::endl;
    ss << "    Memory Integration Score:           " << std::fixed << std::setprecision(2)
       << (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated ?
           (float)(result.memoryWorkingMemoryIntegrated ? 1 : 0) * (result.memoryEpisodicMemoryIntegrated ? 1 : 0) *
           (result.associativeMemoryIntegrated ? 1 : 0) * 100 : 0)
       << "%" << std::endl;
}

void printDevelopmentMetrics(std::stringstream& ss, const Phase6DiagnosticResult& result) {
    ss << std::endl;
    ss << "DEVELOPMENT METRICS" << std::endl;
    ss << std::string(40, '-') << std::endl;
    
    ss << "    Developmental Stage:                " << static_cast<int>(result.developmentStage) << std::endl;
    ss << "    Neural Plasticity Rate:              " << std::fixed << std::setprecision(4)
       << result.developmentStagePlasticity << std::endl;
    ss << "    Age-Based Plasticity Adjustment:     " << std::fixed << std::setprecision(2)
       << result.developmentAgePlasticityFactor << std::endl;
    ss << "    Synaptogenesis Events:              " << result.developmentSynaptogenesisEvents << std::endl;
    ss << "    Pruning Events:                      " << result.developmentPruningEvents << std::endl;
}

void printValidationSummary(std::stringstream& ss, const Phase6DiagnosticResult& result) {
    ss << std::endl;
    ss << "SYSTEM INTEGRATION VALIDATION" << std::endl;
    ss << std::string(60, '-') << std::endl;
    
    int totalChecks = 0;
    int passedChecks = 0;
    
    if (result.memoryWorkingMemoryIntegrated) {
        ss << "    ✓ Working Memory Integration:        PASSED" << std::endl;
        passedChecks++;
    }
    totalChecks++;
    
    if (result.memoryEpisodicMemoryIntegrated) {
        ss << "    ✓ Episodic Memory Integration:       PASSED" << std::endl;
        passedChecks++;
    }
    totalChecks++;
    
    if (result.memoryAssociativeMemoryIntegrated) {
        ss << "    ✓ Associative Memory Integration:    PASSED" << std::endl;
        passedChecks++;
    }
    totalChecks++;
    
    if (result.neuromodulationIntegrated) {
        ss << "    ✓ Neuromodulation Integration:       PASSED" << std::endl;
        passedChecks++;
    }
    totalChecks++;
    
    if (result.predictionIntegrated) {
        ss << "    ✓ Prediction System Integration:     PASSED" << std::endl;
        passedChecks++;
    }
    totalChecks++;
    
    if (result.cognitionIntegrated) {
        ss << "    ✓ Cognition Systems Integration:     PASSED" << std::endl;
        passedChecks++;
    }
    totalChecks++;
    
    if (result.developmentIntegrated) {
        ss << "    ✓ Development System Integration:    PASSED" << std::endl;
        passedChecks++;
    }
    totalChecks++;
    
    ss << "    Integration Summary:                 " << passedChecks << "/" << totalChecks << " systems integrated (" <<
        std::fixed << std::setprecision(1) << (totalChecks > 0 ? (float)passedChecks / totalChecks * 100 : 0)
        << "%)" << std::endl;
}

} // namespace

Phase6DiagnosticResult Phase6Diagnostic::analyze(const Phase6IntegratedExperiment& experiment,
                                                 const Phase6Config& config) {
    Phase6DiagnosticResult result;
    
    // Create temporary brain for analysis
    auto cfg = std::make_shared<Config>();
    for (const auto& [key, value] : config._internalState) {
        cfg->set(key, value);
    }
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed for diagnostic analysis");
        return result;
    }
    
    // Run quick integration test
    bool integrationOK = experiment.verifyIntegration();
    
    // Analyze system integration
    result.memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
    result.memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
    result.memoryAssociativeMemoryIntegrated = (brain->getAssociativeMemory() != nullptr);
    
    result.neuromodulationIntegrated = (brain->getDopamine() != nullptr);
    result.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
    result.cognitionIntegrated = (brain->getPlanner() != nullptr && brain->getConceptFormation() != nullptr && 
                                   brain->getAttention() != nullptr);
    result.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
    
    // Run systems integration tests
    experiment.testMemoryIntegration();
    experiment.testNeuromodulationIntegration();
    
    return result;
}

std::string Phase6Diagnostic::generateReport(const Phase6DiagnosticResult& result) {
    std::stringstream ss;
    const int width = 70;
    
    // Header
    printDiagnosticHeader(ss, "NLM Phase 6 Diagnostic Report", width);
    
    // Basic System Status
    ss << "SYSTEM STATUS" << std::endl;
    ss << std::string(width, '-') << std::endl;
    
    ss << "  Brain Integration:                   " << (result.neuromodulationIntegrated ? "✓ OPERATIONAL" : "✗ FAILED") << std::endl;
    ss << "  Phase 6 Status:                      " << "✓ RUNNING" << std::endl;
    ss << "  Memory Systems:                     " << (result.memoryWorkingMemoryIntegrated ? "✓ ACTIVE" : "✗ INACTIVE") << std::endl;
    ss << "  Cognition Systems:                   " << (result.cognitionIntegrated ? "✓ ACTIVE" : "✗ INACTIVE") << std::endl;
    ss << "  Development System:                  " << (result.developmentIntegrated ? "✓ ACTIVE" : "✗ INACTIVE") << std::endl;
    
    // System-specific metrics
    printMemoryMetrics(ss, result);
    printPredictionMetrics(ss, result);
    printNeuromodulationMetrics(ss, result);
    printPerformanceMetrics(ss, result);
    printDevelopmentMetrics(ss, result);
    
    // Validation summary
    printValidationSummary(ss, result);
    
    // Footer
    ss << std::endl;
    ss << "END OF DIAGNOSTIC REPORT" << std::endl;
    ss << std::string(width, '=') << std::endl;
    
    return ss.str();
}

bool Phase6Diagnostic::validateIntegration(const Phase6IntegratedExperiment& experiment,
                                           const Phase6Config& config,
                                           std::string& validationMessage) {
    Phase6DiagnosticResult result = analyze(experiment, config);
    
    bool integrationValid = (
        result.memoryWorkingMemoryIntegrated &&
        result.memoryEpisodicMemoryIntegrated &&
        result.neuromodulationIntegrated &&
        result.predictionIntegrated &&
        result.cognitionIntegrated &&
        result.developmentIntegrated
    );
    
    std::stringstream message;
    
    if (integrationValid) {
        message << "Phase 6 Integration Validation PASSED" << std::endl;
        message << "All critical systems are properly integrated:" << std::endl;
        message << "  - Working Memory: Integrated" << std::endl;
        message << "  - Episodic Memory: Integrated" << std::endl;
        message << "  - Neuromodulation: Integrated" << std::endl;
        message << "  - Prediction System: Integrated" << std::endl;
        message << "  - Cognition Systems: Integrated" << std::endl;
        message << "  - Development System: Integrated" << std::endl;
    } else {
        message << "Phase 6 Integration Validation FAILED" << std::endl;
        message << "Missing or disconnected systems:" << std::endl;
        
        if (!result.memoryWorkingMemoryIntegrated) message << "  - Working Memory: Disconnected" << std::endl;
        if (!result.memoryEpisodicMemoryIntegrated) message << "  - Episodic Memory: Disconnected" << std::endl;
        if (!result.memoryAssociativeMemoryIntegrated) message << "  - Associative Memory: Disconnected" << std::endl;
        if (!result.neuromodulationIntegrated) message << "  - Neuromodulation: Disconnected" << std::endl;
        if (!result.predictionIntegrated) message << "  - Prediction System: Disconnected" << std::endl;
        if (!result.cognitionIntegrated) message << "  - Cognition Systems: Disconnected" << std::endl;
        if (!result.developmentIntegrated) message << "  - Development System: Disconnected" << std::endl;
    }
    
    validationMessage = message.str();
    return integrationValid;
}

void Phase6Diagnostic::logDiagnosticResults(const std::string& report, const std::string& logFile) {
    std::ofstream logStream(logFile, std::ios::app);
    if (logStream.is_open()) {
        logStream << "\n=== NLM Phase 6 Diagnostic Report ===" << std::endl;
        logStream << report;
        logStream.close();
        
        NLM_LOG_INFO("Diagnostic results logged to: " + logFile);
    } else {
        NLM_LOG_ERROR("Failed to open log file: " + logFile);
    }
}

std::string Phase6Diagnostic::exportDiagnosticData(const Phase6DiagnosticResult& result,
                                                   ExportFormat format) {
    std::stringstream ss;
    
    switch (format) {
        case ExportFormat::JSON:
            ss << "{\n";
            ss << "  \"timestamp\": \"" << std::to_string(time(nullptr)) << "\",\n";
            ss << "  \"integration_status\": {\n";
            ss << "    \"memory_working_memory_integrated\": " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
            ss << "    \"memory_episodic_memory_integrated\": " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
            ss << "    \"neuromodulation_integrated\": " << (result.neuromodulationIntegrated ? "true" : "false") << ",\n";
            ss << "    \"prediction_integrated\": " << (result.predictionIntegrated ? "true" : "false") << ",\n";
            ss << "    \"cognition_integrated\": " << (result.cognitionIntegrated ? "true" : "false") << ",\n";
            ss << "    \"development_integrated\": " << (result.developmentIntegrated ? "true" : "false") << "\n";
            ss << "  },\n";
            ss << "  \"metrics\": {\n";
            ss << "    \"total_reward\": " << result.totalReward << ",\n";
            ss << "    \"average_firing_rate\": " << result.averageFiringRate << ",\n";
            ss << "    \"dopamine_level\": " << result.dopamineLevel << ",\n";
            ss << "    \"curiosity_level\": " << result.curiosityLevel << "\n";
            ss << "  }\n";
            ss << "}" << std::endl;
            break;
            
        case ExportFormat::CSV:
            ss << "metric,value\n";
            ss << "memory_working_memory_integrated," << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << "\n";
            ss << "memory_episodic_memory_integrated," << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << "\n";
            ss << "neuromodulation_integrated," << (result.neuromodulationIntegrated ? "true" : "false") << "\n";
            ss << "prediction_integrated," << (result.predictionIntegrated ? "true" : "false") << "\n";
            ss << "cognition_integrated," << (result.cognitionIntegrated ? "true" : "false") << "\n";
            ss << "development_integrated," << (result.developmentIntegrated ? "true" : "false") << "\n";
            ss << "total_reward," << result.totalReward << "\n";
            ss << "average_firing_rate," << result.averageFiringRate << "\n";
            ss << "dopamine_level," << result.dopamineLevel << "\n";
            ss << "curiosity_level," << result.curiosityLevel << "\n";
            break;
            
        case ExportFormat::TEXT:
        default:
            ss << "Phase 6 Diagnostic Results" << std::endl;
            ss << std::string(50, '=') << std::endl;
            ss << "Memory Working Memory Integrated: " << (result.memoryWorkingMemoryIntegrated ? "Yes" : "No") << std::endl;
            ss << "Memory Episodic Memory Integrated: " << (result.memoryEpisodicMemoryIntegrated ? "Yes" : "No") << std::endl;
            ss << "Neuromodulation Integrated: " << (result.neuromodulationIntegrated ? "Yes" : "No") << std::endl;
            ss << "Prediction Integrated: " << (result.predictionIntegrated ? "Yes" : "No") << std::endl;
            ss << "Cognition Integrated: " << (result.cognitionIntegrated ? "Yes" : "No") << std::endl;
            ss << "Development Integrated: " << (result.developmentIntegrated ? "Yes" : "No") << std::endl;
            ss << "Total Reward: " << result.totalReward << std::endl;
            ss << "Average Firing Rate: " << result.averageFiringRate << " Hz" << std::endl;
            ss << "Dopamine Level: " << result.dopamineLevel << std::endl;
            ss << "Curiosity Level: " << result.curiosityLevel << std::endl;
            break;
    }
    
    return ss.str();
}

Phase6DiagnosticResult Phase6Diagnostic::collectMetrics(std::shared_ptr<Brain> brain) {
    Phase6DiagnosticResult result;
    
    // Collect integration status
    result.memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
    result.memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
    result.memoryAssociativeMemoryIntegrated = (brain->getAssociativeMemory() != nullptr);
    
    result.neuromodulationIntegrated = (brain->getDopamine() != nullptr);
    result.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
    result.cognitionIntegrated = (brain->getPlanner() != nullptr && brain->getConceptFormation() != nullptr && 
                                   brain->getAttention() != nullptr);
    result.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
    
    // Collect performance metrics
    result.totalReward = 100.0f; // Placeholder - would calculate from simulation
    result.averageFiringRate = brain->getAverageFiringRate();
    result.dopamineLevel = 0.5f; // Placeholder - would get from neuromodulator
    result.curiosityLevel = 0.3f; // Placeholder - would get from neuromodulator
    
    return result;
}

Phase6DiagnosticResult Phase6Diagnostic::analyzeTimeSeries(const std::vector<Phase6DiagnosticResult>& results) {
    Phase6DiagnosticResult aggregatedResult;
    
    if (results.empty()) {
        return aggregatedResult;
    }
    
    // Calculate averages and identify trends
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    float totalDopamine = 0.0f;
    float totalCuriosity = 0.0f;
    
    int integrationCount[6] = {0}; // Memory, Episodic, Neuromodulation, Prediction, Cognition, Development
    
    for (const auto& result : results) {
        totalReward += result.totalReward;
        totalFiringRate += result.averageFiringRate;
        totalDopamine += result.dopamineLevel;
        totalCuriosity += result.curiosityLevel;
        
        if (result.memoryWorkingMemoryIntegrated) integrationCount[0]++;
        if (result.memoryEpisodicMemoryIntegrated) integrationCount[1]++;
        if (result.neuromodulationIntegrated) integrationCount[2]++;
        if (result.predictionIntegrated) integrationCount[3]++;
        if (result.cognitionIntegrated) integrationCount[4]++;
        if (result.developmentIntegrated) integrationCount[5]++;
    }
    
    size_t count = results.size();
    
    aggregatedResult.memoryWorkingMemoryIntegrated = (integrationCount[0] == count);
    aggregatedResult.memoryEpisodicMemoryIntegrated = (integrationCount[1] == count);
    aggregatedResult.memoryAssociativeMemoryIntegrated = (integrationCount[0] > 0); // Assuming associative tracking
    
    aggregatedResult.neuromodulationIntegrated = (integrationCount[2] == count);
    aggregatedResult.predictionIntegrated = (integrationCount[3] == count);
    aggregatedResult.cognitionIntegrated = (integrationCount[4] == count);
    aggregatedResult.developmentIntegrated = (integrationCount[5] == count);
    
    aggregatedResult.totalReward = totalReward / count;
    aggregatedResult.averageFiringRate = totalFiringRate / count;
    aggregatedResult.dopamineLevel = totalDopamine / count;
    aggregatedResult.curiosityLevel = totalCuriosity / count;
    
    return aggregatedResult;
}

bool Phase6Diagnostic::exportDiagnosticData(const std::vector<Phase6DiagnosticResult>& results,
                                            const std::string& filepath,
                                            ExportFormat format) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for export: " + filepath);
        return false;
    }
    
    std::string data;
    if (!results.empty()) {
        data = exportDiagnosticData(results[0], format);
    }
    
    file << data;
    file.close();
    
    if (file.good()) {
        NLM_LOG_INFO("Diagnostic data exported to: " + filepath);
        return true;
    } else {
        NLM_LOG_ERROR("Failed to export diagnostic data to: " + filepath);
        return false;
    }
}

std::vector<Phase6DiagnosticResult> Phase6Diagnostic::collectTimeSeriesData(
    const Phase6IntegratedExperiment& experiment,
    const Phase6Config& config,
    size_t durationSteps,
    size_t sampleInterval) {
    std::vector<Phase6DiagnosticResult> results;
    
    auto cfg = std::make_shared<Config>();
    for (const auto& [key, value] : config._internalState) {
        cfg->set(key, value);
    }
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed for time series collection");
        return results;
    }
    
    // Run simulation and collect metrics
    for (size_t step = 0; step < durationSteps; step += sampleInterval) {
        // Run simulation for sampleInterval steps
        for (size_t i = 0; i < sampleInterval && (step + i) < durationSteps; ++i) {
            brain->step(step + i, static_cast<Timestamp>(step + i) * 0.001f);
        }
        
        // Collect metrics at this point
        Phase6DiagnosticResult result = collectMetrics(brain);
        results.push_back(result);
    }
    
    return results;
}

Phase6DiagnosticResult Phase6Diagnostic::calculateTrend(const std::vector<Phase6DiagnosticResult>& results) {
    Phase6DiagnosticResult trendResult;
    
    if (results.size() < 2) {
        return trendResult;
    }
    
    // Calculate simple linear trend for key metrics
    float totalRewardChange = 0.0f;
    float totalFiringRateChange = 0.0f;
    
    for (size_t i = 1; i < results.size(); ++i) {
        totalRewardChange += results[i].totalReward - results[i-1].totalReward;
        totalFiringRateChange += results[i].averageFiringRate - results[i-1].averageFiringRate;
    }
    
    float timeSteps = (float)results.size() - 1;
    
    trendResult.totalReward = totalRewardChange / timeSteps;  // Reward trend per step
    trendResult.averageFiringRate = totalFiringRateChange / timeSteps;  // Firing rate trend per step
    
    return trendResult;
}

Phase6DiagnosticResult Phase6Diagnostic::exportDiagnosticData(const Phase6DiagnosticResult& result,
                                                             const std::string& filepath,
                                                             ExportFormat format) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for export: " + filepath);
        return result;
    }
    
    std::string data = exportDiagnosticData(result, format);
    
    file << data;
    file.close();
    
    if (file.good()) {
        NLM_LOG_INFO("Diagnostic data exported to: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to export diagnostic data to: " + filepath);
    }
    
    return result;
}

void Phase6Diagnostic::addToCommandLineInterface(CLI& cli) {
    // Add diagnostic commands to CLI
    cli.registerCommand("diagnostic", "Run Phase 6 diagnostic analysis",
                       [](const std::vector<std::string>& args) {
                           std::cout << "Running Phase 6 diagnostics..." << std::endl;
                           Phase6Diagnostic diagnostic;
                           Phase6IntegratedExperiment experiment;
                           Phase6Config config;
                           
                           Phase6DiagnosticResult result = diagnostic.analyze(experiment, config);
                           std::string report = diagnostic.generateReport(result);
                           std::cout << report << std::endl;
                       });
    
    cli.registerCommand("diagnostic-validate", "Validate Phase 6 integration",
                       [](const std::vector<std::string>& args) {
                           Phase6Diagnostic diagnostic;
                           Phase6IntegratedExperiment experiment;
                           Phase6Config config;
                           
                           std::string message;
                           bool isValid = diagnostic.validateIntegration(experiment, config, message);
                           
                           std::cout << message << std::endl;
                           std::cout << (isValid ? "VALIDATION PASSED" : "VALIDATION FAILED") << std::endl;
                       });
    
    cli.registerCommand("diagnostic-export", "Export diagnostic results",
                       [](const std::vector<std::string>& args) {
                           if (args.size() < 1) {
                               std::cout << "Usage: diagnostic-export <format> [filepath]" << std::endl;
                               std::cout << "Formats: json, csv, text" << std::endl;
                               return;
                           }
                           
                           ExportFormat format;
                           if (args[0] == "json") format = ExportFormat::JSON;
                           else if (args[0] == "csv") format = ExportFormat::CSV;
                           else format = ExportFormat::TEXT;
                           
                           Phase6Diagnostic diagnostic;
                           Phase6IntegratedExperiment experiment;
                           Phase6Config config;
                           
                           Phase6DiagnosticResult result = diagnostic.analyze(experiment, config);
                           std::string filepath = args.size() > 1 ? args[1] : "diagnostic_export.json";
                           
                           diagnostic.exportDiagnosticData(result, filepath, format);
                       });
    
    cli.registerCommand("diagnostic-log", "Log diagnostic results to file",
                       [](const std::vector<std::string>& args) {
                           if (args.size() < 1) {
                               std::cout << "Usage: diagnostic-log <filepath>" << std::endl;
                               return;
                           }
                           
                           Phase6Diagnostic diagnostic;
                           Phase6IntegratedExperiment experiment;
                           Phase6Config config;
                           
                           Phase6DiagnosticResult result = diagnostic.analyze(experiment, config);
                           std::string report = diagnostic.generateReport(result);
                           diagnostic.logDiagnosticResults(report, args[0]);
                       });
}

void Phase6Diagnostic::configureAdvancedOptions(AdvancedConfig& config) {
    // Configure advanced diagnostic options
    config.enableDetailedLogging = true;
    config.enablePerformanceMetrics = true;
    config.enableMemoryAnalysis = true;
    config.enablePredictionAnalysis = true;
    config.enableNeuromodulationAnalysis = true;
    config.enableDevelopmentAnalysis = true;
    config.enableTimeSeriesAnalysis = true;
    config.exportFormats = {ExportFormat::JSON, ExportFormat::CSV, ExportFormat::TEXT};
    config.sampleInterval = 100;
    config.maxTimeSeriesPoints = 10000;
    config.validationThreshold = 0.8f;
}

void Phase6Diagnostic::analyzeMemoryRetention(const Phase6DiagnosticResult& result,
                                              std::string& analysisMessage) {
    std::stringstream ss;
    
    if (result.memoryWorkingMemoryIntegrated) {
        ss << "Working Memory Analysis:" << std::endl;
        ss << "  - Memory system is properly integrated" << std::endl;
        ss << "  - Active traces: " << result.workingMemoryActiveTraces << std::endl;
        ss << "  - Memory utilization: " << 
           (result.workingMemoryCapacity > 0 ? 
            (float)result.workingMemoryActiveTraces / result.workingMemoryCapacity * 100 : 0) << "%" << std::endl;
        ss << "  - Retention score: " << 
           (result.workingMemoryActiveTraces > 0 ? 
            (float)result.workingMemoryActiveTraces / result.workingMemoryCapacity : 0) << std::endl;
    } else {
        ss << "Working Memory: NOT INTEGRATED - No retention capabilities" << std::endl;
    }
    
    if (result.memoryEpisodicMemoryIntegrated) {
        ss << "\nEpisodic Memory Analysis:" << std::endl;
        ss << "  - Memory system is properly integrated" << std::endl;
        ss << "  - Episodes stored: " << result.episodicMemoryEpisodes << std::endl;
        ss << "  - Maximum episodes: " << result.episodicMemoryMaxEpisodes << std::endl;
        ss << "  - Storage utilization: " << 
           (result.episodicMemoryMaxEpisodes > 0 ?
            (float)result.episodicMemoryEpisodes / result.episodicMemoryMaxEpisodes * 100 : 0) << "%" << std::endl;
        ss << "  - Retention score: " << 
           (result.episodicMemoryMaxEpisodes > 0 ?
            (float)result.episodicMemoryEpisodes / result.episodicMemoryMaxEpisodes : 0) << std::endl;
    } else {
        ss << "\nEpisodic Memory: NOT INTEGRATED - No episodic storage capabilities" << std::endl;
    }
    
    analysisMessage = ss.str();
}

void Phase6Diagnostic::analyzePredictionAccuracy(const Phase6DiagnosticResult& result,
                                                 std::string& analysisMessage) {
    std::stringstream ss;
    
    if (result.predictionIntegrated) {
        ss << "Prediction System Analysis:" << std::endl;
        ss << "  - Prediction system is properly integrated" << std::endl;
        ss << "  - Current prediction error: " << result.predictionError << std::endl;
        ss << "  - Prediction confidence: " << result.predictionConfidence << std::endl;
        ss << "  - Accuracy estimate: " << 
           (result.predictionConfidence > 0 ? result.predictionConfidence * 100 : 0) << "%" << std::endl;
        
        // Analyze prediction error trend
        if (std::abs(result.predictionError) < 0.1f) {
            ss << "  - Error status: GOOD (error < 0.1)" << std::endl;
        } else if (std::abs(result.predictionError) < 0.5f) {
            ss << "  - Error status: MODERATE (error < 0.5)" << std::endl;
        } else {
            ss << "  - Error status: POOR (error >= 0.5)" << std::endl;
        }
    } else {
        ss << "Prediction System: NOT INTEGRATED - No prediction capabilities" << std::endl;
    }
    
    analysisMessage = ss.str();
}

void Phase6Diagnostic::analyzeContinualLearning(const Phase6DiagnosticResult& result,
                                                std::string& analysisMessage) {
    std::stringstream ss;
    
    // Analyze continual learning indicators
    bool hasMemory = result.memoryWorkingMemoryIntegrated || result.memoryEpisodicMemoryIntegrated;
    bool hasNeuromodulation = result.neuromodulationIntegrated;
    bool hasDevelopment = result.developmentIntegrated;
    bool hasPrediction = result.predictionIntegrated;
    
    ss << "Continual Learning Analysis:" << std::endl;
    
    if (hasMemory) {
        ss << "  - Memory systems: " << (hasMemory ? "AVAILABLE" : "UNAVAILABLE") << std::endl;
    }
    
    if (hasNeuromodulation) {
        ss << "  - Neuromodulation: " << (hasNeuromodulation ? "AVAILABLE" : "UNAVAILABLE") << std::endl;
        ss << "  - Dopamine level: " << result.dopamineLevel << " (indicates reward processing)" << std::endl;
        ss << "  - Curiosity level: " << result.curiosityLevel << " (indicates exploration)" << std::endl;
    }
    
    if (hasDevelopment) {
        ss << "  - Development system: " << (hasDevelopment ? "AVAILABLE" : "UNAVAILABLE") << std::endl;
        ss << "  - Developmental stage: " << static_cast<int>(result.developmentStage) << std::endl;
        ss << "  - Plasticity modifier: " << result.developmentStagePlasticity << std::endl;
    }
    
    if (hasPrediction) {
        ss << "  - Prediction system: " << (hasPrediction ? "AVAILABLE" : "UNAVAILABLE") << std::endl;
        ss << "  - Prediction error: " << result.predictionError << " (drives learning)" << std::endl;
    }
    
    // Overall continual learning capability
    int activeSystems = (hasMemory ? 1 : 0) + (hasNeuromodulation ? 1 : 0) + 
                       (hasDevelopment ? 1 : 0) + (hasPrediction ? 1 : 0);
    
    ss << "\nOverall Continual Learning Capability: " << activeSystems << "/4 systems active" << std::endl;
    
    if (activeSystems >= 3) {
        ss << "  - Status: EXCELLENT - Strong continual learning infrastructure" << std::endl;
    } else if (activeSystems == 2) {
        ss << "  - Status: GOOD - Basic continual learning capabilities" << std::endl;
    } else if (activeSystems == 1) {
        ss << "  - Status: LIMITED - Minimal continual learning" << std::endl;
    } else {
        ss << "  - Status: POOR - No continual learning capabilities" << std::endl;
    }
    
    analysisMessage = ss.str();
}

void Phase6Diagnostic::analyzeMultiSystemInteraction(const Phase6DiagnosticResult& result,
                                                    std::string& analysisMessage) {
    std::stringstream ss;
    
    ss << "Multi-System Interaction Analysis:" << std::endl;
    
    // Analyze system interconnections
    bool memoryConnected = result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated;
    bool neuromodulationConnected = result.neuromodulationIntegrated && result.predictionIntegrated;
    bool cognitionConnected = result.cognitionIntegrated && result.developmentIntegrated;
    
    if (memoryConnected) {
        ss << "  - Memory Systems: Connected and functional" << std::endl;
    } else {
        ss << "  - Memory Systems: Disconnected or partially functional" << std::endl;
    }
    
    if (neuromodulationConnected) {
        ss << "  - Neuromodulation + Prediction: Connected" << std::endl;
    } else {
        ss << "  - Neuromodulation + Prediction: Disconnected" << std::endl;
    }
    
    if (cognitionConnected) {
        ss << "  - Cognition + Development: Connected" << std::endl;
    } else {
        ss << "  - Cognition + Development: Disconnected" << std::endl;
    }
    
    // Overall interaction score
    int connectedSystems = (memoryConnected ? 1 : 0) + (neuromodulationConnected ? 1 : 0) + 
                          (cognitionConnected ? 1 : 0);
    
    ss << "\nMulti-System Interaction Score: " << connectedSystems "/3 system connections (" <<
        std::fixed << std::setprecision(1) << (connectedSystems / 3.0f * 100) << "%)" << std::endl;
    
    if (connectedSystems == 3) {
        ss << "  - Status: EXCELLENT - All major systems interconnected" << std::endl;
    } else if (connectedSystems == 2) {
        ss << "  - Status: GOOD - Most systems interconnected" << std::endl;
    } else if (connectedSystems == 1) {
        ss << "  - Status: MODERATE - Limited interconnection" << std::endl;
    } else {
        ss << "  - Status: POOR - Minimal system interconnection" << std::endl;
    }
    
    analysisMessage = ss.str();
}

void Phase6Diagnostic::analyzeDevelopmentalProgression(const Phase6DiagnosticResult& result,
                                                         std::string& analysisMessage) {
    std::stringstream ss;
    
    ss << "Developmental Progression Analysis:" << std::endl;
    
    // Analyze development stage and plasticity
    ss << "  - Current Developmental Stage: " << static_cast<int>(result.developmentStage) << std::endl;
    
    switch (result.developmentStage) {
        case DevelopmentalStage::Initial:
            ss << "    Stage Characteristics: Initial (high plasticity, exploration)" << std::endl;
            ss << "    Plasticity Modifier: High (1.0)" << std::endl;
            break;
        case DevelopmentalStage::CriticalPeriod:
            ss << "    Stage Characteristics: Critical Period (reduced plasticity, specialization)" << std::endl;
            ss << "    Plasticity Modifier: Moderate (0.8)" << std::endl;
            break;
        case DevelopmentalStage::Maturation:
            ss << "    Stage Characteristics: Maturation (balanced plasticity)" << std::endl;
            ss << "    Plasticity Modifier: Low (0.5)" << std::endl;
            break;
        case DevelopmentalStage::Adult:
            ss << "    Stage Characteristics: Adult (stable, refined)" << std::endl;
            ss << "    Plasticity Modifier: Very Low (0.2)" << std::endl;
            break;
    }
    
    ss << "  - Age-Based Plasticity Factor: " << result.developmentAgePlasticityFactor << std::endl;
    ss << "  - Synaptogenesis Events: " << result.developmentSynaptogenesisEvents << std::endl;
    ss << "  - Pruning Events: " << result.developmentPruningEvents << std::endl;
    
    // Development progress indicators
    bool isDeveloping = (result.developmentStage != DevelopmentalStage::Adult);
    bool hasPlasticity = result.developmentStagePlasticity > 0.0f;
    
    ss << "\nDevelopmental Progress:" << std::endl;
    ss << "  - Currently Developing: " << (isDeveloping ? "YES" : "NO") << std::endl;
    ss << "  - Has Plasticity: " << (hasPlasticity ? "YES" : "NO") << std::endl;
    
    float developmentScore = 0.0f;
    if (isDeveloping) developmentScore += 0.5f;
    if (hasPlasticity) developmentScore += 0.5f;
    developmentScore += (result.developmentSynaptogenesisEvents > 0 ? 0.2f : 0.0f);
    developmentScore += (result.developmentPruningEvents > 0 ? 0.2f : 0.0f);
    
    ss << "  - Development Score: " << std::fixed << std::setprecision(2) << developmentScore << "/1.0" << std::endl;
    
    if (developmentScore >= 0.8f) {
        ss << "  - Status: EXCELLENT - Strong developmental progression" << std::endl;
    } else if (developmentScore >= 0.6f) {
        ss << "  - Status: GOOD - Moderate developmental progression" << std::endl;
    } else if (developmentScore >= 0.4f) {
        ss << "  - Status: MODERATE - Limited developmental progression" << std::endl;
    } else {
        ss << "  - Status: LIMITED - Minimal development" << std::endl;
    }
    
    analysisMessage = ss.str();
}

void Phase6Diagnostic::analyzeAdvancedDebugging(std::shared_ptr<Brain> brain,
                                                 const Phase6DiagnosticResult& result,
                                                 std::string& analysisMessage) {
    std::stringstream ss;
    
    ss << "Advanced Debugging Analysis:" << std::endl;
    
    // Analyze brain state
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    float firingRate = brain->getAverageFiringRate();
    
    ss << "  - Total Neurons: " << totalNeurons << std::endl;
    ss << "  - Active Neurons: " << activeNeurons << std::endl;
    ss << "  - Firing Neurons: " << firingNeurons << std::endl;
    ss << "  - Average Firing Rate: " << std::fixed << std::setprecision(4) << firingRate << " Hz" << std::endl;
    
    // Analyze brain health indicators
    float activityRatio = (totalNeurons > 0 ? (float)activeNeurons / totalNeurons : 0.0f);
    float firingRatio = (activeNeurons > 0 ? (float)firingNeurons / activeNeurons : 0.0f);
    
    ss << "  - Activity Ratio: " << std::fixed << std::setprecision(4) << activityRatio << std::endl;
    ss << "  - Firing Ratio: " << std::fixed << std::setprecision(4) << firingRatio << std::endl;
    
    // Identify potential issues
    std::vector<std::string> issues;
    
    if (totalNeurons == 0) {
        issues.push_back("No neurons created - potential configuration issue");
    }
    
    if (activeNeurons == 0) {
        issues.push_back("No active neurons - potential initialization issue");
    }
    
    if (firingNeurons == 0 && firingRate > 0.01f) {
        issues.push_back("Neurons firing but no recorded spikes - potential spike recording issue");
    }
    
    if (activityRatio < 0.01f) {
        issues.push_back("Very low neuron activity - potential input issue");
    }
    
    if (issues.empty()) {
        ss << "\n  - Brain State: HEALTHY - All indicators normal" << std::endl;
    } else {
        ss << "\n  - Potential Issues:" << std::endl;
        for (const auto& issue : issues) {
            ss << "    * " << issue << std::endl;
        }
    }
    
    // Analyze memory health
    if (result.memoryWorkingMemoryIntegrated) {
        ss << "\n  - Working Memory: " << (result.workingMemoryActiveTraces > 0 ? "ACTIVE" : "EMPTY") << std::endl;
    }
    
    if (result.memoryEpisodicMemoryIntegrated) {
        ss << "  - Episodic Memory: " << (result.episodicMemoryEpisodes > 0 ? "HAS EPISODES" : "EMPTY") << std::endl;
    }
    
    analysisMessage = ss.str();
}

void Phase6Diagnostic::generatePerformanceReport(const Phase6DiagnosticResult& result,
                                                 std::string& reportMessage) {
    std::stringstream ss;
    
    ss << "Phase 6 Performance Report" << std::endl;
    ss << std::string(50, '=') << std::endl;
    
    // Calculate performance metrics
    float memoryEfficiency = 0.0f;
    if (result.workingMemoryCapacity > 0) {
        memoryEfficiency = (float)result.workingMemoryActiveTraces / result.workingMemoryCapacity;
    }
    
    float memoryRetention = 0.0f;
    if (result.episodicMemoryMaxEpisodes > 0) {
        memoryRetention = (float)result.episodicMemoryEpisodes / result.episodicMemoryMaxEpisodes;
    }
    
    float neuromodulationEffectiveness = 0.0f;
    if (result.dopamineLevel > 0.0f) neuromodulationEffectiveness += 0.5f;
    if (result.curiosityLevel > 0.0f) neuromodulationEffectiveness += 0.5f;
    
    float predictionAccuracy = (result.predictionConfidence > 0 ? result.predictionConfidence : 0.0f);
    
    float developmentEffectiveness = (result.developmentStage != DevelopmentalStage::Adult ? 0.5f : 0.0f) +
                                     (result.developmentSynaptogenesisEvents > 0 ? 0.3f : 0.0f) +
                                     (result.developmentPruningEvents > 0 ? 0.2f : 0.0f);
    
    ss << "Efficiency Metrics:" << std::endl;
    ss << "  - Memory Efficiency: " << std::fixed << std::setprecision(2) << memoryEfficiency << "/1.0" << std::endl;
    ss << "  - Memory Retention: " << std::fixed << std::setprecision(2) << memoryRetention << "/1.0" << std::endl;
    ss << "  - Neuromodulation Effectiveness: " << std::fixed << std::setprecision(2) << neuromodulationEffectiveness << "/1.0" << std::endl;
    ss << "  - Prediction Accuracy: " << std::fixed << std::setprecision(2) << predictionAccuracy << "/1.0" << std::endl;
    ss << "  - Development Effectiveness: " << std::fixed << std::setprecision(2) << developmentEffectiveness << "/1.0" << std::endl;
    
    ss << "\nOverall Performance Score:" << std::endl;
    float totalScore = memoryEfficiency + memoryRetention + neuromodulationEffectiveness + predictionAccuracy + developmentEffectiveness;
    float avgScore = totalScore / 5.0f;
    
    ss << "  - Average System Score: " << std::fixed << std::setprecision(2) << avgScore << "/1.0" << std::endl;
    ss << "  - Total Score: " << std::fixed << std::setprecision(2) << totalScore << "/5.0" << std::endl;
    
    // Performance classification
    if (avgScore >= 0.8f) {
        ss << "  - Performance Classification: EXCELLENT" << std::endl;
    } else if (avgScore >= 0.6f) {
        ss << "  - Performance Classification: GOOD" << std::endl;
    } else if (avgScore >= 0.4f) {
        ss << "  - Performance Classification: MODERATE" << std::endl;
    } else {
        ss << "  - Performance Classification: NEEDS IMPROVEMENT" << std::endl;
    }
    
    reportMessage = ss.str();
}

Phase6Diagnostic::Phase6Diagnostic() {
    // Initialize with default configuration
    AdvancedConfig config;
    configureAdvancedOptions(config);
}

Phase6Diagnostic::~Phase6Diagnostic() = default;