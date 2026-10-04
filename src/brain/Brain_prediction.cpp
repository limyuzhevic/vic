// Brain_prediction.cpp implementation
// Contains:
// - Brain::getPredictionSystem() method (lines 1022-1024 from original Brain.cpp)
// - Any other prediction system methods

namespace nlm {

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}
}