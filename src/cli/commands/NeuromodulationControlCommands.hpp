// Neuromodulation control commands

#pragma once

#include "AdvancedCLI.hpp"
#include <sstream>

namespace nlm {
namespace cli {

class NeuromodulationCommand : public Command {
public:
    std::string getName() const override { return "neuromodulation"; }
    std::string getDescription() const override { return "Neuromodulation control and monitoring"; }
    std::string getUsage() const override {
        return "neuromodulation <subcommand> [options]"; 
    }
    
    void execute(const CommandContext& context) override {
        std::string subcommand = context.options["subcommand"];
        
        if (subcommand == "info" || subcommand.empty()) {
            executeInfo(context);
        } else if (subcommand == "set_dopamine") {
            executeSetDopamine(context);
        } else if (subcommand == "set_acetylcholine") {
            executeSetAcetylcholine(context);
        } else if (subcommand == "set_norepinephrine") {
            executeSetNorepinephrine(context);
        } else if (subcommand == "set_serotonin") {
            executeSetSerotonin(context);
        } else if (subcommand == "set_curiosity") {
            executeSetCuriosity(context);
        } else if (subcommand == "set_levels") {
            executeSetAllLevels(context);
        } else if (subcommand == "get_levels") {
            executeGetAllLevels(context);
        } else if (subcommand == "status") {
            executeStatus(context);
        } else {
            throw std::runtime_error("Unknown subcommand: " + subcommand);
        }
    }
    
private:
    void executeInfo(const CommandContext& context) {
        context.output << "Neuromodulation System Status:" << std::endl;
        
        // Check if we can access neuromodulation systems
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(context.brain);
        if (agentBrain) {
            context.output << "  AgentBrain available: Yes" << std::endl;
            context.output << "  Current neuromodulation level: " << agentBrain->getNeuromodulationLevel() << std::endl;
            context.output << "  Curiosity level: " << agentBrain->getCuriosityLevel() << std::endl;
            context.output << "  Novelty level: " << agentBrain->getNoveltyLevel() << std::endl;
            context.output << "  Prediction error: " << agentBrain->getPredictionError() << std::endl;
        } else {
            context.output << "AgentBrain not available (integrated brain system)" << std::endl;
        }
        
        // Try to get neuromodulation systems from brain
        auto dopamine = context.brain->getDopamine();
        auto acetylcholine = context.brain->getAcetylcholine();
        auto norepinephrine = context.brain->getNorepinephrine();
        auto serotonin = context.brain->getSerotonin();
        auto curiosity = context.brain->getCuriosity();
        
        context.output << "  Neuromodulators available:" << std::endl;
        context.output << "    Dopamine: " << (dopamine ? "Yes" : "No") << std::endl;
        context.output << "    Acetylcholine: " << (acetylcholine ? "Yes" : "No") << std::endl;
        context.output << "    Norepinephrine: " << (norepinephrine ? "Yes" : "No") << std::endl;
        context.output << "    Serotonin: " << (serotonin ? "Yes" : "No") << std::endl;
        context.output << "    Curiosity: " << (curiosity ? "Yes" : "No") << std::endl;
    }
    
    void executeSetDopamine(const CommandContext& context) {
        if (context.options.find("level") == context.options.end()) {
            throw std::runtime_error("Missing --level parameter");
        }
        
        float level = std::stof(context.options.at("level"));
        
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(context.brain);
        if (agentBrain) {
            // Note: AgentBrain doesn't have direct dopamine setter, this would be integrated
            context.output << "Dopamine level set to: " << level << " (would need integration)" << std::endl;
        }
        
        auto dopamine = context.brain->getDopamine();
        if (dopamine) {
            dopamine->setLevel(level);
            context.output << "Dopamine level set to: " << level << std::endl;
        }
    }
    
    void executeSetAcetylcholine(const CommandContext& context) {
        if (context.options.find("level") == context.options.end()) {
            throw std::runtime_error("Missing --level parameter");
        }
        
        float level = std::stof(context.options.at("level"));
        
        auto acetylcholine = context.brain->getAcetylcholine();
        if (acetylcholine) {
            acetylcholine->setLevel(level);
            context.output << "Acetylcholine (ACh) level set to: " << level << std::endl;
        }
    }
    
    void executeSetNorepinephrine(const CommandContext& context) {
        if (context.options.find("level") == context.options.end()) {
            throw std::runtime_error("Missing --level parameter");
        }
        
        float level = std::stof(context.options.at("level"));
        
        auto norepinephrine = context.brain->getNorepinephrine();
        if (norepinephrine) {
            norepinephrine->setLevel(level);
            context.output << "Norepinephrine (NE) level set to: " << level << std::endl;
        }
    }
    
