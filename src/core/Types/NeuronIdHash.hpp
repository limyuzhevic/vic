// NeuronId hash specialization for std::unordered_map
#pragma once

#include "core/Types/Types.hpp"

namespace std {
    template<>
    struct hash<nlm::NeuronId> {
        size_t operator()(const nlm::NeuronId& id) const {
            return hash<uint64_t>{}(id);
        }
    };
}