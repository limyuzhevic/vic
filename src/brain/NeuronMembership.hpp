#pragma once

namespace nlm {

struct NeuronMembershipImpl {
    RegionId regionId;
    PopulationId populationId;
    
    NeuronMembershipImpl() : regionId(INVALID_REGION_ID), populationId(INVALID_POPULATION_ID) {}
};

}