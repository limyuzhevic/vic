#ifndef NLM_COMMAND_LINE_HPP
#define NLM_COMMAND_LINE_HPP

#include "NLMCommandLine.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <iostream>

namespace nlmc {

void NLMCommandLine::initializeCommands() {
    // Debug commands
    commands["debug-neurons"] = [this](const std::vector<std::string>& args, auto, auto) {
        return debugNeurons(args);
    };
    
    commands["debug-synapses"] = [this](const std::vector<std::string>& args, auto, auto) {
        return debugSynapses(args);
    };
    
    commands["debug-region"] = [this](const std::vector<std::string>& args, auto, auto) {
        return debugRegion(args);
    };
    
    // Analysis commands
    commands["stats"] = [this](const std::vector<std::string>& args, auto, auto) {
        return analyzeStats(args);
    };
    
    commands["learning-curve"] = [this](const std::vector<std::string>& args, auto, auto) {
        return analyzeLearningCurve(args);
    };
    
    commands["network-analysis"] = [this](const std::vector<std::string>& args, auto, auto) {
        return analyzeNetwork(args);
    };
    
    // Control commands
    commands["save"] = [this](const std::vector<std::string>& args, auto, auto) {
        return saveState(args);
    };
    
    commands["load"] = [this](const std::vector<std::string>& args, auto, auto) {
        return loadState(args);
    };
    
    commands["reset"] = [this](const std::vector<std::string>& args, auto, auto) {
        return resetBrain(args);
    };
    
    // Configuration commands
    commands["config"] = [this](const std::vector<std::string>& args, auto, auto) {
        return configCommand(args);
    };
    
    commands["config-set"] = [this](const std::vector<std::string>& args, auto, auto) {
        return configSet(args);
    };
    
    // Experiment control
    commands["run-experiment"] = [this](const std::vector<std::string>& args, auto, auto) {
        return runExperiment(args);
    };
    
    // Visualization and export
    commands["export-neurons"] = [this](const std::vector<std::string>& args, auto, auto) {
        return exportNeurons(args);
    };
    
    commands["export-synapses"] = [this](const std::vector<std::string>& args, auto, auto) {
        return exportSynapses(args);
    };
    
    commands["heatmap"] = [this](const std::vector<std::string>& args, auto, auto) {
        return generateHeatmap(args);
    };
    
    // System information
    commands["help"] = [this](const std::vector<std::string>& args, auto, auto) {
        return showHelp(args);
    };
    
    commands["version"] = [this](const std::vector<std::string>& args, auto, auto) {
        return showVersion(args);
    };
}

} // namespace nlmc

#endif // NLM_COMMAND_LINE_HPP
