#include "NeuronGroup.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Forward declaration for MotorActionGroup to avoid duplicate enum
enum class MotorActionGroup {
    Forward,
    Backward,
    TurnLeft,
    TurnRight,
    Interact,
    Wait
};

// Concrete implementation of NeuronGroup
class MotorActionGroupImpl : public NeuronGroup {
private:
    std::vector<Neuron*> neurons_;
    std::string name_;
    MotorActionGroup action_;
    bool dirty_;
    
    NeuronGroupType getType() const override { return NeuronGroupType::Motor; }
    uint32_t getId() const override { return static_cast<uint32_t>(action_); }
    const std::string& getName() const override { return name_; }
    std::vector<Neuron*> getNeurons() const override { return neurons_; }
    Neuron* getNeuron(size_t index) override { return neurons_[index]; }
    const Neuron* getNeuron(size_t index) const override { return neurons_[index]; }
    size_t size() const override { return neurons_.size(); }
    bool empty() const override { return neurons_.empty(); }
    void addNeuron(Neuron* neuron) override {
        neurons_.push_back(neuron);
        markDirty();
    }
    bool removeNeuron(Neuron* neuron) override {
        auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
        if (it != neurons_.end()) {
            neurons_.erase(it);
            markDirty();
            return true;
        }
        return false;
    }
    bool removeNeuron(size_t index) override {
        if (index < neurons_.size()) {
            neurons_.erase(neurons_.begin() + index);
            markDirty();
            return true;
        }
        return false;
    }
    void clear() override {
        neurons_.clear();
        markDirty();
    }
    bool contains(Neuron* neuron) const override {
        return std::find(neurons_.begin(), neurons_.end(), neuron) != neurons_.end();
    }
    size_t getIndex(Neuron* neuron) const override {
        auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
        if (it != neurons_.end()) {
            return std::distance(neurons_.begin(), it);
        }
        return size();
    }
    float getActivityLevel() const override {
        if (neurons_.empty()) return 0.0f;
        
        float totalActivity = 0.0f;
        for (Neuron* neuron : neurons_) {
            const auto& state = neuron->getState();
            totalActivity += std::abs(state.membranePotential - state.restingPotential);
        }
        return totalActivity / neurons_.size();
    }
    float getAverageMembranePotential() const override {
        if (neurons_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* neuron : neurons_) {
            sum += neuron->getMembranePotential();
        }
        return sum / neurons_.size();
    }
    float getAverageFiringRate() const override {
        if (neurons_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* neuron : neurons_) {
            sum += neuron->getFiringRate();
        }
        return sum / neurons_.size();
    }
    size_t getActiveCount() const override {
        size_t count = 0;
        for (Neuron* neuron : neurons_) {
            if (neuron->getMembranePotential() > neuron->getThreshold()) {
                count++;
            }
        }
        return count;
    }
    size_t getFiringCount() const override {
        size_t count = 0;
        for (Neuron* neuron : neurons_) {
            if (neuron->isFiring()) {
                count++;
            }
        }
        return count;
    }
    void reset() override {
        for (Neuron* neuron : neurons_) {
            neuron->reset();
        }
        markDirty();
    }
    void update(double timestep) override {
        // Update any group-specific logic
        markDirty();
    }
    void markDirty() override {
        dirty_ = true;
    }
    bool isDirty() const override {
        return dirty_;
    }
    
public:
    MotorActionGroupImpl(MotorActionGroup action) {
        switch (action) {
            case MotorActionGroup::Forward: name_ = "motor_forward"; break;
            case MotorActionGroup::Backward: name_ = "motor_backward"; break;
            case MotorActionGroup::TurnLeft: name_ = "motor_turn_left"; break;
            case MotorActionGroup::TurnRight: name_ = "motor_turn_right"; break;
            case MotorActionGroup::Interact: name_ = "motor_interact"; break;
            case MotorActionGroup::Wait: name_ = "motor_wait"; break;
        }
        action_ = action;
        dirty_ = true;
    }
    
