#ifndef STDP_TEST_H
#define STDP_TEST_H

#include <memory>
#include "brain/Brain.hpp"

using namespace nlm;

void runStdpVerification(std::shared_ptr<Brain> brain);

#endif // STDP_TEST_H