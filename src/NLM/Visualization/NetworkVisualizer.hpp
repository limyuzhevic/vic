#pragma once

#include "VisualizationInterface.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nlm {

class NetworkVisualizer : public VisualizationInterface {
public:
    NetworkVisualizer();
    ~NetworkVisualizer() override;
    
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    bool supports3D() const override;
    std::string getName() const override;
    std::string getVersion() const override;
    void onResize(int width, int height) override;
    void setParameter(const std::string& name, const std::string& value) override;
    std::string getParameter(const std::string& name) const override;
    void setComponentEnabled(const std::string& component, bool enabled) override;
    bool isComponentEnabled(const std::string& component) const override;
    void addEventCallback(const std::string& eventType, std::function<void(const std::string&)> callback) override;
    bool exportState(const std::string& filename, const std::string& format) override;
    bool importState(const std::string& filename, const std::string& format) override;
    
    // Network visualization specific methods
    void visualizeNetworkStructure(const class Brain& brain);
    void visualizeNeurons(const std::vector<const class Neuron*>& neurons, const glm::vec3& position, float scale);
    void visualizeSynapses(const std::vector<const class Synapse*>& synapses, const glm::vec3& startPos, const glm::vec3& endPos);
    void visualizeRegions(const std::vector<const class Neuron*>& neurons, const glm::vec3& center, float size);
    
    // Layout algorithms
    void setLayoutAlgorithm(const std::string& algorithm);
    void forceDirectedLayout(const class Brain& brain);
    void radialLayout(const class Brain& brain, const glm::vec3& center);
    void hierarchicalLayout(const class Brain& brain);
    void springEmbeddedLayout(const class Brain& brain);
    
    // Visualization modes
    enum class VisualizationMode {
        WIRE_FRAME,
        SOLID,
        POINTS,
        BILINES,
        GRADIENT_COLORS
    };
    
    void setVisualizationMode(VisualizationMode mode);
    void toggleConnections(bool show);
    void setConnectionStrengthThreshold(float threshold);
    void setNeuronSize(float size);
    
    // Color schemes
    enum class ColorScheme {
        DEFAULT,
        NEURAL,
        BIOLOGICAL,
        METABOLIC,
        FUNCTIONAL
    };
    
    void setColorScheme(ColorScheme scheme);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm