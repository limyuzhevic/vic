#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// SpatialRepresentation: Learns spatial relationships from experience
// NOT a map with coordinates - encodes spatial relationships in neural patterns
//
// Key mechanisms:
// - Place neurons form through spatial exploration
// - Spatial relationships encoded in synaptic connectivity
// - Path integration through dead reckoning
// - Landmark associations

class SpatialRepresentation {
public:
    SpatialRepresentation();
    ~SpatialRepresentation();

    void initialize(Brain* brain);

    // Record current position experience
    void recordPosition(float x, float y, const std::vector<float>& sensoryCues);

    // Get predicted position based on path integration
    std::pair<float, float> getPredictedPosition() const;

    // Update position estimate based on movement
    void integrateMovement(float dx, float dy);

    // Get position neuron activation for a given position
    float getPositionActivation(float x, float y) const;

    // Find place neurons near a position
    std::vector<NeuronId> getPlaceNeuronsNear(float x, float y, float radius) const;

    // Get grid spacing (if grid cells have formed)
    float getGridSpacing() const { return gridSpacing_; }

    // Clear spatial representations
    void clear();

    // Have place cells formed?
    bool hasPlaceCells() const { return !placeCells_.empty(); }

private:
    struct PlaceCell {
        NeuronId neuron;
        float x;
        float y;
        float fieldRadius;
        float activation;
    };

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<PlaceCell> placeCells_;
    float currentX_;
    float currentY_;
    float gridSpacing_;
    std::vector<float> lastSensoryCue_;
};

} // namespace nlm