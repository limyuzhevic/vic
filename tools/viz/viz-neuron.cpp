// Neuron visualization tool implementation
//
// This file provides the implementation of neuron visualization CLI tool

#include "viz-neuron.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

ExecutionResult VizNeuronTool::execute(const std::vector<std::string>& args, Config& config) {
    std::string brainFile = getStringArg(args, {"--brain", "-b"}, "");
    int neuronId = getIntArg(args, {"--neuron", "-n"}, -1);
    std::string outputFile = getStringArg(args, {"--output", "-o"}, "");
    std::string format = getStringArg(args, {"--format", "-f"}, "png");
    std::string vizType = getStringArg(args, {"--type", "-t"}, "membrane_potential");
    
    // Validate arguments
    std::vector<std::string> requiredFlags = {"--brain", "--neuron", "--output"};
    std::map<std::string, std::string> optionalFlags = {
        {"--format", "png"},
        {"--type", "membrane_potential"}
    };
    
    validateArgs(args, requiredFlags, optionalFlags);
    
    if (neuronId < 0) {
        printError("Neuron ID must be a positive integer");
        return {false, "Invalid neuron ID"};
    }
    
    printInfo("Visualizing neuron " + std::to_string(neuronId) + " from brain: " + brainFile);
    printInfo("Output file: " + outputFile + " with format: " + format);
    printInfo("Visualization type: " + vizType);
    
    // Create visualization
    bool success = visualizeNeuronImpl(brainFile, neuronId, outputFile, format, vizType);
    
    if (success) {
        printSuccess("Neuron visualization completed successfully");
        return {true, "Neuron visualization completed"};
    } else {
        printError("Failed to create neuron visualization");
        return {false, "Neuron visualization failed"};
    }
}

bool VizNeuronTool::visualizeNeuronImpl(const std::string& brainFile, int neuronId,
                                       const std::string& outputFile, const std::string& format,
                                       const std::string& vizType) const {
    if (!fileExists(brainFile)) {
        printError("Brain state file does not exist: " + brainFile);
        return false;
    }
    
    try {
        nlohmann::json brainState = parseJsonFromFile(brainFile);
        
        // Get neuron data from brain state
        std::vector<nlohmann::json> neurons = brainState["neurons"].get<std::vector<nlohmann::json>>();
        
        if (neuronId >= neurons.size()) {
            printError("Neuron ID " + std::to_string(neuronId) + " does not exist. Valid range: 0-" + std::to_string(neurons.size() - 1));
            return false;
        }
        
        nlohmann::json neuron = neurons[neuronId];
        
        // Create visualization based on type
        if (vizType == "membrane_potential") {
            std::ofstream out(outputFile);
            out << "Neuron Membrane Potential Visualization" << std::endl;
            out << "Neuron ID: " << neuronId << std::endl;
            out << "Type: " << neuron["type"] << std::endl;
            out << "Average Membrane Potential: " << neuron["membrane_potential"] << " mV" << std::endl;
            out << "Resting Potential: " << neuron["resting_potential"] << " mV" << std::endl;
            out << "Threshold Potential: " << neuron["threshold_potential"] << " mV" << std::endl;
            out << "Membrane Time Constant: " << neuron["membrane_time_constant"] << " ms" << std::endl;
            out.close();
            
        } else if (vizType == "spike_raster") {
            std::ofstream out(outputFile);
            out << "Neuron Spike Raster Visualization" << std::endl;
            out << "Neuron ID: " << neuronId << std::endl;
            out << "Total Spikes: " << neuron["spike_count"] << std::endl;
            out << "Firing Rate: " << neuron["firing_rate"] << " Hz" << std::endl;
            out << "Last Spike Time: " << neuron["last_spike_time"] << " ms" << std::endl;
            out << "Refractory Period: " << neuron["refractory_period"] << " ms" << std::endl;
            out.close();
            
        } else if (vizType == "connectivity") {
            std::ofstream out(outputFile);
            out << "Neuron Connectivity Visualization" << std::endl;
            out << "Neuron ID: " << neuronId << std::endl;
            out << "Number of Inputs: " << neuron["inputs"].size() << std::endl;
            out << "Number of Outputs: " << neuron["outputs"].size() << std::endl;
            out << "Total Connections: " << neuron["inputs"].size() + neuron["outputs"].size() << std::endl;
            out.close();
            
        } else if (vizType == "plasticity") {
            std::ofstream out(outputFile);
            out << "Neuron Plasticity Visualization" << std::endl;
            out << "Neuron ID: " << neuronId << std::endl;
            out << "Plasticity State: " << neuron["plasticity_state"] << std::endl;
            out << "Hebbian Weight: " << neuron["hebbian_weight"] << std::endl;
            out << "STDP Weight: " << neuron["stdp_weight"] << std::endl;
            out << "Structural Plasticity: " << neuron["structural_plasticity"] << std::endl;
            out.close();
        }
        
        printInfo("Neuron visualization data written to: " + outputFile);
        return true;
        
    } catch (const std::exception& e) {
        printError("Failed to visualize neuron: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
