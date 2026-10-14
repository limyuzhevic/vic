#pragma once

#include <memory>
#include <string>
#include "brain/Brain.hpp"
#include "learning_experiment.hpp"

namespace nlm {

/**
 * Run plasticity learning experiment
 * 
 * This function tests activity-dependent synaptic plasticity by applying
 * repeated input patterns and measuring learning through weight changes.
 * 
 * @param brain Shared pointer to the brain instance to test
 * @note Extracted from main.cpp to improve modularity and testability.
 *       Combines plasticity enabling, stimulus application, and learning analysis.
 */
void runPlasticityExperiment(std::shared_ptr<Brain> brain);

} // namespace nlm
