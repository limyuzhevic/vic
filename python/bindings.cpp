#include <unordered_set>

namespace py = pybind11;
namespace nlm {

// Exception translation helper
inline void translateException(const std::exception& e) {
    std::string msg = e.what();
    
    if (const std::runtime_error* runtimeError = dynamic_cast<const std::runtime_error*>(&e)) {
        throw py::value_error(runtimeError->what());
    } else if (const std::invalid_argument* invalidArg = dynamic_cast<const std::invalid_argument*>(&e)) {
        throw py::value_error(std::string("Invalid argument: ") + invalidArg->what());
    } else if (const std::out_of_range* outOfRange = dynamic_cast<const std::out_of_range*>(&e)) {
        throw py::index_error(std::string("Out of range: ") + outOfRange->what());
    } else if (const std::length_error* lengthError = dynamic_cast<const std::length_error*>(&e)) {
        throw py::value_error(std::string("Length error: ") + lengthError->what());
    } else if (const std::bad_alloc* badAlloc = dynamic_cast<const std::bad_alloc*>(&e)) {
        throw py::memory_error(std::string("Memory allocation failed: ") + badAlloc->what());
    } else {
        throw py::runtime_error(std::string("C++ exception: ") + msg);
    }
}

// Parameter validation helpers
inline void validateNonEmpty(const std::string& value, const std::string& paramName) {
    if (value.empty()) {
        throw std::invalid_argument(paramName + " cannot be empty");
    }
}

inline void validatePositive(const size_t value, const std::string& paramName) {
    if (value == 0) {
        throw std::invalid_argument(paramName + " must be positive");
    }
}

inline void validateRange(const size_t value, size_t min, size_t max, const std::string& paramName) {
    if (value < min || value > max) {
        throw std::out_of_range(paramName + " must be in range [" + 
                              std::to_string(min) + ", " + std::to_string(max) + "]");
    }
}

inline void validateFloatRange(const float value, float min, float max, const std::string& paramName) {
    if (value < min || value > max) {
        throw std::out_of_range(paramName + " must be in range [" + 
                              std::to_string(min) + ", " + std::to_string(max) + "]");
    }
}

inline void validateType(const NeuronType& value) {
    static const std::unordered_set<NeuronType> validTypes = {
        NeuronType::Excitatory, NeuronType::Inhibitory, NeuronType::Modulatory,
        NeuronType::Sensory, NeuronType::Motor, NeuronType::Internal
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid NeuronType value");
    }
}

inline void validateType(const SynapseType& value) {
    static const std::unordered_set<SynapseType> validTypes = {
        SynapseType::Excitatory, SynapseType::Inhibitory, SynapseType::Modulatory,
        SynapseType::Electrical, SynapseType::GapJunction
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid SynapseType value");
    }
}

inline void validateType(const DevelopmentalStage& value) {
    static const std::unordered_set<DevelopmentalStage> validTypes = {
        DevelopmentalStage::Initial, DevelopmentalStage::CriticalPeriod,
        DevelopmentalStage::Maturation, DevelopmentalStage::Adult,
        DevelopmentalStage::Aging
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid DevelopmentalStage value");
    }
}

inline void validateType(const ActionType& value) {
    static const std::unordered_set<ActionType> validTypes = {
        ActionType::MoveForward, ActionType::MoveBackward, ActionType::MoveLeft,
        ActionType::MoveRight, ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Look, ActionType::LookUp, ActionType::LookDown,
        ActionType::Interact, ActionType::Eat, ActionType::Drink,
        ActionType::Rest, ActionType::Wait, ActionType::Custom
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid ActionType value");
    }
}

inline void validateType(const MotorCommand& value) {
    static const std::unordered_set<MotorCommand> validTypes = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward, MotorCommand::TurnLeft,
        MotorCommand::TurnRight, MotorCommand::LookLeft, MotorCommand::LookRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid MotorCommand value");
    }
}

inline void validateType(const WorldObjectType& value) {
    static const std::unordered_set<WorldObjectType> validTypes = {
        WorldObjectType::Empty, WorldObjectType::Resource,
        WorldObjectType::Hazard, WorldObjectType::Wall,
        WorldObjectType::Marker
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid WorldObjectType value");
    }
}

inline void validateNeuronId(const NeuronId& neuron, size_t maxNeurons) {
    if (neuron.value >= maxNeurons) {
        throw std::out_of_range("NeuronId out of range");
    }
}

inline void validateRegionId(const RegionId& region, size_t maxRegions) {
    if (region.value >= maxRegions) {
        throw std::out_of_range("RegionId out of range");
    }
}

inline void validatePopulationId(const PopulationId& population, size_t maxPopulations) {
    if (population.value >= maxPopulations) {
        throw std::out_of_range("PopulationId out of range");
    }
}

inline void validatePosition(float x, float y, float width, float height) {
    validateFloatRange(x, 0.0f, width, "x coordinate");
    validateFloatRange(y, 0.0f, height, "y coordinate");
}

inline void validateDimensions(size_t width, size_t height, size_t minWidth = 1, size_t minHeight = 1) {
    validateRange(width, minWidth, SIZE_MAX, "width");
    validateRange(height, minHeight, SIZE_MAX, "height");
}

