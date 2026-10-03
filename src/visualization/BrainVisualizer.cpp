#include "BrainVisualizer.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>

namespace nlm {

// Helper functions for visualization
namespace VisualizationUtils {
    // Position neurons in a radial pattern
    inline void placeNeuronsRadial(const std::vector<NeuronId>& neuronIds, 
                                  std::vector<NeuronVisualizationData>& neuronData,
                                  float radius, int layers = 1) {
        float angleStep = 2.0f * M_PI / neuronIds.size();
        
        for (size_t i = 0; i < neuronIds.size(); ++i) {
            float angle = i * angleStep;
            NeuronId id = neuronIds[i];
            
            // Find neuron data reference
            for (auto& nd : neuronData) {
                if (nd.id == id) {
                    // Place in concentric circles
                    int layer = static_cast<int>(i / static_cast<float>(neuronIds.size()) * layers);
                    float layerRadius = radius * (layer + 1) / static_cast<float>(layers + 1);
                    nd.x = layerRadius * cos(angle);
                    nd.y = layerRadius * sin(angle);
                    nd.z = 0.0f;  // Can be expanded for 3D
                    break;
                }
            }
        }
    }
    
    // Calculate distances between neurons
    inline float calculateDistance(const NeuronVisualizationData& a, const NeuronVisualizationData& b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }
    
    // Generate color based on neuron properties
    inline std::string getNeuronColor(NeuronType type, bool isActive, float membranePotential) {
        if (isActive) {
            return "#FF0000";  // Red for active neurons
        }
        
        switch (type) {
            case NeuronType::Excitatory:
                return "#00FF00";  // Green for excitatory
            case NeuronType::Inhibitory:
                return "#0000FF";  // Blue for inhibitory
            case NeuronType::Sensory:
                return "#FFFF00";  // Yellow for sensory
            case NeuronType::Motor:
                return "#FF00FF";  // Magenta for motor
            default:
                return "#FFFFFF";  // White for unknown
        }
    }
    
    // Generate color based on synapse weight
    inline std::string getSynapseColor(float weight) {
        if (weight > 0.5f) return "#FF0000";      // Strong red for strong positive
        if (weight > 0.2f) return "#FF6600";      // Orange for moderate
        if (weight > 0.0f) return "#FF9900";      // Light orange for weak
        if (weight < -0.5f) return "#0000FF";     // Strong blue for strong negative
        if (weight < -0.2f) return "#3333FF";     // Dark blue for moderate negative
        return "#CCCCCC";                          // Gray for zero
    }
    
    // Generate color based on activity level
    inline std::string getActivityColor(float level) {
        int intensity = static_cast<int>(255.0f * std::min(1.0f, level));
        std::stringstream ss;
        ss << "#" << std::hex << std::setw(2) << std::setfill('0') << intensity
           << std::setw(2) << 0 << std::setw(2) << 0;
        return ss.str();
    }
    
    // Generate color based on neuromodulator type and concentration
    inline std::string getNeuromodulatorColor(const std::string& type, float concentration) {
        float intensity = std::min(1.0f, concentration * 2.0f);
        int alpha = static_cast<int>(255.0f * intensity);
        
        if (type == "dopamine") {
            return "#FF6600";  // Orange for dopamine
        } else if (type == "curiosity") {
            return "#00FF00";  // Green for curiosity
        } else if (type == "novelty") {
            return "#FFFF00";  // Yellow for novelty
        } else if (type == "prediction_error") {
            return "#FF0080";  // Pink for prediction error
        }
        
        std::stringstream ss;
        ss << "#" << std::hex << std::setw(2) << 0x99
           << std::setw(2) << 0x99 << std::setw(2) << 0x99
           << std::setw(2) << alpha;
        return ss.str();
    }
    
    // Convert color to hex string
    inline std::string colorToHex(float r, float g, float b, float a = 1.0f) {
        int ir = static_cast<int>(255 * r);
        int ig = static_cast<int>(255 * g);
        int ib = static_cast<int>(255 * b);
        int ia = static_cast<int>(255 * a);
        
        std::stringstream ss;
        ss << "#" << std::hex << std::setw(2) << std::setfill('0')
           << ir << ig << ib << ia;
        return ss.str();
    }
    
    // Format time for display
    inline std::string formatTime(double time) {
        if (time < 0.001) {
            return std::to_string(static_cast<int>(time * 1000000)) + "μs";
        } else if (time < 1.0) {
            return std::to_string(static_cast<int>(time * 1000)) + "ms";
        } else {
            return std::to_string(static_cast<int>(time)) + "s";
        }
    }
    
    // Format percentage
    inline std::string formatPercentage(float value) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << (value * 100.0f) << "%";
        return ss.str();
    }
}

// Neural Network Visualizer
class NeuralNetworkVisualizer : public BrainVisualizer {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    NeuralNetworkVisualizer(const std::string& type = "default");
    ~NeuralNetworkVisualizer() override;
    
    void initialize() override;
    void update(const Brain& brain) override;
    void render() override;
    void close() override;
    
    std::string getName() const override { return "NeuralNetwork"; }
    std::string getDescription() const override { return "Visualizes neural network structure and activity"; }
    
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
};

// Memory Visualizer
class MemoryVisualizer : public BrainVisualizer {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    MemoryVisualizer(const std::string& type = "default");
    ~MemoryVisualizer() override;
    
    void initialize() override;
    void update(const Brain& brain) override;
    void render() override;
    void close() override;
    
    std::string getName() const override { return "Memory"; }
    std::string getDescription() const override { return "Visualizes memory systems and their contents"; }
    
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
};

// Neuromodulation Visualizer
class NeuromodulationVisualizer : public BrainVisualizer {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    NeuromodulationVisualizer(const std::string& type = "default");
    ~NeuromodulationVisualizer() override;
    
    void initialize() override;
    void update(const Brain& brain) override;
    void render() override;
    void close() override;
    
    std::string getName() const override { return "Neuromodulation"; }
    std::string getDescription() const override { return "Visualizes neuromodulation systems and their effects"; }
    
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
};

// Cognitive Visualizer
class CognitiveVisualizer : public BrainVisualizer {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    CognitiveVisualizer(const std::string& type = "default");
    ~CognitiveVisualizer() override;
    
    void initialize() override;
    void update(const Brain& brain) override;
    void render() override;
    void close() override;
    
    std::string getName() const override { return "Cognitive"; }
    std::string getDescription() const override { return "Visualizes cognitive systems and their operations"; }
    
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
};

// Development Visualizer
class DevelopmentVisualizer : public BrainVisualizer {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    DevelopmentVisualizer(const std::string& type = "default");
    ~DevelopmentVisualizer() override;
    
    void initialize() override;
    void update(const Brain& brain) override;
    void render() override;
    void close() override;
    
    std::string getName() const override { return "Development"; }
    std::string getDescription() const override { return "Visualizes developmental stages and capability growth"; }
    
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
};

// Performance Visualizer
class PerformanceVisualizer : public BrainVisualizer {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    PerformanceVisualizer(const std::string& type = "default");
    ~PerformanceVisualizer() override;
    
    void initialize() override;
    void update(const Brain& brain) override;
    void render() override;
    void close() override;
    
    std::string getName() const override { return "Performance"; }
    std::string getDescription() const override { return "Visualizes performance metrics and learning progress"; }
    
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
};

// Implementation details (to be continued in .cpp file)

} // namespace nlm