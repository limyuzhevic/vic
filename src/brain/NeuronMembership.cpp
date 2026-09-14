#include "NeuronMembership.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

void NeuronMembership::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

RegionId NeuronMembership::getRegionId() const {
    return pImpl->regionId;
}

void NeuronMembership::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

PopulationId NeuronMembership::getPopulationId() const {
    return pImpl->populationId;
}

}