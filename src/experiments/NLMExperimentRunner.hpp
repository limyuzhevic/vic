#include "src/experiments/ExperimentFramework.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>

namespace nlm {

// Unified experiment runner with command-line interface
class NLMExperimentRunner {
public:
    NLMExperimentRunner() = default;
    ~NLMExperimentRunner() = default;
    
    // Run experiments based on command line arguments
    bool run(int argc, char** argv);
    
    // Print help information
    void printHelp() const;
    
    // Print experiment list
    void printExperimentList() const;
    
    // Get experiment result summary
    std::string getExperimentSummary(const ExperimentResult& result) const;
    
private:
    // Parse command line arguments
    struct CommandArgs {
        std::string experimentName;
        std::string configFile;
        std::string outputFile;
        bool runAll;
        bool verbose;
        bool help;
        int numTrials;
        int stepsPerEpisode;
        
        CommandArgs() : runAll(false), verbose(false), help(false), 
                       numTrials(5), stepsPerEpisode(200) {}
    };
    
    CommandArgs parseCommandLine(int argc, char** argv);
    
    // Load configuration
    std::shared_ptr<Config> loadConfig(const std::string& configFile) const;
    
    // Initialize brain with configuration
    std::shared_ptr<Brain> initializeBrain(std::shared_ptr<Config> config) const;
    
    // Run single experiment
    ExperimentResult runSingleExperiment(
        std::shared_ptr<Experiment> experiment,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Config> config) const;
    
    // Display experiment results
    void displayResults(const std::vector<ExperimentResult>& results) const;
    
    // Save results to file
    bool saveResults(const std::string& filepath, 
                    const std::vector<ExperimentResult>& results) const;
};

} // namespace nlm

#endif // NLM_EXPERIMENT_RUNNER_HPP
