// Brain IO header - contains declarations for brain I/O operations

#pragma once

#include "Brain.hpp"

namespace nlm {
    namespace BrainIO {
        void addRegion(Brain& brain, const std::string& name);
        NeuralRegion* getRegion(Brain& brain, RegionId regionId);
        const NeuralRegion* getRegion(const Brain& brain, RegionId regionId);
        std::vector<NeuralRegion*> getAllRegions(Brain& brain);
        const std::vector<NeuralRegion*>& getAllRegions(const Brain& brain);
        void receiveSensoryInput(Brain& brain, const class SensoryInput& input);
        void injectCurrent(Brain& brain, NeuronId neuron, MembranePotential current);
        void injectCurrentToNeurons(Brain& brain, NeuronType type, MembranePotential current);
        std::unique_ptr<class Action> produceAction(Brain& brain);
        float getExcitationInhibitionRatio(const Brain& brain);
        void reset(Brain& brain);
    }
}
