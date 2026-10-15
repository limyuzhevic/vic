#include "NeuromodulationVisualizer.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuromodulationVisualizer::Impl {
    std::shared_ptr<NeuromodulationSystem> neuromodulationSystem;
    
    // Neuromodulator states
    std::unordered_map<NeuromodulatorType, NeuromodulatorState> neuromodulatorStates;
    
    // Network visualization
    NeuromodulationNetwork network;
    
    // Visualization options
    bool showNetworkInteractions = true;
    bool showHomeostasis = true;
    bool showCrossEffects = true;
    bool highlightActive = true;
    
    // Performance tracking
    double lastUpdateTime = 0.0;
    size_t updateCount = 0;
    
    Impl() {
        // Initialize all neuromodulator types
        neuromodulatorStates[NeuromodulatorType::Dopamine] = NeuromodulatorState(NeuromodulatorType::Dopamine);
        neuromodulatorStates[NeuromodulatorType::Serotonin] = NeuromodulatorState(NeuromodulatorType::Serotonin);
        neuromodulatorStates[NeuromodulatorType::Norepinephrine] = NeuromodulatorState(NeuromodulatorType::Norepinephrine);
        neuromodulatorStates[NeuromodulatorType::Acetylcholine] = NeuromodulatorState(NeuromodulatorType::Acetylcholine);
        neuromodulatorStates[NeuromodulatorType::GABA] = NeuromodulatorState(NeuromodulatorType::GABA);
        
        network.activeNeuromodulators = {
            NeuromodulatorType::Dopamine,
            NeuromodulatorType::Serotonin,
            NeuromodulatorType::Norepinephrine,
            NeuromodulatorType::Acetylcholine
        };
        
        // Initialize interaction weights
        for (auto& pair1 : network.activeNeuromodulators) {
            for (auto& pair2 : network.activeNeuromodulators) {
                if (pair1 != pair2) {
                    network.interactionWeights[pair1] = network.interactionWeights[pair1] + 0.1f;
                    network.interactionWeights[pair2] = network.interactionWeights[pair2] + 0.1f;
                }
            }
        }
    }
};

NeuromodulationVisualizer::NeuromodulationVisualizer() : pImpl(new Impl) {}

NeuromodulationVisualizer::~NeuromodulationVisualizer() = default;

bool NeuromodulationVisualizer::initialize() {
    return true;
}

void NeuromodulationVisualizer::update(double deltaTime) {
    pImpl->lastUpdateTime += deltaTime;
    pImpl->updateCount++;
    
    // Update neuromodulator states
    float homeostasis = 1.0f;
    for (auto& pair : pImpl->neuromodulatorStates) {
        auto& state = pair.second;
        
        // Simple homeostatic regulation
        if (state.level > state.targetLevel) {
            state.rate = -0.01f;
            state.status = "decreasing";
        } else if (state.level < state.targetLevel) {
            state.rate = 0.01f;
            state.status = "increasing";
        } else {
            state.rate = 0.0f;
            state.status = "stable";
        }
        
        // Update level
        state.level += state.rate * deltaTime;
        
        // Clamp to reasonable range
        state.level = std::max(0.0f, std::min(1.0f, state.level));
        
        // Calculate effect (simplified)
        state.effect = state.level * (1.0f - std::abs(state.level - state.targetLevel)) * 2.0f;
        
        // Update homeostasis
        homeostasis = std::min(homeostasis, 1.0f - std::abs(state.level - state.targetLevel));
        
        // Simulate cross-effects
        for (const auto& otherPair : pImpl->network.activeNeuromodulators) {
            if (otherPair.first != pair.first) {
                float effect = otherPair.second * 0.1f; // Simplified cross-effect
                pImpl->network.crossEffects[pair.first] = effect;
            }
        }
    }
    
    pImpl->network.homeostasis = homeostasis;
    pImpl->network.networkActivity = 0.0f;
    for (const auto& pair : pImpl->neuromodulatorStates) {
        pImpl->network.networkActivity += pair.second.effect;
    }
}

void NeuromodulationVisualizer::render() {
    std::cout << "=== Neuromodulation Visualization ===" << std::endl;
    
    // Generate level visualization
    std::string levelViz = generateLevelVisualization();
    std::cout << "Neuromodulator Levels:" << std::endl;
    std::cout << levelViz;
    
    // Generate network visualization
    std::string networkViz = generateNetworkVisualization();
    std::cout << std::endl << "Interaction Network:" << std::endl;
    std::cout << networkViz;
    
    // Generate homeostasis visualization
    std::string homeostasisViz = generateHomeostasisVisualization();
    std::cout << std::endl << "Network Homeostasis:" << std::endl;
    std::cout << homeostasisViz;
    
    // Show metrics
    std::string metrics = getNeuromodulationMetrics();
    std::cout << std::endl << metrics << std::endl;
}

void NeuromodulationVisualizer::close() {
    pImpl->neuromodulatorStates.clear();
    pImpl->network.activeNeuromodulators.clear();
    pImpl->network.interactionWeights.clear();
    pImpl->network.crossEffects.clear();
}

void NeuromodulationVisualizer::setNeuromodulationSystem(std::shared_ptr<NeuromodulationSystem> neuromodulation) {
    pImpl->neuromodulationSystem = neuromodulation;
}

void NeuromodulationVisualizer::updateNeuromodulator(NeuromodulatorType type, float level, float effect) {
    if (pImpl->neuromodulatorStates.find(type) != pImpl->neuromodulatorStates.end()) {
        pImpl->neuromodulatorStates[type].level = std::max(0.0f, std::min(1.0f, level));
        pImpl->neuromodulatorStates[type].effect = effect;
        pImpl->neuromodulatorStates[type].lastUpdated = pImpl->updateCount;
        
        // Update status
        if (level > pImpl->neuromodulatorStates[type].targetLevel * 1.1f) {
            pImpl->neuromodulatorStates[type].status = "high";
        } else if (level < pImpl->neuromodulatorStates[type].targetLevel * 0.9f) {
            pImpl->neuromodulatorStates[type].status = "low";
        } else {
            pImpl->neuromodulatorStates[type].status = "stable";
        }
    }
}

void NeuromodulationVisualizer::addNeuromodulator(NeuromodulatorType type) {
    if (pImpl->neuromodulatorStates.find(type) == pImpl->neuromodulatorStates.end()) {
        pImpl->neuromodulatorStates[type] = NeuromodulatorState(type);
        pImpl->network.activeNeuromodulators.push_back(type);
    }
}

void NeuromodulationVisualizer::setInteraction(NeuromodulatorType source, NeuromodulatorType target, float weight) {
    pImpl->network.interactionWeights[source] = weight;
}

const std::unordered_map<NeuromodulatorType, NeuromodulatorState>& NeuromodulationVisualizer::getNeuromodulatorStates() const {
    return pImpl->neuromodulatorStates;
}

const NeuromodulationNetwork& NeuromodulationVisualizer::getNetwork() const {
    return pImpl->network;
}

std::string NeuromodulationVisualizer::generateLevelVisualization() const {
    std::stringstream ss;
    
    ss << "  Neuromodulator Concentrations (Level 0-1):" << std::endl;
    
    for (const auto& pair : pImpl->neuromodulatorStates) {
        const auto& state = pair.second;
        
        std::string typeStr;
        switch (state.type) {
            case NeuromodulatorType::Dopamine: typeStr = "Dopamine"; break;
            case NeuromodulatorType::Serotonin: typeStr = "Serotonin"; break;
            case NeuromodulatorType::Norepinephrine: typeStr = "Norepinephrine"; break;
            case NeuromodulatorType::Acetylcholine: typeStr = "Acetylcholine"; break;
            case NeuromodulatorType::GABA: typeStr = "GABA"; break;
        }
        
        std::string statusMarker = (pImpl->highlightActive && state.status != "stable") ? " *" : "";
        
        ss << "    " << typeStr << ": [";
        for (int i = 0; i < 10; ++i) {
            if (state.level * 10 > i) ss << "=";
            else ss << "-";
        }
        ss << "] " << std::fixed << std::setprecision(2) << state.level << 
           " effect=" << std::fixed << std::setprecision(2) << state.effect << 
           " status=" << state.status << statusMarker << std::endl;
    }
    
    return ss.str();
}

