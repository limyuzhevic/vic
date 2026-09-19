// NLM CLI Monitor Interface
// Real-time monitoring CLI interface

#ifndef MONITOR_CLI_HPP
#define MONITOR_CLI_HPP

#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {
namespace cli {

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class MonitorCLI {
public:
    MonitorCLI();
    ~MonitorCLI() = default;
    
    int run(int argc, char** argv);
    
private:
    void initializeLogger();
    void printUsage() const;
    void printMonitorHelp() const;
    
    int handleLiveMonitorCommand(int argc, char** argv);
    int runLiveMonitoring(const std::string& configFile, double interval, 
                         size_t duration, const std::string& outputFile,
                         bool plotData);
    int handleReplayMonitorCommand(int argc, char** argv);
    int handleMetricsMonitorCommand(int argc, char** argv);
    int handleLogMonitorCommand(int argc, char** argv);
    int handleExportMonitorCommand(int argc, char** argv);
    
    LogLevel stringToLogLevel(const std::string& levelStr);
    std::string logLevelToString(LogLevel level);
};

} // namespace cli
} // namespace nlm

#endif // MONITOR_CLI_HPP