    // Define action enum
    MotorActionGroup getAction() const { return action_; }
};

// Forward declaration for SensoryInputGroup to avoid duplicate enum
enum class SensoryInputGroup {
    Vision,
    Touch,
    Internal,
    Proprioception
};

class SensoryInputGroupImpl : public NeuronGroup {
private:
    std::vector<Neuron*> neurons_;
    std::string name_;
    SensoryInputGroup input_;
    bool dirty_;
    
    NeuronGroupType getType() const override { return NeuronGroupType::Sensory; }
    uint32_t getId() const override { return static_cast<uint32_t>(input_); }
    const std::string& getName() const override { return name_; }
    std::vector<Neuron*> getNeurons() const override { return neurons_; }
    Neuron* getNeuron(size_t index) override { return neurons_[index]; }
    const Neuron* getNeuron(size_t index) const override { return neurons_[index]; }
    size_t size() const override { return neurons_.size(); }
    bool empty() const override { return neurons_.empty(); }
    void addNeuron(Neuron* neuron) override {
        neurons_.push_back(neuron);
        markDirty();
    }
    bool removeNeuron(Neuron* neuron) override {
        auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
        if (it != neurons_.end()) {
            neurons_.erase(it);
            markDirty();
            return true;
        }
        return false;
    }
    bool removeNeuron(size_t index) override {
        if (index < neurons_.size()) {
            neurons_.erase(neurons_.begin() + index);
            markDirty();
            return true;
        }
        return false;
    }
    void clear() override {
        neurons_.clear();
        markDirty();
    }
    bool contains(Neuron* neuron) const override {
        return std::find(neurons_.begin(), neurons_.end(), neuron) != neurons_.end();
    }
    size_t getIndex(Neuron* neuron) const override {
        auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
        if (it != neurons_.end()) {
            return std::distance(neurons_.begin(), it);
        }
        return size();
    }
    float getActivityLevel() const override {
        if (neurons_.empty()) return 0.0f;
        
        float totalActivity = 0.0f;
        for (Neuron* neuron : neurons_) {
            const auto& state = neuron->getState();
            totalActivity += std::abs(state.membranePotential - state.restingPotential);
        }
        return totalActivity / neurons_.size();
    }
    float getAverageMembranePotential() const override {
        if (neurons_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* neuron : neurons_) {
            sum += neuron->getMembranePotential();
        }
        return sum / neurons_.size();
    }
    float getAverageFiringRate() const override {
        if (neurons_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* neuron : neurons_) {
            sum += neuron->getFiringRate();
        }
        return sum / neurons_.size();
    }
    size_t getActiveCount() const override {
        size_t count = 0;
        for (Neuron* neuron : neurons_) {
            if (neuron->getMembranePotential() > neuron->getThreshold()) {
                count++;
            }
        }
        return count;
    }
    size_t getFiringCount() const override {
        size_t count = 0;
        for (Neuron* neuron : neurons_) {
            if (neuron->isFiring()) {
                count++;
            }
        }
        return count;
    }
    void reset() override {
        for (Neuron* neuron : neurons_) {
            neuron->reset();
        }
        markDirty();
    }
    void update(double timestep) override {
        // Update any group-specific logic
        markDirty();
    }
    void markDirty() override {
        dirty_ = true;
    }
    bool isDirty() const override {
        return dirty_;
    }
    
public:
    SensoryInputGroupImpl(SensoryInputGroup input) {
        switch (input) {
            case SensoryInputGroup::Vision: name_ = "sensory_vision"; break;
            case SensoryInputGroup::Touch: name_ = "sensory_touch"; break;
            case SensoryInputGroup::Internal: name_ = "sensory_internal"; break;
            case SensoryInputGroup::Proprioception: name_ = "sensory_proprioception"; break;
        }
        input_ = input;
        dirty_ = true;
    }
    
    // Define input enum
    SensoryInputGroup getInput() const { return input_; }
};

