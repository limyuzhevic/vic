// Version Command - Display version information

#pragma once

#include "CommandBase.hpp"
#include "CLIParser.hpp"
#include <iostream>
#include <string>

class VersionCommand : public CommandBase {
public:
    VersionCommand() : CommandBase("version", "Show version information") {}
    
    int execute(std::shared_ptr<Config> config, const CLIParser& parser) override {
        std::cout << "NLM — 熙然" << std::endl;
        std::cout << "Neural Learning Machine" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << std::endl;
        
        // Get version from config
        std::string version = config->getOr<std::string>("nlm.version", "2.0.0");
        std::string author = config->getOr<std::string>("nlm.author", "NLM Development Team");
        std::string license = config->getOr<std::string>("nlm.license", "MIT");
        
        std::cout << "Version:    " << version << std::endl;
        std::cout << "Author:     " << author << std::endl;
        std::cout << "License:    " << license << std::endl;
        std::cout << "Build Date: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << std::endl;
        
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
        std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
        std::cout << "  - Event-driven spike propagation with delays" << std::endl;
        std::cout << "  - STDP and Hebbian plasticity" << std::endl;
        std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "NLM is an experimental artificial developmental brain system" << std::endl;
        std::cout << "capable of changing its own synaptic connections through experience." << std::endl;
        
        return 0;
    }
};
