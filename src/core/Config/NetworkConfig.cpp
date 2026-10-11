#include "NetworkConfig.hpp"
#include <algorithm>
#include <sstream>
#include <cmath>

namespace nlm {

struct NetworkConfig::Impl {
    int layerCount = 3;
    int neuronCount = 100;
    int inputSize = 10;
    int outputSize = 2;
    double connectionProbability = 0.2;
    int maxConnectionsPerNeuron = 10;
    bool recurrentConnections = false;
    double neuronThreshold = 1.0;
    double neuronDecay = 0.9;
    double neuronRefractoryPeriod = 5.0;
    std::string topologyType = "feedforward";
    double smallWorldParameter = 0.1;
    double clusteringCoefficient = 0.3;
};

NetworkConfig::NetworkConfig() {
    pImplNetwork = std::make_unique<Impl>();
    
    // Set up default configuration
    setLayerCount(pImplNetwork->layerCount);
    setNeuronCount(pImplNetwork->neuronCount);
    setInputSize(pImplNetwork->inputSize);
    setOutputSize(pImplNetwork->outputSize);
    
    setConnectionProbability(pImplNetwork->connectionProbability);
    setMaxConnectionsPerNeuron(pImplNetwork->maxConnectionsPerNeuron);
    setRecurrentConnections(pImplNetwork->recurrentConnections);
    
    setNeuronThreshold(pImplNetwork->neuronThreshold);
    setNeuronDecay(pImplNetwork->neuronDecay);
    setNeuronRefractoryPeriod(pImplNetwork->neuronRefractoryPeriod);
    
    setTopologyType(pImplNetwork->topologyType);
    setSmallWorldParameter(pImplNetwork->smallWorldParameter);
    setClusteringCoefficient(pImplNetwork->clusteringCoefficient);
}

NetworkConfig::~NetworkConfig() = default;

NetworkConfig::NetworkConfig(NetworkConfig&&) noexcept = default;
NetworkConfig& NetworkConfig::operator=(NetworkConfig&&) noexcept = default;

void NetworkConfig::setLayerCount(int layers) {
    pImplNetwork->layerCount = layers;
    set("nn_layer_count", layers, ConfigSource::Runtime, 
        "Number of layers in the neural network", 
        NEURAL_NETWORK_CATEGORY, TYPE_INT, 1, 100);
}

void NetworkConfig::setNeuronCount(int neurons) {
    pImplNetwork->neuronCount = neurons;
    set("nn_neuron_count", neurons, ConfigSource::Runtime,
        "Number of neurons per layer", 
        NEURAL_NETWORK_CATEGORY, TYPE_INT, 10, 10000);
}

void NetworkConfig::setInputSize(int size) {
    pImplNetwork->inputSize = size;
    set("nn_input_size", size, ConfigSource::Runtime,
        "Size of input layer", 
        NEURAL_NETWORK_CATEGORY, TYPE_INT, 1, 1000);
}

void NetworkConfig::setOutputSize(int size) {
    pImplNetwork->outputSize = size;
    set("nn_output_size", size, ConfigSource::Runtime,
        "Size of output layer", 
        NEURAL_NETWORK_CATEGORY, TYPE_INT, 1, 1000);
}

void NetworkConfig::setConnectionProbability(double prob) {
    pImplNetwork->connectionProbability = prob;
    set("nn_connection_prob", prob, ConfigSource::Runtime,
        "Probability of connections between neurons", 
        NEURAL_NETWORK_CATEGORY, TYPE_DOUBLE, 0.0, 1.0);
}

void NetworkConfig::setMaxConnectionsPerNeuron(int max) {
    pImplNetwork->maxConnectionsPerNeuron = max;
    set("nn_max_connections", max, ConfigSource::Runtime,
        "Maximum connections per neuron", 
        NEURAL_NETWORK_CATEGORY, TYPE_INT, 1, 1000);
}

void NetworkConfig::setRecurrentConnections(bool recurrent) {
    pImplNetwork->recurrentConnections = recurrent;
    set("nn_recurrent_connections", static_cast<bool>(recurrent), ConfigSource::Runtime,
        "Whether the network has recurrent connections", 
        NEURAL_NETWORK_CATEGORY, TYPE_BOOL);
}

void NetworkConfig::setNeuronThreshold(double threshold) {
    pImplNetwork->neuronThreshold = threshold;
    set("nn_neuron_threshold", threshold, ConfigSource::Runtime,
        "Membrane threshold for neurons", 
        NEURAL_NETWORK_CATEGORY, TYPE_DOUBLE, 0.0, 100.0);
}

void NetworkConfig::setNeuronDecay(double decay) {
    pImplNetwork->neuronDecay = decay;
    set("nn_neuron_decay", decay, ConfigSource::Runtime,
        "Membrane decay rate for neurons", 
        NEURAL_NETWORK_CATEGORY, TYPE_DOUBLE, 0.0, 1.0);
}

void NetworkConfig::setNeuronRefractoryPeriod(double period) {
    pImplNetwork->neuronRefractoryPeriod = period;
    set("nn_refractory_period", period, ConfigSource::Runtime,
        "Refractory period after spike (ms)", 
        NEURAL_NETWORK_CATEGORY, TYPE_DOUBLE, 0.0, 100.0);
}

void NetworkConfig::setTopologyType(const std::string& topology) {
    pImplNetwork->topologyType = topology;
    set("nn_topology_type", topology, ConfigSource::Runtime,
        "Network topology type", 
        NEURAL_NETWORK_CATEGORY, TYPE_STRING);
}

void NetworkConfig::setSmallWorldParameter(double parameter) {
    pImplNetwork->smallWorldParameter = parameter;
    set("nn_small_world_param", parameter, ConfigSource::Runtime,
        "Small-world network parameter (0=regular, 1=random)", 
        NEURAL_NETWORK_CATEGORY, TYPE_DOUBLE, 0.0, 1.0);
}

void NetworkConfig::setClusteringCoefficient(double coefficient) {
    pImplNetwork->clusteringCoefficient = coefficient;
    set("nn_clustering_coefficient", coefficient, ConfigSource::Runtime,
        "Clustering coefficient (0=no clustering, 1=complete)", 
        NEURAL_NETWORK_CATEGORY, TYPE_DOUBLE, 0.0, 1.0);
}

std::string NetworkConfig::getNetworkSummary() const {
    std::ostringstream oss;
    oss << "Neural Network Configuration:";
    oss << "\n  Layers: " << pImplNetwork->layerCount;
    oss << "\n  Neurons: " << pImplNetwork->neuronCount;
    oss << "\n  Input size: " << pImplNetwork->inputSize;
    oss << "\n  Output size: " << pImplNetwork->outputSize;
    oss << "\n  Connection probability: " << pImplNetwork->connectionProbability;
    oss << "\n  Max connections per neuron: " << pImplNetwork->maxConnectionsPerNeuron;
    oss << "\n  Recurrent connections: " << (pImplNetwork->recurrentConnections ? "yes" : "no");
    oss << "\n  Neuron threshold: " << pImplNetwork->neuronThreshold;
    oss << "\n  Neuron decay: " << pImplNetwork->neuronDecay;
    oss << "\n  Refractory period: " << pImplNetwork->neuronRefractoryPeriod;
    oss << "\n  Topology: " << pImplNetwork->topologyType;
    oss << "\n  Small world parameter: " << pImplNetwork->smallWorldParameter;
    oss << "\n  Clustering coefficient: " << pImplNetwork->clusteringCoefficient;
    oss << "\n  Validation: " << (validateWithErrors().empty() ? "PASS" : "FAIL");
    return oss.str();
}

} // namespace nlm