std::string NeuromodulationVisualizer::generateNetworkVisualization() const {
    std::stringstream ss;
    
    if (!pImpl->showNetworkInteractions) {
        return "  Network interactions hidden";
    }
    
    ss << "  Interaction Network (influence matrix):" << std::endl;
    
    // Display as a simple matrix
    std::vector<NeuromodulatorType> types;
    for (const auto& pair : pImpl->network.activeNeuromodulators) {
        types.push_back(pair);
    }
    
    // Header
    ss << "      ";
    for (const auto& type : types) {
        std::string shortName;
        switch (type) {
            case NeuromodulatorType::Dopamine: shortName = "DOP"; break;
            case NeuromodulatorType::Serotonin: shortName = "SER"; break;
            case NeuromodulatorType::Norepinephrine: shortName = "NOR"; break;
            case NeuromodulatorType::Acetylcholine: shortName = "ACH"; break;
            case NeuromodulatorType::GABA: shortName = "GABA"; break;
        }
        ss << " " << std::setw(4) << shortName;
    }
    ss << std::endl;
    
    // Matrix rows
    for (const auto& source : types) {
        std::string sourceShort;
        switch (source) {
            case NeuromodulatorType::Dopamine: sourceShort = "DOP"; break;
            case NeuromodulatorType::Serotonin: sourceShort = "SER"; break;
            case NeuromodulatorType::Norepinephrine: sourceShort = "NOR"; break;
            case NeuromodulatorType::Acetylcholine: sourceShort = "ACH"; break;
            case NeuromodulatorType::GABA: sourceShort = "GABA"; break;
        }
        
        ss << "  " << std::setw(6) << sourceShort << "|";
        
        for (const auto& target : types) {
            float weight = 0.0f;
            if (pImpl->network.interactionWeights.find(source) != pImpl->network.interactionWeights.end()) {
                weight = pImpl->network.interactionWeights[source];
            }
            
            // Display as simple bar
            int barLength = static_cast<int>(weight * 10);
            ss << " ";
            for (int i = 0; i < barLength; ++i) ss << "#";
            ss << " ";
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string NeuromodulationVisualizer::generateHomeostasisVisualization() const {
    std::stringstream ss;
    
    ss << "  Network Homeostasis Status:" << std::endl;
    
    // Homeostasis bar
    ss << "    Overall Homeostasis: [";
    int homeostasisBar = static_cast<int>(pImpl->network.homeostasis * 10);
    for (int i = 0; i < homeostasisBar; ++i) ss << "#";
    for (int i = homeostasisBar; i < 10; ++i) ss << "-";
    ss << "] " << std::fixed << std::setprecision(2) << pImpl->network.homeostasis << std::endl;
    
    // Individual neuromodulator homeostasis
    ss << "    Individual Homeostasis:" << std::endl;
    for (const auto& pair : pImpl->neuromodulatorStates) {
        const auto& state = pair.second;
        float homeostasis = 1.0f - std::abs(state.level - state.targetLevel);
        
        ss << "      " << static_cast<int>(state.type) << ": [";
        int barLength = static_cast<int>(homeostasis * 10);
        for (int i = 0; i < barLength; ++i) ss << "#";
        for (int i = barLength; i < 10; ++i) ss << "-";
        ss << "] " << std::fixed << std::setprecision(2) << homeostasis << std::endl;
    }
    
    ss << "    Network Activity: " << std::fixed << std::setprecision(2) << pImpl->network.networkActivity << std::endl;
    
    return ss.str();
}

std::string NeuromodulationVisualizer::getNeuromodulationMetrics() const {
    std::stringstream ss;
    
    ss << "Neuromodulation Metrics:" << std::endl;
    ss << "  Total Updates: " << pImpl->updateCount << std::endl;
    ss << "  Active Neuromodulators: " << pImpl->network.activeNeuromodulators.size() << std::endl;
    ss << "  Average Level: ";
    
    if (!pImpl->neuromodulatorStates.empty()) {
        float totalLevel = 0.0f;
        for (const auto& pair : pImpl->neuromodulatorStates) {
            totalLevel += pair.second.level;
        }
        float avgLevel = totalLevel / pImpl->neuromodulatorStates.size();
        ss << std::fixed << std::setprecision(2) << avgLevel << std::endl;
    } else {
        ss << "N/A" << std::endl;
    }
    
    ss << "  Update Rate: " << (pImpl->updateCount / std::max(pImpl->lastUpdateTime, 1.0)) << " updates/s" << std::endl;
    
    return ss.str();
}

} // namespace nlm
