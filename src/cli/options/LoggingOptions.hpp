#pragma once

#include <string>

namespace nlm {
namespace cli {
namespace options {

class LoggingOptions {
public:
    LoggingOptions();
    ~LoggingOptions() = default;
    
    // Parse options from argument list
    void parse(const std::vector<std::string>& args);
    
    // Get parsed values
    std::string getLogLevel() const { return logLevel; }
    std::string getLogFile() const { return logFile; }
    bool getVerbose() const { return verbose; }
    bool getDebug() const { return debug; }
    bool getTrace() const { return trace; }
    
    // Set values
    void setLogLevel(const std::string& level) { logLevel = level; }
    void setLogFile(const std::string& file) { logFile = file; }
    void setVerbose(bool verbose) { this->verbose = verbose; }
    void setDebug(bool debug) { this->debug = debug; }
    void setTrace(bool trace) { this->trace = trace; }
    
private:
    std::string logLevel;
    std::string logFile;
    bool verbose;
    bool debug;
    bool trace;
};

} // namespace options
} // namespace cli
} // namespace nlm
