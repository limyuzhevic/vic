#include "debugging/DebuggingTools.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

struct DebuggingTools::Impl {
    // Simple debug tracking
    struct DebugSession {
        std::chrono::steady_clock::time_point startTime;
        SimulationStep step;
        std::string status;
    };
    
    std::vector<DebugSession> debugSessions;
    std::unordered_map<std::string, std::vector<ValidationResult>> validationResults;
};

DebuggingTools::DebuggingTools()
    : brain_(nullptr)
    , currentStep_(0)
    , debugLevel_(1)
{
    pImpl = std::make_unique<Impl>();
    startTime_ = std::chrono::steady_clock::now();
    
    // Initialize default features
    enabledFeatures_["stack_trace_capture"] = false;
    enabledFeatures_["performance_monitoring"] = false;
    
    NLM_LOG_INFO("DebuggingTools initialized");
}

DebuggingTools::~DebuggingTools() {
    NLM_LOG_INFO("DebuggingTools cleaned up");
}

void DebuggingTools::integrateWithBrain(Brain* brain) {
    brain_ = brain;
    
    if (brain) {
        NLM_LOG_INFO("DebuggingTools integrated with Brain");
        pImpl->debugSessions.push_back({std::chrono::steady_clock::now(), currentStep_, "active"});
    }
}

void DebuggingTools::updateAnalysisData() {
    currentStep_++;
}

DebugReport DebuggingTools::generateDebugReport() const {
    DebugReport report;
    report.reportId = "DBG_" + std::to_string(currentStep_) + "_" + 
                      std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    report.generationTime = std::chrono::steady_clock::now();
    report.currentStep = currentStep_;
    
    // Collect validation results
    std::vector<ValidationResult> allValidations;
    for (const auto& pair : pImpl->validationResults) {
        allValidations.insert(allValidations.end(), pair.second.begin(), pair.second.end());
    }
    
    // Count issues
    for (const auto& validation : allValidations) {
        if (!validation.isValid) {
            report.consistencyIssues.push_back(validation.validationType + ": " + validation.message);
        }
    }
    
    // Generate summary
    report.brainStateSummary = brain_ ? ("Integrated with " + std::to_string(brain_->getRegionCount()) + " regions") : "Not integrated";
    
    // Set metrics
    report.metrics["total_validations"] = static_cast<double>(allValidations.size());
    report.metrics["successful_validations"] = allValidations.empty() ? 0.0 : 
        std::count_if(allValidations.begin(), allValidations.end(), 
                     [](const ValidationResult& v) { return v.isValid; });
    
    report.hasCriticalIssues = !allValidations.empty() && 
        std::any_of(allValidations.begin(), allValidations.end(),
                   [](const ValidationResult& v) { return !v.isValid; });
    
    // Generate recommendations
    report.recommendations = generateDebugRecommendations(report);
    
    return report;
}

void DebuggingTools::saveDebugReport(const DebugReport& report, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save debug report: " + filepath);
        return;
    }
    
    file << "{\n";
    file << "  \"reportId\": \"" << report.reportId << "\",\n";
    file << "  \"generationTime\": \"" << getTimeString(report.generationTime) << "\",\n";
    file << "  \"currentStep\": " << report.currentStep << ",\n";
    file << "  \"brainStateSummary\": \"" << report.brainStateSummary << "\",\n";
    file << "  \"hasCriticalIssues\": " << (report.hasCriticalIssues ? "true" : "false") << ",\n";
    
    file << "  \"consistencyIssues\": [";
    for (size_t i = 0; i < report.consistencyIssues.size(); ++i) {
        file << "\"" << report.consistencyIssues[i] << "\"";
        if (i < report.consistencyIssues.size() - 1) file << ", ";
    }
    file << "],\n";
    
    file << "  \"metrics\": {";
    bool firstMetric = true;
    for (const auto& pair : report.metrics) {
        if (!firstMetric) file << ", ";
        file << "\"" << pair.first << "\": " << pair.second;
        firstMetric = false;
    }
    file << "}\n";
    file << "}\n";
    file.close();
    
    NLM_LOG_INFO("Debug report saved to: " + filepath);
}

