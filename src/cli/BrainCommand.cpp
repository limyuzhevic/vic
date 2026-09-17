#include "BrainCommand.hpp"
#include "CLIContext.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace nlm {

bool BrainCommand::executeStatus(const CLIContext& context, const std::map<std::string, std::string>& options) {
    std::cout << "=== Brain Statistics ===" << std::endl;
    
    if (!context.brain) {
        std::cout << "No brain instance available." << std::endl;
        return false;
    }
    
    // Print general statistics
    std::cout << "Total Neurons: " << context.brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << context.brain->getTotalSynapseCount() << std::endl;
    std::cout << "Active Neurons: " << context.brain->getActiveNeuronCount() << std::endl;
    std::cout << "Firing Neurons: " << context.brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total Spike Events: " << context.brain->getTotalSpikeCount() << std::endl;
    std::cout << "Pending Spike Events: " << context.brain->getPendingSpikeEventCount() << std::endl;
    std::cout << "Excitation/Inhibition Ratio: " << context.brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << "Average Firing Rate: " << context.brain->getAverageFiringRate() << std::endl;
    
    // Get region information
    auto regionIds = context.brain->getRegionIds();
    std::cout << "\nRegions: " << regionIds.size() << std::endl;
    for (const auto& regionId : regionIds) {
        auto* region = context.brain->getRegion(regionId);
        if (region) {
            std::cout << "  Region " << regionId.value << ": " << region->getName();
            std::cout << " (Population: " << region->getTotalPopulationCount() << ")" << std::endl;
        }
    }
    
    return true;
}

