# NLM Types and Definitions API

## Overview

The Types and Definitions module provides the foundational type system for the NLM neural simulation framework. It includes strong type definitions for neural identifiers, enumeration types for neural properties, and specialized numeric types for precise neural computation. This type system ensures type safety, prevents logical errors, and provides compile-time guarantees for neural network operations.

## Key Features

- **Strong typing**: Custom ID types prevent logical errors and enable type-safe operations
- **Enumeration types**: Comprehensive type safety for neural states and behaviors
- **Specialized numeric types**: Precise representation of neural quantities with physical units
- **Type safety utilities**: Functions for type validation and conversion
- **Hash support**: Efficient storage and lookup in associative containers
- **Serialization support**: Type-safe serialization for persistence and communication

## Core Type Classes

### NeuronId Type

```cpp
struct NeuronId {
    uint64_t value;
    
    // Constructor
    NeuronId() : value(0) {}
    explicit NeuronId(uint64_t val) : value(val) {}
    
    // Comparisons
    bool operator==(const NeuronId& other) const { return value == other.value; }
    bool operator!=(const NeuronId& other) const { return value != other.value; }
    bool operator<(const NeuronId& other) const { return value < other.value; }
    bool operator<=(const NeuronId& other) const { return value <= other.value; }
    bool operator>(const NeuronId& other) const { return value > other.value; }
    bool operator>=(const NeuronId& other) const { return value >= other.value; }
    
    // Hash for unordered containers
    size_t hash() const { return std::hash<uint64_t>()(value); }
    
    // Conversion
    explicit operator uint64_t() const { return value; }
    explicit operator int() const { return static_cast<int>(value); }
    
    // Arithmetic operations
    NeuronId operator+(uint64_t rhs) const { return NeuronId(value + rhs); }
    NeuronId operator-(uint64_t rhs) const { return NeuronId(value - rhs); }
    NeuronId& operator+=(uint64_t rhs) { value += rhs; return *this; }
};
```

### SynapseId Type

```cpp
struct SynapseId {
    uint64_t value;
    
    SynapseId() : value(0) {}
    explicit SynapseId(uint64_t val) : value(val) {}
    
    // Comparisons
    bool operator==(const SynapseId& other) const { return value == other.value; }
    bool operator!=(const SynapseId& other) const { return value != other.value; }
    bool operator<(const SynapseId& other) const { return value < other.value; }
    
    // Hash for unordered containers
    size_t hash() const { return std::hash<uint64_t>()(value); }
};
```

### RegionId Type

```cpp
struct RegionId {
    uint32_t value;
    
    RegionId() : value(0) {}
    explicit RegionId(uint32_t val) : value(val) {}
    
    // Comparisons and hash (similar to NeuronId)
};
```

### PopulationId Type

```cpp
struct PopulationId {
    uint32_t value;
    
    PopulationId() : value(0) {}
    explicit PopulationId(uint32_t val) : value(val) {}
    
    // Comparisons and hash
};
```

### SynapseHandle Type

```cpp
struct SynapseHandle {
    SynapseId id;
    bool isValid() const { return id.value != 0; }
    void invalidate() { id.value = 0; }
    
    // Comparisons
    bool operator==(const SynapseHandle& other) const { return id == other.id; }
    bool operator!=(const SynapseHandle& other) const { return id != other.id; }
};
```

## Enumeration Types

### NeuronType Enum

```cpp
enum class NeuronType {
    Internal,    // Internal processing neurons
    Sensory,     // Sensory input neurons
    Motor,       // Motor output neurons
    Error,       // Error or warning neurons
    Unknown      // Unknown or unclassified neurons
};
```

### NeuronState Enum

```cpp
enum class NeuronState {
    Resting,           // Neuron at resting potential
    Active,            // Neuron receiving input
    Spiking,           // Neuron currently generating spikes
    Refractory,        // Neuron in refractory period
    Adapting,          // Neuron adapting firing rate
    Dead,              // Neuron no longer functional
    Unknown            // Unknown or unclassified state
};
```

### FiringState Enum

