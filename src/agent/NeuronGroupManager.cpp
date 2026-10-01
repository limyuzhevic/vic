#include "NeuronGroup.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <functional>

namespace nlm {

struct NeuronGroupManager::Impl {
    std::unique_ptr<NeuronGroupFactory> factory_;
    std::unordered_map<MotorActionGroup, std::unique_ptr<NeuronGroup>> motorGroups_;
    std::unordered_map<SensoryInputGroup, std::unique_ptr<NeuronGroup>> sensoryGroups_;
    std::unordered_map<uint32_t, std::unique_ptr<NeuronGroup>> groupsById_;
    std::unordered_map<std::string, NeuronGroup*> groupsByName_;
    
    bool dirty_ = true;
    bool initialized_ = false;
    
    std::shared_ptr<Brain> brain_;
    
    // Configuration for intelligent distribution
    struct DistributionConfig {
        float visionThreshold = 0.1f;
        float touchThreshold = 0.3f;
        float internalThreshold = 0.2f;
        float proprioceptionThreshold = 0.15f;
        float motorActivityThreshold = 0.5f;
    };
    
    DistributionConfig config_;
    
    Impl(std::unique_ptr<NeuronGroupFactory> factory) 
        : factory_(std::move(factory)) {
    }
    
    void initialize(std::shared_ptr<Brain> brain) {
        brain_ = brain;
        initialized_ = true;
        dirty_ = true;
        
        // Register default motor groups
        registerGroup(MotorActionGroup::Forward, "motor_forward");
        registerGroup(MotorActionGroup::Backward, "motor_backward");
        registerGroup(MotorActionGroup::TurnLeft, "motor_turn_left");
        registerGroup(MotorActionGroup::TurnRight, "motor_turn_right");
        registerGroup(MotorActionGroup::Interact, "motor_interact");
        registerGroup(MotorActionGroup::Wait, "motor_wait");
        
        // Register default sensory groups
        registerGroup(SensoryInputGroup::Vision, "sensory_vision");
        registerGroup(SensoryInputGroup::Touch, "sensory_touch");
        registerGroup(SensoryInputGroup::Internal, "sensory_internal");
        registerGroup(SensoryInputGroup::Proprioception, "sensory_proprioception");
        
        // Initialize default distributions
        setupDefaultDistributions();
    }
    
    void setupDefaultDistributions() {
        // Set up intelligent distribution strategies
        // This would normally use machine learning or learned patterns
    }
    
    void registerGroup(MotorActionGroup actionGroup, const std::string& name) {
        if (motorGroups_.find(actionGroup) == motorGroups_.end()) {
            auto group = factory_->createMotorGroup(actionGroup);
            group->setName(name);
            
            uint32_t id = group->getId();
            motorGroups_[actionGroup] = std::move(group);
            groupsById_[id] = motorGroups_[actionGroup].get();
            groupsByName_[name] = groupsById_[id];
            
            dirty_ = true;
        }
    }
    
    void registerGroup(SensoryInputGroup inputGroup, const std::string& name) {
        if (sensoryGroups_.find(inputGroup) == sensoryGroups_.end()) {
            auto group = factory_->createSensoryGroup(inputGroup);
            group->setName(name);
            
            uint32_t id = group->getId();
            sensoryGroups_[inputGroup] = std::move(group);
            groupsById_[id] = sensoryGroups_[inputGroup].get();
            groupsByName_[name] = groupsById_[id];
            
            dirty_ = true;
        }
    }
    
    void registerGroup(NeuronGroupType type, const std::string& name) {
        auto group = factory_->createCustomGroup(name, type);
        
        uint32_t id = group->getId();
        groupsById_[id] = group.get();
        groupsByName_[name] = groupsById_[id];
        
        // Store custom groups in a separate map
        customGroups_[name] = std::move(group);
        dirty_ = true;
    }
    
