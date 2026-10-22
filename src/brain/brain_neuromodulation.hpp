#pragma once

#include "Brain.hpp"

namespace nlm {

class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class Config;
class RandomGenerator;

// Neuromodulation system accessors
Dopamine* Brain::getDopamine();
Curiosity* Brain::getCuriosity();
Novelty* Brain::getNovelty();
PredictionError* Brain::getPredictionErrorSignal();

// Other accessor methods
std::shared_ptr<const Config> Brain::getConfig() const;
RandomGenerator* Brain::getRandomGenerator();

// Logging
void Brain::logStatus() const;

} // namespace nlm
