#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <ctime>

namespace nlm {

class CLIDebugger {
public:
    CLIDebugger();
    ~CLIDebugger();

    // Parse command line arguments
    bool parseCommandLine(int argc, char** argv);

    // Handle specific commands
    bool handleMemoryCommand(int argc, char** argv);
    bool handleThreadsCommand(int argc, char** argv);
    bool handleValidationCommand(int argc, char** argv);
    bool handleBrainCommand(int argc, char** argv);

    // Print help messages
    void printHelp();
    void printMemoryHelp();
    void printThreadsHelp();
    void printValidationHelp();
    void printBrainHelp();

    // Debugging functions
    bool debugMemory(bool detailed, bool histogram, const std::string& outputFile);
    bool debugThreads(bool listThreads, bool status, const std::string& outputFile);
    bool validateBrain(const std::string& testType, const std::string& configFile,
                       bool fix, bool verbose);
    bool debugBrainStatus(const std::string& outputFile);
    bool debugNeurons(const std::string& outputFile);
    bool debugSynapses(const std::string& outputFile);
    bool debugRegions(const std::string& outputFile);

private:
    // Helper functions
    std::string getCurrentTime() const;
    std::string formatTime(time_t timestamp) const;
};

} // namespace nlm