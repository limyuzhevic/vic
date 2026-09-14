#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Inter-region connection structure for connecting neurons in different brain regions
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    SynapseId synapseId;
    SynapticWeight weight;
    Delay delay;
    
    InterRegionConnection() 
        : sourceRegion(0), targetRegion(0), synapseId(0), weight(0.0f), delay(0) {}
    
    InterRegionConnection(RegionId src, RegionId tgt, SynapseId syn, 
                         SynapticWeight w, Delay d)
        : sourceRegion(src), targetRegion(tgt), synapseId(syn), 
          weight(w), delay(d) {}
};

} // namespace nlm
