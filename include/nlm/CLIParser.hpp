#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Types/Types.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>

namespace nlm {

class CLIParser {
public:
    struct Options {
        // Configuration
        std::string configFile = "configs/default.cfg";
        
        // Test modes
        bool runNeuronTests = false;
        bool runSynapseTests = false;
        bool runPlasticityTests = false;
        bool runBrainTests = false;
        bool runAllTests = false;
        
        // Simulation control
        double simulationRunTime = 0.0;
        double timestep = 0.001;
        int steps = 0;
        
        // Output options
        bool verbose = false;
        bool quiet = false;
        bool jsonOutput = false;
        std::string outputFile;
        
        // Random seed
        uint64_t randomSeed = 42;
        bool showSeed = false;
        
        // Performance profiling
        bool profile = false;
        bool measureMemory = false;
        bool countSpikes = false;
        bool countNeurons = false;
        bool countSynapses = false;
        
        // Checkpoint
        std::string checkpointSave;
        std::string checkpointLoad;
        
        // Visualization
        bool enableVisualization = false;
        
        // Logging
        LogLevel logLevel = LogLevel::Info;
        std::vector<std::string> enableCategories;
        std::vector<std::string> disableCategories;
        bool progressIndicators = true;
        
        // Output format
        bool humanReadable = true;
        bool csvOutput = false;
        bool jsonOutputFormat = false;
    };

    struct PerformanceStats {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        uint64_t spikesGenerated = 0;
        uint64_t neuronsCreated = 0;
        uint64_t synapsesCreated = 0;
        size_t peakMemoryUsage = 0;
        double simulationTime = 0.0;
        
        double getElapsedSeconds() const {
            return std::chrono::duration<double>(
                endTime - startTime
            ).count();
        }
    };

    CLIParser(int argc, char** argv);
    ~CLIParser() = default;
    
    bool parse(int argc, char** argv);
    const Options& getOptions() const { return options_; }
    bool isValid() const { return valid_; }
    const std::string& getError() const { return error_; }
    
    // Help and usage
    void printHelp(const std::string& programName) const;
    void printVersion() const;
    
    // Processing
    PerformanceStats runSimulation(std::shared_ptr<Brain> brain);
    bool runTests(std::shared_ptr<Brain> brain);
    void saveCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath);
    bool loadCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    // Output formatting
    void outputJSON(const PerformanceStats& stats, std::ostream& out) const;
    void outputCSV(const PerformanceStats& stats, std::ostream& out) const;
    void outputHumanReadable(const PerformanceStats& stats, std::ostream& out) const;
    
    // Performance monitoring
    void startProfiling();
    void stopProfiling();
    void incrementSpikeCount();
    void incrementNeuronCount();
    void incrementSynapseCount();
    void updateMemoryUsage(size_t memory);

private:
    Options options_;
    bool valid_ = true;
    std::string error_;
    PerformanceStats perfStats_;
    
    // Helper methods
    bool parseArguments(int argc, char** argv);
    bool parseShortOption(char c, const std::string& arg, size_t& i, int argc, char** argv);
    bool parseLongOption(const std::string& arg, size_t& i, int argc, char** argv);
    void setLogLevelFromString(const std::string& level);
    bool validateOptions() const;
    std::string trim(const std::string& str) const;
    
    // Subcommand handling
    bool handleSubcommand(const std::string& cmd, size_t& i, int argc, char** argv);
    bool handleTestSubcommand(const std::string& testName, size_t& i, int argc, char** argv);
    bool handleSimulationSubcommand(size_t& i, int argc, char** argv);
    
    // Output helpers
    void initializeOutputStream(std::ofstream& outFile) const;
    void writeOutput(const PerformanceStats& stats) const;
};

} // namespace nlm