class NeuronGroupFactoryImpl : public NeuronGroupFactory {
public:
    std::unique_ptr<NeuronGroup> createMotorGroup(MotorActionGroup action) override {
        return std::make_unique<MotorActionGroupImpl>(action);
    }
    
    std::unique_ptr<NeuronGroup> createSensoryGroup(SensoryInputGroup input) override {
        return std::make_unique<SensoryInputGroupImpl>(input);
    }
    
    std::unique_ptr<NeuronGroup> createCustomGroup(const std::string& name, 
                                                 NeuronGroupType type) override {
        return std::make_unique<CustomNeuronGroup>(name, type);
    }
};

class CustomNeuronGroup : public NeuronGroup {
private:
    std::vector<Neuron*> neurons_;
    std::string name_;
    NeuronGroupType type_;
    bool dirty_;
    
    NeuronGroupType getType() const override { return type_; }
    uint32_t getId() const override { return std::hash<std::string>{}(name_); }
    const std::string& getName() const override { return name_; }
    std::vector<Neuron*> getNeurons() const override { return neurons_; }
    Neuron* getNeuron(size_t index) override { return neurons_[index]; }
    const Neuron* getNeuron(size_t index) const override { return neurons_[index]; }
    size_t size() const override { return neurons_.size(); }
    bool empty() const override { return neurons_.empty(); }
    void addNeuron(Neuron* neuron) override {
        neurons_.push_back(neuron);
        markDirty();
    }
    bool removeNeuron(Neuron* neuron) override {
        auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
        if (it != neurons_.end()) {
            neurons_.erase(it);
            markDirty();
            return true;
        }
        return false;
    }
    bool removeNeuron(size_t index) override {
        if (index < neurons_.size()) {
            neurons_.erase(neurons_.begin() + index);
            markDirty();
            return true;
        }
        return false;
    }
    void clear() override {
        neurons_.clear();
        markDirty();
    }
    bool contains(Neuron* neuron) const override {
        return std::find(neurons_.begin(), neurons_.end(), neuron) != neurons_.end();
    }
    size_t getIndex(Neuron* neuron) const override {
        auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
        if (it != neurons_.end()) {
            return std::distance(neurons_.begin(), it);
        }
        return size();
    }
    float getActivityLevel() const override {
        if (neurons_.empty()) return 0.0f;
        
        float totalActivity = 0.0f;
        for (Neuron* neuron : neurons_) {
            const auto& state = neuron->getState();
            totalActivity += std::abs(state.membranePotential - state.restingPotential);
        }
        return totalActivity / neurons_.size();
    }
    float getAverageMembranePotential() const override {
        if (neurons_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* neuron : neurons_) {
            sum += neuron->getMembranePotential();
        }
        return sum / neurons_.size();
    }
    float getAverageFiringRate() const override {
        if (neurons_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* neuron : neurons_) {
            sum += neuron->getFiringRate();
        }
        return sum / neurons_.size();
    }
    size_t getActiveCount() const override {
        size_t count = 0;
        for (Neuron* neuron : neurons_) {
            if (neuron->getMembranePotential() > neuron->getThreshold()) {
                count++;
            }
        }
        return count;
    }
    size_t getFiringCount() const override {
        size_t count = 0;
        for (Neuron* neuron : neurons_) {
            if (neuron->isFiring()) {
                count++;
            }
        }
        return count;
    }
    void reset() override {
        for (Neuron* neuron : neurons_) {
            neuron->reset();
        }
        markDirty();
    }
    void update(double timestep) override {
        // Update any group-specific logic
        markDirty();
    }
    void markDirty() override {
        dirty_ = true;
    }
    bool isDirty() const override {
        return dirty_;
    }
    
public:
    CustomNeuronGroup(const std::string& name, NeuronGroupType type) 
        : name_(name), type_(type) {
        dirty_ = true;
    }
};

