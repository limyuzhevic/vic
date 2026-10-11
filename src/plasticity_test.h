#ifndef PLASTICITY_TEST_H
#define PLASTICITY_TEST_H

#include <memory>
#include "brain/Brain.hpp"
#include "learning_experiment.h"

using namespace nlm;

void runPlasticityExperiment(std::shared_ptr<Brain> brain);

#endif // PLASTICITY_TEST_H