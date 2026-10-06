#include "Phase6Visualization.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>

namespace nlm {

struct Phase6Visualization::Impl {
    bool active;
    int width;
    int height;
    double updateRate;
    
    Impl() : active(false), width(800), height(600), updateRate(30.0) {}
};

Phase6Visualization::Phase6Visualization() : pImpl(new Impl) {}

Phase6Visualization::~Phase6Visualization() = default;

bool Phase6Visualization::initialize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
    pImpl->active = true;
    return true;
}

void Phase6Visualization::render(const Phase6IntegrationResult& result, const Phase6Config& config) {
    renderHeader();
    renderSystemStatus(result);
    renderPerformanceMetrics(result);
    renderIntegrationResults(result);
    renderDebugInfo(result);
}

void Phase6Visualization::renderHeader() {
    std::cout << "===========================================" << std::endl;
    std::cout << "       NLM PHASE 6: INTEGRATION VISUALIZATION    " << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Generated: " << getCurrentTimeString() << std::endl;
    std::cout << std::endl;
}

void Phase6Visualization::renderSystemStatus(const Phase6IntegrationResult& result) {
    std::cout << "--- SYSTEM INTEGRATION STATUS ---" << std::endl;
    std::cout << "Memory Systems:" << std::endl;
    std::cout << "  Working Memory:      " << getStatusSymbol(result.memoryWorkingMemoryIntegrated) << " " << getStatusColor(result.memoryWorkingMemoryIntegrated) << std::endl;
    std::cout << "  Episodic Memory:     " << getStatusSymbol(result.memoryEpisodicMemoryIntegrated) << " " << getStatusColor(result.memoryEpisodicMemoryIntegrated) << std::endl;
    
    std::cout << "Neuromodulation:" << std::endl;
    std::cout << "  Dopamine:            " << getStatusSymbol(result.neuromodulationIntegrated) << " " << getStatusColor(result.neuromodulationIntegrated) << std::endl;
    
    std::cout << "Cognitive Systems:" << std::endl;
    std::cout << "  Prediction:          " << getStatusSymbol(result.predictionIntegrated) << " " << getStatusColor(result.predictionIntegrated) << std::endl;
    std::cout << "  Development:         " << getStatusSymbol(result.developmentIntegrated) << " " << getStatusColor(result.developmentIntegrated) << std::endl;
    
    std::cout << "Persistence:" << std::endl;
    std::cout << "  Checkpointing:       " << getStatusSymbol(result.checkpointingWorks) << " " << getStatusColor(result.checkpointingWorks) << std::endl;
    std::cout << std::endl;
}

void Phase6Visualization::renderPerformanceMetrics(const Phase6IntegrationResult& result) {
    std::cout << "--- PERFORMANCE METRICS ---" << std::endl;
    std::cout << formatMetric("Total Reward", result.totalReward) << std::endl;
    std::cout << formatMetric("Average Firing Rate", result.avgFiringRate, " Hz") << std::endl;
    std::cout << formatMetric("Episodes Stored", result.memoryEpisodesStored) << std::endl;
    std::cout << formatMetric("Novelty Level", result.noveltyLevel) << std::endl;
    std::cout << formatMetric("Curiosity Level", result.curiosityLevel) << std::endl;
    std::cout << formatMetric("Dopamine Level", result.dopamineLevel) << std::endl;
    std::cout << formatMetric("Wall Clock Time", result.totalWallClockTime, "s") << std::endl;
    std::cout << std::endl;
}

void Phase6Visualization::renderIntegrationResults(const Phase6IntegrationResult& result) {
    std::cout << "--- INTEGRATION VERIFICATION ---" << std::endl;
    
    int connectedCount = 0;
    if (result.memoryWorkingMemoryIntegrated) connectedCount++;
    if (result.memoryEpisodicMemoryIntegrated) connectedCount++;
    if (result.neuromodulationIntegrated) connectedCount++;
    if (result.predictionIntegrated) connectedCount++;
    if (result.developmentIntegrated) connectedCount++;
    
    std::cout << "Systems Connected: " << connectedCount << "/5 (" << (connectedCount * 20) << "%)" << std::endl;
    std::cout << "Integration Success: " << (connectedCount >= 4 ? "HIGH" : "MODERATE") << std::endl;
    std::cout << std::endl;
}

