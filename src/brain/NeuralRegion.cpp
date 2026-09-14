/*
 * NeuralRegion implementation
 * Provides the implementation for the NeuralRegion class with improved organization.
 * 
 * This implementation focuses on:
 * 1. Efficient population management
 * 2. Clean synapse tracking
 * 3. Better memory organization
 * 4. Improved error handling
 */

#include "NeuralRegion.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <random>
#include <unordered_set>
#include <stdexcept>
#include <numeric>
#include <unordered_map>
#include <cmath>

namespace nlm {

NeuralRegion::NeuralRegion(RegionId id)
    : brainRef_(nullptr) {
    // Use pImpl for implementation details
    pImpl = new Impl();
    pImpl->regionId = id;
}

NeuralRegion::NeuralRegion(RegionId id, const std::string& name)
    : brainRef_(nullptr) {
    pImpl = new Impl();
    pImpl->regionId = id;
    pImpl->regionName = name;
}

NeuralRegion::~NeuralRegion() {
    delete pImpl;
}

NeuralRegion::NeuralRegion(NeuralRegion&& other) noexcept
    : pImpl(other.pImpl)
    , brainRef_(other.brainRef_) {
    other.pImpl = nullptr;
}

NeuralRegion& NeuralRegion::operator=(NeuralRegion&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        brainRef_ = other.brainRef_;
        other.pImpl = nullptr;
    }
    return *this;
}

/**
 * Implementation details for NeuralRegion methods
 */

} // namespace nlm

#include "NeuralRegion.cpp"
