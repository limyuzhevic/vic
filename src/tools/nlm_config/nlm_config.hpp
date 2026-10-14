// nlm_config tool implementation
// Configuration management tool for NLM

#pragma once

#include "cli/CLI.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class NLMConfigTool : public Tool {
public:
    NLMConfigTool();
    ~NLMConfigTool() override = default;
    
    std::string getName() const override { return "nlm_config"; }
    std::string getDescription() const override;
    std::string getVersion() const override;
    std::string getUsage() const override;
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override;
    bool execute(const std::vector<std::string>& args) override;
    void shutdown() override;
    
    bool isInteractive() const override { return true; }
    void setInteractive(bool interactive) override {}
    
    void setupArgumentParser();
    
private:
    std::shared_ptr<ArgumentParser> parser_;
    std::shared_ptr<nlm::Config> config_;
    std::unique_ptr<Logger> logger_;
    
    // Command implementations
    bool executeGetCommand(const std::vector<std::string>& args);
    bool executeSetCommand(const std::vector<std::string>& args);
    bool executeLoadCommand(const std::vector<std::string>& args);
    bool executeSaveCommand(const std::vector<std::string>& args);
    bool executeListCommand(const std::vector<std::string>& args);
    bool executeValidateCommand(const std::vector<std::string>& args);
    bool executeInitCommand(const std::vector<std::string>& args);
    
    static void printHelp() { std::cout << "Usage: nlm_config <command> [options]" << std::endl; }
};

} // namespace nlm

} // namespace nlm