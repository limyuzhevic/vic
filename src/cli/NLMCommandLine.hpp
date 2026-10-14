// NLM Command Line Interface Header
// Enhanced command-line interface for NLM with comprehensive options

#pragma once

#include <string>
#include <memory>
#include <map>

namespace nlm {

// Forward declarations
class Config;
class Brain;

class NLMCommandLine {
public:
    NLMCommandLine();
    ~NLMCommandLine();
    
    // Disable copying, enable moving
    NLMCommandLine(const NLMCommandLine&) = delete;
    NLMCommandLine& operator=(const NLMCommandLine&) = delete;
    NLMCommandLine(NLMCommandLine&&) noexcept;
    NLMCommandLine& operator=(NLMCommandLine&&) noexcept;
    
    int parseArgs(int argc, char** argv);
    int runExperiment();
    
    const std::string& getExperimentName() const { return experimentName_; }
    const std::string& getOutputFile() const { return outputFile_; }
    bool isVerbose() const { return verbose_; }
    bool isQuiet() const { return quiet_; }
    
private:
    void printHelp() const;
    void printVersion() const;
    bool loadConfigFile(const std::string& filename);
    void applyPreset(const std::string& preset);
    void setupLogging(const std::string& level, bool useColors, bool logToFile);
    void printExperimentHeader(const std::string& title) const;
    
    // Experiment execution methods
    int runPhase2Demo(std::shared_ptr<Brain> brain);
    int runPhase6Demo();
    int runCustomExperiment(std::shared_ptr<Brain> brain);
    
    // Configuration objects
    std::shared_ptr<Config> config_;
    bool configLoaded_;
    
    // Command line options
    std::string configFile_;
    std::string outputFile_;
    std::string experimentMode_;
    std::string presetConfig_;
    std::string logLevel_;
    bool useColors_;
    bool logToFile_;
    bool verbose_;
    bool quiet_;
    int maxSteps_;
    double timestep_;
    int neuronCount_;
    int regionCount_;
    std::string experimentName_;
    std::map<std::string, std::string> customArgs_;
};

} // namespace nlm
