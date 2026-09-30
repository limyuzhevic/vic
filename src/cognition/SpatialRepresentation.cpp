#include "SpatialRepresentation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct SpatialRepresentation::Impl {
    Brain* brain;
    std::mt19937 rng;
    
    Impl() : brain(nullptr), rng(std::random_device()()) {}
};

SpatialRepresentation::SpatialRepresentation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , currentX_(0.0f)
    , currentY_(0.0f)
    , gridSpacing_(1.0f)
{
    lastSensoryCue_.resize(10, 0.0f);  // Standard sensory cue size
    
    NLM_LOG_INFO("SpatialRepresentation created");
}

SpatialRepresentation::~SpatialRepresentation() {
    NLM_LOG_INFO("SpatialRepresentation destroyed");
}

void SpatialRepresentation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize with random place cells
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
    
    for (size_t i = 0; i < 50; ++i) {  // Create 50 place cells
        PlaceCell cell;
        cell.neuron = static_cast<NeuronId>(i + 1000);  // Reserve neuron IDs
        cell.x = dist(pImpl->rng);
        cell.y = dist(pImpl->rng);
        cell.fieldRadius = dist(pImpl->rng) * 0.5f + 0.5f;
        cell.activation = 0.0f;
        
        placeCells_.push_back(cell);
    }
    
    NLM_LOG_INFO("SpatialRepresentation initialized with %zu place cells", placeCells_.size());
}

void SpatialRepresentation::recordPosition(float x, float y, const std::vector<float>& sensoryCues) {
    currentX_ = x;
    currentY_ = y;
    lastSensoryCue_ = sensoryCues;
    
    // Activate nearby place cells
    for (auto& cell : placeCells_) {
        float distance = std::sqrt(std::pow(cell.x - x, 2) + std::pow(cell.y - y, 2));
        if (distance < cell.fieldRadius) {
            cell.activation = 1.0f - (distance / cell.fieldRadius);
            cell.activation = std::max(0.0f, cell.activation);
        } else {
            cell.activation = 0.0f;
        }
    }
}

std::pair<float, float> SpatialRepresentation::getPredictedPosition() const {
    // Path integration: accumulate movement
    std::pair<float, float> predictedPos = std::make_pair(currentX_, currentY_);
    
    // Apply dead reckoning based on velocity (if available)
    // This would integrate velocity vectors over time
    // For now, just return current position
    
    return predictedPos;
}

void SpatialRepresentation::integrateMovement(float dx, float dy) {
    currentX_ += dx;
    currentY_ += dy;
    
    // Update place cell activations based on new position
    for (auto& cell : placeCells_) {
        float distance = std::sqrt(std::pow(cell.x - currentX_, 2) + std::pow(cell.y - currentY_, 2));
        if (distance < cell.fieldRadius) {
            cell.activation = 1.0f - (distance / cell.fieldRadius);
            cell.activation = std::max(0.0f, cell.activation);
        } else {
            cell.activation = 0.0f;
        }
    }
}

float SpatialRepresentation::getPositionActivation(float x, float y) const {
    float maxActivation = 0.0f;
    
    for (const auto& cell : placeCells_) {
        float distance = std::sqrt(std::pow(cell.x - x, 2) + std::pow(cell.y - y, 2));
        if (distance < cell.fieldRadius) {
            float activation = 1.0f - (distance / cell.fieldRadius);
            activation = std::max(0.0f, activation);
            maxActivation = std::max(maxActivation, activation);
        }
    }
    
    return maxActivation;
}

std::vector<NeuronId> SpatialRepresentation::getPlaceNeuronsNear(float x, float y, float radius) const {
    std::vector<NeuronId> nearbyNeurons;
    
    for (const auto& cell : placeCells_) {
        float distance = std::sqrt(std::pow(cell.x - x, 2) + std::pow(cell.y - y, 2));
        if (distance < radius) {
            nearbyNeurons.push_back(cell.neuron);
        }
    }
    
    return nearbyNeurons;
}

void SpatialRepresentation::clear() {
    for (auto& cell : placeCells_) {
        cell.activation = 0.0f;
    }
    
    currentX_ = 0.0f;
    currentY_ = 0.0f;
    lastSensoryCue_.clear();
}

} // namespace nlm