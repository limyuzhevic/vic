#include "NeuromodulatorSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

struct NeuromodulatorSystem::Impl {
    std::unordered_map<std::string, Neuromodulator*> neuromodulators;
    std::vector<std::function<void(const std::string&, float)>> levelChangeCallbacks;
    bool initialized;
    
    Impl() : initialized(false) {
        // Initialize neuromodulators
        neuromodulators["DA"] = new Dopamine();
        neuromodulators["Curiosity"] = new Curiosity();
        neuromodulators["PredictionError"] = new PredictionError();
        neuromodulators["Novelty"] = new Novelty();
    }
    
    ~Impl() {
        for (auto& pair : neuromodulators) {
            delete pair.second;
        }
        neuromodulators.clear();
    }
    
    void configureFromConfig(const std::shared_ptr<Config>& config) {
        if (!config) return;
        
        // Configure each neuromodulator from config
        if (auto daLevel = config->getOr<float>("dopamine_level", 0.0f)) {
            neuromodulators["DA"]->setLevel(*daLevel);
        }
        
        if (auto curiosityLevel = config->getOr<float>("curiosity_baseline", 0.1f)) {
            neuromodulators["Curiosity"]->setLevel(*curiosityLevel);
        }
        
        if (auto noveltyLevel = config->getOr<float>("novelty_baseline", 0.1f)) {
            neuromodulators["Novelty"]->setLevel(*noveltyLevel);
        }
        
        if (auto predErrorLevel = config->getOr<float>("prediction_error_baseline", 0.0f)) {
            neuromodulators["PredictionError"]->setLevel(*predErrorLevel);
        }
    }
    
    void update(TimestepDuration dt, Brain* brain) {
        if (!brain) return;
        
        // Update each neuromodulator
        for (auto& pair : neuromodulators) {
            Neuromodulator* mod = pair.second;
            mod->update(dt);
            
            // Check for level changes and call callbacks
            static float lastLevel = -1.0f;
            float currentLevel = mod->getLevel();
            if (std::abs(currentLevel - lastLevel) > 0.001f) {
                for (auto& callback : levelChangeCallbacks) {
                    callback(pair.first, currentLevel);
                }
                lastLevel = currentLevel;
            }
        }
        
        // Neuromodulators can influence brain systems
        applyNeuromodulatoryEffects(brain);
    }
    
    void applyNeuromodulatoryEffects(Brain* brain) {
        if (!brain) return;
        
        // Dopamine affects excitability and plasticity
        if (auto* dopamine = neuromodulators["DA"]) {
            float level = dopamine->getLevel();
            float plasticityFactor = dopamine->getPlasticityFactor();
            
            // Apply dopamine effects to neurons
            applyDopamineEffects(brain, level, plasticityFactor);
        }
        
        // Curiosity drives exploration
        if (auto* curiosity = neuromodulators["Curiosity"]) {
            float level = curiosity->getLevel();
            applyCuriosityEffects(brain, level);
        }
        
        // Novelty signals unexpected events
        if (auto* novelty = neuromodulators["Novelty"]) {
            float level = novelty->getLevel();
            applyNoveltyEffects(brain, level);
        }
        
        // Prediction error drives learning
        if (auto* predError = neuromodulators["PredictionError"]) {
            float level = predError->getLevel();
            applyPredictionErrorEffects(brain, level);
        }
    }
    
    void applyDopamineEffects(Brain* brain, float level, float plasticityFactor) {
        // Dopamine modulates neural excitability
        // Higher dopamine increases excitability (lower effective threshold)
        float excitabilityMod = level * 0.5f;
        
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    void applyCuriosityEffects(Brain* brain, float level) {
        // Curiosity drives exploration behavior
        // Higher curiosity increases random actions and exploration
        if (auto* curiosity = neuromodulators["Curiosity"]) {
            // In a real implementation, curiosity would influence action selection
            // For now, we just store the level for access by action selection systems
        }
    }
    
    void applyNoveltyEffects(Brain* brain, float level) {
        // Novelty signals unexpected events
        // Higher novelty increases learning rate and attention
        if (auto* novelty = neuromodulators["Novelty"]) {
            // Novelty could increase plasticity or attention
        }
    }
    
    void applyPredictionErrorEffects(Brain* brain, float level) {
        // Prediction error drives learning
        // Positive prediction error (better than expected) increases learning
        // Negative prediction error (worse than expected) decreases learning
        if (auto* predError = neuromodulators["PredictionError"]) {
            // Prediction error could modulate plasticity
        }
    }
    
    std::string getStatistics() const {
        std::ostringstream oss;
        oss << "NeuromodulatorSystem Statistics:\n";
        oss << "  Total neuromodulators: " << neuromodulators.size() << "\n";
        oss << "  Initialized: " << (initialized ? "true" : "false") << "\n";
        oss << "  Level change callbacks: " << levelChangeCallbacks.size() << "\n";
        
        for (const auto& pair : neuromodulators) {
            oss << "  " << pair.first << ": level=" << pair.second->getLevel()
                << ", plasticity_factor=" << pair.second->getPlasticityFactor() << "\n";
        }
        
        return oss.str();
    }
    
    void reset() {
        for (auto& pair : neuromodulators) {
            pair.second->setLevel(0.0f);
        }
        levelChangeCallbacks.clear();
        initialized = false;
    }
};

NeuromodulatorSystem::NeuromodulatorSystem() : pImpl(std::make_unique<Impl>()) {}

NeuromodulatorSystem::~NeuromodulatorSystem() = default;

bool NeuromodulatorSystem::initialize(std::shared_ptr<Config> config) {
    if (!config) {
        return false;
    }
    
    // Configure neuromodulators from config
    configureFromConfig(config);
    
    // Initialize each neuromodulator
    for (auto& pair : pImpl->neuromodulators) {
        // In a real implementation, neuromodulators would need initialization
        // For now, we just set them to baseline values
        if (pair.first == "DA") {
            pair.second->setLevel(0.0f);
        } else if (pair.first == "Curiosity") {
            pair.second->setLevel(0.1f);
        } else if (pair.first == "Novelty") {
            pair.second->setLevel(0.1f);
        } else if (pair.first == "PredictionError") {
            pair.second->setLevel(0.0f);
        }
    }
    
    pImpl->initialized = true;
    return true;
}

void NeuromodulatorSystem::update(TimestepDuration dt, Brain* brain) {
    if (!pImpl->initialized) return;
    pImpl->update(dt, brain);
}

Neuromodulator* NeuromodulatorSystem::getNeuromodulator(const std::string& name) {
    auto it = pImpl->neuromodulators.find(name);
    if (it != pImpl->neuromodulators.end()) {
        return it->second;
    }
    return nullptr;
}

float NeuromodulatorSystem::getLevel(const std::string& name) const {
    auto it = pImpl->neuromodulators.find(name);
    if (it != pImpl->neuromodulators.end()) {
        return it->second->getLevel();
    }
    return 0.0f;
}

void NeuromodulatorSystem::setLevel(const std::string& name, float level) {
    auto it = pImpl->neuromodulators.find(name);
    if (it != pImpl->neuromodulators.end()) {
        it->second->setLevel(level);
    }
}

float NeuromodulatorSystem::getPlasticityFactor(const std::string& name) const {
    auto it = pImpl->neuromodulators.find(name);
    if (it != pImpl->neuromodulators.end()) {
        return it->second->getPlasticityFactor();
    }
    return 1.0f;
}

void NeuromodulatorSystem::signalReward(float reward) {
    if (auto* dopamine = pImpl->neuromodulators["DA"]) {
        dopamine->signalReward(reward);
    }
}

void NeuromodulatorSystem::signalPredictionError(float predictionError) {
    if (auto* predError = pImpl->neuromodulators["PredictionError"]) {
        predError->setLevel(predictionError);
    }
}

void NeuromodulatorSystem::signalNovelty(float novelty) {
    if (auto* novelty = pImpl->neuromodulators["Novelty"]) {
        novelty->setLevel(novelty);
    }
}

void NeuromodulatorSystem::signalCuriosity(float curiosity) {
    if (auto* curiosityMod = pImpl->neuromodulators["Curiosity"]) {
        curiosityMod->setLevel(curiosity);
    }
}

std::vector<std::string> NeuromodulatorSystem::getNeuromodulatorNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->neuromodulators.size());
    for (const auto& pair : pImpl->neuromodulators) {
        names.push_back(pair.first);
    }
    return names;
}

void NeuromodulatorSystem::configureFromConfig(const std::shared_ptr<Config>& config) {
    pImpl->configureFromConfig(config);
}

std::string NeuromodulatorSystem::getStatistics() const {
    return pImpl->getStatistics();
}

void NeuromodulatorSystem::reset() {
    pImpl->reset();
}

void NeuromodulatorSystem::registerLevelChangeCallback(
    std::function<void(const std::string&, float)> callback) {
    pImpl->levelChangeCallbacks.push_back(callback);
}

void NeuromodulatorSystem::clearLevelChangeCallbacks() {
    pImpl->levelChangeCallbacks.clear();
}

} // namespace nlm