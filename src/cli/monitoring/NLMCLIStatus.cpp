#include "NLMCLIStatus.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

NLMCLIStatus::NLMCLIStatus() : responsive(true), startTime(std::chrono::system_clock::now()) {
}

NLMCLIStatus::~NLMCLIStatus() {
}

void NLMCLIStatus::updateStatus(const std::string& command, bool success, long long executionTime) {
    auto now = std::chrono::system_clock::now();
    
    if (commandStats.find(command) == commandStats.end()) {
        CommandInfo info;
        info.command = command;
        info.executionCount = 0;
        info.successCount = 0;
        info.totalExecutionTime = 0;
        info.firstExecution = now;
        info.lastExecution = now;
        commandStats[command] = info;
    }
    
    CommandInfo& info = commandStats[command];
    info.executionCount++;
    if (success) {
        info.successCount++;
    }
    info.totalExecutionTime += executionTime;
    info.lastExecution = now;
    
    // Check responsiveness
    responsive = true;
}

std::string NLMCLIStatus::getStatusReport() const {
    std::stringstream ss;
    
    ss << "=== NLM CLI Status Report ===" << std::endl;
    ss << "Uptime: " << formatDuration(getUptime()) << std::endl;
    ss << "Responsive: " << (isResponsive() ? "Yes" : "No") << std::endl;
    ss << std::endl;
    
    ss << "Command Statistics:" << std::endl;
    for (const auto& pair : commandStats) {
        const CommandInfo& info = pair.second;
        double successRate = (info.executionCount > 0) ? 
            (static_cast<double>(info.successCount) / info.executionCount * 100.0) : 0.0;
        
        ss << "  " << info.command << ":" << std::endl;
        ss << "    Executions: " << info.executionCount << std::endl;
        ss << "    Success Rate: " << std::fixed << std::setprecision(1) << successRate << "%" << std::endl;
        ss << "    Average Time: " << formatDuration(info.totalExecutionTime / info.executionCount) << std::endl;
        ss << "    Total Time: " << formatDuration(info.totalExecutionTime) << std::endl;
        ss << "    First Execution: " << std::chrono::system_clock::to_time_t(info.firstExecution) << std::endl;
        ss << "    Last Execution: " << std::chrono::system_clock::to_time_t(info.lastExecution) << std::endl;
    }
    
    return ss.str();
}

std::map<std::string, std::map<std::string, std::string>> NLMCLIStatus::getCommandStats() const {
    std::map<std::string, std::map<std::string, std::string>> stats;
    
    for (const auto& pair : commandStats) {
        const CommandInfo& info = pair.second;
        std::map<std::string, std::string> stat;
        
        double successRate = (info.executionCount > 0) ? 
            (static_cast<double>(info.successCount) / info.executionCount * 100.0) : 0.0;
        
        stat["command"] = info.command;
        stat["executions"] = std::to_string(info.executionCount);
        stat["success_count"] = std::to_string(info.successCount);
        stat["success_rate"] = std::to_string(successRate);
        stat["avg_execution_time_ms"] = std::to_string(info.totalExecutionTime / info.executionCount);
        stat["total_execution_time_ms"] = std::to_string(info.totalExecutionTime);
        
        stats[info.command] = stat;
    }
    
    return stats;
}

std::string NLMCLIStatus::getOverallStats() const {
    std::stringstream ss;
    
    ss << "=== Overall Statistics ===" << std::endl;
    ss << "Total Commands Executed: " << commandStats.size() << std::endl;
    
    int totalExecutions = 0;
    int totalSuccesses = 0;
    long long totalTime = 0;
    
    for (const auto& pair : commandStats) {
        const CommandInfo& info = pair.second;
        totalExecutions += info.executionCount;
        totalSuccesses += info.successCount;
        totalTime += info.totalExecutionTime;
    }
    
    double overallSuccessRate = (totalExecutions > 0) ? 
        (static_cast<double>(totalSuccesses) / totalExecutions * 100.0) : 0.0;
    
    ss << "Total Execution Count: " << totalExecutions << std::endl;
    ss << "Total Successful Executions: " << totalSuccesses << std::endl;
    ss << "Overall Success Rate: " << std::fixed << std::setprecision(1) << overallSuccessRate << "%" << std::endl;
    ss << "Total Execution Time: " << formatDuration(totalTime) << std::endl;
    ss << "Average Execution Time: " << formatDuration(totalTime / totalExecutions) << std::endl;
    
    return ss.str();
}

void NLMCLIStatus::reset() {
    commandStats.clear();
    startTime = std::chrono::system_clock::now();
    responsive = true;
}

long long NLMCLIStatus::getUptime() const {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
}

bool NLMCLIStatus::isResponsive() const {
    // Check if any command was executed recently
    auto now = std::chrono::system_clock::now();
    auto maxIdleTime = std::chrono::minutes(5); // Consider unresponsive after 5 minutes
    
    bool hasRecentActivity = false;
    for (const auto& pair : commandStats) {
        const CommandInfo& info = pair.second;
        auto idleTime = now - info.lastExecution;
        if (idleTime < maxIdleTime) {
            hasRecentActivity = true;
            break;
        }
    }
    
    return hasRecentActivity && responsive;
}

std::string NLMCLIStatus::formatDuration(long long milliseconds) const {
    std::stringstream ss;
    
    long long hours = milliseconds / (1000 * 60 * 60);
    long long minutes = (milliseconds % (1000 * 60 * 60)) / (1000 * 60);
    long long seconds = (milliseconds % (1000 * 60)) / 1000;
    long long ms = milliseconds % 1000;
    
    if (hours > 0) {
        ss << hours << "h ";
    }
    if (minutes > 0 || hours > 0) {
        ss << minutes << "m ";
    }
    if (seconds > 0 || minutes > 0 || hours > 0) {
        ss << seconds << "s ";
    }
    ss << ms << "ms";
    
    return ss.str();
}

} // namespace nlm
