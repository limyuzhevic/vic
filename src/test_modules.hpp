#pragma once

#include "Brain.hpp"

namespace nlm {
namespace test_modules {

void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

} // namespace test_modules
} // namespace nlm
