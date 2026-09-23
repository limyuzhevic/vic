#pragma once

#include <functional>

namespace std {
    // Hash function specializations for NLM ID types
    template<>
    struct hash<nlm::NeuronId> {
        size_t operator()(const nlm::NeuronId& id) const {
            return std::hash<uint64_t>{}(id.value);
        }
    };
    
    template<>
    struct hash<nlm::SynapseId> {
        size_t operator()(const nlm::SynapseId& id) const {
            return std::hash<uint64_t>{}(id.value);
        }
    };
    
    template<>
    struct hash<nlm::RegionId> {
        size_t operator()(const nlm::RegionId& id) const {
            return std::hash<uint64_t>{}(id.value);
        }
    };
    
    template<>
    struct hash<nlm::PopulationId> {
        size_t operator()(const nlm::PopulationId& id) const {
            return std::hash<uint64_t>{}(id.value);
        }
    };
}
