// CommandOutputFormatter implementation
#include "CommandOutputFormatter.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

CommandOutputFormatter::CommandOutputFormatter(std::shared_ptr<Logger> logger)
    : logger(logger) {}

std::string CommandOutputFormatter::formatBrainInfo(const Brain* brain) {
    std::stringstream ss;
    ss << "=== Brain Status ===" << std::endl;
    
    if (brain) {
        ss << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
        ss << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "Current Time: " << brain->getCurrentTime() << std::endl;
    }
    
    return ss.str();
}

std::string CommandOutputFormatter::formatNeuronInfo(const Neuron* neuron) {
    std::stringstream ss;
    ss << "=== Neuron Info ===" << std::endl;
    
    if (neuron) {
        ss << "Neuron ID: " << neuron->getId() << std::endl;
        ss << "Type: " << static_cast<int>(neuron->getType()) << std::endl;
        ss << "Membrane Potential: " << neuron->getMembranePotential() << std::endl;
        ss << "Threshold: " << neuron->getThreshold() << std::endl;
        ss << "Is Firing: " << (neuron->isFiring() ? "true" : "false") << std::endl;
    }
    
    return ss.str();
}

std::string CommandOutputFormatter::formatSynapseInfo(const Synapse* synapse) {
    std::stringstream ss;
    ss << "=== Synapse Info ===" << std::endl;
    
    if (synapse) {
        ss << "Synapse ID: " << synapse->getId() << std::endl;
        ss << "Weight: " << synapse->getWeight() << std::endl;
        ss << "Pre-synaptic Neuron: " << synapse->getPreSynapticNeuronId() << std::endl;
        ss << "Post-synaptic Neuron: " << synapse->getPostSynapticNeuronId() << std::endl;
        ss << "Is Plastic: " << (synapse->isPlasticityEnabled() ? "true" : "false") << std::endl;
    }
    
    return ss.str();
}

std::string CommandOutputFormatter::formatRegionInfo(const NeuralRegion* region) {
    std::stringstream ss;
    ss << "=== Region Info ===" << std::endl;
    
    if (region) {
        ss << "Region ID: " << region->getId() << std::endl;
        ss << "Neuron Count: " << region->getNeuronCount() << std::endl;
        ss << "Synapse Count: " << region->getSynapseCount() << std::endl;
        ss << "Population Count: " << region->getPopulationCount() << std::endl;
    }
    
    return ss.str();
}

std::string CommandOutputFormatter::formatPopulationInfo(const NeuralPopulation* population) {
    std::stringstream ss;
    ss << "=== Population Info ===" << std::endl;
    
    if (population) {
        ss << "Population ID: " << population->getId() << std::endl;
        ss << "Type: " << static_cast<int>(population->getType()) << std::endl;
        ss << "Neuron Count: " << population->getNeuronCount() << std::endl;
        ss << "Mean Firing Rate: " << population->getFiringRate() << std::endl;
    }
    
    return ss.str();
}

std::string CommandOutputFormatter::formatMemoryStatus(const NeuralWorkingMemory& memory) {
    std::stringstream ss;
    ss << "=== Working Memory Status ===" << std::endl;
    ss << "Capacity: " << memory.getCapacity() << std::endl;
    ss << "Current Items: " << memory.getCurrentItemCount() << std::endl;
    ss << "Memory Strength: " << memory.getMemoryStrength() << std::endl;
    return ss.str();
}

std::string CommandOutputFormatter::formatEpisodicMemory(const NeuralEpisodicMemory& memory) {
    std::stringstream ss;
    ss << "=== Episodic Memory Status ===" << std::endl;
    ss << "Total Episodes: " << memory.getTotalEpisodeCount() << std::endl;
    ss << "Replay Events: " << memory.getReplayEventCount() << std::endl;
    ss << "Consolidation Rate: " << memory.getConsolidationRate() << std::endl;
    return ss.str();
}

std::string CommandOutputFormatter::formatAssociativeMemory(const NeuralAssociativeMemory& memory) {
    std::stringstream ss;
    ss << "=== Associative Memory Status ===" << std::endl;
    ss << "Association Count: " << memory.getAssociationCount() << std::endl;
    ss << "Hebbian Strength: " << memory.getHebbianStrength() << std::endl;
    ss << "Pattern Completion Rate: " << memory.getPatternCompletionRate() << std::endl;
    return ss.str();
}

std::string CommandOutputFormatter::formatNeuromodulation(const Dopamine& dopamine,
                                                         const Curiosity& curiosity,
                                                         const PredictionError& predictionError,
                                                         const Novelty& novelty) {
    std::stringstream ss;
    ss << "=== Neuromodulation Status ===" << std::endl;
    ss << "Dopamine Level: " << dopamine.getLevel() << std::endl;
    ss << "Curiosity Level: " << curiosity.getLevel() << std::endl;
    ss << "Prediction Error: " << predictionError.getLevel() << std::endl;
    ss << "Novelty Level: " << novelty.getLevel() << std::endl;
    return ss.str();
}

std::string CommandOutputFormatter::formatProgress(double value, int width) {
    std::stringstream ss;
    ss << "[";
    
    int barWidth = static_cast<int>(value * width);
    for (int i = 0; i < width; ++i) {
        if (i < barWidth) ss << "=";
        else if (i == barWidth) ss << ">";
        else ss << " ";
    }
    
    ss << "] " << std::fixed << std::setprecision(1) << (value * 100) << "%";
    return ss.str();
}

std::string CommandOutputFormatter::formatMemorySize(size_t bytes) {
    std::stringstream ss;
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024 * 1024)) + " MB";
    return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
}

std::string CommandOutputFormatter::formatTime(double seconds) {
    std::stringstream ss;
    int minutes = static_cast<int>(seconds / 60);
    int secs = static_cast<int>(seconds) % 60;
    int millisecs = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
    
    ss << minutes << "m " << secs << "s " << millisecs << "ms";
    return ss.str();
}

std::string CommandOutputFormatter::formatTimestamp(const std::string& timestamp) {
    return "[" + timestamp + "]";
}

std::string CommandOutputFormatter::escapeMarkdown(const std::string& text) {
    std::string result = text;
    // Basic markdown escaping
    size_t pos = 0;
    while ((pos = result.find('*', pos)) != std::string::npos) {
        result.replace(pos, 1, "\\*");
        pos += 2;
    }
    while ((pos = result.find('_', pos)) != std::string::npos) {
        result.replace(pos, 1, "\\_");
        pos += 2;
    }
    while ((pos = result.find('`', pos)) != std::string::npos) {
        result.replace(pos, 1, "\\`");
        pos += 2;
    }
    return result;
}

std::string CommandOutputFormatter::wrapText(const std::string& text, size_t width) {
    std::stringstream ss;
    size_t pos = 0;
    size_t len = text.length();
    
    while (pos < len) {
        size_t end = std::min(pos + width, len);
        
        // Try to break at word boundary
        if (end < len) {
            size_t spacePos = text.find_last_of(' ', end);
            if (spacePos > pos && spacePos < end) {
                end = spacePos;
            }
        }
        
        ss << text.substr(pos, end - pos);
        if (end < len) ss << std::endl;
        pos = end + 1;
    }
    
    return ss.str();
}

std::string CommandOutputFormatter::formatCommandHelp(const std::string& command) {
    std::stringstream ss;
    ss << "=== Help for command: " << command << " ===" << std::endl;
    ss << "Detailed help for command " << command << " would appear here." << std::endl;
    return ss.str();
}

std::string CommandOutputFormatter::formatCommandError(const std::string& command, const std::string& error) {
    std::stringstream ss;
    ss << "ERROR: " << error;
    if (!command.empty()) {
        ss << " (command: " << command << ")";
    }
    ss << std::endl;
    return ss.str();
}

std::string CommandOutputFormatter::formatCommandSuccess(const std::string& command) {
    std::stringstream ss;
    ss << "Command executed successfully: " << command << std::endl;
    return ss.str();
}
