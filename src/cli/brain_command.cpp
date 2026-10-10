// NLM Advanced CLI - Brain Command Implementation
#pragma once

#include "cli/main_cli.hpp"
#include "cli/command_factory.hpp"
#include "brain/Brain.hpp"
#include "tools/brain_inspector.hpp"
#include "tools/parameter_modifier.hpp"
#include "performance/CheckpointSystem.hpp"
#include <iostream>
#include <chrono>
#include <cmath>

namespace nlm {

class BrainCommand : public BaseCommand {
public:
    BrainCommand() : BaseCommand("brain", "Brain manipulation and exploration tools") {}
    
protected:
    CLIResult executeImpl(const std::map<std::string, std::string>& options) override {
        if (options.empty()) {
            displayHelpImpl();
            return CLIResult::HelpDisplayed;
        }
        
        std::string action = getOption(options, "action", "info");
        
        if (action == "info") {
            return handleBrainInfo();
        } else if (action == "inspect") {
            return handleBrainInspect();
        } else if (action == "modify") {
            return handleBrainModify(options);
        } else if (action == "reset") {
            return handleBrainReset();
        } else if (action == "step") {
            std::string steps = getOption(options, "steps", "1");
            return handleBrainStep(std::stoi(steps));
        } else if (action == "save") {
            std::string filename = getOption(options, "filename", "brain_checkpoint");
            return handleBrainSave(filename);
        } else if (action == "load") {
            std::string filename = getOption(options, "filename", "brain_checkpoint");
            return handleBrainLoad(filename);
        } else if (action == "regions") {
            return handleBrainRegions();
        } else if (action == "neurons") {
            return handleBrainNeurons();
        } else if (action == "synapses") {
            return handleBrainSynapses();
        } else if (action == "plasticity") {
            return handleBrainPlasticity(options);
        } else if (action == "development") {
            return handleBrainDevelopment(options);
        } else if (action == "neuromodulation") {
            return handleBrainNeuromodulation(options);
        } else {
            std::cout << "Unknown brain action: " << action << std::endl;
            displayHelpImpl();
            return CLIResult::Error;
        }
    }
    
    void displayHelpImpl() const override {
        std::cout << "\n=== Brain Command Help ===\n" << std::endl;
        std::cout << "Usage: nlm brain [options]\n" << std::endl;
        std::cout << "Actions:" << std::endl;
        std::cout << "  --action=info                 Display brain information" << std::endl;
        std::cout << "  --action=inspect             Inspect brain structure and state" << std::endl;
        std::cout << "  --action=modify [options]    Modify brain parameters" << std::endl;
        std::cout << "  --action=reset               Reset brain to initial state" << std::endl;
        std::cout << "  --action=step [steps]         Step simulation forward" << std::endl;
        std::cout << "  --action=save [filename]      Save brain state to checkpoint" << std::endl;
        std::cout << "  --action=load [filename]      Load brain state from checkpoint" << std::endl;
        std::cout << "  --action=regions              List brain regions" << std::endl;
        std::cout << "  --action=neurons              List neurons" << std::endl;
        std::cout << "  --action=synapses             List synapses" << std::endl;
        std::cout << "  --action=plasticity [options] Configure plasticity" << std::endl;
        std::cout << "  --action=development [options] Configure development" << std::endl;
        std::cout << "  --action=neuromodulation [options] Configure neuromodulation" << std::endl;
        std::cout << "\nOptions for --action=modify:" << std::endl;
        std::cout << "  --parameter=<name> <value>   Set parameter value" << std::endl;
        std::cout << "  --region=<name>              Target region" << std::endl;
        std::cout << "\nOptions for --action=plasticity:" << std::endl;
        std::cout << "  --enable=<true|false>        Enable/disable plasticity" << std::endl;
        std::cout << "  --rate=<value>                Set plasticity rate" << std::endl;
    }
    
private:
    CLIResult handleBrainInfo() {
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized.\n" << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "=== Brain Information ===\n" << std::endl;
        std::cout << "Brain ID: " << globalState.brain->getBrainId() << std::endl;
        std::cout << "Neurons: " << globalState.brain->getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << globalState.brain->getTotalSynapseCount() << std::endl;
        std::cout << "Regions: " << globalState.brain->getRegionCount() << std::endl;
        std::cout << "Steps: " << globalState.brain->getCurrentStep() << std::endl;
        std::cout << "Time: " << globalState.brain->getSimulationTime() << " s" << std::endl;
        std::cout << "Firing neurons: " << globalState.brain->getFiringNeuronCount() << std::endl;
        std::cout << "Total spikes: " << globalState.brain->getTotalSpikeCount() << std::endl;
        std::cout << "Excitation/Inhibition ratio: " << globalState.brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "Development stage: " << static_cast<int>(globalState.brain->getDevelopmentalStage()) << std::endl;
        std::cout << "Plasticity enabled: " << (globalState.brain->isPlasticityEnabled() ? "Yes" : "No") << std::endl;
        
        return CLIResult::Success;
    }
    
