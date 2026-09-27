#pragma once

#include <cstdint>
#include <type_traits>
#include <optional>
#include <variant>
#include <string_view>

namespace nlm {

// Strong type aliases with tag dispatching for better type safety
struct NeuronTag {};
struct SynapseTag {};
struct RegionTag {};
struct PopulationTag {};

// Base strongly-typed identifier template
template<typename Tag, typename T = uint64_t>
class StrongId {
    T value;
public:
    constexpr StrongId() : value(0) {}
    explicit constexpr StrongId(T v) : value(v) {}
    constexpr T get() const { return value; }
    constexpr explicit operator T() const { return value; }
    constexpr bool operator==(const StrongId& other) const = default;
    constexpr bool operator!=(const StrongId& other) const = default;
    constexpr bool operator<(const StrongId& other) const { return value < other.value; }
    constexpr T index() const { return value; }
};

// Convenient type definitions
using NeuronId = StrongId<NeuronTag>;
using SynapseId = StrongId<SynapseTag>;
using RegionId = StrongId<RegionTag>;
using PopulationId = StrongId<PopulationTag>;

// Simulation time types with unit safety
struct Timestamp {
    double value;  // seconds
    constexpr explicit Timestamp(double v = 0.0) : value(v) {}
    constexpr double seconds() const { return value; }
    constexpr operator double() const { return value; }
    constexpr bool operator==(const Timestamp& other) const = default;
    constexpr bool operator!=(const Timestamp& other) const = default;
    constexpr bool operator<(const Timestamp& other) const { return value < other.value; }
};

struct TimestepDuration {
    double value;  // seconds
    constexpr explicit TimestepDuration(double v = 0.0) : value(v) {}
    constexpr double seconds() const { return value; }
    constexpr operator double() const { return value; }
};

// Memory efficiency with bit packing where appropriate
struct PlasticityFlags {
    union {
        struct {
            uint8_t hebbian : 1;
            uint8_t stdp : 1;
            uint8_t reward_modulated : 1;
            uint8_t structural : 1;
            uint8_t eligible : 1;
            uint8_t : 0;  // padding
        } bits;
        uint8_t packed;
    };
    
    constexpr PlasticityFlags() : packed(0) {}
    constexpr PlasticityFlags(uint8_t packed_value) : packed(packed_value) {}
    constexpr explicit operator uint8_t() const { return packed; }
    constexpr operator bool() const { return packed != 0; }
};

// Type-safe enumeration with underlying type
enum class NeuronType : uint8_t {
    Excitatory = 1 << 0,
    Inhibitory = 1 << 1,
    Modulatory = 1 << 2,
    Sensory = 1 << 3,
    Motor = 1 << 4,
    Internal = 1 << 5
};

inline constexpr NeuronType operator|(NeuronType a, NeuronType b) {
    return static_cast<NeuronType>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline constexpr bool operator&(NeuronType a, NeuronType b) {
    return (static_cast<uint8_t>(a) & static_cast<uint8_t>(b)) != 0;
}

// Binary algebraic types using std::variant
using SynapticWeight = float;

// Use std::variant for multi-type values
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Error handling with std::expected
struct NeuronError {
    std::string message;
    int code;
};

using NeuronResult = std::expected<NeuronId, NeuronError>;

// Invalid ID sentinel using constexpr inline variable
inline constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
inline constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
inline constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
inline constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

// Compile-time type traits for type safety
namespace type_traits {
    template<typename T>
    struct is_neuron_id : std::false_type {};
    
    template<>
    struct is_neuron_id<NeuronId> : std::true_type {};
    
    template<typename T>
    constexpr bool is_neuron_id_v = is_neuron_id<T>::value;
}

// Compile-time validation functions
namespace compile_time {
    template<typename T>
    constexpr bool is_valid_neuron_id() {
        return type_traits::is_neuron_id_v<T>;
    }
}

// Visitor pattern support for event processing
struct EventVisitor {
    virtual ~EventVisitor() = default;
    virtual void visit(const NeuronId& neuron) = 0;
    virtual void visit(const SynapseId& synapse) = 0;
};

} // namespace nlm
