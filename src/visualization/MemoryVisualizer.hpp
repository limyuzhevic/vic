#include "NeuralNetworkVisualizer.hpp"
#include "BrainVisualizer.hpp"
#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>

namespace nlm {

void NeuralNetworkVisualizer::setRegionFilter(std::vector<RegionId> regions) {
    regionFilter_ = std::move(regions);
}

void NeuralNetworkVisualizer::setNeuronTypeFilter(std::vector<NeuronType> types) {
    neuronTypeFilter_ = std::move(types);
}

void NeuralNetworkVisualizer::setFocusNeuron(NeuronId neuron) {
    focusNeuron_ = neuron;
}

void NeuralNetworkVisualizer::setOutputFormat(const std::string& format) {
    outputFormat_ = format;
}

void NeuralNetworkVisualizer::setDetailLevel(int level) {
    detailLevel_ = std::max(0, std::min(level, 5));
}

void NeuralNetworkVisualizer::setColorScheme(const std::string& scheme) {
    colorScheme_ = scheme;
}

} // namespace nlm