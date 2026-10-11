#pragma once

#include "AdvancedConfig.hpp"
#include <string>

namespace nlm {

class NetworkConfig : public AdvancedConfig {
public:
    NetworkConfig();
    ~NetworkConfig();
    
    // Neural architecture parameters
    void setLayerCount(int layers);
    void setNeuronCount(int neurons);
    void setInputSize(int size);
    void setOutputSize(int size);
    
    // Connection parameters
    void setConnectionProbability(double prob);
    void setMaxConnectionsPerNeuron(int max);
    void setRecurrentConnections(bool recurrent);
    
    // Neuron parameters
    void setNeuronThreshold(double threshold);
    void setNeuronDecay(double decay);
    void setNeuronRefractoryPeriod(double period);
    
    // Network topology
    void setTopologyType(const std::string& topology);
    void setSmallWorldParameter(double parameter);
    void setClusteringCoefficient(double coefficient);
    
    // Get network configuration summary
    std::string getNetworkSummary() const;
    
    // Disable copying, enable moving
    NetworkConfig(const NetworkConfig&) = delete;
    NetworkConfig& operator=(const NetworkConfig&) = delete;
    NetworkConfig(NetworkConfig&&) noexcept;
    NetworkConfig& operator=(NetworkConfig&&) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> pImplNetwork;
};

} // namespace nlm
