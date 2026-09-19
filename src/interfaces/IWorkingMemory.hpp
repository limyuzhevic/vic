// IWorkingMemory.hpp
// Abstract interface for working memory systems
// Implements working memory using persistent neural activity patterns
// Supports dependency injection and plugin architecture

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;

class IWorkingMemory {
public:
    virtual ~IWorkingMemory() = default;

    // Initialize working memory with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Store information in working memory
    // The information is encoded as neural activity pattern
    virtual void store(const std::vector<float>& pattern, float strength = 1.0f) = 0;

    // Store specific neuron activation
    virtual void storeToNeuron(NeuronId neuron, float activation) = 0;

    // Retrieve current working memory content as activity levels
    virtual std::vector<float> retrieve() const = 0;

    // Check if specific neuron is part of working memory
    virtual bool contains(NeuronId neuron) const = 0;

    // Get activation level of a specific neuron in working memory
    virtual float getNeuronActivation(NeuronId neuron) const = 0;

    // Update working memory (maintenance and decay)
    virtual void update(TimestepDuration dt) = 0;

    // Clear working memory
    virtual void clear() = 0;

    // Get number of active memory traces
    virtual size_t getActiveTraces() const = 0;

    // Get capacity
    virtual size_t getCapacity() const = 0;
    virtual void setCapacity(size_t cap) = 0;

    // Decay rate for memory traces
    virtual float getDecayRate() const = 0;
    virtual void setDecayRate(float rate) = 0;

    // Get neurons currently in working memory
    virtual const std::vector<NeuronId>& getMemoryNeurons() const = 0;

    // Strengthen working memory representation (for rehearsal)
    virtual void strengthenMemory(float factor) = 0;

    // Competition between memory traces
    virtual void runCompetition() = 0;

    // Is this neuron part of the winning population?
    virtual bool isWinning(NeuronId neuron) const = 0;

    // Get overall memory activity level
    virtual float getMemoryActivity() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IWorkingMemory"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for memory events
    using MemoryStoreCallback = std::function<void(const std::vector<float>& pattern, float strength)>;
    virtual void setStoreCallback(const MemoryStoreCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

protected:
    IWorkingMemory() = default;
};

} // namespace nlm