    NeuronGroup* getMotorGroup(MotorActionGroup action) {
        auto it = motorGroups_.find(action);
        if (it != motorGroups_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    const NeuronGroup* getMotorGroup(MotorActionGroup action) const {
        auto it = motorGroups_.find(action);
        if (it != motorGroups_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    NeuronGroup* getSensoryGroup(SensoryInputGroup input) {
        auto it = sensoryGroups_.find(input);
        if (it != sensoryGroups_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    const NeuronGroup* getSensoryGroup(SensoryInputGroup input) const {
        auto it = sensoryGroups_.find(input);
        if (it != sensoryGroups_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    NeuronGroup* getGroup(const std::string& name) {
        auto it = groupsByName_.find(name);
        if (it != groupsByName_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    const NeuronGroup* getGroup(const std::string& name) const {
        auto it = groupsByName_.find(name);
        if (it != groupsByName_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    std::vector<NeuronGroup*> getAllGroups() {
        std::vector<NeuronGroup*> allGroups;
        for (auto& pair : groupsById_) {
            allGroups.push_back(pair.second);
        }
        return allGroups;
    }
    
    std::vector<const NeuronGroup*> getAllGroups() const {
        std::vector<const NeuronGroup*> allGroups;
        for (const auto& pair : groupsById_) {
            allGroups.push_back(pair.second);
        }
        return allGroups;
    }
    
    std::vector<NeuronGroup*> getGroupsByType(NeuronGroupType type) {
        std::vector<NeuronGroup*> filteredGroups;
        for (auto& pair : groupsById_) {
            if (pair.second->getType() == type) {
                filteredGroups.push_back(pair.second);
            }
        }
        return filteredGroups;
    }
    
    std::vector<const NeuronGroup*> getGroupsByType(NeuronGroupType type) const {
        std::vector<const NeuronGroup*> filteredGroups;
        for (const auto& pair : groupsById_) {
            if (pair.second->getType() == type) {
                filteredGroups.push_back(pair.second);
            }
        }
        return filteredGroups;
    }
    
    void processNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
        if (!initialized_) {
            return;
        }
        
        // Clear all groups
        for (auto& pair : motorGroups_) {
            pair.second->clear();
        }
        for (auto& pair : sensoryGroups_) {
            pair.second->clear();
        }
        
        // Distribute neurons
        for (const auto& region : regions) {
            for (const auto& population : region->getPopulations()) {
                NeuronType type = population->getNeuronType();
                
                for (Neuron* neuron : population->getNeurons()) {
                    distributeNeuron(neuron);
                }
            }
        }
        
        dirty_ = true;
    }
    
    void distributeNeuron(Neuron* neuron) {
        if (!neuron) return;
        
        // Intelligent distribution based on neuron properties
        NeuronType type = neuron->getType();
        
        if (type == NeuronType::Motor) {
            // Motor neurons: distribute based on properties and learned patterns
            int distributionIndex = calculateMotorDistributionIndex(neuron);
            
            // Use intelligent grouping
            MotorActionGroup actionGroup = static_cast<MotorActionGroup>(distributionIndex % 6);
            NeuronGroup* group = getMotorGroup(actionGroup);
            if (group) {
                group->addNeuron(neuron);
            }
        } else if (type == NeuronType::Sensory) {
            // Sensory neurons: distribute based on sensory modality
            int distributionIndex = calculateSensoryDistributionIndex(neuron);
            
            SensoryInputGroup inputGroup = static_cast<SensoryInputGroup>(distributionIndex % 4);
            NeuronGroup* group = getSensoryGroup(inputGroup);
            if (group) {
                group->addNeuron(neuron);
            }
        }
    }
    
    int calculateMotorDistributionIndex(Neuron* neuron) {
        // Calculate based on neuron properties and learned patterns
        float weight = 0.0f;
        
        // Consider neuron state
        const auto& state = neuron->getState();
        weight += state.membranePotential * 0.3f;
        
        // Consider firing rate
        weight += neuron->getFiringRate() * 0.2f;
        
        // Consider adaptation variable
        weight += state.adaptationVariable * 0.1f;
        
        // Consider last spike time
        if (state.lastSpikeTime >= 0.0f) {
            weight += (1.0f - std::min(1.0f, state.lastSpikeTime / 100.0f)) * 0.2f;
        }
        
        // Consider refractory state
        if (neuron->isRefractory()) {
            weight *= 0.5f;  // Refractory neurons have lower weight
        }
        
        // Normalize to [0, 1]
        weight = std::clamp(weight, 0.0f, 1.0f);
        
        // Map to distribution index
        return static_cast<int>(weight * 5.0f);
    }
    
    int calculateSensoryDistributionIndex(Neuron* neuron) {
        // Calculate based on neuron properties
        float weight = 0.0f;
        
        // Consider neuron state
        const auto& state = neuron->getState();
        weight += state.membranePotential * 0.4f;
        
        // Consider firing rate
        weight += neuron->getFiringRate() * 0.3f;
        
        // Consider total current
        weight += neuron->getTotalCurrent() * 0.1f;
        
        // Normalize to [0, 1]
        weight = std::clamp(weight, 0.0f, 1.0f);
        
        // Map to distribution index
        return static_cast<int>(weight * 3.0f);
    }
    
    void update(double timestep) {
        if (!initialized_) return;
        
        // Update all groups
        for (auto& pair : motorGroups_) {
            pair.second->update(timestep);
        }
        for (auto& pair : sensoryGroups_) {
            pair.second->update(timestep);
        }
        
        // Update custom groups
        for (auto& pair : customGroups_) {
            pair.second->update(timestep);
        }
        
        dirty_ = true;
    }
    
    void reset() {
        if (!initialized_) return;
        
        for (auto& pair : motorGroups_) {
            pair.second->reset();
        }
        for (auto& pair : sensoryGroups_) {
            pair.second->reset();
        }
        
        for (auto& pair : customGroups_) {
            pair.second->reset();
        }
        
        dirty_ = true;
    }
    
    size_t getTotalNeuronCount() const {
        size_t total = 0;
        for (const auto& pair : motorGroups_) {
            total += pair.second->size();
        }
        for (const auto& pair : sensoryGroups_) {
            total += pair.second->size();
        }
        for (const auto& pair : customGroups_) {
            total += pair.second->size();
        }
        return total;
    }
    
    size_t getGroupCount() const {
        return motorGroups_.size() + sensoryGroups_.size() + customGroups_.size();
    }
    
    float getAverageActivityLevel() const {
        if (getGroupCount() == 0) return 0.0f;
        
        float totalActivity = 0.0f;
        size_t groupCount = 0;
        
        for (const auto& pair : motorGroups_) {
            totalActivity += pair.second->getActivityLevel();
            groupCount++;
        }
        for (const auto& pair : sensoryGroups_) {
            totalActivity += pair.second->getActivityLevel();
            groupCount++;
        }
        for (const auto& pair : customGroups_) {
            totalActivity += pair.second->getActivityLevel();
            groupCount++;
        }
        
        return totalActivity / groupCount;
    }
    
    void setFactory(std::unique_ptr<NeuronGroupFactory> factory) {
        factory_ = std::move(factory);
        dirty_ = true;
    }
    
    // Custom groups storage
    std::unordered_map<std::string, std::unique_ptr<NeuronGroup>> customGroups_;
};

NeuronGroupManager::NeuronGroupManager(std::unique_ptr<NeuronGroupFactory> factory)
    : pImpl(std::make_unique<Impl>(std::move(factory))) {
}

NeuronGroupManager::~NeuronGroupManager() = default;

void NeuronGroupManager::initialize(std::shared_ptr<Brain> brain) {
    pImpl->initialize(std::move(brain));
}

void NeuronGroupManager::registerGroup(MotorActionGroup actionGroup, const std::string& name) {
    pImpl->registerGroup(actionGroup, name);
}

void NeuronGroupManager::registerGroup(SensoryInputGroup inputGroup, const std::string& name) {
    pImpl->registerGroup(inputGroup, name);
}

void NeuronGroupManager::registerGroup(NeuronGroupType type, const std::string& name) {
    pImpl->registerGroup(type, name);
}

NeuronGroup* NeuronGroupManager::getMotorGroup(MotorActionGroup action) {
    return pImpl->getMotorGroup(action);
}

const NeuronGroup* NeuronGroupManager::getMotorGroup(MotorActionGroup action) const {
    return pImpl->getMotorGroup(action);
}

NeuronGroup* NeuronGroupManager::getSensoryGroup(SensoryInputGroup input) {
    return pImpl->getSensoryGroup(input);
}

const NeuronGroup* NeuronGroupManager::getSensoryGroup(SensoryInputGroup input) const {
    return pImpl->getSensoryGroup(input);
}

NeuronGroup* NeuronGroupManager::getGroup(const std::string& name) {
    return pImpl->getGroup(name);
}

const NeuronGroup* NeuronGroupManager::getGroup(const std::string& name) const {
    return pImpl->getGroup(name);
}

std::vector<NeuronGroup*> NeuronGroupManager::getAllGroups() {
    return pImpl->getAllGroups();
}

std::vector<const NeuronGroup*> NeuronGroupManager::getAllGroups() const {
    return pImpl->getAllGroups();
}

std::vector<NeuronGroup*> NeuronGroupManager::getGroupsByType(NeuronGroupType type) {
    return pImpl->getGroupsByType(type);
}

std::vector<const NeuronGroup*> NeuronGroupManager::getGroupsByType(NeuronGroupType type) const {
    return pImpl->getGroupsByType(type);
}

void NeuronGroupManager::processNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    pImpl->processNeurons(regions);
}

void NeuronGroupManager::distributeNeuron(Neuron* neuron) {
    pImpl->distributeNeuron(neuron);
}

void NeuronGroupManager::update(double timestep) {
    pImpl->update(timestep);
}

void NeuronGroupManager::reset() {
    pImpl->reset();
}

size_t NeuronGroupManager::getTotalNeuronCount() const {
    return pImpl->getTotalNeuronCount();
}

size_t NeuronGroupManager::getGroupCount() const {
    return pImpl->getGroupCount();
}

float NeuronGroupManager::getAverageActivityLevel() const {
    return pImpl->getAverageActivityLevel();
}

void NeuronGroupManager::setFactory(std::unique_ptr<NeuronGroupFactory> factory) {
    pImpl->setFactory(std::move(factory));
}

} // namespace nlm