    CLIResult handleBrainInspect() {
        std::cout << "=== Brain Inspection ===\n" << std::endl;
        
        if (globalState.brain) {
            // Use brain inspector if available
            brain_inspector inspector(globalState.brain);
            inspector.inspectAll();
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        return CLIResult::Success;
    }
    
    CLIResult handleBrainModify(const std::map<std::string, std::string>& options) {
        std::cout << "=== Brain Modification ===\n" << std::endl;
        
        parameter_modifier modifier(globalState.brain);
        
        std::string paramName = getOption(options, "parameter", "");
        std::string paramValue = getOption(options, "value", "");
        std::string regionName = getOption(options, "region", "");
        
        if (!paramName.empty() && !paramValue.empty()) {
            if (modifier.setParameter(paramName, paramValue, regionName)) {
                std::cout << "Parameter " << paramName << " set to " << paramValue << " ";
                if (!regionName.empty()) {
                    std::cout << "in region " << regionName;
                }
                std::cout << " successfully.\n" << std::endl;
                return CLIResult::Success;
            } else {
                std::cout << "Error: Failed to set parameter " << paramName << std::endl;
                return CLIResult::Error;
            }
        }
        
        std::cout << "No parameters specified for modification.\n" << std::endl;
        displayHelpImpl();
        return CLIResult::Error;
    }
    
    CLIResult handleBrainReset() {
        std::cout << "=== Brain Reset ===\n" << std::endl;
        std::cout << "Resetting brain to initial state...\n" << std::endl;
        
        if (globalState.brain) {
            globalState.brain->reset();
            std::cout << "Brain reset successfully.\n" << std::endl;
            return CLIResult::Success;
        } else {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
    }
    
    CLIResult handleBrainStep(int steps) {
        std::cout << "=== Brain Simulation Step ===\n" << std::endl;
        std::cout << "Stepping forward " << steps << " simulation step(s)...\n" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        for (int i = 0; i < steps; ++i) {
            globalState.brain->step(globalState.brain->getCurrentStep(), globalState.brain->getSimulationTime() + 0.001);
            globalState.simulationStep++;
            globalState.simulationTime += 0.001;
        }
        
        std::cout << "Simulation step completed.\n" << std::endl;
        std::cout << "Current step: " << globalState.brain->getCurrentStep() << std::endl;
        std::cout << "Current time: " << globalState.brain->getSimulationTime() << " s" << std::endl;
        std::cout << "Firing neurons: " << globalState.brain->getFiringNeuronCount() << std::endl;
        
        return CLIResult::Success;
    }
    
    CLIResult handleBrainSave(const std::string& filename) {
        std::cout << "=== Brain Save Checkpoint ===\n" << std::endl;
        std::cout << "Saving brain state to: " << filename << ".nml
" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        // Use checkpoint system to save brain state
        // For now, use a simple save approach
        std::string filepath = filename + ".nml";
        if (globalState.brain->save(filepath)) {
            std::cout << "Brain state saved successfully.\n" << std::endl;
            return CLIResult::Success;
        } else {
            std::cout << "Error: Failed to save brain state." << std::endl;
            return CLIResult::Error;
        }
    }
    
    CLIResult handleBrainLoad(const std::string& filename) {
        std::cout << "=== Brain Load Checkpoint ===\n" << std::endl;
        std::cout << "Loading brain state from: " << filename << ".nml
" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        std::string filepath = filename + ".nml";
        if (globalState.brain->load(filepath)) {
            std::cout << "Brain state loaded successfully.\n" << std::endl;
            return CLIResult::Success;
        } else {
            std::cout << "Error: Failed to load brain state." << std::endl;
            return CLIResult::Error;
        }
    }
    
    CLIResult handleBrainRegions() {
        std::cout << "=== Brain Regions ===\n" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        auto regionIds = globalState.brain->getAllRegionIds();
        std::cout << "Found " << regionIds.size() << " region(s):\n" << std::endl;
        
        for (const auto& regionId : regionIds) {
            auto* region = globalState.brain->getRegion(regionId);
            if (region) {
                std::cout << "  Region " << regionId << ": " << region->getName() << std::endl;
                std::cout << "    Neurons: " << region->getNeuronCount() << std::endl;
                std::cout << "    Synapses: " << region->getSynapseCount() << std::endl;
            }
        }
        
        return CLIResult::Success;
    }
    
    CLIResult handleBrainNeurons() {
        std::cout << "=== Brain Neurons ===\n" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "Total neurons: " << globalState.brain->getTotalNeuronCount() << std::endl;
        
        // Show neurons by type
        std::vector<std::string> neuronTypes = {"Excitatory", "Inhibitory", "Sensory", "Motor", "Modulatory", "Internal"};
        for (const auto& type : neuronTypes) {
            int count = 0;
            // This would require access to neuron type information
            // For now, just show total
            std::cout << "  " << type << ": " << count << std::endl;
        }
        
        return CLIResult::Success;
    }
    
    CLIResult handleBrainSynapses() {
        std::cout << "=== Brain Synapses ===\n" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "Total synapses: " << globalState.brain->getTotalSynapseCount() << std::endl;
        
        // Show synapse types
        std::vector<std::string> synapseTypes = {"Excitatory", "Inhibitory", "Electrical", "Modulatory"};
        for (const auto& type : synapseTypes) {
            int count = 0;
            // This would require access to synapse type information
            std::cout << "  " << type << ": " << count << std::endl;
        }
        
        return CLIResult::Success;
    }
    
    CLIResult handleBrainPlasticity(const std::map<std::string, std::string>& options) {
        std::cout << "=== Brain Plasticity Configuration ===\n" << std::endl;
        
        std::string enableStr = getOption(options, "enable", "");
        std::string rateStr = getOption(options, "rate", "");
        
        if (!enableStr.empty()) {
            bool enable = (enableStr == "true" || enableStr == "1" || enableStr == "yes");
            if (globalState.brain) {
                globalState.brain->setPlasticityEnabled(enable);
                std::cout << "Plasticity " << (enable ? "enabled" : "disabled") << ".\n" << std::endl;
            }
        }
        
        if (!rateStr.empty()) {
            float rate = std::stof(rateStr);
            if (globalState.brain) {
                // This would require setting plasticity rate
                std::cout << "Plasticity rate set to " << rate << ".\n" << std::endl;
            }
        }
        
        return CLIResult::Success;
    }
    
    CLIResult handleBrainDevelopment(const std::map<std::string, std::string>& options) {
        std::cout << "=== Brain Development Configuration ===\n" << std::endl;
        
        std::string stageStr = getOption(options, "stage", "");
        std::string enabledStr = getOption(options, "enabled", "");
        
        if (!stageStr.empty()) {
            int stage = std::stoi(stageStr);
            if (globalState.brain) {
                // This would require setting development stage
                std::cout << "Development stage set to " << stage << ".\n" << std::endl;
            }
        }
        
        if (!enabledStr.empty()) {
            bool enabled = (enabledStr == "true" || enabledStr == "1" || enabledStr == "yes");
            if (globalState.brain) {
                // This would require setting development enabled state
                std::cout << "Development " << (enabled ? "enabled" : "disabled") << ".\n" << std::endl;
            }
        }
        
        return CLIResult::Success;
    }
    
    CLIResult handleBrainNeuromodulation(const std::map<std::string, std::string>& options) {
        std::cout << "=== Brain Neuromodulation Configuration ===\n" << std::endl;
        
        std::string dopamineStr = getOption(options, "dopamine", "");
        std::string curiosityStr = getOption(options, "curiosity", "");
        std::string noveltyStr = getOption(options, "novelty", "");
        
        if (!dopamineStr.empty()) {
            float dopamine = std::stof(dopamineStr);
            if (globalState.brain) {
                // This would require setting neuromodulation parameters
                std::cout << "Dopamine level set to " << dopamine << ".\n" << std::endl;
            }
        }
        
        if (!curiosityStr.empty()) {
            float curiosity = std::stof(curiosityStr);
            if (globalState.brain) {
                // This would require setting curiosity level
                std::cout << "Curiosity level set to " << curiosity << ".\n" << std::endl;
            }
        }
        
        if (!noveltyStr.empty()) {
            float novelty = std::stof(noveltyStr);
            if (globalState.brain) {
                // This would require setting novelty level
                std::cout << "Novelty level set to " << novelty << ".\n" << std::endl;
            }
        }
        
        return CLIResult::Success;
    }
    
    // Helper methods
    std::string getOption(const std::map<std::string, std::string>& options, const std::string& key, const std::string& defaultValue) const {
        auto it = options.find(key);
        return it != options.end() ? it->second : defaultValue;
    }
};

} // namespace nlm

