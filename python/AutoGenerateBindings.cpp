// Auto-generation helpers for Python bindings
// Contains helper functions to reduce repetitive binding code

#include <pybind11/pybind11.h>
#include <string>
#include <vector>

namespace py = pybind11;
namespace nlm {

// Helper function to generate enum value bindings
declare function<void(py::enum_<T>&, const std::string&, T)> registerEnumValue;

// Template function to register enum with automatic value generation
template<typename T>
void registerEnum(py::module_& m, const std::string& name, const std::string& doc) {
    auto enum_class = py::enum_<T>(m, name.c_str(), doc.c_str());
    
    // This is a placeholder - actual implementation would need to know the enum values
    // In practice, this would be expanded manually for each enum
    // For now, we use the simpler approach with .export_values()
    
    enum_class.export_values();
}

// Helper to create a list of enum values for template generation
struct EnumValue {
    const char* name;
    uint8_t value;
};

// Template function to register enum with value list
template<typename T>
void registerEnumWithValues(py::module_& m, const std::string& name, 
                           const std::string& doc,
                           const std::vector<EnumValue>& values) {
    auto enum_class = py::enum_<T>(m, name.c_str(), doc.c_str());
    
    for (const auto& val : values) {
        enum_class.value(val.name, static_cast<T>(val.value));
    }
    
    enum_class.export_values();
}

// Helper structure for ID type bindings configuration
struct IdTypeConfig {
    const char* name;
    const char* doc;
};

// Template function to register ID types in bulk
template<typename... IdTypes>
void registerIdTypes(py::module_& m) {
    (bindIdType<IdTypes>(m, IdTypes::name, IdTypes::doc), ...);
}

// Template function to register ID types with config
template<typename... IdTypes>
void registerIdTypesWithConfig(py::module_& m, const std::vector<IdTypeConfig>& configs) {
    // This would need runtime type mapping - for now we use the simpler approach
    // In practice, you'd use SFINAE or type traits to match types to configs
    (bindIdType<IdTypes>(m, configs[0].name, configs[0].doc), ...);
}

// Helper for factory function registration
template<typename T, typename... Args>
void registerFactory(py::module_& m, const std::string& name, 
                     std::function<std::shared_ptr<T>(Args...)> func,
                     const std::string& doc) {
    m.def(name.c_str(), func, doc.c_str());
}

// Helper for simple factory function registration
template<typename T, typename... Args>
void registerSimpleFactory(py::module_& m, const std::string& name,
                           std::function<std::shared_ptr<T>(Args...)> func,
                           const std::string& doc) {
    m.def(name.c_str(), func, doc.c_str());
}

// Macro-style helper for enum value registration (simplified)
#define ENUM_VALUE(enum_name, value_name) {#value_name, static_cast<uint8_t>(enum_name::value_name)}

// Function to get enum value list for a specific enum
inline std::vector<EnumValue> getEnumValues(NeuronType) {
    return {
        {"Excitatory", static_cast<uint8_t>(NeuronType::Excitatory)},
        {"Inhibitory", static_cast<uint8_t>(NeuronType::Inhibitory)},
        {"Modulatory", static_cast<uint8_t>(NeuronType::Modulatory)},
        {"Sensory", static_cast<uint8_t>(NeuronType::Sensory)},
        {"Motor", static_cast<uint8_t>(NeuronType::Motor)},
        {"Internal", static_cast<uint8_t>(NeuronType::Internal)}
    };
}

inline std::vector<EnumValue> getEnumValues(SynapseType) {
    return {
        {"Excitatory", static_cast<uint8_t>(SynapseType::Excitatory)},
        {"Inhibitory", static_cast<uint8_t>(SynapseType::Inhibitory)},
        {"Modulatory", static_cast<uint8_t>(SynapseType::Modulatory)},
        {"Electrical", static_cast<uint8_t>(SynapseType::Electrical)},
        {"GapJunction", static_cast<uint8_t>(SynapseType::GapJunction)}
    };
}

inline std::vector<EnumValue> getEnumValues(DevelopmentalStage) {
    return {
        {"Initial", static_cast<uint8_t>(DevelopmentalStage::Initial)},
        {"CriticalPeriod", static_cast<uint8_t>(DevelopmentalStage::CriticalPeriod)},
        {"Maturation", static_cast<uint8_t>(DevelopmentalStage::Maturation)},
        {"Adult", static_cast<uint8_t>(DevelopmentalStage::Adult)},
        {"Aging", static_cast<uint8_t>(DevelopmentalStage::Aging)}
    };
}

inline std::vector<EnumValue> getEnumValues(FiringState) {
    return {
        {"Resting", static_cast<uint8_t>(FiringState::Resting)},
        {"Active", static_cast<uint8_t>(FiringState::Active)},
        {"Refractory", static_cast<uint8_t>(FiringState::Refractory)},
        {"Inhibited", static_cast<uint8_t>(FiringState::Inhibited)}
    };
}

inline std::vector<EnumValue> getEnumValues(ActionType) {
    return {
        {"MoveForward", static_cast<uint8_t>(ActionType::MoveForward)},
        {"MoveBackward", static_cast<uint8_t>(ActionType::MoveBackward)},
        {"MoveLeft", static_cast<uint8_t>(ActionType::MoveLeft)},
        {"MoveRight", static_cast<uint8_t>(ActionType::MoveRight)},
        {"TurnLeft", static_cast<uint8_t>(ActionType::TurnLeft)},
        {"TurnRight", static_cast<uint8_t>(ActionType::TurnRight)},
        {"Look", static_cast<uint8_t>(ActionType::Look)},
        {"LookUp", static_cast<uint8_t>(ActionType::LookUp)},
        {"LookDown", static_cast<uint8_t>(ActionType::LookDown)},
        {"Interact", static_cast<uint8_t>(ActionType::Interact)},
        {"Eat", static_cast<uint8_t>(ActionType::Eat)},
        {"Drink", static_cast<uint8_t>(ActionType::Drink)},
        {"Rest", static_cast<uint8_t>(ActionType::Rest)},
        {"Wait", static_cast<uint8_t>(ActionType::Wait)},
        {"Custom", static_cast<uint8_t>(ActionType::Custom)}
    };
}

inline std::vector<EnumValue> getEnumValues(MotorCommand) {
    return {
        {"MoveForward", static_cast<uint8_t>(MotorCommand::MoveForward)},
        {"MoveBackward", static_cast<uint8_t>(MotorCommand::MoveBackward)},
        {"TurnLeft", static_cast<uint8_t>(MotorCommand::TurnLeft)},
        {"TurnRight", static_cast<uint8_t>(MotorCommand::TurnRight)},
        {"LookLeft", static_cast<uint8_t>(MotorCommand::LookLeft)},
        {"LookRight", static_cast<uint8_t>(MotorCommand::LookRight)},
        {"Interact", static_cast<uint8_t>(MotorCommand::Interact)},
        {"Wait", static_cast<uint8_t>(MotorCommand::Wait)}
    };
}

inline std::vector<EnumValue> getEnumValues(WorldObjectType) {
    return {
        {"Empty", static_cast<uint8_t>(WorldObjectType::Empty)},
        {"Resource", static_cast<uint8_t>(WorldObjectType::Resource)},
        {"Hazard", static_cast<uint8_t>(WorldObjectType::Hazard)},
        {"Wall", static_cast<uint8_t>(WorldObjectType::Wall)},
        {"Marker", static_cast<uint8_t>(WorldObjectType::Marker)}
    };
}

// Template function to register all enum types in bulk
template<typename... EnumTypes>
void registerAllEnums(py::module_& m) {
    (registerEnum<EnumTypes>(m, #EnumTypes, "Enum documentation"), ...);
}

// Helper to convert enum name to readable string
inline std::string enumToReadableName(const std::string& name) {
    std::string result = name;
    size_t pos = 0;
    while ((pos = result.find('_', pos)) != std::string::npos) {
        result.replace(pos, 1, " ");
        pos++;
    }
    return result;
}

} // namespace nlm