// Brain_neuromodulation.cpp implementation
// Contains:
// - Brain::getDopamine() method (lines 1056-1058 from original Brain.cpp)
// - Brain::getCuriosity() method (lines 1060-1062 from original Brain.cpp)
// - Brain::getNovelty() method (lines 1064-1066 from original Brain.cpp)
// - Brain::getPredictionErrorSignal() method (lines 1068-1070 from original Brain.cpp)
// - Any other neuromodulation methods

namespace nlm {

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}
}