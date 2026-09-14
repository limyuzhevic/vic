#pragma once

namespace nlm {

struct PlasticityFlagsImpl {
    bool hebbian;
    bool stdp;
    bool reward_modulated;
    
    PlasticityFlagsImpl() : hebbian(false), stdp(false), reward_modulated(false) {}
};

}