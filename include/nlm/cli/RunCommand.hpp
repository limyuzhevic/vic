#ifndef NLM_CLI_RUN_COMMAND_HPP
#define NLM_CLI_RUN_COMMAND_HPP

#include "CLI.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {
namespace cli {

class RunCommand : public Command {
public:
    RunCommand() = default;
    ~RunCommand() = default;
    
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { 
        return "Run simulations and experiments"; 
    }
    std::string getUsage() const override { 
        return "run <EXPERIMENT> [OPTIONS]"; 
    }
    bool isHidden() const override { return false; }
    
    Result execute(const std::vector<std::string>& args) override;
    Result executeWithConfig(const std::vector<std::string>& args, 
                           const std::string& configFile) override;
    
private:
    enum class ExperimentType {
        Phase6,
        Phase3,
        Phase4,
        Phase5,
        Custom
    };
    
    ExperimentType parseExperimentType(const std::string& name) const;
    std::string experimentTypeToString(ExperimentType type) const;
    
    Result runPhase6Experiment(const std::vector<std::string>& args, 
                               const std::string& configFile) const;
    Result runPhase3Experiment(const std::vector<std::string>& args, 
                               const std::string& configFile) const;
    Result runPhase4Experiment(const std::vector<std::string>& args, 
                               const std::string& configFile) const;
    Result runPhase5Experiment(const std::vector<std::string>& args, 
                               const std::string& configFile) const;
    
    Result parseCommonOptions(const std::vector<std::string>& args, 
                              std::map<std::string, std::string>& options) const;
    Result applyOptionsToConfig(nlm::Config& config, 
                               const std::map<std::string, std::string>& options) const;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_RUN_COMMAND_HPP