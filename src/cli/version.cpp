#pragma once

#include "CLI.hpp"
#include <iostream>

namespace nlm {
namespace cli {

class VersionSubcommand : public Subcommand {
public:
    std::string getName() const override { return "version"; }
    std::string getDescription() const override { return "Show version information and exit"; }
    
    void addOptions() override {
        // No options for version command
    }
    
    CLIResult execute(const std::shared_ptr<Config>& config) override {
        std::cout << "NLM (熙然)" << std::endl;
        std::cout << "Neural Learning Machine" << std::endl;
        std::cout << "Version: 0.1.0 (Phase 6: Final Integration)" << std::endl;
        std::cout << std::endl;
        std::cout << "An experimental artificial developmental brain." << std::endl;
        std::cout << std::endl;
        std::cout << "Features:" << std::endl;
        std::cout << "  - Working memory with persistent activity" << std::endl;
        std::cout << "  - Episodic memory with experience encoding" << std::endl;
        std::cout << "  - Associative memory with Hebbian pattern associations" << std::endl;
        std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
        std::cout << "  - Prediction system with temporal sequence learning" << std::endl;
        std::cout << "  - Cognitive mechanisms (attention, planning, self-model)" << std::endl;
        std::cout << "  - Developmental stages and structural plasticity" << std::endl;
        std::cout << "  - Checkpoint save/load and replay" << std::endl;
        std::cout << "  - Complete Phase 6 integration" << std::endl;
        
        return CLIResult(true, "Version information displayed");
    }
    
    void showHelp() const override {
        std::cout << "Usage: " << getName() << std::endl;
        std::cout << getDescription() << std::endl;
        std::cout << std::endl;
        std::cout << "Shows version information and exits." << std::endl;
    }
};

} // namespace cli
} // namespace nlm
