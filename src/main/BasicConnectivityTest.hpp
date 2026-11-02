#pragma once

#include <memory>
#include "brain/Brain.hpp"
#include "main/TestUtilities.hpp"
#include "core/Types/Types.hpp"

namespace nlm {

namespace basic_connectivity {

class BasicConnectivityTest {
public:
    static void run(std::shared_ptr<Brain> brain);
    
private:
    static void injectCurrentPattern(std::shared_ptr<Brain> brain, size_t neuronCount, float current);
    static void runConnectivitySteps(std::shared_ptr<Brain> brain, size_t steps);
    static void analyzeResults(std::shared_ptr<Brain> brain, size_t initialSpikes, size_t finalSpikes);
};

} // namespace basic_connectivity

} // namespace nlm