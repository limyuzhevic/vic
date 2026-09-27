#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety

// Neuron identifier
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Region identifier
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Population identifier
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Simulation step counter
using SimulationStep = uint64_t;

// Simulation time in seconds (floating point)
using Timestamp = double;

// Timestep duration in seconds
using TimestepDuration = double;

// Index types for arrays
using NeuronIndex = uint64_t;
using SynapseIndex = uint64_t;
using PopulationIndex = uint64_t;
using RegionIndex = uint64_t;

// Synaptic weight type
using SynapticWeight = float;

// Membrane potential type
using MembranePotential = float;

// Neuron firing rate type
using FiringRate = float;

// Connection delay in simulation steps
using Delay = uint32_t;

// Typed enums for neuron types
enum class NeuronType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};

// Typed enums for synapse types
enum class SynapseType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};

// Synaptic plasticity state flags
struct PlasticityFlags {
    bool hebbian : 1;
    bool stdp : 1;
    bool reward_modulated : 1;
    bool structural : 1;
    bool eligible : 1;
    
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Developmental state
enum class DevelopmentalStage : uint8_t {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

// Spike event for event-driven computation
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event for synaptic transmission with delay
struct DelayedSpikeEvent {
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapseId synapse_id;
    SynapticWeight weight;  // Weight at time of spike (frozen)
    SynapseType synapse_type;
    Timestamp timestamp;      // When spike occurs
    Timestamp delivery_time;  // When spike reaches destination (timestamp + delay)
    SimulationStep step;      // Current simulation step
    SimulationStep delivery_step;  // Step when spike should be delivered
    bool is_excitatory;  // Cached for quick lookup
    
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
};

// Action potential state
enum class FiringState : uint8_t {
    Resting,
    Active,
    Refractory,
    Inhibited
};

// Generic handle types for efficiency
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

// Invalid ID sentinel
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

// Generic circular buffer for fixed-size storage with efficient operations
template<typename T>
class CircularBuffer {
private:
    T* buffer;
    size_t head;
    size_t tail;
    size_t capacity;
    size_t size;
    
public:
    // Construct a circular buffer with fixed capacity
    explicit CircularBuffer(size_t cap) : head(0), tail(0), capacity(cap), size(0) {
        buffer = new T[capacity];
    }
    
    ~CircularBuffer() {
        delete[] buffer;
    }
    
    // Disable copying (pointer management is complex)
    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;
    
    // Enable moving
    CircularBuffer(CircularBuffer&& other) noexcept : 
        buffer(other.buffer), head(other.head), tail(other.tail),
        capacity(other.capacity), size(other.size) {
        other.buffer = nullptr;
        other.head = other.tail = other.size = 0;
    }
    
    CircularBuffer& operator=(CircularBuffer&& other) noexcept {
        if (this != &other) {
            delete[] buffer;
            buffer = other.buffer;
            head = other.head;
            tail = other.tail;
            capacity = other.capacity;
            size = other.size;
            
            other.buffer = nullptr;
            other.head = other.tail = other.size = 0;
        }
        return *this;
    }
    
    // Add element to the end (enqueue)
    void push(const T& value) {
        if (size < capacity) {
            buffer[tail] = value;
            tail = (tail + 1) % capacity;
            ++size;
        } else {
            // Buffer is full, overwrite oldest element
            head = (head + 1) % capacity;
            tail = (tail + 1) % capacity;
            // size remains at capacity
        }
    }
    
    // Remove and return element from the front (dequeue)
    bool pop(T& out) {
        if (size == 0) return false;
        
        out = buffer[head];
        head = (head + 1) % capacity;
        --size;
        return true;
    }
    
    // Get element at index from front (0 = oldest, size-1 = newest)
    bool get(size_t index, T& out) const {
        if (index >= size) return false;
        
        size_t pos = (head + index) % capacity;
        out = buffer[pos];
        return true;
    }
    
    // Get current size
    size_t getSize() const { return size; }
    
    // Get capacity
    size_t getCapacity() const { return capacity; }
    
    // Check if buffer is empty
    bool isEmpty() const { return size == 0; }
    
    // Check if buffer is full
    bool isFull() const { return size == capacity; }
    
    // Clear all elements
    void clear() {
        head = tail = size = 0;
    }
    
    // Get all elements (for compatibility with std::vector interface)
    std::vector<T> getAll() const {
        std::vector<T> result;
        result.reserve(size);
        
        if (size == 0) return result;
        
        for (size_t i = 0; i < size; ++i) {
            T element;
            get(i, element);
            result.push_back(element);
        }
        
        return result;
    }
};

// Specialization for Timestamp type used in spike history
using TimestampBuffer = CircularBuffer<Timestamp>;

// Specialization for SpikeEvent type for event-driven systems
using SpikeEventBuffer = CircularBuffer<SpikeEvent>;

// Specialization for float type for short-term plasticity
using FloatBuffer = CircularBuffer<float>;

// Specialization for NeuronHandle type for synapse management
using NeuronHandleBuffer = CircularBuffer<NeuronHandle>;

// Specialization for SynapseHandle type for synapse management
using SynapseHandleBuffer = CircularBuffer<SynapseHandle>;

// Specialization for SynapticWeight type for plasticity tracking
using WeightBuffer = CircularBuffer<SynapticWeight>;

} // namespace nlm
