#ifndef NLMCLISTATUS_HPP
#define NLMCLISTATUS_HPP

#include <string>
#include <map>
#include <vector>
#include <chrono>

namespace nlm {

class NLMCLIStatus {
public:
    NLMCLIStatus();
    ~NLMCLIStatus();
    
    /**
     * Update status with command execution
     * 
     * @param command Command name
     * @param success Whether command executed successfully
     * @param executionTime Execution time in milliseconds
     */
    void updateStatus(const std::string& command, bool success, long long executionTime);
    
    /**
     * Get status report
     * 
     * @return Status report string
     */
    std::string getStatusReport() const;
    
    /**
     * Get command statistics
     * 
     * @return Map of command statistics
     */
    std::map<std::string, std::map<std::string, std::string>> getCommandStats() const;
    
    /**
     * Get overall statistics
     * 
     * @return Overall statistics string
     */
    std::string getOverallStats() const;
    
    /**
     * Reset status
     */
    void reset();
    
    /**
     * Get current uptime
     * 
     * @return Uptime in milliseconds
     */
    long long getUptime() const;
    
    /**
     * Check if CLI is responsive
     * 
     * @return true if responsive, false otherwise
     */
    bool isResponsive() const;

private:
    struct CommandInfo {
        std::string command;
        int executionCount;
        int successCount;
        long long totalExecutionTime;
        std::chrono::time_point<std::chrono::system_clock> firstExecution;
        std::chrono::time_point<std::chrono::system_clock> lastExecution;
    };
    
    std::map<std::string, CommandInfo> commandStats;
    std::chrono::time_point<std::chrono::system_clock> startTime;
    bool responsive;
    
    std::string formatDuration(long long milliseconds) const;
};

} // namespace nlm

#endif // NLMCLISTATUS_HPP