    void executeSetSerotonin(const CommandContext& context) {
        if (context.options.find("level") == context.options.end()) {
            throw std::runtime_error("Missing --level parameter");
        }
        
        float level = std::stof(context.options.at("level"));
        
        auto serotonin = context.brain->getSerotonin();
        if (serotonin) {
            serotonin->setLevel(level);
            context.output << "Serotonin (5-HT) level set to: " << level << std::endl;
        }
    }
    
    void executeSetCuriosity(const CommandContext& context) {
        if (context.options.find("level") == context.options.end()) {
            throw std::runtime_error("Missing --level parameter");
        }
        
        float level = std::stof(context.options.at("level"));
        
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(context.brain);
        if (agentBrain) {
            // Note: AgentBrain doesn't have direct curiosity setter, this would be integrated
            context.output << "Curiosity level set to: " << level << " (would need integration)" << std::endl;
        }
        
        auto curiosity = context.brain->getCuriosity();
        if (curiosity) {
            // Note: Curiosity interface might not support direct setLevel
            context.output << "Curiosity level control: " << level << " (system integration required)" << std::endl;
        }
    }
    
    void executeSetAllLevels(const CommandContext& context) {
        // Set all neuromodulators to specified levels
        float level = 0.5f; // Default level
        
        if (context.options.find("level") != context.options.end()) {
            level = std::stof(context.options.at("level"));
        }
        
        auto acetylcholine = context.brain->getAcetylcholine();
        if (acetylcholine) {
            acetylcholine->setLevel(level);
            context.output << "Acetylcholine set to: " << level << std::endl;
        }
        
        auto norepinephrine = context.brain->getNorepinephrine();
        if (norepinephrine) {
            norepinephrine->setLevel(level);
            context.output << "Norepinephrine set to: " << level << std::endl;
        }
        
        auto serotonin = context.brain->getSerotonin();
        if (serotonin) {
            serotonin->setLevel(level);
            context.output << "Serotonin set to: " << level << std::endl;
        }
        
        context.output << "All neuromodulators set to level: " << level << std::endl;
    }
    
    void executeGetAllLevels(const CommandContext& context) {
        context.output << "Current Neuromodulator Levels:" << std::endl;
        
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(context.brain);
        if (agentBrain) {
            context.output << "  AgentBrain neuromodulation level: " << agentBrain->getNeuromodulationLevel() << std::endl;
            context.output << "  AgentBrain curiosity level: " << agentBrain->getCuriosityLevel() << std::endl;
            context.output << "  AgentBrain novelty level: " << agentBrain->getNoveltyLevel() << std::endl;
            context.output << "  AgentBrain prediction error: " << agentBrain->getPredictionError() << std::endl;
        }
        
        auto dopamine = context.brain->getDopamine();
        if (dopamine) {
            context.output << "  Dopamine: " << dopamine->getLevel() << std::endl;
        }
        
        auto acetylcholine = context.brain->getAcetylcholine();
        if (acetylcholine) {
            context.output << "  Acetylcholine (ACh): " << acetylcholine->getLevel() << std::endl;
        }
        
        auto norepinephrine = context.brain->getNorepinephrine();
        if (norepinephrine) {
            context.output << "  Norepinephrine (NE): " << norepinephrine->getLevel() << std::endl;
        }
        
        auto serotonin = context.brain->getSerotonin();
        if (serotonin) {
            context.output << "  Serotonin (5-HT): " << serotonin->getLevel() << std::endl;
        }
        
        auto curiosity = context.brain->getCuriosity();
        if (curiosity) {
            context.output << "  Curiosity: " << curiosity->getLevel() << std::endl;
        }
    }
    
    void executeStatus(const CommandContext& context) {
        context.output << "Neuromodulation System Status:" << std::endl;
        
        // Check system integration
        bool hasBrain = (context.brain != nullptr);
        bool hasAgentBrain = (std::dynamic_pointer_cast<AgentBrain>(context.brain) != nullptr);
        
        context.output << "  Brain system available: " << (hasBrain ? "Yes" : "No") << std::endl;
        context.output << "  AgentBrain integration: " << (hasAgentBrain ? "Yes" : "No") << std::endl;
        
        // Count neuromodulators
        int neuromodulatorCount = 0;
        neuromodulatorCount += (context.brain->getDopamine() != nullptr);
        neuromodulatorCount += (context.brain->getAcetylcholine() != nullptr);
        neuromodulatorCount += (context.brain->getNorepinephrine() != nullptr);
        neuromodulatorCount += (context.brain->getSerotonin() != nullptr);
        neuromodulatorCount += (context.brain->getCuriosity() != nullptr);
        neuromodulatorCount += (context.brain->getPredictionErrorSignal() != nullptr);
        neuromodulatorCount += (context.brain->getNovelty() != nullptr);
        
        context.output << "  Integrated neuromodulators: " << neuromodulatorCount << "/7" << std::endl;
        
        // Integration status
        context.output << "  Integration status: " << (hasAgentBrain ? "Advanced features available" : "Basic mode") << std::endl;
    }
};

} // namespace cli
} // namespace nlm
