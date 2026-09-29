#pragma once

#include <memory>
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "main_learning.hpp"

namespace nlm {

void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

} // namespace nlm