inline void validateConfigKey(const std::string& key) {
    validateNonEmpty(key, "key");
}

inline void validateSensoryData(const std::vector<float>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Sensory data cannot be empty");
    }
}

inline void validateTimestamp(double timestamp) {
    if (timestamp < 0.0) {
        throw std::invalid_argument("Timestamp cannot be negative");
    }
}

inline void validateEnergy(float energy, float maxEnergy) {
    if (energy < 0.0f || energy > maxEnergy) {
        throw std::invalid_argument("Energy must be in valid range [0, " + 
                                   std::to_string(maxEnergy) + "]");
    }
}

inline void validateReward(float reward) {
    if (std::isnan(reward) || std::isinf(reward)) {
        throw std::invalid_argument("Reward cannot be NaN or infinite");
    }
}

inline void validateNeuronType(const NeuronId& neuron) {
    if (neuron.value == static_cast<uint64_t>(INVALID_NEURON_ID.value)) {
        throw std::invalid_argument("NeuronId is invalid");
    }
}

inline void validateRegionType(const RegionId& region) {
    if (region.value == static_cast<uint64_t>(INVALID_REGION_ID.value)) {
        throw std::invalid_argument("RegionId is invalid");
    }
}

inline void validateWorldObjectType(const WorldObjectType& type) {
    validateType(type);
}

inline void validateReferenceValid(bool isValid, const std::string& objectType) {
    if (!isValid) {
        throw std::invalid_argument(objectType + " reference is null or invalid");
    }
}