std::string DebuggingTools::formatDebugReport(const DebugReport& report) const {
    std::stringstream ss;
    ss << "DEBUG REPORT - " << report.reportId << "\n";
    ss << "=" + std::string(50, '=') << "\n\n";
    ss << "Report Information:\n";
    ss << "  Report ID: " << report.reportId << "\n";
    ss << "  Generation Time: " << getTimeString(report.generationTime) << "\n";
    ss << "  Current Step: " << report.currentStep << "\n";
    ss << "  Brain State: " << report.brainStateSummary << "\n";
    ss << "  Critical Issues: " << (report.hasCriticalIssues ? "YES" : "NO") << "\n\n";
    
    if (!report.consistencyIssues.empty()) {
        ss << "Consistency Issues:\n";
        for (const auto& issue : report.consistencyIssues) {
            ss << "  - " << issue << "\n";
        }
        ss << "\n";
    }
    
    ss << "Metrics:\n";
    for (const auto& pair : report.metrics) {
        ss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    ss << "\nRecommendations:\n";
    ss << report.recommendations << "\n";
    
    return ss.str();
}

ValidationResult DebuggingTools::validateBrainState(const Brain* brain) const {
    ValidationResult result;
    result.validationType = "brain_state";
    result.step = currentStep_;
    result.timestamp = std::chrono::steady_clock::now();
    
    if (!brain) {
        result.isValid = false;
        result.message = "Brain pointer is null";
        result.errors.push_back("Brain pointer is null");
        return result;
    }
    
    // Validate brain has regions
    size_t regionCount = brain->getRegionCount();
    if (regionCount == 0) {
        result.isValid = false;
        result.message = "Brain has no regions";
        result.errors.push_back("Brain has no regions");
    } else {
        result.isValid = true;
        result.message = "Brain validation passed: " + std::to_string(regionCount) + " regions";
    }
    
    pImpl->validationResults["brain_state"].push_back(result);
    return result;
}

ValidationResult DebuggingTools::validateMemoryConsistency(const Brain* brain) const {
    ValidationResult result;
    result.validationType = "memory_consistency";
    result.step = currentStep_;
    result.timestamp = std::chrono::steady_clock::now();
    
    if (!brain) {
        result.isValid = false;
        result.message = "Brain pointer is null";
        result.errors.push_back("Cannot validate memory consistency with null brain");
        return result;
    }
    
    // Check working memory
    if (brain->getWorkingMemory()) {
        size_t capacity = brain->getWorkingMemory()->getCapacity();
        size_t active = brain->getWorkingMemory()->getActiveTraces();
        
        if (active > capacity) {
            result.errors.push_back("Working memory: active traces exceed capacity");
        }
        
        if (active == capacity) {
            result.warnings.push_back("Working memory at full capacity");
        }
    }
    
    result.isValid = result.errors.empty();
    result.message = result.isValid ? "Memory consistency validation passed" : "Memory consistency validation failed";
    
    pImpl->validationResults["memory_consistency"].push_back(result);
    return result;
}

ValidationResult DebuggingTools::validateNeuralConnections(const Brain* brain) const {
    ValidationResult result;
    result.validationType = "neural_connections";
    result.step = currentStep_;
    result.timestamp = std::chrono::steady_clock::now();
    
    if (!brain) {
        result.isValid = false;
        result.message = "Brain pointer is null";
        result.errors.push_back("Cannot validate neural connections with null brain");
        return result;
    }
    
    // Count synapses
    size_t totalConnections = 0;
    for (const auto& region : brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto& neuron : population->getNeurons()) {
                totalConnections += neuron->getSynapticConnections().size();
            }
        }
    }
    
    if (totalConnections == 0) {
        result.warnings.push_back("No synaptic connections found");
    }
    
    result.isValid = true;
    result.message = "Neural connections validation completed: " + std::to_string(totalConnections) + " connections";
    
    pImpl->validationResults["neural_connections"].push_back(result);
    return result;
}

