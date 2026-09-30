#pragma once

#include "core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// ============================================================================
// DESIGN PATTERNS IMPLEMENTATION
// ============================================================================

// ============================================================================
// COMMAND PATTERN
// ============================================================================

/**
 * @class Command
 * @brief Abstract base class for commands (Command pattern)
 * 
 * This pattern encapsulates a request as an object, allowing parameterizing
 * clients with different requests, queuing requests, and supporting undo operations.
 */
class Command {
public:
    virtual ~Command() = default;
    
    /**
     * @brief Execute the command
     * @return true if command executed successfully
     */
    virtual bool execute() = 0;
    
    /**
     * @brief Undo the command
     * @return true if undo successful
     */
    virtual bool undo() = 0;
    
    /**
     * @brief Get command name for debugging
     * @return Command name
     */
    virtual std::string getName() const = 0;
};

/**
 * @class CommandInvoker
 * @brief Command invoker (Command pattern)
 * 
 * This class manages a queue of commands and executes them in sequence.
 * It also supports undo operations and command history.
 */
class CommandInvoker {
public:
    CommandInvoker() = default;
    ~CommandInvoker() = default;
    
    /**
     * @brief Add command to the queue
     * @param command Command to add
     */
    void addCommand(std::unique_ptr<Command> command) {
        commandQueue_.push_back(std::move(command));
    }
    
    /**
     * @brief Execute all commands in the queue
     * @return Number of commands executed
     */
    size_t executeAll() {
        size_t executed = 0;
        for (auto& command : commandQueue_) {
            if (command->execute()) {
                commandHistory_.push_back(std::move(command));
                executed++;
            }
        }
        commandQueue_.clear();
        return executed;
    }
    
    /**
     * @brief Undo last executed command
     * @return true if undo successful
     */
    bool undoLast() {
        if (!commandHistory_.empty()) {
            auto lastCommand = std::move(commandHistory_.back());
            commandHistory_.pop_back();
            return lastCommand->undo();
        }
        return false;
    }
    
    /**
     * @brief Clear command history
     */
    void clearHistory() {
        commandHistory_.clear();
    }
    
    /**
     * @brief Get number of commands in queue
     * @return Queue size
     */
    size_t getQueueSize() const { return commandQueue_.size(); }
    
    /**
     * @brief Get number of commands in history
     * @return History size
     */
    size_t getHistorySize() const { return commandHistory_.size(); }
    
private:
    std::vector<std::unique_ptr<Command>> commandQueue_;
    std::vector<std::unique_ptr<Command>> commandHistory_;
};

/**
 * @class NeuronCommand
 * @brief Concrete command for neuron operations (Command pattern)
 * 
 * This command encapsulates operations on neurons like injecting current,
 * setting membrane potential, etc.
 */
class NeuronCommand : public Command {
public:
    NeuronCommand(std::function<void()> operation, std::function<void()> undoOperation, const std::string& name)
        : operation_(std::move(operation)), undoOperation_(std::move(undoOperation)), name_(name) {}
    
    bool execute() override {
        operation_();
        return true;
    }
    
    bool undo() override {
        undoOperation_();
        return true;
    }
    
    std::string getName() const override { return name_; }
    
private:
    std::function<void()> operation_;
    std::function<void()> undoOperation_;
    std::string name_;
};

/**
 * @class SynapseCommand
 * @brief Concrete command for synapse operations (Command pattern)
 * 
 * This command encapsulates operations on synapses like setting weights,
 * enabling plasticity, etc.
 */
class SynapseCommand : public Command {
public:
    SynapseCommand(std::function<void()> operation, std::function<void()> undoOperation, const std::string& name)
        : operation_(std::move(operation)), undoOperation_(std::move(undoOperation)), name_(name) {}
    
    bool execute() override {
        operation_();
        return true;
    }
    
    bool undo() override {
        undoOperation_();
        return true;
    }
    
    std::string getName() const override { return name_; }
    
private:
    std::function<void()> operation_;
    std::function<void()> undoOperation_;
    std::string name_;
};

// ============================================================================
// OBSERVER PATTERN
// ============================================================================

