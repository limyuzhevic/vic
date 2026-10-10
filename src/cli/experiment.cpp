#pragma once

#include "CLI.hpp"
#include <iostream>

namespace nlm {
namespace cli {

class ExperimentSubcommand : public Subcommand {
public:
    std::string getName() const override { return "experiment"; }
    std::string getDescription() const override { return "Run scientific experiments"; }
    
    void addOptions() override {
        // Options are parsed in execute()
    }
    
    CLIResult execute(const std::shared_ptr<Config>& config) override {
        std::cout << "Experiment system not yet implemented." << std::endl;
        std::cout << "Use 'benchmark' for basic performance testing." << std::endl;
        return CLIResult(true, "Experiment system coming soon");
    }
    
    void showHelp() const override {
        std::cout << "Usage: experiment [OPTIONS]" << std::endl;
        std::cout << "Run scientific experiments." << std::endl;
        std::cout << std::endl;
        std::cout << "Note: Experiment system is currently under development." << std::endl;
        std::cout << "Use 'benchmark' for basic performance testing." << std::endl;
    }
};

} // namespace cli
} // namespace nlm