```cpp
enum class FiringState {
    Silent,            // No firing activity
    Regular,           // Regular spiking pattern
    Burst,             // Burst firing pattern
    Oscillating,       // Oscillatory activity
    Adapting,          // Adapting firing rate
    Unknown            // Unknown or unclassified state
};
```

### PlasticityFlags Struct

```cpp
struct PlasticityFlags {
    bool hebbian;      // Enable Hebbian learning
    bool stdp;         // Enable Spike-Timing-Dependent Plasticity
    bool structural;   // Enable structural plasticity
    bool homeostatic;  // Enable homeostatic regulation
    
    PlasticityFlags() : hebbian(false), stdp(false), structural(false), homeostatic(false) {}
    
    bool isEnabled() const {
        return hebbian || stdp || structural || homeostatic;
    }
    
    void enableAll() {
        hebbian = true;
        stdp = true;
        structural = true;
        homeostatic = true;
    }
    
    void disableAll() {
        hebbian = false;
        stdp = false;
        structural = false;
        homeostatic = false;
    }
};
```

### LogLevel Enum

```cpp
enum class LogLevel {
    Debug = 0,        // Detailed debugging information
    Info = 1,          // General information messages
    Warning = 2,       // Warning conditions
    Error = 3,         // Error conditions
    Critical = 4       // Critical errors requiring immediate attention
};
```

### ConfigSource Enum

```cpp
enum class ConfigSource {
    Default,    // Built-in default values
    File,       // Loaded from configuration file
    CommandLine, // Parsed from command line arguments
    Runtime     // Set during runtime
};
```

## Specialized Numeric Types

### MembranePotential Type

```cpp
using MembranePotential = float;

// Constants for common membrane potential values
constexpr MembranePotential RESTING_POTENTIAL = -70.0f;    // mV
constexpr MembranePotential THRESHOLD = -55.0f;           // mV
constexpr MembranePotential RESET_POTENTIAL = -70.0f;     // mV
constexpr MembranePotential EXCITATORY_SYNAPSIS = 0.5f;   // mV
constexpr MembranePotential INHIBITORY_SYNAPSIS = -0.5f;  // mV
```

### FiringRate Type

```cpp
using FiringRate = float;

// Constants for firing rate ranges
constexpr FiringRate MIN_FIRING_RATE = 0.0f;            // Hz
constexpr FiringRate MAX_FIRING_RATE = 1000.0f;         // Hz
constexpr FiringRate TYPICAL_FIRING_RATE = 10.0f;       // Hz
```

### Conductance Type

```cpp
using Conductance = float;

// Constants for conductance values
constexpr Conductance LEAK_CONDUCTANCE = 10.0f;         // nS
constexpr Conductance SYNAPTIC_CONDUCTANCE = 1.0f;       // nS
constexpr Conductance EXCITATORY_CONDUCTANCE = 0.5f;   // nS
constexpr Conductance INHIBITORY_CONDUCTANCE = -0.5f;   // nS
```

### Delay Type

```cpp
sysdays
days using Delay = uint32_t;

// Constants for transmission delays
constexpr Delay MIN_DELAY = 1;    // Minimum synaptic delay (time steps)
constexpr Delay DEFAULT_DELAY = 5; // Default synaptic delay (time steps)
constexpr Delay MAX_DELAY = 100;  // Maximum synaptic delay (time steps)
```

### Timestamp Type

```cpp
using Timestamp = double;

// Constants for time measurements
constexpr Timestamp INVALID_TIMESTAMP = -1.0;
constexpr Timestamp MIN_TIMESTAMP = 0.0;
constexpr Timestamp MAX_TIMESTAMP = 1e6; // Maximum simulation time (seconds)
```

### SimulationStep Type

```cpp
using SimulationStep = uint64_t;

// Constants for simulation steps
constexpr SimulationStep MIN_STEP = 0;
constexpr SimulationStep MAX_STEP = std::numeric_limits<SimulationStep>::max();
```

## Type Utilities

### Type Checking Utilities

