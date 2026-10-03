#include "MemoryVisualizer.hpp"
#include "BrainVisualizer.hpp"
#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace nlm {

void MemoryVisualizer::setRegionFilter(std::vector<RegionId> regions) {
    regionFilter_ = std::move(regions);
}

void MemoryVisualizer::setNeuronTypeFilter(std::vector<NeuronType> types) {
    neuronTypeFilter_ = std::move(types);
}

void MemoryVisualizer::setFocusNeuron(NeuronId neuron) {
    focusNeuron_ = neuron;
}

void MemoryVisualizer::setOutputFormat(const std::string& format) {
    outputFormat_ = format;
}

void MemoryVisualizer::setDetailLevel(int level) {
    detailLevel_ = std::max(0, std::min(level, 5));
}

void MemoryVisualizer::setColorScheme(const std::string& scheme) {
    colorScheme_ = scheme;
}

} // namespace nlm