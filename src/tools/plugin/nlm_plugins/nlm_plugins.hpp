#pragma once

#include "cli/CLI.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class NLMPluginsTool : public Tool {
public:
    NLMPluginsTool();
    ~NLMPluginsTool() override = default;
    
    std::string getName() const override { return "nlm_plugins"; }
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
    
    // Plugins implementations
    bool executeListCommand(const std::vector<std::string>& args);
    bool executeEnableCommand(const std::vector<std::string>& args);
    bool executeDisableCommand(const std::vector<std::string>& args);
};

} // namespace nlm