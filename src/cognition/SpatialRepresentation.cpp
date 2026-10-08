#include "SpatialRepresentation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct SpatialRepresentation::Impl {
    Brain* brain;
    std::vector<PlaceCell> placeCells;
    float gridSpacing_;
    float explorationRate_;
    
    Impl() : brain(nullptr), gridSpacing_(0.0f), explorationRate_(0.5f) {}
};

SpatialRepresentation::SpatialRepresentation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , currentX_(0.0f)
    , currentY_(0.0f)
    , gridSpacing_(0.0f)
    , explorationRate_(0.5f)
{
}

SpatialRepresentation::~SpatialRepresentation() = default;

void SpatialRepresentation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SpatialRepresentation initialized");
}

void SpatialRepresentation::recordPosition(float x, float y, const std::vector<float>& sensoryCues) {
    // Record current position experience
    
    // Update current position
    float dx = x - currentX_;
    float dy = y - currentY_;
    
    currentX_ = x;
    currentY_ = y;
    
    // Update place cell activations based on position
    updatePlaceCells(x, y, sensoryCues);
    
    // Occasionally create new place cell (exploration)
    if (explorationRate_ > 0.0f && rand() % 100 < explorationRate_ * 100) {
        createPlaceCell(x, y, sensoryCues);
    }
    
    // Integrate movement for path integration
    integrateMovement(dx, dy);
}

void SpatialRepresentation::updatePlaceCells(float x, float y, const std::vector<float>& sensoryCues) {
    // Update activation of existing place cells
    for (PlaceCell& cell : pImpl->placeCells) {
        float distance = std::sqrt(std::pow(cell.x - x, 2) + std::pow(cell.y - y, 2));
        
        if (distance < cell.fieldRadius) {
            // Cell is activated by position
            cell.activation = std::max(0.0f, 1.0f - (distance / cell.fieldRadius));
            
            // Enhance activation with sensory cues
            for (float cue : sensoryCues) {
                cell.activation = std::min(1.0f, cell.activation + cue * 0.1f);
            }
        } else {
            // Cell is not activated
            cell.activation = 0.0f;
        }
    }
}

void SpatialRepresentation::createPlaceCell(float x, float y, const std::vector<float>& sensoryCues) {
    // Create new place cell at this location
    
    // Determine field radius based on grid spacing
    float fieldRadius = gridSpacing_ * 0.3f;
    
    PlaceCell cell;
    cell.neuron = NeuronId(0);  // Would need actual neuron ID in real implementation
    cell.x = x;
    cell.y = y;
    cell.fieldRadius = fieldRadius;
    cell.activation = 1.0f;  // New place cell is fully activated
    
    // Incorporate sensory cues into cell properties
    float sensoryStrength = 0.0f;
    for (float cue : sensoryCues) {
        sensoryStrength += cue;
    }
    cell.fieldRadius *= (1.0f + sensoryStrength);
    
    pImpl->placeCells.push_back(cell);
    
    NLM_LOG_INFO("SpatialRepresentation: Created place cell at (" + std::to_string(x) + ", " + 
                 std::to_string(y) + ") with radius " + std::to_string(fieldRadius));
}

void SpatialRepresentation::integrateMovement(float dx, float dy) {
    // Path integration: update position estimate based on movement
    
    currentX_ += dx;
    currentY_ += dy;
    
    // Add noise to simulate real-world uncertainty
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> noiseX(0.0f, 0.1f);
    std::normal_distribution<float> noiseY(0.0f, 0.1f);
    
    currentX_ += noiseX(gen);
    currentY_ += noiseY(gen);
}

float SpatialRepresentation::getPositionActivation(float x, float y) const {
    // Get position neuron activation for a given position
    
    if (pImpl->placeCells.empty()) {
        return 0.0f;
    }
    
    float maxActivation = 0.0f;
    
    for (const PlaceCell& cell : pImpl->placeCells) {
        float distance = std::sqrt(std::pow(cell.x - x, 2) + std::pow(cell.y - y, 2));
        
        if (distance < cell.fieldRadius) {
            float activation = std::max(0.0f, 1.0f - (distance / cell.fieldRadius));
            maxActivation = std::max(maxActivation, activation);
        }
    }
    
    return maxActivation;
}

std::vector<NeuronId> SpatialRepresentation::getPlaceNeuronsNear(float x, float y, float radius) const {
    // Find place neurons near a position
    
    std::vector<NeuronId> nearbyNeurons;
    
    for (const PlaceCell& cell : pImpl->placeCells) {
        float distance = std::sqrt(std::pow(cell.x - x, 2) + std::pow(cell.y - y, 2));
        
        if (distance < radius) {
            nearbyNeurons.push_back(cell.neuron);
        }
    }
    
    return nearbyNeurons;
}

std::pair<float, float> SpatialRepresentation::getPredictedPosition() const {
    // Get predicted position based on path integration
    return std::make_pair(currentX_, currentY_);
}

void SpatialRepresentation::clear() {
    pImpl->placeCells.clear();
    currentX_ = 0.0f;
    currentY_ = 0.0f;
    gridSpacing_ = 0.0f;
    explorationRate_ = 0.0f;
}

bool SpatialRepresentation::hasPlaceCells() const {
    return !pImpl->placeCells.empty();
}

} // namespace nlm