#pragma once

#include "BrainVisualizer.hpp"
#include <map>
#include <string>
#include <vector>

namespace nlm {

class NeuralNetworkVisualizer : public BrainVisualizer {
public:
    NeuralNetworkVisualizer(const std::string& type = "default") : type_(type) {}
    ~NeuralNetworkVisualizer() override = default;
    
    void initialize() override {}
    void update(const Brain& brain) override {}
    void render() override {}
    void close() override {}
    
    std::string getName() const override { return "NeuralNetwork"; }
    std::string getDescription() const override { return "Displays neural network topology, connections, and activity"; }
    
    void setUpdateRate(double hz) override {}
    double getUpdateRate() const override { return updateRate_; }
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
    
private:
    std::string type_;
    double updateRate_ = 30.0;
    std::vector<RegionId> regionFilter_;
    std::vector<NeuronType> neuronTypeFilter_;
    NeuronId focusNeuron_ = NeuronId(0);
    std::string outputFormat_ = "text";
    int detailLevel_ = 1;
    std::string colorScheme_ = "default";
};

class MemoryVisualizer : public BrainVisualizer {
public:
    MemoryVisualizer(const std::string& type = "default") : type_(type) {}
    ~MemoryVisualizer() override = default;
    
    void initialize() override {}
    void update(const Brain& brain) override {}
    void render() override {}
    void close() override {}
    
    std::string getName() const override { return "Memory"; }
    std::string getDescription() const override { return "Visualizes working, episodic, and associative memory systems"; }
    
    void setUpdateRate(double hz) override {}
    double getUpdateRate() const override { return updateRate_; }
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
    
private:
    std::string type_;
    double updateRate_ = 30.0;
    std::vector<RegionId> regionFilter_;
    std::vector<NeuronType> neuronTypeFilter_;
    NeuronId focusNeuron_ = NeuronId(0);
    std::string outputFormat_ = "text";
    int detailLevel_ = 1;
    std::string colorScheme_ = "default";
};

class NeuromodulationVisualizer : public BrainVisualizer {
public:
    NeuromodulationVisualizer(const std::string& type = "default") : type_(type) {}
    ~NeuromodulationVisualizer() override = default;
    
    void initialize() override {}
    void update(const Brain& brain) override {}
    void render() override {}
    void close() override {}
    
    std::string getName() const override { return "Neuromodulation"; }
    std::string getDescription() const override { return "Displays neuromodulator levels, effects, and dynamics"; }
    
    void setUpdateRate(double hz) override {}
    double getUpdateRate() const override { return updateRate_; }
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
    
private:
    std::string type_;
    double updateRate_ = 30.0;
    std::vector<RegionId> regionFilter_;
    std::vector<NeuronType> neuronTypeFilter_;
    NeuronId focusNeuron_ = NeuronId(0);
    std::string outputFormat_ = "text";
    int detailLevel_ = 1;
    std::string colorScheme_ = "default";
};

class CognitiveVisualizer : public BrainVisualizer {
public:
    CognitiveVisualizer(const std::string& type = "default") : type_(type) {}
    ~CognitiveVisualizer() override = default;
    
    void initialize() override {}
    void update(const Brain& brain) override {}
    void render() override {}
    void close() override {}
    
    std::string getName() const override { return "Cognitive"; }
    std::string getDescription() const override { return "Visualizes cognitive systems, predictions, and planning"; }
    
    void setUpdateRate(double hz) override {}
    double getUpdateRate() const override { return updateRate_; }
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
    
private:
    std::string type_;
    double updateRate_ = 30.0;
    std::vector<RegionId> regionFilter_;
    std::vector<NeuronType> neuronTypeFilter_;
    NeuronId focusNeuron_ = NeuronId(0);
    std::string outputFormat_ = "text";
    int detailLevel_ = 1;
    std::string colorScheme_ = "default";
};

class DevelopmentVisualizer : public BrainVisualizer {
public:
    DevelopmentVisualizer(const std::string& type = "default") : type_(type) {}
    ~DevelopmentVisualizer() override = default;
    
    void initialize() override {}
    void update(const Brain& brain) override {}
    void render() override {}
    void close() override {}
    
    std::string getName() const override { return "Development"; }
    std::string getDescription() const override { return "Visualizes developmental stages and capability growth over time"; }
    
    void setUpdateRate(double hz) override {}
    double getUpdateRate() const override { return updateRate_; }
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
    
private:
    std::string type_;
    double updateRate_ = 30.0;
    std::vector<RegionId> regionFilter_;
    std::vector<NeuronType> neuronTypeFilter_;
    NeuronId focusNeuron_ = NeuronId(0);
    std::string outputFormat_ = "text";
    int detailLevel_ = 1;
    std::string colorScheme_ = "default";
};

class PerformanceVisualizer : public BrainVisualizer {
public:
    PerformanceVisualizer(const std::string& type = "default") : type_(type) {}
    ~PerformanceVisualizer() override = default;
    
    void initialize() override {}
    void update(const Brain& brain) override {}
    void render() override {}
    void close() override {}
    
    std::string getName() const override { return "Performance"; }
    std::string getDescription() const override { return "Displays performance metrics, spike statistics, and learning progress"; }
    
    void setUpdateRate(double hz) override {}
    double getUpdateRate() const override { return updateRate_; }
    
    void setRegionFilter(std::vector<RegionId> regions) override;
    void setNeuronTypeFilter(std::vector<NeuronType> types) override;
    void setFocusNeuron(NeuronId neuron) override;
    
    void setOutputFormat(const std::string& format) override;
    void setDetailLevel(int level) override;
    void setColorScheme(const std::string& scheme) override;
    
private:
    std::string type_;
    double updateRate_ = 30.0;
    std::vector<RegionId> regionFilter_;
    std::vector<NeuronType> neuronTypeFilter_;
    NeuronId focusNeuron_ = NeuronId(0);
    std::string outputFormat_ = "text";
    int detailLevel_ = 1;
    std::string colorScheme_ = "default";
};

} // namespace nlm