/**
 * @class Observer
 * @brief Abstract base class for observers (Observer pattern)
 * 
 * This pattern defines a one-to-many dependency between objects so that when
 * one object changes state, all its dependents are notified and updated automatically.
 */
class Observer {
public:
    virtual ~Observer() = default;
    
    /**
     * @brief Update observer with new data
     * @param data New data to update with
     */
    virtual void update(const std::string& data) = 0;
};

/**
 * @class Subject
 * @brief Abstract base class for subjects (Observer pattern)
 * 
 * This class maintains a list of observers and notifies them of state changes.
 */
class Subject {
public:
    virtual ~Subject() = default;
    
    /**
     * @brief Attach an observer to the subject
     * @param observer Observer to attach
     */
    virtual void attach(std::shared_ptr<Observer> observer) = 0;
    
    /**
     * @brief Detach an observer from the subject
     * @param observer Observer to detach
     */
    virtual void detach(std::shared_ptr<Observer> observer) = 0;
    
    /**
     * @brief Notify all observers of state change
     * @param data Data to notify observers with
     */
    virtual void notify(const std::string& data) = 0;
};

/**
 * @class NotificationSubject
 * @brief Concrete implementation of subject (Observer pattern)
 * 
 * This class provides the actual implementation of subject functionality.
 */
class NotificationSubject : public Subject {
public:
    NotificationSubject() = default;
    ~NotificationSubject() = default;
    
    void attach(std::shared_ptr<Observer> observer) override {
        observers_.push_back(observer);
    }
    
    void detach(std::shared_ptr<Observer> observer) override {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
    }
    
    void notify(const std::string& data) override {
        for (const auto& observer : observers_) {
            observer->update(data);
        }
    }
    
    size_t getObserverCount() const { return observers_.size(); }
    
private:
    std::vector<std::shared_ptr<Observer>> observers_;
};

/**
 * @class NeuronStateObserver
 * @brief Observer for neuron state changes (Observer pattern)
 * 
 * This observer monitors neuron state changes and responds accordingly.
 */
class NeuronStateObserver : public Observer {
public:
    NeuronStateObserver() = default;
    ~NeuronStateObserver() = default;
    
    void update(const std::string& data) override {
        // Process neuron state change
        // Could log, analyze, or trigger responses
        onNeuronStateChanged(data);
    }
    
    virtual void onNeuronStateChanged(const std::string& stateData) {
        // Override this method to implement specific behavior
    }
};

/**
 * @class SpikeObserver
 * @brief Observer for spike events (Observer pattern)
 * 
 * This observer monitors neural spiking activity and responds to spikes.
 */
class SpikeObserver : public Observer {
public:
    SpikeObserver() = default;
    ~SpikeObserver() = default;
    
    void update(const std::string& data) override {
        // Process spike event
        onSpikeReceived(data);
    }
    
    virtual void onSpikeReceived(const std::string& spikeData) {
        // Override this method to implement specific behavior
    }
};

// ============================================================================
// STRATEGY PATTERN
// ============================================================================

/**
 * @class PlasticityStrategy
 * @brief Abstract base class for plasticity strategies (Strategy pattern)
 * 
 * This pattern defines a family of algorithms, encapsulates each one, and makes
 * them interchangeable. Strategy lets the algorithm vary independently from
 * clients that use it.
 */
class PlasticityStrategy {
public:
    virtual ~PlasticityStrategy() = default;
    
    /**
     * @brief Calculate plasticity factor for given inputs
     * @param preSpikeHistory Pre-synaptic spike history
     * @param postSpikeHistory Post-synaptic spike history
     * @param timeStep Current time step
     * @return Plasticity factor
     */
    virtual float calculatePlasticity(const std::vector<float>& preSpikeHistory,
                                     const std::vector<float>& postSpikeHistory,
                                     float timeStep) = 0;
};

/**
 * @class STDPModel
 * @brief Concrete STDP strategy (Strategy pattern)
 * 
 * This implements the Spike-Timing-Dependent Plasticity (STDP) learning rule.
 */
class STDPModel : public PlasticityStrategy {
public:
    STDPModel(float ltpWeight, float ltdWeight, float tau)
        : ltpWeight_(ltpWeight), ltdWeight_(ltdWeight), tau_(tau) {}
    