ValidationResult DebuggingTools::validateConfiguration(const Config& config) const {
    ValidationResult result;
    result.validationType = "configuration";
    result.step = currentStep_;
    result.timestamp = std::chrono::steady_clock::now();
    
    try {
        // Validate neuron count
        int neuronCount = config.get<int>("neuron_count", -1);
        if (neuronCount <= 0) {
            result.errors.push_back("Invalid neuron_count: must be positive");
        }
        
        // Validate region count
        int regionCount = config.get<int>("region_count", -1);
        if (regionCount <= 0) {
            result.errors.push_back("Invalid region_count: must be positive");
        }
        
        // Validate connection probability
        float connectionProb = config.get<float>("connection_probability", -1.0f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            result.errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
        }
        
    } catch (const std::exception& e) {
        result.isValid = false;
        result.message = "Configuration validation exception: " + std::string(e.what());
        result.errors.push_back("Exception during configuration validation: " + std::string(e.what()));
        return result;
    }
    
    result.isValid = result.errors.empty();
    result.message = result.isValid ? "Configuration validation passed" : "Configuration validation failed";
    
    pImpl->validationResults["configuration"].push_back(result);
    return result;
}

void DebuggingTools::exportMemoryAnalysis(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to export memory analysis: " + filepath);
        return;
    }
    
    file << "{\n";
    file << "  \"timestamp\": \"" << getTimeString(std::chrono::steady_clock::now()) << "\",\n";
    file << "  \"currentStep\": " << currentStep_ << ",\n";
    file << "  \"debugLevel\": " << debugLevel_ << ",\n";
    
    if (brain_) {
        file << "  \"brainIntegrated\": true,\n";
        file << "  \"regionCount\": " << brain_->getRegionCount() << ",\n";
    } else {
        file << "  \"brainIntegrated\": false,\n";
    }
    
    file << "  \"featuresEnabled\": [";
    bool firstFeature = true;
    for (const auto& pair : enabledFeatures_) {
        if (!firstFeature) file << ", ";
        file << "\"" << pair.first << "\": " << (pair.second ? "true" : "false");
        firstFeature = false;
    }
    file << "]\n";
    
    file << "}\n";
    file.close();
    
    NLM_LOG_INFO("Memory analysis exported to: " + filepath);
}

void DebuggingTools::enableFeature(const std::string& featureName, bool enable) {
    enabledFeatures_[featureName] = enable;
    
    if (featureName == "performance_monitoring") {
        if (enable) {
            NLM_LOG_INFO("Performance monitoring enabled");
        } else {
            NLM_LOG_INFO("Performance monitoring disabled");
        }
    }
    
    NLM_LOG_INFO("Feature '" + featureName + "': " + (enable ? "enabled" : "disabled"));
}

bool DebuggingTools::isFeatureEnabled(const std::string& featureName) const {
    auto it = enabledFeatures_.find(featureName);
    return it != enabledFeatures_.end() && it->second;
}

// Helper methods
std::string DebuggingTools::getTimeString(const std::chrono::steady_clock::time_point& tp) const {
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string DebuggingTools::generateDebugRecommendations(const DebugReport& report) const {
    std::stringstream ss;
    
    if (report.hasCriticalIssues) {
        ss << "CRITICAL: System requires immediate attention!\n";
        ss << "  • Run full diagnostic suite\n";
        ss << "  • Check system logs for error details\n";
    } else if (!report.consistencyIssues.empty()) {
        ss << "MINOR ISSUES DETECTED:\n";
        for (const auto& issue : report.consistencyIssues) {
            ss << "  • " << issue << "\n";
        }
        ss << "  • Consider system optimization\n";
    } else {
        ss << "System operating normally. No immediate actions required.\n";
        ss << "  • Continue regular monitoring\n";
    }
    
    ss << "Debug recommendations generated based on " << report.currentStep << " simulation steps.\n";
    
    return ss.str();
}

} // namespace nlm