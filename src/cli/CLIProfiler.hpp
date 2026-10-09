#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <chrono>

namespace nlm {

class CLIProfiler {
public:
    CLIProfiler();
    ~CLIProfiler();

    // Parse command line arguments
    bool parseCommandLine(int argc, char** argv);

    // Handle specific commands
    bool handleGenerateCommand(int argc, char** argv);
    bool handleCompareCommand(int argc, char** argv);
    bool handleAnalyzeCommand(int argc, char** argv);

    // Print help messages
    void printHelp();
    void printGenerateHelp();
    void printCompareHelp();
    void printAnalyzeHelp();

    // Profiling functions
    bool generateProfile(const std::string& configFile, const std::string& profileName,
                        int steps, bool enableMultithreading, bool enableSIMD,
                        bool enableEventDriven, bool enableSparseConnectivity);
    bool compareProfiles(const std::string& profile1, const std::string& profile2);
    bool analyzeProfile(const std::string& profileFile);

    // Profile management
    void saveProfile(const std::string& profileName, const std::string& filepath);
    bool loadProfile(const std::string& filepath);
    std::vector<std::string> listProfiles() const;

private:
    std::map<std::string, std::vector<double>> profileData_;
    std::string currentProfile_;
};

} // namespace nlm