```cpp
namespace TypeUtils {
    // Check if a type is an integer
    template<typename T>
    constexpr bool isInteger() {
        return std::is_integral<T>::value;
    }
    
    // Check if a type is a floating point
    template<typename T>
    constexpr bool isFloatingPoint() {
        return std::is_floating_point<T>::value;
    }
    
    // Check if a type is a string
    template<typename T>
    constexpr bool isString() {
        return std::is_same<T, std::string>::value;
    }
    
    // Check if a type is a neuron identifier
    template<typename T>
    constexpr bool isNeuronId() {
        return std::is_same<T, NeuronId>::value;
    }
    
    // Check if a type is a synapse identifier
    template<typename T>
    constexpr bool isSynapseId() {
        return std::is_same<T, SynapseId>::value;
    }
}
```

### Type Conversion Utilities

```cpp
namespace TypeConversions {
    // Convert string to neuron identifier
    NeuronId stringToNeuronId(const std::string& str);
    
    // Convert neuron identifier to string
    std::string neuronIdToString(NeuronId id);
    
    // Convert neuron identifier to integer
    uint64_t neuronIdToInt(NeuronId id);
    
    // Convert integer to neuron identifier
    NeuronId intToNeuronId(uint64_t value);
    
    // Check if a value is within valid range for a type
    template<typename T>
    constexpr bool isValid(const T& value) {
        return !(std::is_signed<T>::value && value < 0) && 
               (std::is_unsigned<T>::value || value >= 0);
    }
}
```

## Type Aliases for Common Combinations

### Brain Component Types

```cpp
// Type aliases for brain components
using NeuronList = std::vector<NeuronId>;
using SynapseList = std::vector<SynapseId>;
using RegionList = std::vector<RegionId>;
using PopulationList = std::vector<PopulationId>;
using ConnectionList = std::vector<std::pair<NeuronId, NeuronId>>;
```

### Neural Network Types

```cpp
// Neural network representation
struct NeuralNetwork {
    std::vector<NeuronId> neurons;
    std::vector<SynapseId> synapses;
    std::map<NeuronId, std::vector<SynapseId>> neuronToSynapses;
    std::map<SynapseId, std::pair<NeuronId, NeuronId>> synapseToConnections;
    
    // Network properties
    size_t getNeuronCount() const { return neurons.size(); }
    size_t getSynapseCount() const { return synapses.size(); }
    size_t getConnectionCount() const { return synapseToConnections.size(); }
    
    // Network operations
    void addNeuron(NeuronId id);
    void addSynapse(SynapseId id, NeuronId pre, NeuronId post);
    void connect(NeuronId pre, NeuronId post);
    bool isConnected(NeuronId pre, NeuronId post) const;
    std::vector<NeuronId> getConnectedNeurons(NeuronId neuron) const;
};
```

### Configuration Types

```cpp
// Configuration type aliases
using ConfigKey = std::string;
using ConfigValue = std::variant<int, int64_t, double, bool, std::string>;
using ConfigMap = std::map<ConfigKey, ConfigValue>;
using ConfigList = std::vector<ConfigValue>;
```

## Hash Support

### Hash Functions for Containers

```cpp
namespace std {
    // Hash for NeuronId
    struct hash<nlm::NeuronId> {
        size_t operator()(const nlm::NeuronId& id) const {
            return std::hash<uint64_t>()(id.value);
        }
    };
    
    // Hash for SynapseId
    struct hash<nlm::SynapseId> {
        size_t operator()(const nlm::SynapseId& id) const {
            return std::hash<uint64_t>()(id.value);
        }
    };
    
    // Hash for RegionId
    struct hash<nlm::RegionId> {
        size_t operator()(const nlm::RegionId& id) const {
            return std::hash<uint32_t>()(id.value);
        }
    };
    
    // Hash for PopulationId
    struct hash<nlm::PopulationId> {
        size_t operator()(const nlm::PopulationId& id) const {
            return std::hash<uint32_t>()(id.value);
        }
    };
    
    // Hash for pair of neuron identifiers
    template<typename T1, typename T2>
    struct hash<std::pair<T1, T2>> {
        size_t operator()(const std::pair<T1, T2>& p) const {
            auto hash1 = std::hash<T1>{}(p.first);
            auto hash2 = std::hash<T2>{}(p.second);
            return hash1 ^ (hash2 << 1);
        }
    };
}
```

