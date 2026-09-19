// NLM CLI Experiment Interface
// Experiment management CLI interface

#ifndef EXPERIMENT_CLI_HPP
#define EXPERIMENT_CLI_HPP

#include <string>
#include <memory>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "experiments/ExperimentRunner.hpp"

namespace nlm {
namespace cli {

class ExperimentCLI {
public:
    ExperimentCLI();
    ~ExperimentCLI() = default;
    
    int run(int argc, char** argv);
    
private:
    void initializeLogger();
    void printUsage() const;
    
    int handleRunCommand(int argc, char** argv);
    int handleListCommand(int argc, char** argv);
    int handleInfoCommand(int argc, char** argv);
    int handleCompareCommand(int argc, char** argv);
    int handleTrackCommand(int argc, char** argv);
    int handleResultsCommand(int argc, char** argv);
    
    int runPhase3Experiment(std::shared_ptr<Brain> brain, SimulationStep steps, 
                           const std::string& outputFile);
    int runPhase4Experiment(std::shared_ptr<Brain> brain, SimulationStep steps,
                           const std::string& outputFile);
    int runPhase5Experiment(std::shared_ptr<Brain> brain, SimulationStep steps,
                           const std::string& outputFile);
    int runPhase6Experiment(std::shared_ptr<Brain> brain, SimulationStep steps,
                           const std::string& outputFile);
    int runContinuousExperiment(std::shared_ptr<Brain> brain, const std::string& experimentType,
                                SimulationStep steps, const std::string& outputFile,
                                double samplingInterval);
};

} // namespace cli
} // namespace nlm

#endif // EXPERIMENT_CLI_HPP