    float calculatePlasticity(const std::vector<float>& preSpikeHistory,
                             const std::vector<float>& postSpikeHistory,
                             float timeStep) override {
        // Implement STDP calculation
        float totalPlasticity = 0.0f;
        
        // Process pre -> post pairings (LTP)
        for (size_t i = 0; i < preSpikeHistory.size(); ++i) {
            for (size_t j = 0; j < postSpikeHistory.size(); ++j) {
                float deltaT = postSpikeHistory[j] - preSpikeHistory[i];
                if (deltaT > 0 && deltaT < tau_) {
                    totalPlasticity += ltpWeight_ * std::exp(-deltaT / tau_);
                }
            }
        }
        
        // Process post -> pre pairings (LTD)
        for (size_t i = 0; i < postSpikeHistory.size(); ++i) {
            for (size_t j = 0; j < preSpikeHistory.size(); ++j) {
                float deltaT = preSpikeHistory[j] - postSpikeHistory[i];
                if (deltaT > 0 && deltaT < tau_) {
                    totalPlasticity -= ltdWeight_ * std::exp(-deltaT / tau_);
                }
            }
        }
        
        return totalPlasticity;
    }
    
private:
    float ltpWeight_;
    float ltdWeight_;
    float tau_;
};

/**
 * @class HebbianModel
 * @brief Concrete Hebbian learning strategy (Strategy pattern)
 * 
 * This implements Hebbian learning rule (neurons that fire together wire together).
 */
class HebbianModel : public PlasticityStrategy {
public:
    HebbianModel(float learningRate) : learningRate_(learningRate) {}
    
    float calculatePlasticity(const std::vector<float>& preSpikeHistory,
                             const std::vector<float>& postSpikeHistory,
                             float timeStep) override {
        // Simple Hebbian rule: strengthen connections when pre and post neurons fire together
        float totalPlasticity = 0.0f;
        
        if (!preSpikeHistory.empty() && !postSpikeHistory.empty()) {
            // Check if neurons fired recently
            float preRecent = preSpikeHistory.back();
            float postRecent = postSpikeHistory.back();
            
            // If both fired recently, strengthen connection
            if (std::abs(preRecent - postRecent) < 2.0f * timeStep) {
                totalPlasticity = learningRate_ * (preRecent + postRecent);
            }
        }
        
        return totalPlasticity;
    }
    
private:
    float learningRate_;
};

/**
 * @class NeuromodulationStrategy
 * @brief Strategy for neuromodulation effects (Strategy pattern)
 * 
 * This strategy determines how neuromodulators affect neural plasticity.
 */
class NeuromodulationStrategy {
public:
    virtual ~NeuromodulationStrategy() = default;
    
    /**
     * @brief Calculate neuromodulation factor
     * @param neuromodulatorLevel Current neuromodulator level
     * @param baselineFactor Baseline plasticity factor
     * @return Modified plasticity factor
     */
    virtual float calculateModulation(float neuromodulatorLevel, float baselineFactor) = 0;
};

/**
 * @class DopamineStrategy
 * @brief Dopamine neuromodulation strategy (Strategy pattern)
 * 
 * This strategy implements dopamine's effect on plasticity.
 */
class DopamineStrategy : public NeuromodulationStrategy {
public:
    DopamineStrategy(float maxModulation, float saturationPoint)
        : maxModulation_(maxModulation), saturationPoint_(saturationPoint) {}
    
    float calculateModulation(float neuromodulatorLevel, float baselineFactor) override {
        // Dopamine enhances plasticity up to a saturation point
        if (neuromodulatorLevel <= saturationPoint_) {
            return baselineFactor * (1.0f + maxModulation_ * (neuromodulatorLevel / saturationPoint_));
        } else {
            return baselineFactor * (1.0f + maxModulation_);
        }
    }
    
private:
    float maxModulation_;
    float saturationPoint_;
};

/**
 * @class RewardStrategy
 * @brief Reward-based learning strategy (Strategy pattern)
 * 
 * This strategy implements reward-modulated learning.
 */
class RewardStrategy {
public:
    virtual ~RewardStrategy() = default;
    