## Serialization Support

### Serialization Functions

```cpp
namespace Serialization {
    // Serialize neuron identifier
    void serialize(std::ostream& out, const NeuronId& id);
    void deserialize(std::istream& in, NeuronId& id);
    
    // Serialize synapse identifier
    void serialize(std::ostream& out, const SynapseId& id);
    void deserialize(std::istream& in, SynapseId& id);
    
    // Serialize region identifier
    void serialize(std::ostream& out, const RegionId& id);
    void deserialize(std::istream& in, RegionId& id);
    
    // Serialize population identifier
    void serialize(std::ostream& out, const PopulationId& id);
    void deserialize(std::istream& in, PopulationId& id);
    
    // Serialize enumeration types
    template<typename T>
    typename std::enable_if<std::is_enum<T>::value, void>::type
    serialize(std::ostream& out, const T& value);
    
    template<typename T>
    typename std::enable_if<std::is_enum<T>::value, void>::type
    deserialize(std::istream& in, T& value);
}
```

## Type Safety Examples

### Static Type Checking

```cpp
void processNeuron(const nlm::NeuronId& id) {
    // This function only accepts NeuronId, not SynapseId
    std::cout << "Processing neuron " << id.value << std::endl;
}

int main() {
    nlm::NeuronId neuronId(1);
    nlm::SynapseId synapseId(2);
    
    processNeuron(neuronId);      // OK
    // processNeuron(synapseId); // Compile error - wrong type
}
```

### Runtime Type Validation

```cpp
bool validateNeuronId(const nlm::NeuronId& id) {
    // Validate neuron identifier
    return id.value > 0 && id.value < 1000000;
}

bool validateSynapseId(const nlm::SynapseId& id) {
    // Validate synapse identifier
    return id.value > 0 && id.value < 1000000;
}
```

### Type Conversion

```cpp
void exampleTypeConversions() {
    // String to NeuronId
    std::string neuronStr = "123";
    nlm::NeuronId neuronId = nlm::TypeConversions::stringToNeuronId(neuronStr);
    
    // NeuronId to integer
    uint64_t neuronInt = nlm::TypeConversions::neuronIdToInt(neuronId);
    
    // Integer to NeuronId
    nlm::NeuronId neuronId2 = nlm::TypeConversions::intToNeuronId(neuronInt);
}
```

## Usage Examples

### Using Type Aliases

```cpp
#include <nlm/types/Types.hpp>

int main() {
    // Use type aliases
    nlm::NeuronList neurons;
    neurons.push_back(nlm::NeuronId{1});
    neurons.push_back(nlm::NeuronId{2});
    
    nlm::SynapseList synapses;
    synapses.push_back(nlm::SynapseId{1});
    
    // Process using type-safe interface
    for (const auto& neuronId : neurons) {
        std::cout << "Neuron " << neuronId.value << std::endl;
    }
    
    return 0;
}
```

### Working with Enumeration Types

```cpp
#include <nlm/types/Types.hpp>

void demonstrateEnumerationTypes() {
    // Use enumeration types
    nlm::NeuronType neuronType = nlm::NeuronType::Internal;
    nlm::NeuronState neuronState = nlm::NeuronState::Active;
    nlm::LogLevel logLevel = nlm::LogLevel::Info;
    
    // Switch on enumeration (C++17 structured bindings)
    switch (neuronType) {
        case nlm::NeuronType::Internal:
            std::cout << "Internal neuron" << std::endl;
            break;
        case nlm::NeuronType::Sensory:
            std::cout << "Sensory neuron" << std::endl;
            break;
        case nlm::NeuronType::Motor:
            std::cout << "Motor neuron" << std::endl;
            break;
        default:
            std::cout << "Unknown neuron type" << std::endl;
    }
}
```

### Type Safety in Containers

```cpp
#include <unordered_map>
#include <nlm/types/Types.hpp>

int main() {
    // Use neuron identifiers as keys (hash support)
    std::unordered_map<nlm::NeuronId, float> neuronProperties;
    
    neuronProperties[nlm::NeuronId{1}] = -70.0f;
    neuronProperties[nlm::NeuronId{2}] = -65.0f;
    
    // Access using type-safe interface
    float restingPotential = neuronProperties[nlm::NeuronId{1}];
    
    // Iterate using type-safe interface
    for (const auto& pair : neuronProperties) {
        std::cout << "Neuron " << pair.first.value 
                  << " has resting potential " << pair.second << " mV" << std::endl;
    }
    
    return 0;
}
```

## Performance Considerations

### Memory Efficiency

```cpp
// Use compact storage for type IDs
struct NeuronWithType {
    nlm::NeuronId id;
    nlm::NeuronType type;
    // ... other fields
};

// Compare with individual storage
struct NeuronWithSeparateType {
    uint64_t id;
    uint8_t type;  // Smaller but less type-safe
    // ... other fields
};
```

### Type Checking Overhead

```cpp
// Compile-time type safety has zero runtime overhead
// All type checking is performed by the compiler

// Example: Function with template parameter
template<typename T>
void processNeuron() {
    // Compiler ensures T is a NeuronId at compile time
    // Zero runtime overhead
}

// Usage
processNeuron<nlm::NeuronId>();  // OK at compile time
processNeuron<nlm::SynapseId>(); // Compile error
```

## Testing and Validation

### Unit Testing Type System

```cpp
#include <nlm/types/Types.hpp>
#include <gtest/gtest.h>

TEST(TypeTest, NeuronIdComparisons) {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    
    EXPECT_NE(id1, id2);
    EXPECT_LT(id1, id2);
    EXPECT_LE(id1, id2);
    EXPECT_GT(id2, id1);
    EXPECT_GE(id2, id1);
}

TEST(TypeTest, NeuronIdHash) {
    std::unordered_map<nlm::NeuronId, std::string> map;
    map[nlm::NeuronId{1}] = "Neuron 1";
    map[nlm::NeuronId{2}] = "Neuron 2";
    
    EXPECT_EQ(map[nlm::NeuronId{1}], "Neuron 1");
    EXPECT_EQ(map[nlm::NeuronId{2}], "Neuron 2");
}
```

### Type Conversion Testing

```cpp
TEST(TypeConversionTest, StringToNeuronId) {
    std::string str = "123";
    nlm::NeuronId id = nlm::TypeConversions::stringToNeuronId(str);
    
    EXPECT_EQ(id.value, 123);
}

TEST(TypeConversionTest, NeuronIdToString) {
    nlm::NeuronId id(456);
    std::string str = nlm::TypeConversions::neuronIdToString(id);
    
    EXPECT_EQ(str, "456");
}
```

## Migration Guide

### From Earlier Versions

Type system changes:

- **Version 0.1.0**: Simple integer IDs for all identifiers
- **Version 0.2.0**: Strong typing with custom ID structures
- **Version 0.3.0**: Added enumeration types for neural states
- **Version 0.4.0**: Added specialized numeric types with constants
- **Version 0.5.0**: Enhanced type utilities and conversion functions
- **Version 0.6.0**: Added hash support and serialization

### API Changes

- **Identifier types**: Changed from simple integers to structured types
- **Enumeration types**: Added comprehensive type safety for neural properties
- **Numeric types**: Added specialized types with physical units
- **Type utilities**: Added comprehensive type checking and conversion functions
- **Container support**: Added hash support for efficient storage and lookup
- **Serialization**: Added serialization support for persistence

## References

1. [Type Safety in C++](https://isocpp.org/wiki/faq/const-correctness#const-overloaded-functions)
2. [Enum Classes in C++11](https://isocpp.org/wiki/faq/enum-class)
3. [Hash Requirements](https://en.cppreference.com/w/cpp/named_req/Hash)
4. [Serialization in C++](https://en.cppreference.com/w/cpp/named_req/Serialize)

## See Also

- [Brain API](brain_api.md) - Brain implementation with types
- [Neuron API](neuron_api.md) - Neuron implementation with type safety
- [Synapse API](synapse_api.md) - Synapse implementation with type safety
- [Config API](config_api.md) - Configuration type system
- [Logging API](logging_api.md) - Logging type system

**Last updated:** October 18, 2026
**Version:** 0.6.0