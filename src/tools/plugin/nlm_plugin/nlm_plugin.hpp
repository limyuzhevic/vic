#pragma once

#include "cli/CLI.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class NLMPluginTool : public Tool {
public:
    NLMPluginTool();
    ~NLMPluginTool() override = default;
    
    std::string getName() const override { return "nlm_plugin"; }
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
    
    // Plugin implementations
    bool executeDiscoverCommand(const std::vector<std::string>& args);
    bool executeLoadCommand(const std::vector<std::string>& args);
    bool executeUnloadCommand(const std::vector<std::string>& args);
    bool executeInfoCommand(const std::vector<std::string>& args);
};

} // namespace nlm