#pragma once

#include "VisualizationInterface.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nlm {

class Brain;
class MemorySystem;

class MemoryVisualizer : public VisualizationInterface {
public:
    MemoryVisualizer();
    ~MemoryVisualizer() override;
    
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
    
    // Memory visualization specific methods
    void visualizeMemorySystems(const MemorySystem& memorySystem);
    void visualizeWorkingMemory(const std::vector<const class Neuron*>& neurons, const glm::vec3& position, float size);
    void visualizeEpisodicMemory(const std::vector<const class Neuron*>& neurons, const glm::vec3& position, float size, float time);
    void visualizeSemanticMemory(const std::vector<const class Neuron*>& neurons, const glm::vec3& position, float size);
    void visualizeMemoryConsolidation(const Brain& brain, double consolidationProgress);
    
    // Memory states and phases
    enum class MemoryState {
        CONSOLIDATING,
        REPLAYING,
        STABILIZING,
        RETRIEVING,
        FORGETTING,
        INTEGRATING
    };
    
    void setMemoryState(MemoryState state);
    void visualizeMemoryTrace(const std::vector<const class Neuron*>& neurons, double time, float strength);
    void visualizePatternCompletion(const std::vector<const class Neuron*>& neurons, const glm::vec3& position, float radius);
    
    // Memory visualization modes
    enum class MemoryMode {
        TEMPORAL,
        SPATIAL,
        PATTERN_SPACE,
        ASSOCIATIVE,
        HIERARCHICAL,
        DISTRIBUTED
    };
    
    void setMemoryMode(MemoryMode mode);
    void toggleMemoryReplay(bool enable);
    void setReplaySpeed(float speed);
    void visualizeMemoryStrengthMap(const std::vector<const class Neuron*>& neurons, const glm::vec2& gridSize, int resolution);
    
    // Memory metrics visualization
    void visualizeMemoryCapacityUtilization(const MemorySystem& memorySystem);
    void visualizeMemoryStability(const std::vector<const class Neuron*>& neurons);
    void visualizeMemoryEfficiency(const Brain& brain);
    
    // Memory association visualization
    void visualizeMemoryAssociations(const std::vector<const class Neuron*>& neurons, float associationStrength);
    void visualizeMemoryClusters(const std::vector<const class Neuron*>& neurons, const glm::vec3& center, float clusterRadius);
    void visualizeMemoryTrajectories(const std::vector<std::vector<const class Neuron*>>& trajectories, double timeStep);
    
    // Timeline visualization
    void visualizeMemoryTimeline(double timelineLength, double currentTime);
    void visualizeMemoryEpochs(const std::vector<std::pair<std::string, double>>& epochs);
    void visualizeMemoryLifeCycle(const Brain& brain);
    
    // Memory analysis visualization
    void visualizeMemoryLeakage(const Brain& brain);
    void visualizeMemorySaturation(const MemorySystem& memorySystem);
    void visualizeMemoryInterference(const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm