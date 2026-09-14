#include "NeuronIO.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

void NeuronIO::receiveExcitatoryInput(MembranePotential amplitude) {
    pImpl->synapticInput += amplitude;
}

void NeuronIO::receiveInhibitoryInput(MembranePotential amplitude) {
    pImpl->synapticInput -= amplitude;
}

void NeuronIO::receiveModulatoryInput(MembranePotential amplitude) {
    pImpl->adaptationVariable += amplitude * 0.1f;
}

void NeuronIO::injectCurrent(MembranePotential current) {
    pImpl->synapticInput += current;
}

void NeuronIO::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

}