#include "DevelopmentVisualizer.hpp"
#include "BrainVisualizer.hpp"
#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace nlm {

void DevelopmentVisualizer::setRegionFilter(std::vector<RegionId> regions) {
    regionFilter_ = std::move(regions);
}

void DevelopmentVisualizer::setNeuronTypeFilter(std::vector<NeuronType> types) {
    neuronTypeFilter_ = std::move(types);
}

void DevelopmentVisualizer::setFocusNeuron(NeuronId neuron) {
    focusNeuron_ = neuron;
}

void DevelopmentVisualizer::setOutputFormat(const std::string& format) {
    outputFormat_ = format;
}

void DevelopmentVisualizer::setDetailLevel(int level) {
    detailLevel_ = std::max(0, std::min(level, 5));
}

void DevelopmentVisualizer::setColorScheme(const std::string& scheme) {
    colorScheme_ = scheme;
}

} // namespace nlm