bool BrainCommand::executeNeurons(CLIContext& context, const std::map<std::string, std::string>& options) {
    std::cout << "=== Neurons by Type ===" << std::endl;
    
    if (!context.brain) {
        std::cout << "No brain instance available." << std::endl;
        return false;
    }
    
    // Determine type filter from options
    std::string typeFilter = "";
    size_t count = 0;
    
    auto it = options.find("type");
    if (it != options.end()) {
        typeFilter = it->second;
    }
    
    it = options.find("count");
    if (it != options.end()) {
        count = std::stoul(it->second);
    }
    
    // Get all regions
    auto regions = context.brain->getRegions();
    size_t totalNeurons = 0;
    
    std::cout << std::left << std::setw(15) << "Type" << std::setw(20) << "Count" << std::setw(25) << "Percentage" << std::endl;
    std::cout << "---------------------------------------------------------------------" << std::endl;
    
    // Get counts for each neuron type
    size_t excitatoryCount = 0;
    size_t inhibitoryCount = 0;
    size_t modulatoryCount = 0;
    size_t sensoryCount = 0;
    size_t motorCount = 0;
    size_t internalCount = 0;
    
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            NeuronType type = neuron->getType();
            switch (type) {
                case NeuronType::Excitatory: excitatoryCount++; break;
                case NeuronType::Inhibitory: inhibitoryCount++; break;
                case NeuronType::Modulatory: modulatoryCount++; break;
                case NeuronType::Sensory: sensoryCount++; break;
                case NeuronType::Motor: motorCount++; break;
                case NeuronType::Internal: internalCount++; break;
            }
        }
    }
    
    // Apply type filter if specified
    if (!typeFilter.empty()) {
        std::transform(typeFilter.begin(), typeFilter.end(), typeFilter.begin(), ::tolower);
        
        if (typeFilter == "excitatory" && excitatoryCount > 0) {
            std::cout << std::left << std::setw(15) << "Excitatory" << std::setw(20) << excitatoryCount;
            if (totalNeurons > 0) {
                float percentage = (static_cast<float>(excitatoryCount) / totalNeurons) * 100.0f;
                std::cout << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
            }
        } else if (typeFilter == "inhibitory" && inhibitoryCount > 0) {
            std::cout << std::left << std::setw(15) << "Inhibitory" << std::setw(20) << inhibitoryCount;
            if (totalNeurons > 0) {
                float percentage = (static_cast<float>(inhibitoryCount) / totalNeurons) * 100.0f;
                std::cout << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
            }
        } else if (typeFilter == "modulatory" && modulatoryCount > 0) {
            std::cout << std::left << std::setw(15) << "Modulatory" << std::setw(20) << modulatoryCount;
            if (totalNeurons > 0) {
                float percentage = (static_cast<float>(modulatoryCount) / totalNeurons) * 100.0f;
                std::cout << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
            }
        } else if (typeFilter == "sensory" && sensoryCount > 0) {
            std::cout << std::left << std::setw(15) << "Sensory" << std::setw(20) << sensoryCount;
            if (totalNeurons > 0) {
                float percentage = (static_cast<float>(sensoryCount) / totalNeurons) * 100.0f;
                std::cout << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
            }
        } else if (typeFilter == "motor" && motorCount > 0) {
            std::cout << std::left << std::setw(15) << "Motor" << std::setw(20) << motorCount;
            if (totalNeurons > 0) {
                float percentage = (static_cast<float>(motorCount) / totalNeurons) * 100.0f;
                std::cout << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
            }
        } else if (typeFilter == "internal" && internalCount > 0) {
            std::cout << std::left << std::setw(15) << "Internal" << std::setw(20) << internalCount;
            if (totalNeurons > 0) {
                float percentage = (static_cast<float>(internalCount) / totalNeurons) * 100.0f;
                std::cout << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
            }
        } else {
            std::cout << "Unknown neuron type: " << typeFilter << std::endl;
            return false;
        }
    } else {
        // Print all types
        totalNeurons = excitatoryCount + inhibitoryCount + modulatoryCount + sensoryCount + motorCount + internalCount;
        
        if (excitatoryCount > 0) {
            float percentage = (totalNeurons > 0) ? (static_cast<float>(excitatoryCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Excitatory" << std::setw(20) << excitatoryCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        }
        if (inhibitoryCount > 0) {
            float percentage = (totalNeurons > 0) ? (static_cast<float>(inhibitoryCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Inhibitory" << std::setw(20) << inhibitoryCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        }
        if (modulatoryCount > 0) {
            float percentage = (totalNeurons > 0) ? (static_cast<float>(modulatoryCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Modulatory" << std::setw(20) << modulatoryCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        }
        if (sensoryCount > 0) {
            float percentage = (totalNeurons > 0) ? (static_cast<float>(sensoryCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Sensory" << std::setw(20) << sensoryCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        }
        if (motorCount > 0) {
            float percentage = (totalNeurons > 0) ? (static_cast<float>(motorCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Motor" << std::setw(20) << motorCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        }
        if (internalCount > 0) {
            float percentage = (totalNeurons > 0) ? (static_cast<float>(internalCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Internal" << std::setw(20) << internalCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        }
    }
    
    std::cout << "---------------------------------------------------------------------" << std::endl;
    std::cout << "Total Neurons: " << totalNeurons << std::endl;
    
    return true;
}

bool BrainCommand::executeSynapses(CLIContext& context, const std::map<std::string, std::string>& options) {
    std::cout << "=== Synapses Above Weight Threshold ===" << std::endl;
    
    if (!context.brain) {
        std::cout << "No brain instance available." << std::endl;
        return false;
    }
    
    // Determine min weight from options
    float minWeight = 0.0f;
    auto it = options.find("min-weight");
    if (it != options.end()) {
        minWeight = std::stof(it->second);
    }
    
    std::cout << "Threshold: " << minWeight << std::endl;
    std::cout << std::left << std::setw(20) << "Source Neuron" << std::setw(20) << "Destination Neuron" << std::setw(15) << "Weight" << std::setw(15) << "Type" << std::endl;
    std::cout << "------------------------------------------------------------------------------------------" << std::endl;
    
    size_t totalSynapses = context.brain->getTotalSynapseCount();
    size_t aboveThreshold = 0;
    
    for (const auto& region : context.brain->getRegions()) {
        auto& synapses = region->getSynapses();
        for (const auto& synapse : synapses) {
            float weight = synapse->getWeight();
            if (weight >= minWeight) {
                std::cout << std::left << std::setw(20) << synapse->getSourceNeuron().value;
                std::cout << std::left << std::setw(20) << synapse->getDestinationNeuron().value;
                std::cout << std::left << std::setw(15) << std::fixed << std::setprecision(4) << weight;
                
                SynapseType type = synapse->getType();
                std::string typeStr;
                switch (type) {
                    case SynapseType::Excitatory: typeStr = "Excitatory"; break;
                    case SynapseType::Inhibitory: typeStr = "Inhibitory"; break;
                    case SynapseType::Modulatory: typeStr = "Modulatory"; break;
                    case SynapseType::Electrical: typeStr = "Electrical"; break;
                    case SynapseType::GapJunction: typeStr = "Gap Junction"; break;
                }
                std::cout << std::left << std::setw(15) << typeStr << std::endl;
                
                aboveThreshold++;
            }
        }
    }
    
    std::cout << "------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Total Synapses: " << totalSynapses << std::endl;
    std::cout << "Above Threshold: " << aboveThreshold << std::endl;
    
    return true;
}

bool BrainCommand::executeSave(const CLIContext& context, const std::string& filename,
                              const std::map<std::string, std::string>& options) {
    std::cout << "=== Saving Brain State ===" << std::endl;
    std::cout << "Filename: " << filename << std::endl;
    
    if (!context.brain) {
        std::cout << "No brain instance available." << std::endl;
        return false;
    }
    
    if (context.brain->save(filename)) {
        std::cout << "Brain state saved successfully: " << filename << std::endl;
        return true;
    } else {
        std::cout << "Failed to save brain state: " << filename << std::endl;
        return false;
    }
}

bool BrainCommand::executeLoad(CLIContext& context, const std::string& filename,
                               const std::map<std::string, std::string>& options) {
    std::cout << "=== Loading Brain State ===" << std::endl;
    std::cout << "Filename: " << filename << std::endl;
    
    if (!context.brain) {
        std::cout << "No brain instance available." << std::endl;
        return false;
    }
    
    if (context.brain->load(filename)) {
        std::cout << "Brain state loaded successfully: " << filename << std::endl;
        return true;
    } else {
        std::cout << "Failed to load brain state: " << filename << std::endl;
        return false;
    }
}

} // namespace nlm
