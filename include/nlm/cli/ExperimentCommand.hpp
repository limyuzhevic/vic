#ifndef NLM_CLI_EXPERIMENT_COMMAND_HPP
#define NLM_CLI_EXPERIMENT_COMMAND_HPP

#include "CLI.hpp"
#include "../experiments/Experiment.hpp"
#include "../experiments/Phase3Experiment.hpp"
#include "../experiments/Phase4Experiment.hpp"
#include "../experiments/Phase5Experiment.hpp"
#include "../experiments/Phase6IntegratedExperiment.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <iostream>

namespace nlm {
namespace cli {

class ExperimentCommand : public Command {
public:
    ExperimentCommand() = default;
    ~ExperimentCommand() = default;
    
    std::string getName() const override { return "experiment"; }
    std::string getDescription() const override { 
        return "Experiment management and execution"; 
    }
    std::string getUsage() const override { 
        return "experiment <COMMAND> [OPTIONS]"; 
    }
    bool isHidden() const override { return false; }
    
    Result execute(const std::vector<std::string>& args) override;
    Result executeWithConfig(const std::vector<std::string>& args, 
                           const std::string& configFile) override;
    
private:
    enum class ExperimentSubcommand {
        Run,
        List,
        Save,
        Load,
        Compare,
        Analyze
    };
    
    ExperimentSubcommand parseSubcommand(const std::string& name) const;
    
    Result executeRun(const std::vector<std::string>& args, 
                      const std::string& configFile) const;
    Result executeList(const std::vector<std::string>& args, 
                      const std::string& configFile) const;
    Result executeSave(const std::vector<std::string>& args, 
                      const std::string& configFile) const;
    Result executeLoad(const std::vector<std::string>& args, 
                      const std::string& configFile) const;
    Result executeCompare(const std::vector<std::string>& args, 
                         const std::string& configFile) const;
    Result executeAnalyze(const std::vector<std::string>& args, 
                         const std::string& configFile) const;
    
    Result createExperimentFromConfig(const std::string& experimentName,
                                      const std::string& configFile,
                                      const std::map<std::string, std::string>& options) const;
    Result runExperiment(const std::string& experimentName,
                        const std::string& configFile,
                        const std::map<std::string, std::string>& options) const;
    Result saveExperimentResult(const std::string& experimentName,
                               const std::string& outputFile,
                               const nlm::ExperimentResult& result) const;
    Result loadExperimentResult(const std::string& filepath,
                               nlm::ExperimentResult& result) const;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_EXPERIMENT_COMMAND_HPP