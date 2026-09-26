#include "AssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AssociativeMemory::Impl {
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    size_t maxAssociations;
    
    Impl(size_t max) : maxAssociations(max) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl(5000)) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // TODO PHASE 2: Implement association management with Hebbian learning
    
    // Check if association exists
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Update existing association (symmetric)
            std::get<2>(assoc) = std::max(-1.0f, std::min(strength, 1.0f));
            return;
        }
    }
    
    // Add new association if within capacity
    if (pImpl->associations.size() < pImpl->maxAssociations) {
        pImpl->associations.emplace_back(a, b, strength);
    }
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> neighbors;
    
    for (const auto& assoc : pImpl->associations) {
        if (std::get<0>(assoc) == neuron) {
            neighbors.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            neighbors.push_back(std::get<0>(assoc));
        }
    }
    
    return neighbors;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) += delta;
            std::get<2>(assoc) = std::max(-1.0f, std::min(std::get<2>(assoc), 1.0f));
            break;
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
}

} // namespace nlm