void Phase6Visualization::renderDebugInfo(const Phase6IntegrationResult& result) {
    std::cout << "--- DEBUG INFORMATION ---" << std::endl;
    std::cout << "Timestamp: " << getCurrentTimeString() << std::endl;
    std::cout << "Experiment Started: " << (result.startTime ? "Yes" : "No") << std::endl;
    std::cout << "Experiment Completed: " << (result.endTime ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
}

std::string Phase6Visualization::generateASCIIArt(const Phase6IntegrationResult& result) {
    std::stringstream ss;
    ss << "NLM PHASE 6 INTEGRATION STATUS" << std::endl;
    ss << "============================" << std::endl;
    ss << std::endl;
    ss << "NEURAL CORE:           OPERATIONAL" << std::endl;
    ss << "MEMORY SYSTEMS:       " << (result.memoryWorkingMemoryIntegrated ? "✓ WORKING" : "✗ DISCONNECTED") << std::endl;
    ss << "Episodic Memory:       " << (result.memoryEpisodicMemoryIntegrated ? "✓ OPERATIONAL" : "✗ DISCONNECTED") << std::endl;
    ss << "Neuromodulation:       " << (result.neuromodulationIntegrated ? "✓ FUNCTIONAL" : "✗ LIMITED") << std::endl;
    ss << "Prediction System:     " << (result.predictionIntegrated ? "✓ INTEGRATED" : "✗ DISCONNECTED") << std::endl;
    ss << "Development System:    " << (result.developmentIntegrated ? "✓ ACTIVE" : "✗ MINIMAL") << std::endl;
    ss << "Persistence:           " << (result.checkpointingWorks ? "✓ OPERATIONAL" : "✗ FAILED") << std::endl;
    ss << std::endl;
    ss << "PERFORMANCE:" << std::endl;
    ss << "  Reward Score:        " << result.totalReward << std::endl;
    ss << "  Neural Activity:     " << result.avgFiringRate << " Hz" << std::endl;
    ss << "  Memory Capacity:     " << result.memoryEpisodesStored << " episodes" << std::endl;
    ss << "  System Health:       " << (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated ? "GOOD" : "NEEDS ATTENTION") << std::endl;
    ss << std::endl;
    ss << "Phase 6 Status:        " << (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated && result.developmentIntegrated ? "SUCCESS" : "IN PROGRESS") << std::endl;
    
    return ss.str();
}

std::string Phase6Visualization::generateHTMLReport(const Phase6IntegrationResult& result, const Phase6Config& config) {
    std::stringstream ss;
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head>" << std::endl;
    ss << "    <title>Phase 6 Integration Report</title>" << std::endl;
    ss << "    <style>" << std::endl;
    ss << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    ss << "        .header { background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; }" << std::endl;
    ss << "        .system { margin: 10px 0; padding: 10px; border-left: 4px solid #3498db; background-color: #ecf0f1; }" << std::endl;
    ss << "        .connected { border-color: #27ae60; }" << std::endl;
    ss << "        .disconnected { border-color: #e74c3c; }" << std::endl;
    ss << "        .metric { display: inline-block; margin: 5px 15px; padding: 5px; background-color: #f8f9fa; border-radius: 3px; }" << std::endl;
    ss << "    </style>" << std::endl;
    ss << "</head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "    <div class=\"header\">" << std::endl;
    ss << "        <h1>NLM Phase 6 Integration Report</h1>" << std::endl;
    ss << "        <p>Generated: " << getCurrentTimeString() << "</p>" << std::endl;
    ss << "    </div>" << std::endl;
    ss << "    <h2>System Integration Status</h2>" << std::endl;
    ss << "    <div class=\"system \" + std::string(result.memoryWorkingMemoryIntegrated ? "connected" : "disconnected\") + "\">" << std::endl;
    ss << "        <h3>Memory Systems</h3>" << std::endl;
    ss << "        <p>Working Memory: <strong>" << (result.memoryWorkingMemoryIntegrated ? "OPERATIONAL" : "DISCONNECTED") << "</strong></p>" << std::endl;
    ss << "        <p>Episodic Memory: <strong>" << (result.memoryEpisodicMemoryIntegrated ? "FUNCTIONAL" : "DISCONNECTED") << "</strong></p>" << std::endl;
    ss << "    </div>" << std::endl;
    ss << "    <div class=\"system \" + std::string(result.neuromodulationIntegrated ? "connected" : "disconnected\") + "\">" << std::endl;
    ss << "        <h3>Neuromodulation</h3>" << std::endl;
    ss << "        <p>Dopamine System: <strong>" << (result.neuromodulationIntegrated ? "OPERATIONAL" : "LIMITED") << "</strong></p>" << std::endl;
    ss << "    </div>" << std::endl;
    ss << "    <div class=\"system \" + std::string(result.developmentIntegrated ? "connected" : "disconnected\") + "\">" << std::endl;
    ss << "        <h3>Development</h3>" << std::endl;
    ss << "        <p>Development System: <strong>" << (result.developmentIntegrated ? "ACTIVE" : "MINIMAL") << "</strong></p>" << std::endl;
    ss << "    </div>" << std::endl;
    ss << "    <h2>Performance Metrics</h2>" << std::endl;
    ss << "    <div>" << std::endl;
    ss << "        <span class=\"metric\">Total Reward: <strong>" << result.totalReward << "</strong></span>" << std::endl;
    ss << "        <span class=\"metric\">Firing Rate: <strong>" << result.avgFiringRate << " Hz</strong></span>" << std::endl;
    ss << "        <span class=\"metric\">Episodes: <strong>" << result.memoryEpisodesStored << "</strong></span>" << std::endl;
    ss << "    </div>" << std::endl;
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;
    
    return ss.str();
}

std::string Phase6Visualization::exportMetricsCSV(const Phase6IntegrationResult& result) {
    std::stringstream ss;
    ss << "Metric,Value,Unit" << std::endl;
    ss << "Total Reward," << result.totalReward << ",points" << std::endl;
    ss << "Average Firing Rate," << result.avgFiringRate << ",Hz" << std::endl;
    ss << "Episodes Stored," << result.memoryEpisodesStored << ",count" << std::endl;
    ss << "Novelty Level," << result.noveltyLevel << ",scale" << std::endl;
    ss << "Curiosity Level," << result.curiosityLevel << ",scale" << std::endl;
    ss << "Dopamine Level," << result.dopamineLevel << ",scale" << std::endl;
    ss << "Wall Clock Time," << result.totalWallClockTime << ",seconds" << std::endl;
    ss << "Memory Working Memory," << (result.memoryWorkingMemoryIntegrated ? "1" : "0") << ",status" << std::endl;
    ss << "Memory Episodic Memory," << (result.memoryEpisodicMemoryIntegrated ? "1" : "0") << ",status" << std::endl;
    ss << "Neuromodulation," << (result.neuromodulationIntegrated ? "1" : "0") << ",status" << std::endl;
    ss << "Prediction," << (result.predictionIntegrated ? "1" : "0") << ",status" << std::endl;
    ss << "Development," << (result.developmentIntegrated ? "1" : "0") << ",status" << std::endl;
    ss << "Checkpointing," << (result.checkpointingWorks ? "1" : "0") << ",status" << std::endl;
    
    return ss.str();
}

bool Phase6Visualization::isActive() const {
    return pImpl->active;
}

void Phase6Visualization::renderHeader() {
    std::cout << "===========================================" << std::endl;
    std::cout << "       NLM PHASE 6: INTEGRATION VISUALIZATION    " << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Generated: " << getCurrentTimeString() << std::endl;
    std::cout << std::endl;
}

std::string Phase6Visualization::formatMetric(const std::string& name, float value, const std::string& unit) {
    std::stringstream ss;
    ss << std::left << std::setw(25) << name << ": " << std::fixed << std::setprecision(3) << value;
    if (!unit.empty()) {
        ss << " " << unit;
    }
    ss << std::endl;
    return ss.str();
}

std::string Phase6Visualization::getStatusColor(bool connected) {
    return connected ? "✓ GREEN" : "✗ RED";
}

std::string Phase6Visualization::getStatusSymbol(bool connected) {
    return connected ? "✓" : "✗";
}

} // namespace nlm