// Global factory instance
std::unique_ptr<NeuronGroupFactory> createDefaultNeuronGroupFactory() {
    return std::make_unique<NeuronGroupFactoryImpl>();
}

} // namespace nlm

class SensoryInputGroup : public NeuronGroup {
private:
    std::vector<Neuron*> neurons_;
    std::string name_;
    
    NeuronGroupType getType() const override { return NeuronGroupType::Sensory; }
    uint32_t getId() const override { return static_cast<uint32_t>(input_); }
    const std::string& getName() const override { return name_; }
    std::vector<Neuron*> getNeurons() const override { return neurons_; }
    Neuron* getNeuron(size_t index) override { return neurons_[index]; }
    const Neuron* getNeuron(size_t index) const override { return neurons_[index]; }
    size_t size() const override { return neurons_.size(); }
    bool empty() const override { return neurons_.empty(); }
    void addNeuron(Neuron* neuron) override {
        neurons_.push_back(neuron);
        markDirty();
    }
    bool removeNeuron(Neuron* neuron) override {
        auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
        if (it != neurons_.end()) {
            neurons_.erase(it);
            markDirty();
            return true;
        }
        return false;
    }
    bool removeNeuron(size_t index) override {
        if (index < neurons_.size()) {
            neurons_.erase(neurons_.begin() + index);
            markDirty();
            return true;
        }
        return false;
    }
    void clear() override {
        neurons_.clear();
        markDirty();
    }
    bool contains(Neuron* neuron) const override {
        return std::find(neurons_.begin(), neurons_.end(), neuron) != neurons_.end();
    }
    size_t getIndex(Neuron* neuron) const override {
        auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
        if (it != neurons_.end()) {
            return std::distance(neurons_.begin(), it);
        }
        return size();
    }
    float getActivityLevel() const override {
        if (neurons_.empty()) return 0.0f;
        
        float totalActivity = 0.0f;
        for (Neuron* neuron : neurons_) {
            const auto& state = neuron->getState();
            totalActivity += std::abs(state.membranePotential - state.restingPotential);
        }
        return totalActivity / neurons_.size();
    }
    float getAverageMembranePotential() const override {
        if (neurons_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* neuron : neurons_) {
            sum += neuron->getMembranePotential();
        }
        return sum / neurons_.size();
    }
    float getAverageFiringRate() const override {
        if (neurons_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* neuron : neurons_) {
            sum += neuron->getFiringRate();
        }
        return sum / neurons_.size();
    }
    size_t getActiveCount() const override {
        size_t count = 0;
        for (Neuron* neuron : neurons_) {
            if (neuron->getMembranePotential() > neuron->getThreshold()) {
                count++;
            }
        }
        return count;
    }
    size_t getFiringCount() const override {
        size_t count = 0;
        for (Neuron* neuron : neurons_) {
            if (neuron->isFiring()) {
                count++;
            }
        }
        return count;
    }
    void reset() override {
        for (Neuron* neuron : neurons_) {
            neuron->reset();
        }
        markDirty();
    }
    void update(double timestep) override {
        // Update any group-specific logic
        markDirty();
    }
    void markDirty() override {
        dirty_ = true;
    }
    bool isDirty() const override {
        return dirty_;
    }
    
public:
    SensoryInputGroup(SensoryInputGroup input) {
        switch (input) {
            case SensoryInputGroup::Vision: name_ = "sensory_vision"; break;
            case SensoryInputGroup::Touch: name_ = "sensory_touch"; break;
            case SensoryInputGroup::Internal: name_ = "sensory_internal"; break;
            case SensoryInputGroup::Proprioception: name_ = "sensory_proprioception"; break;
        }
        input_ = input;
        dirty_ = true;
    }
    
    // Define input enum
    SensoryInputGroup getInput() const { return input_; }
    
private:
    SensoryInputGroup input_;
    bool dirty_;
};

class NeuronGroupFactoryImpl : public NeuronGroupFactory {
public:
    std::unique_ptr<NeuronGroup> createMotorGroup(MotorActionGroup action) override {
        return std::make_unique<MotorActionGroup>(action);
    }
    
