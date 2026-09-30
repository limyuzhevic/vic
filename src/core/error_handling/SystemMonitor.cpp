#include "SystemMonitor.hpp"
#include "exceptions/BrainException.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

void SystemMonitor::updateSystemState(const std::string& component, bool healthy,
                                     const std::vector<std::string>& errors) {
    SystemState state;
    state.component = component;
    state.healthy = healthy;
    state.healthScore = healthy ? 1.0 : 0.0;
    state.activeErrors = errors;
    state.timestamp = 0.0;  // Would get from SimulationClock
    
    // Update system state
    // Implementation would update internal state tracking
    LOG_DEBUG("System state updated for component: " + component + ", healthy: " + 
              (healthy ? "true" : "false"));
}

SystemMonitor::SystemState SystemMonitor::getSystemState(const std::string& component) {
    SystemState state;
    state.component = component;
    // Implementation would retrieve from internal state tracking
    return state;
}

std::vector<SystemMonitor::SystemState> SystemMonitor::getAllSystemStates() {
    std::vector<SystemState> states;
    // Implementation would retrieve all system states
    return states;
}

void SystemMonitor::correlateErrors(const std::string& error1, const std::string& error2) {
    LOG_DEBUG("Correlating errors: " + error1 + " and " + error2);
    // Implementation would establish correlation between errors
}

std::vector<std::string> SystemMonitor::getErrorCorrelations(const std::string& error) {
    std::vector<std::string> correlations;
    // Implementation would retrieve correlated errors
    return correlations;
}

void SystemMonitor::generateHealthReport() {
    std::ostringstream oss;
    oss << "System Health Report\n"
        << "==================\n\n"
        << "Generated at: [timestamp]\n\n";
    
    oss << "Component Health:\n";
    for (const auto& state : getAllSystemStates()) {
        oss << "  " << state.component << ": "
            << (state.healthy ? "HEALTHY" : "UNHEALTHY") << " "
            << std::fixed << std::setprecision(2) << (state.healthScore * 100.0) << "%\n";
        if (!state.activeErrors.empty()) {
            oss << "    Errors:\n";
            for (const auto& error : state.activeErrors) {
                oss << "      - " << error << "\n";
            }
        }
    }
    
    oss << "\nSummary:\n"
        << "  Total Components: " << getAllSystemStates().size() << "\n"
        << "  Healthy Components: " << std::count_if(getAllSystemStates().begin(),
                                                   getAllSystemStates().end(),
                                                   [](const SystemState& s) { return s.healthy; })
        << "\n"
        << "  Total Errors: " << std::accumulate(getAllSystemStates().begin(),
                                               getAllSystemStates().end(), 0,
                                               [](int sum, const SystemState& s) {
                                                   return sum + s.activeErrors.size();
                                               }) << "\n";
    
    std::string report = oss.str();
    LOG_INFO("Health report generated\n" + report);
}

} // namespace nlm
