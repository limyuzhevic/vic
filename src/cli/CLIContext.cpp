#include "CLIContext.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {

bool CLIContext::parseArgs(int argc, char** argv, std::string& command,
                          std::map<std::string, std::string>& options,
                          std::vector<std::string>& args) {
    if (argc < 2) {
        std::cout << "No command provided" << std::endl;
        return false;
    }
    
    command = argv[1];
    
    // Parse options and arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            // Option
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                options[key] = value;
            } else {
                options[arg.substr(2)] = "true";
            }
        } else {
            // Argument
            args.push_back(arg);
        }
    }
    
    return true;
}

void CLIContext::printConfigSummary() const {
    std::cout << "=== Configuration Summary ===" << std::endl;
    std::string summary = config->summary();
    std::cout << summary;
}

void CLIContext::printBrainStats() const {
    std::cout << "=== Brain Statistics ===" << std::endl;
    
    if (!brain) {
        std::cout << "No brain instance available." << std::endl;
        return;
    }
    
    std::cout << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total Spike Events: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Pending Spike Events: " << brain->getPendingSpikeEventCount() << std::endl;
    std::cout << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
}

void CLIContext::printNeuronsByType(const std::string& typeFilter, size_t count) const {
    std::cout << "=== Neurons by Type ===" << std::endl;
    
    if (!brain) {
        std::cout << "No brain instance available." << std::endl;
        return;
    }
    
    // Get all regions
    auto regions = brain->getRegions();
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
            totalNeurons += excitatoryCount;
            float percentage = (totalNeurons > 0) ? (static_cast<float>(excitatoryCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Excitatory" << std::setw(20) << excitatoryCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        } else if (typeFilter == "inhibitory" && inhibitoryCount > 0) {
            totalNeurons += inhibitoryCount;
            float percentage = (totalNeurons > 0) ? (static_cast<float>(inhibitoryCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Inhibitory" << std::setw(20) << inhibitoryCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        } else if (typeFilter == "modulatory" && modulatoryCount > 0) {
            totalNeurons += modulatoryCount;
            float percentage = (totalNeurons > 0) ? (static_cast<float>(modulatoryCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Modulatory" << std::setw(20) << modulatoryCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        } else if (typeFilter == "sensory" && sensoryCount > 0) {
            totalNeurons += sensoryCount;
            float percentage = (totalNeurons > 0) ? (static_cast<float>(sensoryCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Sensory" << std::setw(20) << sensoryCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        } else if (typeFilter == "motor" && motorCount > 0) {
            totalNeurons += motorCount;
            float percentage = (totalNeurons > 0) ? (static_cast<float>(motorCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Motor" << std::setw(20) << motorCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        } else if (typeFilter == "internal" && internalCount > 0) {
            totalNeurons += internalCount;
            float percentage = (totalNeurons > 0) ? (static_cast<float>(internalCount) / totalNeurons) * 100.0f : 0.0f;
            std::cout << std::left << std::setw(15) << "Internal" << std::setw(20) << internalCount << std::setw(25) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        } else {
            std::cout << "Unknown neuron type: " << typeFilter << std::endl;
        }
    } else {
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
}

void CLIContext::printSynapsesAboveWeight(float minWeight) const {
    std::cout << "=== Synapses Above Weight Threshold ===" << std::endl;
    std::cout << "Threshold: " << minWeight << std::endl;
    
    if (!brain) {
        std::cout << "No brain instance available." << std::endl;
        return;
    }
    
    std::cout << std::left << std::setw(20) << "Source Neuron" << std::setw(20) << "Destination Neuron" << std::setw(15) << "Weight" << std::setw(15) << "Type" << std::endl;
    std::cout << "------------------------------------------------------------------------------------------" << std::endl;
    
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t aboveThreshold = 0;
    
    for (const auto& region : brain->getRegions()) {
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
}

} // namespace nlm
