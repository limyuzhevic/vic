#include "NeuromodulationVisualizer.hpp"
#include "BrainVisualizer.hpp"
#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace nlm {

void NeuromodulationVisualizer::setRegionFilter(std::vector<RegionId> regions) {
    regionFilter_ = std::move(regions);
}

void NeuromodulationVisualizer::setNeuronTypeFilter(std::vector<NeuronType> types) {
    neuronTypeFilter_ = std::move(types);
}

void NeuromodulationVisualizer::setFocusNeuron(NeuronId neuron) {
    focusNeuron_ = neuron;
}

void NeuromodulationVisualizer::setOutputFormat(const std::string& format) {
    outputFormat_ = format;
}

void NeuromodulationVisualizer::setDetailLevel(int level) {
    detailLevel_ = std::max(0, std::min(level, 5));
}

void NeuromodulationVisualizer::setColorScheme(const std::string& scheme) {
    colorScheme_ = scheme;
}

} // namespace nlm