    /**
     * @brief Calculate reward signal
     * @param reward Reward value
     * @param prediction Prediction error
     * @return Reward signal
     */
    virtual float calculateRewardSignal(float reward, float prediction) = 0;
};

/**
 * @class TemporalDifferenceStrategy
 * @brief Temporal difference reward strategy (Strategy pattern)
 * 
 * This implements temporal difference learning for reward prediction.
 */
class TemporalDifferenceStrategy : public RewardStrategy {
public:
    TemporalDifferenceStrategy(float discountFactor) : discountFactor_(discountFactor) {}
    
    float calculateRewardSignal(float reward, float prediction) override {
        return reward + discountFactor_ * prediction - reward;
    }
    
private:
    float discountFactor_;
};

// ============================================================================
// FACTORY PATTERN
// ============================================================================

/**
 * @class PlasticityFactory
 * @brief Factory for creating plasticity strategies (Factory pattern)
 * 
 * This factory creates different types of plasticity strategies based on configuration.
 */
class PlasticityFactory {
public:
    static std::unique_ptr<PlasticityStrategy> createPlasticityStrategy(const std::string& type, const std::vector<float>& params) {
        if (type == "stdp") {
            return std::make_unique<STDPModel>(params[0], params[1], params[2]);
        } else if (type == "hebbian") {
            return std::make_unique<HebbianModel>(params[0]);
        } else {
            throw std::runtime_error("Unknown plasticity strategy: " + type);
        }
    }
};

/**
 * @class NeuromodulationFactory
 * @brief Factory for creating neuromodulation strategies (Factory pattern)
 * 
 * This factory creates different types of neuromodulation strategies.
 */
class NeuromodulationFactory {
public:
    static std::unique_ptr<NeuromodulationStrategy> createNeuromodulationStrategy(const std::string& type, const std::vector<float>& params) {
        if (type == "dopamine") {
            return std::make_unique<DopamineStrategy>(params[0], params[1]);
        } else {
            throw std::runtime_error("Unknown neuromodulation strategy: " + type);
        }
    }
};

/**
 * @class RewardFactory
 * @brief Factory for creating reward strategies (Factory pattern)
 * 
 * This factory creates different types of reward strategies.
 */
class RewardFactory {
public:
    static std::unique_ptr<RewardStrategy> createRewardStrategy(const std::string& type, const std::vector<float>& params) {
        if (type == "temporal_difference") {
            return std::make_unique<TemporalDifferenceStrategy>(params[0]);
        } else {
            throw std::runtime_error("Unknown reward strategy: " + type);
        }
    }
};

/**
 * @class AgentFactory
 * @brief Abstract base factory for creating brain systems (Factory pattern)
 * 
 * This abstract factory defines the interface for creating different types of brain systems.
 */
class AgentFactory {
public:
    virtual ~AgentFactory() = default;
    
    virtual std::unique_ptr<PlasticityStrategy> createPlasticityStrategy() = 0;
    virtual std::unique_ptr<NeuromodulationStrategy> createNeuromodulationStrategy() = 0;
    virtual std::unique_ptr<RewardStrategy> createRewardStrategy() = 0;
};

/**
 * @class DefaultAgentFactory
 * @brief Default factory for creating brain systems (Factory pattern)
 * 
 * This factory creates default implementations of brain system components.
 */
class DefaultAgentFactory : public AgentFactory {
public:
    std::unique_ptr<PlasticityStrategy> createPlasticityStrategy() override {
        std::vector<float> params = {0.02f, 0.015f, 20.0f};  // LTP weight, LTD weight, tau
        return PlasticityFactory::createPlasticityStrategy("stdp", params);
    }
    
    std::unique_ptr<NeuromodulationStrategy> createNeuromodulationStrategy() override {
        std::vector<float> params = {0.5f, 0.8f};  // Max modulation, saturation point
        return NeuromodulationFactory::createNeuromodulationStrategy("dopamine", params);
    }
    
    std::unique_ptr<RewardStrategy> createRewardStrategy() override {
        std::vector<float> params = {0.9f};  // Discount factor
        return RewardFactory::createRewardStrategy("temporal_difference", params);
    }
};

} // namespace nlm
