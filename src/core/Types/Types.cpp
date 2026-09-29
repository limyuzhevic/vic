#include "Types.hpp"
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <sstream>

namespace nlm {

// JSON serialization helper functions for type system
nlohmann::json toJson(const NeuronId& id) {
    return nlohmann::json{{"type", "NeuronId"}, {"value", id.value}};
}

NeuronId fromJson(const nlohmann::json& j) {
    if (j.is_object() && j.contains("type") && j["type"] == "NeuronId" && j.contains("value")) {
        return NeuronId(j["value"].get<uint64_t>());
    } else if (j.is_number()) {
        return NeuronId(j.get<uint64_t>());
    }
    throw std::runtime_error("Invalid NeuronId JSON format");
}

nlohmann::json toJson(const SynapseId& id) {
    return nlohmann::json{{"type", "SynapseId"}, {"value", id.value}};
}

SynapseId fromJson(const nlohmann::json& j) {
    if (j.is_object() && j.contains("type") && j["type"] == "SynapseId" && j.contains("value")) {
        return SynapseId(j["value"].get<uint64_t>());
    } else if (j.is_number()) {
        return SynapseId(j.get<uint64_t>());
    }
    throw std::runtime_error("Invalid SynapseId JSON format");
}

nlohmann::json toJson(const RegionId& id) {
    return nlohmann::json{{"type", "RegionId"}, {"value", id.value}};
}

RegionId fromJson(const nlohmann::json& j) {
    if (j.is_object() && j.contains("type") && j["type"] == "RegionId" && j.contains("value")) {
        return RegionId(j["value"].get<uint64_t>());
    } else if (j.is_number()) {
        return RegionId(j.get<uint64_t>());
    }
    throw std::runtime_error("Invalid RegionId JSON format");
}

nlohmann::json toJson(const PopulationId& id) {
    return nlohmann::json{{"type", "PopulationId"}, {"value", id.value}};
}

PopulationId fromJson(const nlohmann::json& j) {
    if (j.is_object() && j.contains("type") && j["type"] == "PopulationId" && j.contains("value")) {
        return PopulationId(j["value"].get<uint64_t>());
    } else if (j.is_number()) {
        return PopulationId(j.get<uint64_t>());
    }
    throw std::runtime_error("Invalid PopulationId JSON format");
}

// Enum serialization helpers
nlohmann::json toJson(NeuronType type) {
    return nlohmann::json::array({static_cast<int>(type), "NeuronType"});
}

NeuronType fromJsonNeuronType(const nlohmann::json& j) {
    if (j.is_number()) {
        return static_cast<NeuronType>(j.get<int>());
    } else if (j.is_array() && j.size() >= 2 && j[1] == "NeuronType") {
        return static_cast<NeuronType>(j[0].get<int>());
    }
    throw std::runtime_error("Invalid NeuronType JSON format");
}

nlohmann::json toJson(SynapseType type) {
    return nlohmann::json::array({static_cast<int>(type), "SynapseType"});
}

SynapseType fromJsonSynapseType(const nlohmann::json& j) {
    if (j.is_number()) {
        return static_cast<SynapseType>(j.get<int>());
    } else if (j.is_array() && j.size() >= 2 && j[1] == "SynapseType") {
        return static_cast<SynapseType>(j[0].get<int>());
    }
    throw std::runtime_error("Invalid SynapseType JSON format");
}

nlohmann::json toJson(DevelopmentalStage stage) {
    return nlohmann::json::array({static_cast<int>(stage), "DevelopmentalStage"});
}

DevelopmentalStage fromJsonDevelopmentalStage(const nlohmann::json& j) {
    if (j.is_number()) {
        return static_cast<DevelopmentalStage>(j.get<int>());
    } else if (j.is_array() && j.size() >= 2 && j[1] == "DevelopmentalStage") {
        return static_cast<DevelopmentalStage>(j[0].get<int>());
    }
    throw std::runtime_error("Invalid DevelopmentalStage JSON format");
}

nlohmann::json toJson(FiringState state) {
    return nlohmann::json::array({static_cast<int>(state), "FiringState"});
}

FiringState fromJsonFiringState(const nlohmann::json& j) {
    if (j.is_number()) {
        return static_cast<FiringState>(j.get<int>());
    } else if (j.is_array() && j.size() >= 2 && j[1] == "FiringState") {
        return static_cast<FiringState>(j[0].get<int>());
    }
    throw std::runtime_error("Invalid FiringState JSON format");
}

// String representation for debugging
std::string toString(const NeuronId& id) {
    std::stringstream ss;
    ss << "<NeuronId: " << id.value << ">";
    return ss.str();
}

std::string toString(const SynapseId& id) {
    std::stringstream ss;
    ss << "<SynapseId: " << id.value << ">";
    return ss.str();
}

std::string toString(const RegionId& id) {
    std::stringstream ss;
    ss << "<RegionId: " << id.value << ">";
    return ss.str();
}

std::string toString(const PopulationId& id) {
    std::stringstream ss;
    ss << "<PopulationId: " << id.value << ">";
    return ss.str();
}

// Factory functions for Python-friendly creation
NeuronId makeNeuronId(uint64_t value) {
    return NeuronId(value);
}

SynapseId makeSynapseId(uint64_t value) {
    return SynapseId(value);
}

RegionId makeRegionId(uint64_t value) {
    return RegionId(value);
}

PopulationId makePopulationId(uint64_t value) {
    return PopulationId(value);
}

// Explicit template instantiations
template void NeuronId::operator<<(std::ostream&) const;
template void SynapseId::operator<<(std::ostream&) const;
template void RegionId::operator<<(std::ostream&) const;
template void PopulationId::operator<<(std::ostream&) const;

// Type conversion utilities
template<typename T>
std::string toString(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

// Comparison operators for all types
template bool NeuronId::operator==(const NeuronId&) const = default;
template bool NeuronId::operator!=(const NeuronId&) const = default;
template bool NeuronId::operator<(const NeuronId&) const = default;

template bool SynapseId::operator==(const SynapseId&) const = default;
template bool SynapseId::operator!=(const SynapseId&) const = default;
template bool SynapseId::operator<(const SynapseId&) const = default;

template bool RegionId::operator==(const RegionId&) const = default;
template bool RegionId::operator!=(const RegionId&) const = default;
template bool RegionId::operator<(const RegionId&) const = default;

template bool PopulationId::operator==(const PopulationId&) const = default;
template bool PopulationId::operator!=(const PopulationId&) const = default;
template bool PopulationId::operator<(const PopulationId&) const = default;

} // namespace nlm
