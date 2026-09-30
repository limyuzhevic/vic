#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Legacy Hebbian class for backward compatibility
// Now inherits from HebbianRule to use the real implementation
class Hebbian : public HebbianRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // All methods inherited from HebbianRule
    // Some may override for specific Hebbian behavior
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
