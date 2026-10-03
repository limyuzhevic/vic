#include "PerformanceVisualizer.hpp"
#include "BrainVisualizer.hpp"
#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>

namespace nlm {

void PerformanceVisualizer::setRegionFilter(std::vector<RegionId> regions) {
    regionFilter_ = std::move(regions);
}

void PerformanceVisualizer::setNeuronTypeFilter(std::vector<NeuronType> types) {
    neuronTypeFilter_ = std::move(types);
}

void PerformanceVisualizer::setFocusNeuron(NeuronId neuron) {
    focusNeuron_ = neuron;
}

void PerformanceVisualizer::setOutputFormat(const std::string& format) {
    outputFormat_ = format;
}

void PerformanceVisualizer::setDetailLevel(int level) {
    detailLevel_ = std::max(0, std::min(level, 5));
}

void PerformanceVisualizer::setColorScheme(const std::string& scheme) {
    colorScheme_ = scheme;
}

} // namespace nlm