    std::unique_ptr<NeuronGroup> createSensoryGroup(SensoryInputGroup input) override {
        return std::make_unique<SensoryInputGroup>(input);
    }
    
    std::unique_ptr<NeuronGroup> createCustomGroup(const std::string& name, 
                                                 NeuronGroupType type) override {
        return std::make_unique<CustomNeuronGroup>(name, type);
    }
};

class CustomNeuronGroup : public NeuronGroup {
private:
    std::vector<Neuron*> neurons_;
    std::string name_;
    NeuronGroupType type_;
    
    NeuronGroupType getType() const override { return type_; }
    uint32_t getId() const override { return std::hash<std::string>{}(name_); }
    const std::string& getName() const override { return name_; }
    std::vector<Neuron*> getNeurons() const override { return neurons_; }
    Neuron* getNeuron(size_t index) override { return neurons_[index]; }
    const Neuron* getNeuron(size_t index) const override { return neurons_[index]; }
    size_t size() const override { return neurons_.size(); }
    bool empty() const override { return neurons_.empty(); }
    void addNeuron(Neuron* neuron) override {
        neurons_.push_back(neuron);
        markDirty();
    }
    bool removeNeuron(Neuron* neuron) override {
        auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
        if (it != neurons_.end()) {
            neurons_.erase(it);
            markDirty();
            return true;
        }
        return false;
    }
    bool removeNeuron(size_t index) override {
        if (index < neurons_.size()) {
            neurons_.erase(neurons_.begin() + index);
            markDirty();
            return true;
        }
        return false;
    }
    void clear() override {
        neurons_.clear();
        markDirty();
    }
    bool contains(Neuron* neuron) const override {
        return std::find(neurons_.begin(), neurons_.end(), neuron) != neurons_.end();
    }
    size_t getIndex(Neuron* neuron) const override {
        auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
        if (it != neurons_.end()) {
            return std::distance(neurons_.begin(), it);
        }
        return size();
    }
    float getActivityLevel() const override {
        if (neurons_.empty()) return 0.0f;
        
        float totalActivity = 0.0f;
        for (Neuron* neuron : neurons_) {
            const auto& state = neuron->getState();
            totalActivity += std::abs(state.membranePotential - state.restingPotential);
        }
        return totalActivity / neurons_.size();
    }
    float getAverageMembranePotential() const override {
        if (neurons_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* neuron : neurons_) {
            sum += neuron->getMembranePotential();
        }
        return sum / neurons_.size();
    }
    float getAverageFiringRate() const override {
        if (neurons_.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* neuron : neurons_) {
            sum += neuron->getFiringRate();
        }
        return sum / neurons_.size();
    }
    size_t getActiveCount() const override {
        size_t count = 0;
        for (Neuron* neuron : neurons_) {
            if (neuron->getMembranePotential() > neuron->getThreshold()) {
                count++;
            }
        }
        return count;
    }
    size_t getFiringCount() const override {
        size_t count = 0;
        for (Neuron* neuron : neurons_) {
            if (neuron->isFiring()) {
                count++;
            }
        }
        return count;
    }
    void reset() override {
        for (Neuron* neuron : neurons_) {
            neuron->reset();
        }
        markDirty();
    }
    void update(double timestep) override {
        // Update any group-specific logic
        markDirty();
    }
    void markDirty() override {
        dirty_ = true;
    }
    bool isDirty() const override {
        return dirty_;
    }
    
public:
    CustomNeuronGroup(const std::string& name, NeuronGroupType type) 
        : name_(name), type_(type) {
        dirty_ = true;
    }
    
private:
    bool dirty_;
};

// Global factory instance
std::unique_ptr<NeuronGroupFactory> createDefaultNeuronGroupFactory() {
    return std::make_unique<NeuronGroupFactoryImpl>();
}

} // namespace nlm