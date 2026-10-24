#ifndef NLM_CLI_CONFIG_COMMAND_HPP
#define NLM_CLI_CONFIG_COMMAND_HPP

#include "CLI.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {
namespace cli {

class ConfigCommand : public Command {
public:
    ConfigCommand() = default;
    ~ConfigCommand() = default;
    
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { 
        return "Configuration file management and validation"; 
    }
    std::string getUsage() const override { 
        return "config <COMMAND> [OPTIONS]"; 
    }
    bool isHidden() const override { return false; }
    
    Result execute(const std::vector<std::string>& args) override;
    Result executeWithConfig(const std::vector<std::string>& args, 
                           const std::string& configFile) override;
    
private:
    enum class ConfigSubcommand {
        Show,
        Load,
        Save,
        Validate,
        Create,
        Profile,
        Templates
    };
    
    ConfigSubcommand parseSubcommand(const std::string& name) const;
    
    Result executeShow(const std::vector<std::string>& args, 
                      const std::string& configFile) const;
    Result executeLoad(const std::vector<std::string>& args, 
                      const std::string& configFile) const;
    Result executeSave(const std::vector<std::string>& args, 
                      const std::string& configFile) const;
    Result executeValidate(const std::vector<std::string>& args, 
                          const std::string& configFile) const;
    Result executeCreate(const std::vector<std::string>& args, 
                        const std::string& configFile) const;
    Result executeProfile(const std::vector<std::string>& args, 
                         const std::string& configFile) const;
    Result executeTemplates(const std::vector<std::string>& args, 
                           const std::string& configFile) const;
    
    Result loadConfigFile(const std::string& filepath, 
                         nlm::Config& config) const;
    Result saveConfigFile(const std::string& filepath, 
                         const nlm::Config& config) const;
    Result validateConfig(const nlm::Config& config) const;
    Result createDefaultConfig(const std::string& filepath, 
                              const std::string& profile) const;
    std::string formatConfigSummary(const nlm::Config& config) const;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_CONFIG_COMMAND_HPP