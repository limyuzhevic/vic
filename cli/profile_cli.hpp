// NLM CLI Profile - Enhanced profiling commands header
// Provides detailed memory, CPU, and execution time profiling

#ifndef NLM_CLI_PROFILE_HPP
#define NLM_CLI_PROFILE_HPP

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <thread>
#include <numeric>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace cli {

class ProfileCLI {
public:
    ProfileCLI();
    
    int run(int argc, char** argv);
    
private:
    void initializeLogger();
    
    // Profile commands
    int handleMemoryProfileCommand(int argc, char** argv);
    int handleCPUProfileCommand(int argc, char** argv);
    int handleNetworkProfileCommand(int argc, char** argv);
    int handleGPUProfileCommand(int argc, char** argv);
    int handleMemoryLeakCommand(int argc, char** argv);
    int handleTimelineProfileCommand(int argc, char** argv);
    int handleProfileStartCommand(int argc, char** argv);
    int handleProfileStopCommand(int argc, char** argv);
    int handleProfileReportCommand(int argc, char** argv);
    int handleProfileExportCommand(int argc, char** argv);
    
    // Helper structures
    struct MemoryProfile {
        size_t totalAllocated;
        size_t peakAllocated;
        std::unordered_map<std::string, size_t> allocationsByType;
        std::vector<std::pair<std::string, size_t>> allocationHistory;
        std::chrono::system_clock::time_point startTime;
    };
    
    // Helper functions
    void collectMemoryStats(MemoryProfile& profile);
    void generateMemoryReport(const MemoryProfile& profile, const std::string& outputFile);
    void generateCPUProfileReport(const std::vector<double>& cpuHistory, const std::string& outputFile);
    void generateNetworkProfileReport(const std::vector<size_t>& memoryAccesses,
                                      const std::vector<size_t>& spikeCounts,
                                      const std::string& outputFile);
    void analyzeMemoryLeaks(const std::vector<size_t>& snapshots, const std::string& outputFile);
    void generateTimelineReport(const std::vector<std::chrono::system_clock::time_point>& timestamps,
                               const std::vector<size_t>& spikeCounts,
                               const std::vector<size_t>& neuronCounts,
                               const std::string& outputFile);
    
    void printUsage();
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_PROFILE_HPP
