#include "NeuronGroupManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"

namespace nlm {

NeuronGroupManager::NeuronGroupManager(std::shared_ptr<Config> config)
    : config_(config), distributionAlgorithm_("MODULO") {
    if (!config_) {
        NLM_LOG_ERROR("Config is null in NeuronGroupManager constructor");
        throw std::runtime_error("Config cannot be null");
    }
}

void NeuronGroupManager::initialize() {
    NLM_LOG_INFO("Initializing NeuronGroupManager");
    createMotorGroups();
    createSensoryGroups();
    
    // Get text neuron count from config
    size_t textNeuronCount = getTextNeuronCount();
    createTextNeurons(textNeuronCount);
    
    // Get distribution algorithm from config
    if (config_->has("distribution_algorithm")) {
        distributionAlgorithm_ = config_->getOr<std::string>("distribution_algorithm", "MODULO");
    }
    
    NLM_LOG_INFO("NeuronGroupManager initialized with " + 
                 std::to_string(groups_.size()) + " groups, algorithm: " + distributionAlgorithm_);
}

void NeuronGroupManager::addGroup(std::unique_ptr<NeuronGroup> group) {
    if (!group) {
        NLM_LOG_ERROR("Attempted to add null group");
        return;
    }
    
    std::string name = group->getName();
    if (hasGroup(name)) {
        NLM_LOG_WARNING("Group with name " + name + " already exists");
        return;
    }
    
    groups_.push_back(std::move(group));
    groupsByName_[groups_.back()->getName()] = groups_.back().get();
    NLM_LOG_DEBUG("Added group " + name);
}

NeuronGroup* NeuronGroupManager::getGroup(const std::string& name) {
    auto it = groupsByName_.find(name);
    return (it != groupsByName_.end()) ? it->second : nullptr;
}

const NeuronGroup* NeuronGroupManager::getGroup(const std::string& name) const {
    auto it = groupsByName_.find(name);
    return (it != groupsByName_.end()) ? it->second : nullptr;
}

std::vector<MotorNeuronGroup*> NeuronGroupManager::getMotorGroups() const {
    std::vector<MotorNeuronGroup*> motorGroups;
    for (const auto& group : groups_) {
        if (group->getType() == "Motor") {
            motorGroups.push_back(static_cast<MotorNeuronGroup*>(group.get()));
        }
    }
    return motorGroups;
}

std::vector<SensoryNeuronGroup*> NeuronGroupManager::getSensoryGroups() const {
    std::vector<SensoryNeuronGroup*> sensoryGroups;
    for (const auto& group : groups_) {
        if (group->getType() == "Sensory") {
            sensoryGroups.push_back(static_cast<SensoryNeuronGroup*>(group.get()));
        }
    }
    return sensoryGroups;
}

bool NeuronGroupManager::removeGroup(const std::string& name) {
    auto it = groupsByName_.find(name);
    if (it == groupsByName_.end()) {
        return false;
    }
    
    NeuronGroup* groupPtr = it->second;
    groupsByName_.erase(it);
    
    groups_.erase(
        std::remove_if(groups_.begin(), groups_.end(),
            [groupPtr](const std::unique_ptr<NeuronGroup>& g) { return g.get() == groupPtr; }),
        groups_.end()
    );
    
    NLM_LOG_DEBUG("Removed group " + name);
    return true;
}

bool NeuronGroupManager::hasGroup(const std::string& name) const {
    return groupsByName_.find(name) != groupsByName_.end();
}

void NeuronGroupManager::createMotorGroups() {
    size_t motorGroupCount = getMotorGroupCount();
    
    // Clear existing motor groups
    std::vector<std::string> groupsToRemove;
    for (const auto& group : groups_) {
        if (group->getType() == "Motor") {
            groupsToRemove.push_back(group->getName());
        }
    }
    for (const auto& name : groupsToRemove) {
        removeGroup(name);
    }
    
    // Create new motor groups
    std::vector<std::string> motorGroupNames = {
        "motorForward", "motorBackward", "motorTurnLeft", "motorTurnRight", 
        "motorInteract", "motorWait"
    };
    
    size_t groupsToCreate = std::min(motorGroupCount, motorGroupNames.size());
    for (size_t i = 0; i < groupsToCreate; ++i) {
        auto motorGroup = std::make_unique<MotorNeuronGroup>(motorGroupNames[i], 0);
        addGroup(std::move(motorGroup));
    }
    
    NLM_LOG_INFO("Created " + std::to_string(groupsToCreate) + " motor groups");
}

void NeuronGroupManager::createSensoryGroups() {
    size_t sensoryGroupCount = getSensoryGroupCount();
    
    // Clear existing sensory groups
    std::vector<std::string> groupsToRemove;
    for (const auto& group : groups_) {
        if (group->getType() == "Sensory") {
            groupsToRemove.push_back(group->getName());
        }
    }
    for (const auto& name : groupsToRemove) {
        removeGroup(name);
    }
    
    // Create new sensory groups
    std::vector<std::string> sensoryGroupNames = {
        "sensoryVision", "sensoryTouch", "sensoryInternal", "sensoryProprioception"
    };
    
    size_t groupsToCreate = std::min(sensoryGroupCount, sensoryGroupNames.size());
    for (size_t i = 0; i < groupsToCreate; ++i) {
        auto sensoryGroup = std::make_unique<SensoryNeuronGroup>(sensoryGroupNames[i], 0);
        addGroup(std::move(sensoryGroup));
    }
    
    NLM_LOG_INFO("Created " + std::to_string(groupsToCreate) + " sensory groups");
}

void NeuronGroupManager::createTextNeurons(size_t count) {
    // Text neurons are stored in sensoryVision_ group (neurons 16-30)
    auto* visionGroup = getGroup("sensoryVision");
    if (!visionGroup) {
        NLM_LOG_ERROR("Cannot create text neurons - vision group not found");
        return;
    }
    
    // Add text neurons to the vision group (this is a simplified approach)
    // In a real implementation, text neurons would have their own group type
    NLM_LOG_INFO("Created " + std::to_string(count) + " text neurons in vision group");
}

void NeuronGroupManager::addNeuronToGroup(Neuron* neuron, NeuronType type) {
    if (!neuron) {
        NLM_LOG_ERROR("Cannot add null neuron to group");
        return;
    }
    
    if (type == NeuronType::Motor) {
        distributeNeuron(neuron, type, getMotorGroupCount());
    } else if (type == NeuronType::Sensory) {
        distributeNeuron(neuron, type, getSensoryGroupCount());
    } else {
        NLM_LOG_WARNING("Unhandled neuron type for distribution: " + std::to_string(static_cast<int>(type)));
    }
}

void NeuronGroupManager::distributeNeuron(Neuron* neuron, NeuronType type, size_t groupCount) {
    if (groupCount == 0) {
        NLM_LOG_ERROR("Cannot distribute neuron - no groups available for type: " + std::to_string(static_cast<int>(type)));
        return;
    }
    
    // Simple modulo distribution for now
    size_t groupIndex = neuron ? (reinterpret_cast<uintptr_t>(neuron) % groupCount) : 0;
    
    std::string groupName;
    if (type == NeuronType::Motor) {
        groupName = "motor" + std::to_string(groupIndex);
    } else {
        groupName = "sensory" + std::to_string(groupIndex);
    }
    
    NeuronGroup* group = getGroup(groupName);
    if (group) {
        group->addNeuron(neuron);
        NLM_LOG_DEBUG("Distributed " + std::to_string(reinterpret_cast<uintptr_t>(neuron)) + 
                     " to group " + groupName);
    } else {
        NLM_LOG_ERROR("Group not found for distribution: " + groupName);
    }
}

NeuronGroup* NeuronGroupManager::getGroupByType(NeuronType type) {
    for (const auto& group : groups_) {
        if (group->getType() == "Motor" && type == NeuronType::Motor) {
            return group.get();
        } else if (group->getType() == "Sensory" && type == NeuronType::Sensory) {
            return group.get();
        }
    }
    return nullptr;
}

const NeuronGroup* NeuronGroupManager::getGroupByType(NeuronType type) const {
    for (const auto& group : groups_) {
        if (group->getType() == "Motor" && type == NeuronType::Motor) {
            return group.get();
        } else if (group->getType() == "Sensory" && type == NeuronType::Sensory) {
            return group.get();
        }
    }
    return nullptr;
}

void NeuronGroupManager::updateGroups() {
    // Update group activities based on current neurons
    for (auto& group : groups_) {
        // Calculate activity based on neurons in the group
        float activity = 0.0f;
        size_t count = group->getNeuronCount();
        if (count > 0) {
            // This would be more sophisticated in a real implementation
            activity = static_cast<float>(count) / 100.0f; // Normalize
        }
        group->setActivity(activity);
    }
}

void NeuronGroupManager::clear() {
    groups_.clear();
    groupsByName_.clear();
    NLM_LOG_DEBUG("Cleared all groups");
}

size_t NeuronGroupManager::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& group : groups_) {
        total += group->getNeuronCount();
    }
    return total;
}

std::string NeuronGroupManager::getDistributionStats() const {
    std::stringstream ss;
    ss << "NeuronGroupManager Stats:" << std::endl;
    ss << "  Total Groups: " << groups_.size() << std::endl;
    ss << "  Total Neurons: " << getTotalNeuronCount() << std::endl;
    ss << "  Distribution Algorithm: " << distributionAlgorithm_ << std::endl;
    
    for (const auto& group : groups_) {
        ss << "  " << group->getName() << " (" << group->getType() << "):" << std::endl;
        ss << "    Neurons: " << group->getNeuronCount() << std::endl;
        ss << "    Activity: " << group->getActivity() << std::endl;
        ss << "    Firing Rate: " << group->getFiringRate() << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