inline bool isValidEnumValue(NeuronType value) {
    static const std::unordered_set<NeuronType> validTypes = {
        NeuronType::Excitatory, NeuronType::Inhibitory, NeuronType::Modulatory,
        NeuronType::Sensory, NeuronType::Motor, NeuronType::Internal
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(SynapseType value) {
    static const std::unordered_set<SynapseType> validTypes = {
        SynapseType::Excitatory, SynapseType::Inhibitory, SynapseType::Modulatory,
        SynapseType::Electrical, SynapseType::GapJunction
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(DevelopmentalStage value) {
    static const std::unordered_set<DevelopmentalStage> validTypes = {
        DevelopmentalStage::Initial, DevelopmentalStage::CriticalPeriod,
        DevelopmentalStage::Maturation, DevelopmentalStage::Adult,
        DevelopmentalStage::Aging
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(ActionType value) {
    static const std::unordered_set<ActionType> validTypes = {
        ActionType::MoveForward, ActionType::MoveBackward, ActionType::MoveLeft,
        ActionType::MoveRight, ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Look, ActionType::LookUp, ActionType::LookDown,
        ActionType::Interact, ActionType::Eat, ActionType::Drink,
        ActionType::Rest, ActionType::Wait, ActionType::Custom
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(MotorCommand value) {
    static const std::unordered_set<MotorCommand> validTypes = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward, MotorCommand::TurnLeft,
        MotorCommand::TurnRight, MotorCommand::LookLeft, MotorCommand::LookRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(WorldObjectType value) {
    static const std::unordered_set<WorldObjectType> validTypes = {
        WorldObjectType::Empty, WorldObjectType::Resource,
        WorldObjectType::Hazard, WorldObjectType::Wall,
        WorldObjectType::Marker
    };
    return validTypes.find(value) != validTypes.end();
}

inline const char* toConstCharPtr(const std::string& str) {
    return str.c_str();
}

inline std::string toString(const std::string& str) {
    return str;
}

// File I/O error handling
inline bool safeLoadFile(const std::string& filepath, std::string& content, const std::string& operation) {
    try {
        validateNonEmpty(filepath, "filepath");
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for " + operation + ": " + filepath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
        
        if (content.empty() && operation.find("load") != std::string::npos) {
            throw std::runtime_error("File is empty: " + filepath);
        }
        
        return true;
    } catch (const std::exception& e) {
        translateException(e);
        return false;
    }
}

inline bool safeSaveFile(const std::string& filepath, const std::string& content, const std::string& operation) {
    try {
        validateNonEmpty(filepath, "filepath");
        validateNonEmpty(content, "content");
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for " + operation + ": " + filepath);
        }
        
        file << content;
        if (!file.good()) {
            throw std::runtime_error("Failed to write to file: " + filepath);
        }
        
        return true;
    } catch (const std::exception& e) {
        translateException(e);
        return false;
    }
}

inline void validateType(const NeuronType& value) {
    static const std::unordered_set<NeuronType> validTypes = {
        NeuronType::Excitatory, NeuronType::Inhibitory, NeuronType::Modulatory,
        NeuronType::Sensory, NeuronType::Motor, NeuronType::Internal
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid NeuronType value");
    }
}

inline void validateType(const SynapseType& value) {
    static const std::unordered_set<SynapseType> validTypes = {
        SynapseType::Excitatory, SynapseType::Inhibitory, SynapseType::Modulatory,
        SynapseType::Electrical, SynapseType::GapJunction
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid SynapseType value");
    }
}

inline void validateType(const DevelopmentalStage& value) {
    static const std::unordered_set<DevelopmentalStage> validTypes = {
        DevelopmentalStage::Initial, DevelopmentalStage::CriticalPeriod,
        DevelopmentalStage::Maturation, DevelopmentalStage::Adult,
        DevelopmentalStage::Aging
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid DevelopmentalStage value");
    }
}

inline void validateType(const ActionType& value) {
    static const std::unordered_set<ActionType> validTypes = {
        ActionType::MoveForward, ActionType::MoveBackward, ActionType::MoveLeft,
        ActionType::MoveRight, ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Look, ActionType::LookUp, ActionType::LookDown,
        ActionType::Interact, ActionType::Eat, ActionType::Drink,
        ActionType::Rest, ActionType::Wait, ActionType::Custom
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid ActionType value");
    }
}

inline void validateType(const MotorCommand& value) {
    static const std::unordered_set<MotorCommand> validTypes = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward, MotorCommand::TurnLeft,
        MotorCommand::TurnRight, MotorCommand::LookLeft, MotorCommand::LookRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid MotorCommand value");
    }
}

inline void validateType(const WorldObjectType& value) {
    static const std::unordered_set<WorldObjectType> validTypes = {
        WorldObjectType::Empty, WorldObjectType::Resource,
        WorldObjectType::Hazard, WorldObjectType::Wall,
        WorldObjectType::Marker
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid WorldObjectType value");
    }
}

inline void validateNeuronId(const NeuronId& neuron, size_t maxNeurons) {
    if (neuron.value >= maxNeurons) {
        throw std::out_of_range("NeuronId out of range");
    }
}

inline void validateRegionId(const RegionId& region, size_t maxRegions) {
    if (region.value >= maxRegions) {
        throw std::out_of_range("RegionId out of range");
    }
}

inline void validatePopulationId(const PopulationId& population, size_t maxPopulations) {
    if (population.value >= maxPopulations) {
        throw std::out_of_range("PopulationId out of range");
    }
}

inline void validatePosition(float x, float y, float width, float height) {
    validateFloatRange(x, 0.0f, width, "x coordinate");
    validateFloatRange(y, 0.0f, height, "y coordinate");
}

inline void validateDimensions(size_t width, size_t height, size_t minWidth = 1, size_t minHeight = 1) {
    validateRange(width, minWidth, SIZE_MAX, "width");
    validateRange(height, minHeight, SIZE_MAX, "height");
}

inline void validateConfigKey(const std::string& key) {
    validateNonEmpty(key, "key");
}

inline void validateSensoryData(const std::vector<float>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Sensory data cannot be empty");
    }
}

inline void validateTimestamp(double timestamp) {
    if (timestamp < 0.0) {
        throw std::invalid_argument("Timestamp cannot be negative");
    }
}

inline void validateEnergy(float energy, float maxEnergy) {
    if (energy < 0.0f || energy > maxEnergy) {
        throw std::invalid_argument("Energy must be in valid range [0, " + 
                                   std::to_string(maxEnergy) + "]");
    }
}

inline void validateReward(float reward) {
    if (std::isnan(reward) || std::isinf(reward)) {
        throw std::invalid_argument("Reward cannot be NaN or infinite");
    }
}

inline void validateNeuronType(const NeuronId& neuron) {
    if (neuron.value == static_cast<uint64_t>(INVALID_NEURON_ID.value)) {
        throw std::invalid_argument("NeuronId is invalid");
    }
}

inline void validateRegionType(const RegionId& region) {
    if (region.value == static_cast<uint64_t>(INVALID_REGION_ID.value)) {
        throw std::invalid_argument("RegionId is invalid");
    }
}

inline void validateWorldObjectType(const WorldObjectType& type) {
    validateType(type);
}

inline void validateReferenceValid(bool isValid, const std::string& objectType) {
    if (!isValid) {
        throw std::invalid_argument(objectType + " reference is null or invalid");
    }
}

inline bool isValidEnumValue(NeuronType value) {
    static const std::unordered_set<NeuronType> validTypes = {
        NeuronType::Excitatory, NeuronType::Inhibitory, NeuronType::Modulatory,
        NeuronType::Sensory, NeuronType::Motor, NeuronType::Internal
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(SynapseType value) {
    static const std::unordered_set<SynapseType> validTypes = {
        SynapseType::Excitatory, SynapseType::Inhibitory, SynapseType::Modulatory,
        SynapseType::Electrical, SynapseType::GapJunction
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(DevelopmentalStage value) {
    static const std::unordered_set<DevelopmentalStage> validTypes = {
        DevelopmentalStage::Initial, DevelopmentalStage::CriticalPeriod,
        DevelopmentalStage::Maturation, DevelopmentalStage::Adult,
        DevelopmentalStage::Aging
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(ActionType value) {
    static const std::unordered_set<ActionType> validTypes = {
        ActionType::MoveForward, ActionType::MoveBackward, ActionType::MoveLeft,
        ActionType::MoveRight, ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Look, ActionType::LookUp, ActionType::LookDown,
        ActionType::Interact, ActionType::Eat, ActionType::Drink,
        ActionType::Rest, ActionType::Wait, ActionType::Custom
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(MotorCommand value) {
    static const std::unordered_set<MotorCommand> validTypes = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward, MotorCommand::TurnLeft,
        MotorCommand::TurnRight, MotorCommand::LookLeft, MotorCommand::LookRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(WorldObjectType value) {
    static const std::unordered_set<WorldObjectType> validTypes = {
        WorldObjectType::Empty, WorldObjectType::Resource,
        WorldObjectType::Hazard, WorldObjectType::Wall,
        WorldObjectType::Marker
    };
    return validTypes.find(value) != validTypes.end();
}

inline const char* toConstCharPtr(const std::string& str) {
    return str.c_str();
}

inline std::string toString(const std::string& str) {
    return str;
}

namespace py = pybind11;
namespace nlm {

// Exception translation helper
inline void translateException(const std::exception& e) {
    std::string msg = e.what();
    
    if (const std::runtime_error* runtimeError = dynamic_cast<const std::runtime_error*>(&e)) {
        throw py::value_error(runtimeError->what());
    } else if (const std::invalid_argument* invalidArg = dynamic_cast<const std::invalid_argument*>(&e)) {
        throw py::value_error(std::string("Invalid argument: ") + invalidArg->what());
    } else if (const std::out_of_range* outOfRange = dynamic_cast<const std::out_of_range*>(&e)) {
        throw py::index_error(std::string("Out of range: ") + outOfRange->what());
    } else if (const std::length_error* lengthError = dynamic_cast<const std::length_error*>(&e)) {
        throw py::value_error(std::string("Length error: ") + lengthError->what());
    } else if (const std::bad_alloc* badAlloc = dynamic_cast<const std::bad_alloc*>(&e)) {
        throw py::memory_error(std::string("Memory allocation failed: ") + badAlloc->what());
    } else {
        throw py::runtime_error(std::string("C++ exception: ") + msg);
    }
}

// Parameter validation helpers
inline void validateNonEmpty(const std::string& value, const std::string& paramName) {
    if (value.empty()) {
        throw std::invalid_argument(paramName + " cannot be empty");
    }
}

inline void validatePositive(const size_t value, const std::string& paramName) {
    if (value == 0) {
        throw std::invalid_argument(paramName + " must be positive");
    }
}

inline void validateRange(const size_t value, size_t min, size_t max, const std::string& paramName) {
    if (value < min || value > max) {
        throw std::out_of_range(paramName + " must be in range [" + 
                              std::to_string(min) + ", " + std::to_string(max) + "]");
    }
}

inline void validateFloatRange(const float value, float min, float max, const std::string& paramName) {
    if (value < min || value > max) {
        throw std::out_of_range(paramName + " must be in range [" + 
                              std::to_string(min) + ", " + std::to_string(max) + "]");
    }
}

inline void validateType(const NeuronType& value) {
    static const std::unordered_set<NeuronType> validTypes = {
        NeuronType::Excitatory, NeuronType::Inhibitory, NeuronType::Modulatory,
        NeuronType::Sensory, NeuronType::Motor, NeuronType::Internal
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid NeuronType value");
    }
}

inline void validateType(const SynapseType& value) {
    static const std::unordered_set<SynapseType> validTypes = {
        SynapseType::Excitatory, SynapseType::Inhibitory, SynapseType::Modulatory,
        SynapseType::Electrical, SynapseType::GapJunction
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid SynapseType value");
    }
}

inline void validateType(const DevelopmentalStage& value) {
    static const std::unordered_set<DevelopmentalStage> validTypes = {
        DevelopmentalStage::Initial, DevelopmentalStage::CriticalPeriod,
        DevelopmentalStage::Maturation, DevelopmentalStage::Adult,
        DevelopmentalStage::Aging
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid DevelopmentalStage value");
    }
}

inline void validateType(const ActionType& value) {
    static const std::unordered_set<ActionType> validTypes = {
        ActionType::MoveForward, ActionType::MoveBackward, ActionType::MoveLeft,
        ActionType::MoveRight, ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Look, ActionType::LookUp, ActionType::LookDown,
        ActionType::Interact, ActionType::Eat, ActionType::Drink,
        ActionType::Rest, ActionType::Wait, ActionType::Custom
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid ActionType value");
    }
}

inline void validateType(const MotorCommand& value) {
    static const std::unordered_set<MotorCommand> validTypes = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward, MotorCommand::TurnLeft,
        MotorCommand::TurnRight, MotorCommand::LookLeft, MotorCommand::LookRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid MotorCommand value");
    }
}

inline void validateType(const WorldObjectType& value) {
    static const std::unordered_set<WorldObjectType> validTypes = {
        WorldObjectType::Empty, WorldObjectType::Resource,
        WorldObjectType::Hazard, WorldObjectType::Wall,
        WorldObjectType::Marker
    };
    if (validTypes.find(value) == validTypes.end()) {
        throw std::invalid_argument("Invalid WorldObjectType value");
    }
}

inline void validateNeuronId(const NeuronId& neuron, size_t maxNeurons) {
    if (neuron.value >= maxNeurons) {
        throw std::out_of_range("NeuronId out of range");
    }
}

inline void validateRegionId(const RegionId& region, size_t maxRegions) {
    if (region.value >= maxRegions) {
        throw std::out_of_range("RegionId out of range");
    }
}

inline void validatePopulationId(const PopulationId& population, size_t maxPopulations) {
    if (population.value >= maxPopulations) {
        throw std::out_of_range("PopulationId out of range");
    }
}

inline void validatePosition(float x, float y, float width, float height) {
    validateFloatRange(x, 0.0f, width, "x coordinate");
    validateFloatRange(y, 0.0f, height, "y coordinate");
}

inline void validateDimensions(size_t width, size_t height, size_t minWidth = 1, size_t minHeight = 1) {
    validateRange(width, minWidth, SIZE_MAX, "width");
    validateRange(height, minHeight, SIZE_MAX, "height");
}

inline void validateConfigKey(const std::string& key) {
    validateNonEmpty(key, "key");
}

inline void validateSensoryData(const std::vector<float>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Sensory data cannot be empty");
    }
}

inline void validateTimestamp(double timestamp) {
    if (timestamp < 0.0) {
        throw std::invalid_argument("Timestamp cannot be negative");
    }
}

inline void validateEnergy(float energy, float maxEnergy) {
    if (energy < 0.0f || energy > maxEnergy) {
        throw std::invalid_argument("Energy must be in valid range [0, " + 
                                   std::to_string(maxEnergy) + "]");
    }
}

inline void validateReward(float reward) {
    if (std::isnan(reward) || std::isinf(reward)) {
        throw std::invalid_argument("Reward cannot be NaN or infinite");
    }
}

inline void validateNeuronType(const NeuronId& neuron) {
    if (neuron.value == static_cast<uint64_t>(INVALID_NEURON_ID.value)) {
        throw std::invalid_argument("NeuronId is invalid");
    }
}

inline void validateRegionType(const RegionId& region) {
    if (region.value == static_cast<uint64_t>(INVALID_REGION_ID.value)) {
        throw std::invalid_argument("RegionId is invalid");
    }
}

inline void validateWorldObjectType(const WorldObjectType& type) {
    validateType(type);
}

inline void validateReferenceValid(bool isValid, const std::string& objectType) {
    if (!isValid) {
        throw std::invalid_argument(objectType + " reference is null or invalid");
    }
}

inline bool isValidEnumValue(NeuronType value) {
    static const std::unordered_set<NeuronType> validTypes = {
        NeuronType::Excitatory, NeuronType::Inhibitory, NeuronType::Modulatory,
        NeuronType::Sensory, NeuronType::Motor, NeuronType::Internal
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(SynapseType value) {
    static const std::unordered_set<SynapseType> validTypes = {
        SynapseType::Excitatory, SynapseType::Inhibitory, SynapseType::Modulatory,
        SynapseType::Electrical, SynapseType::GapJunction
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(DevelopmentalStage value) {
    static const std::unordered_set<DevelopmentalStage> validTypes = {
        DevelopmentalStage::Initial, DevelopmentalStage::CriticalPeriod,
        DevelopmentalStage::Maturation, DevelopmentalStage::Adult,
        DevelopmentalStage::Aging
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(ActionType value) {
    static const std::unordered_set<ActionType> validTypes = {
        ActionType::MoveForward, ActionType::MoveBackward, ActionType::MoveLeft,
        ActionType::MoveRight, ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Look, ActionType::LookUp, ActionType::LookDown,
        ActionType::Interact, ActionType::Eat, ActionType::Drink,
        ActionType::Rest, ActionType::Wait, ActionType::Custom
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(MotorCommand value) {
    static const std::unordered_set<MotorCommand> validTypes = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward, MotorCommand::TurnLeft,
        MotorCommand::TurnRight, MotorCommand::LookLeft, MotorCommand::LookRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    return validTypes.find(value) != validTypes.end();
}

inline bool isValidEnumValue(WorldObjectType value) {
    static const std::unordered_set<WorldObjectType> validTypes = {
        WorldObjectType::Empty, WorldObjectType::Resource,
        WorldObjectType::Hazard, WorldObjectType::Wall,
        WorldObjectType::Marker
    };
    return validTypes.find(value) != validTypes.end();
}

inline const char* toConstCharPtr(const std::string& str) {
    return str.c_str();
}

inline std::string toString(const std::string& str) {
    return str;
}

// File I/O error handling
inline bool safeLoadFile(const std::string& filepath, std::string& content, const std::string& operation) {
    try {
        validateNonEmpty(filepath, "filepath");
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for " + operation + ": " + filepath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
        
        if (content.empty() && operation.find("load") != std::string::npos) {
            throw std::runtime_error("File is empty: " + filepath);
        }
        
        return true;
    } catch (const std::exception& e) {
        translateException(e);
        return false;
    }
}

inline bool safeSaveFile(const std::string& filepath, const std::string& content, const std::string& operation) {
    try {
        validateNonEmpty(filepath, "filepath");
        validateNonEmpty(content, "content");
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for " + operation + ": " + filepath);
        }
        
        file << content;
        if (!file.good()) {
            throw std::runtime_error("Failed to write to file: " + filepath);
        }
        
        return true;
    } catch (const std::exception& e) {
        translateException(e);
        return false;
    }
}

// Exception registration
// Performance monitoring and debugging tools
namespace performance {
    // Performance timing utilities
    class PerformanceTimer {
    public:
        PerformanceTimer() : start_time(std::chrono::high_resolution_clock::now()) {}
        
        double elapsed() const {
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double, std::milli>(end - start_time).count();
        }
        
        void reset() {
            start_time = std::chrono::high_resolution_clock::now();
        }
        
    private:
        std::chrono::high_resolution_clock::time_point start_time;
    };
    
    // Memory monitoring utilities
    class MemoryMonitor {
    public:
        static size_t getAvailableMemory() {
            return std::numeric_limits<size_t>::max(); // Platform-specific implementation
        }
        
        static size_t getProcessMemoryUsage() {
            return 0; // Platform-specific implementation
        }
    };
    
    // Profiler for timing operations
    class Profiler {
    public:
        Profiler() = default;
        
        void start(const std::string& operation) {
            timers[operation] = std::make_unique<PerformanceTimer>();
            timers[operation]->reset();
        }
        
        double stop(const std::string& operation) {
            if (timers.find(operation) != timers.end()) {
                double elapsed = timers[operation]->elapsed();
                accumulators[operation].push_back(elapsed);
                return elapsed;
            }
            return 0.0;
        }
        
        std::unordered_map<std::string, double> getAverageTimes() const {
            std::unordered_map<std::string, double> averages;
            for (const auto& pair : accumulators) {
                if (!pair.second.empty()) {
                    double sum = std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
                    averages[pair.first] = sum / pair.second.size();
                }
            }
            return averages;
        }
        
        void reset() {
            timers.clear();
            accumulators.clear();
        }
        
        size_t getSampleCount(const std::string& operation) const {
            auto it = accumulators.find(operation);
            return (it != accumulators.end()) ? it->second.size() : 0;
        }
        
    private:
        std::unordered_map<std::string, std::unique_ptr<PerformanceTimer>> timers;
        std::unordered_map<std::string, std::vector<double>> accumulators;
    };
}

// Debug utilities
namespace debug {
    // Step-by-step execution tracer
    class ExecutionTracer {
    public:
        ExecutionTracer() = default;
        
        void start(const std::string& operation) {
            traceLog.push_back({"START", operation, std::chrono::high_resolution_clock::now()});
        }
        
        void stop(const std::string& operation) {
            traceLog.push_back({"STOP", operation, std::chrono::high_resolution_clock::now()});
        }
        
        std::vector<std::tuple<std::string, std::string, std::chrono::high_resolution_clock::time_point>> 
        getTrace() const { return traceLog; }
        
        void clear() { traceLog.clear(); }
        
    private:
        struct TraceEntry {
            std::string action;
            std::string operation;
            std::chrono::high_resolution_clock::time_point timestamp;
        };
        
        std::vector<TraceEntry> traceLog;
    };
    
    // Memory inspection utilities
    class MemoryInspector {
    public:
        static std::vector<uint64_t> getNeuronMemoryUsage(const Brain& brain);
        static std::vector<uint64_t> getSynapseMemoryUsage(const Brain& brain);
        static std::vector<float> getMemoryHeatMap(const Brain& brain);
    };
}

PYBIND11_MODULE(pynlm, m) {
    // Performance monitoring and debugging tools
    namespace performance {
        // Performance timing utilities
        class PerformanceTimer {
        public:
            PerformanceTimer() : start_time(std::chrono::high_resolution_clock::now()) {}
            
            double elapsed() const {
                auto end = std::chrono::high_resolution_clock::now();
                return std::chrono::duration<double, std::milli>(end - start_time).count();
            }
            
            void reset() {
                start_time = std::chrono::high_resolution_clock::now();
            }
            
        private:
            std::chrono::high_resolution_clock::time_point start_time;
        };
        
        // Memory monitoring utilities
        class MemoryMonitor {
        public:
            static size_t getAvailableMemory() {
                return std::numeric_limits<size_t>::max(); // Platform-specific implementation
            }
            
            static size_t getProcessMemoryUsage() {
                return 0; // Platform-specific implementation
            }
        };
        
        // Profiler for timing operations
        class Profiler {
        public:
            Profiler() = default;
            
            void start(const std::string& operation) {
                timers[operation] = std::make_unique<PerformanceTimer>();
                timers[operation]->reset();
            }
            
            double stop(const std::string& operation) {
                if (timers.find(operation) != timers.end()) {
                    double elapsed = timers[operation]->elapsed();
                    accumulators[operation].push_back(elapsed);
                    return elapsed;
                }
                return 0.0;
            }
            
            std::unordered_map<std::string, double> getAverageTimes() const {
                std::unordered_map<std::string, double> averages;
                for (const auto& pair : accumulators) {
                    if (!pair.second.empty()) {
                        double sum = std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
                        averages[pair.first] = sum / pair.second.size();
                    }
                }
                return averages;
            }
            
            void reset() {
                timers.clear();
                accumulators.clear();
            }
            
            size_t getSampleCount(const std::string& operation) const {
                auto it = accumulators.find(operation);
                return (it != accumulators.end()) ? it->second.size() : 0;
            }
            
        private:
            std::unordered_map<std::string, std::unique_ptr<PerformanceTimer>> timers;
            std::unordered_map<std::string, std::vector<double>> accumulators;
        };

        // Performance statistics collector
        class PerformanceStats {
        public:
            PerformanceStats() = default;
            
            void recordOperation(const std::string& operation, double elapsed) {
                operationTimes[operation].push_back(elapsed);
                totalOperations++;
                totalTime += elapsed;
            }
            
            std::unordered_map<std::string, std::vector<double>> getOperationTimes() const {
                return operationTimes;
            }
            
            double getAverageTime(const std::string& operation) const {
                auto it = operationTimes.find(operation);
                if (it != operationTimes.end() && !it->second.empty()) {
                    double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
                    return sum / it->second.size();
                }
                return 0.0;
            }
            
            size_t getOperationCount(const std::string& operation) const {
                auto it = operationTimes.find(operation);
                return (it != operationTimes.end()) ? it->second.size() : 0;
            }
            
            size_t getTotalOperations() const { return totalOperations; }
            double getTotalTime() const { return totalTime; }
            double getAverageOverall() const { return totalOperations ? totalTime / totalOperations : 0.0; }
            
            void reset() {
                operationTimes.clear();
                totalOperations = 0;
                totalTime = 0.0;
            }
            
        private:
            std::unordered_map<std::string, std::vector<double>> operationTimes;
            size_t totalOperations = 0;
            double totalTime = 0.0;
        };

        // Visualization utilities
        class VisualizationTools {
        public:
            static std::string createPerformanceChart(const PerformanceStats& stats);
            static std::string createMemoryUsageChart(const Brain& brain);
            static std::string createNetworkActivityGraph(const Brain& brain);
        };
    }

    // Debug utilities
    namespace debug {
        // Step-by-step execution tracer
        class ExecutionTracer {
        public:
            ExecutionTracer() = default;
            
            void start(const std::string& operation) {
                traceLog.push_back({"START", operation, std::chrono::high_resolution_clock::now()});
            }
            
            void stop(const std::string& operation) {
                traceLog.push_back({"STOP", operation, std::chrono::high_resolution_clock::now()});
            }
            
            std::vector<std::tuple<std::string, std::string, std::chrono::high_resolution_clock::time_point>> 
            getTrace() const { return traceLog; }
            
            void clear() { traceLog.clear(); }
            
        private:
            struct TraceEntry {
                std::string action;
                std::string operation;
                std::chrono::high_resolution_clock::time_point timestamp;
            };
            
            std::vector<TraceEntry> traceLog;
        };
        
        // Memory inspection utilities
        class MemoryInspector {
        public:
            static std::vector<uint64_t> getNeuronMemoryUsage(const Brain& brain);
            static std::vector<uint64_t> getSynapseMemoryUsage(const Brain& brain);
            static std::vector<float> getMemoryHeatMap(const Brain& brain);
        };
    }

    // Register all C++ exceptions for Python translation
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "InvalidArgument", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "OutOfRange", PyExc_IndexError);
    py::register_exception<std::length_error>(m, "LengthError", PyExc_ValueError);
    py::register_exception<std::bad_alloc>(m, "MemoryError", PyExc_MemoryError);
    
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Performance Monitoring Features:
        - Execution timing and profiling
        - Memory usage tracking
        - Performance statistics collection
        
        Debugging Tools:
        - Execution tracing
        - Memory inspection
        - Debug logging
        
        Exceptions raised:
        - RuntimeError: Generic runtime errors, file I/O failures, memory issues
        - ValueError: Invalid parameter values or empty strings
        - IndexError: Index out of bounds or invalid range
        - MemoryError: Memory allocation failures
    )pbdoc";

    // NeuronId class with validation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with comprehensive error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        Provides methods to load/save configuration from files, get/set values, and manage
        configuration entries.
        
        Raises:
            RuntimeError: File I/O errors during loading/saving
            ValueError: Invalid parameter values, duplicate keys, malformed data
            IndexError: Key not found or invalid index
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            std::string error;
            if (!safeLoadFile(filepath, error, "load")) {
                translateException(std::runtime_error(error));
            }
            return self.loadFromFile(filepath);
        }, py::arg("filepath"),
             "Load configuration from a JSON file. Raises RuntimeError if file cannot be read or parsed.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            if (argc < 0) {
                throw std::invalid_argument("argc cannot be negative");
            }
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. Raises ValueError for invalid arguments.")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            std::string content = self.summary();
            if (!safeSaveFile(filepath, content, "save")) {
                throw std::runtime_error("Failed to save configuration to " + filepath);
            }
            return true;
        }, py::arg("filepath"),
             "Save configuration to a JSON file. Raises RuntimeError if file cannot be written.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Raises ValueError if key is empty.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        Abstract base class for all sensory modalities (vision, audio, internal signals).
        
        Raises:
            ValueError: Invalid data dimensions or timestamp values
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector of floats")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality of the data")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp (nanoseconds)")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp. Raises ValueError if timestamp is negative.");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        Represents visual input with width, height, and color channels.
        
        Raises:
            ValueError: Invalid image dimensions (non-positive values)
            RuntimeError: Memory allocation failure for image data
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), 
             py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Create a Vision object. Raises ValueError for invalid dimensions.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                if (data.empty()) {
                    throw std::invalid_argument("Vision data cannot be empty");
                }
                self.setData(data);
            } catch (const std::exception& e) {
                translateException(e);
            }
        }, py::arg("data"),
             "Set vision data. Raises ValueError for empty data or invalid size.")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        Represents audio input with sample rate and number of samples.
        
        Raises:
            ValueError: Invalid sample parameters (non-positive values)
            RuntimeError: Memory allocation failure for audio data
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), 
             py::arg("sampleRate"), py::arg("numSamples"),
             "Create an Audio object. Raises ValueError for invalid sample parameters.")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                if (data.empty()) {
                    throw std::invalid_argument("Audio data cannot be empty");
                }
                self.setData(data);
            } catch (const std::exception& e) {
                translateException(e);
            }
        }, py::arg("data"),
             "Set audio data. Raises ValueError for empty data.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate. Raises ValueError for non-positive sample rate.")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        Represents internal neural signals from the brain's internal systems.
        
        Raises:
            ValueError: Invalid signal values
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value. Raises ValueError for NaN or infinite values.")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        Represents a motor action with type and parameters.
        
        Raises:
            ValueError: Invalid action type or parameter values
            RuntimeError: Memory allocation failure
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create an Action with specified type. Raises ValueError for invalid type.")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an Action with type and parameters. Raises ValueError for invalid inputs.")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type. Raises ValueError for invalid type.")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters. Raises ValueError for invalid parameters.")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        Represents an object in the 2D simulation world.
        
        Raises:
            ValueError: Invalid object coordinates or parameters
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object. Raises ValueError for invalid coordinates or radius.")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        Represents the physical state of an agent in the world.
        
        Raises:
            ValueError: Invalid state values (e.g., negative energy)
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset,
             "Reset body state to initial values.");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        Represents the result of an action taken by an agent.
        
        Raises:
            ValueError: Invalid reward or success values
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an ActionResult with reward, success, and message.")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        Contains all sensory inputs available to an agent at a given time.
        
        Raises:
            ValueError: Invalid sensory data
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision input. Raises ValueError for invalid vision data.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch input. Raises ValueError for invalid touch data.")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals. Raises ValueError for invalid internal data.")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception. Raises ValueError for invalid proprioception data.")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio input. Raises ValueError for invalid audio data.")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp. Raises ValueError for negative timestamp.");

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        A simple 2D world where agents interact with objects and environment.
        
        Raises:
            ValueError: Invalid world parameters or coordinates
            RuntimeError: Memory allocation failure
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions. Raises ValueError for invalid dimensions.")
        .def("reset", &SimpleWorld::reset,
             "Reset world state to initial conditions.")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position. Raises ValueError for invalid coordinates.")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation. Raises ValueError for invalid timestep.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent. Raises ValueError for invalid command.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from agent.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state.")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world. Raises ValueError for invalid object.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position. Raises ValueError for invalid coordinates.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid. Raises ValueError for invalid coordinates.")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy. Raises ValueError for negative energy.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate. Raises ValueError for invalid rate.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed. Raises ValueError for invalid seed.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        The central coordinator of the neural system, implementing real spiking neural
        computation with integrated memory, prediction, cognition, and neuromodulation systems.
        
        Raises:
            RuntimeError: Memory allocation failure, file I/O errors
            ValueError: Invalid configuration, invalid parameters
            IndexError: Region ID not found, neuron ID out of range
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration. Raises ValueError for invalid config.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Raises RuntimeError if initialization fails.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step. Raises ValueError for invalid step.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp. Raises ValueError for invalid parameters.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain. Raises ValueError for invalid input.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. Raises ValueError for invalid parameters.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type. Raises ValueError for invalid parameters.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Raises RuntimeError if action production fails.")
        .def("reset", &Brain::reset,
             "Reset brain state.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Raises RuntimeError if save fails.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Raises RuntimeError if load fails.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Raises ValueError for invalid name.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Raises IndexError if region not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage. Raises ValueError for invalid stage.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status.");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        Connects the NLM brain to the simulation world, handling sensory transduction
        and motor decoding between neural activity and agent actions.
        
        Raises:
            ValueError: Invalid parameters, corrupted brain reference
            RuntimeError: Memory allocation failure
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with brain reference. Raises ValueError for null brain.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world. Raises ValueError for invalid world.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain. Raises ValueError for invalid percept.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation. Raises ValueError for invalid reward values.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system. Raises ValueError for invalid timestep.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Performance monitoring and debugging classes (nested within Brain module registration)
    
    // Performance monitoring module
    m.def("createPerformanceTimer", []() {
        return performance::PerformanceTimer();
    }, "Create a performance timer");
    
    m.def("getMemoryUsage", []() {
        return performance::MemoryMonitor::getProcessMemoryUsage();
    }, "Get current process memory usage");
    
    m.def("createProfiler", []() {
        return performance::Profiler();
    }, "Create a profiler for timing operations");
    
    m.def("createPerformanceStats", []() {
        return performance::PerformanceStats();
    }, "Create performance statistics collector");
    
    // Debug module functions
    m.def("createExecutionTracer", []() {
        return debug::ExecutionTracer();
    }, "Create an execution tracer for step-by-step debugging");
}
