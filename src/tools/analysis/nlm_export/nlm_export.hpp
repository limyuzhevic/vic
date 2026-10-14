#pragma once

#include "cli/CLI.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class NLMExportTool : public Tool {
public:
    NLMExportTool();
    ~NLMExportTool() override = default;
    
    std::string getName() const override { return "nlm_export"; }
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
    
    // Export implementations
    bool executeExportCommand(const std::vector<std::string>& args);
    bool executeImportCommand(const std::vector<std::string>& args);
};

} // namespace nlm