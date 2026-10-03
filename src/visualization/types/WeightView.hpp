#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Types/Types.hpp"

namespace nlm {

class WeightView {
public:
    WeightView();
    virtual ~WeightView();

    // Initialize the view
    virtual bool initialize() = 0;

    // Update the view with new data
    virtual void update(const VisualizationData& data) = 0;

    // Render the view
    virtual void render() = 0;

    // Get view name
    const std::string& getName() const;

    // Set view name
    void setName(const std::string& name);

    // Enable/disable view
    void enable(bool enabled);
    bool isEnabled() const;

    // Set matrix type
    void setMatrixType(const std::string& type);
    const std::string& getMatrixType() const;

    // Enable/disable color mapping
    void enableColorMapping(bool enabled);
    bool isColorMappingEnabled() const;

    // Set weight scale
    void setWeightScale(float scale);
    float getWeightScale() const;

    // Set threshold
    void setThreshold(float threshold);
    float getThreshold() const;

    // Enable/disable normalization
    void enableNormalization(bool enabled);
    bool isNormalizationEnabled() const;

    // Enable/disable smoothing
    void enableSmoothing(bool enabled);
    bool isSmoothingEnabled() const;

    // Set smoothing factor
    void setSmoothingFactor(float factor);
    float getSmoothingFactor() const;

    // Enable/disable adjacency matrix
    void enableAdjacencyMatrix(bool enabled);
    bool isAdjacencyMatrixEnabled() const;

    // Enable/disable connectivity map
    void enableConnectivityMap(bool enabled);
    bool isConnectivityMapEnabled() const;

    // Enable/disable plasticity visualization
    void enablePlasticityVisualization(bool enabled);
    bool isPlasticityVisualizationEnabled() const;

    // Set plasticity strength
    void setPlasticityStrength(float strength);
    float getPlasticityStrength() const;

    // Set time window
    void setTimeWindow(Timestamp start, Timestamp end);
    std::pair<Timestamp, Timestamp> getTimeWindow() const;

    // Reset view to default
    virtual void reset();

    // Export view data
    virtual bool exportView(const std::string& filepath) = 0;

    // Get view statistics
    virtual std::map<std::string, double> getStatistics() const = 0;

protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm