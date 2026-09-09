#include "Maturation.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../brain/Neuron.hpp"
#include "../../development/DevelopmentSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    DevelopmentSystem* devSystem = brain->getDevelopmentSystem();
    if (devSystem) {
        double developmentalAge = devSystem->getDevelopmentalAge();
        setProgress(static_cast<float>(developmentalAge / 2000.0));
    }
    
    DevelopmentSystem* devSystemStage = brain->getDevelopmentSystem();
    DevelopmentalStage stage = devSystemStage ? devSystemStage->getStage() : DevelopmentalStage::Adult;
    
    float stageFactor = 0.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            stageFactor = getProgress() * 0.2f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            stageFactor = getProgress() * 0.6f;
            break;
        case DevelopmentalStage::Maturation:
            stageFactor = getProgress() * 1.0f;
            break;
        case DevelopmentalStage::Adult:
            stageFactor = std::min(1.0f, getProgress());
            break;
        case DevelopmentalStage::Aging:
            stageFactor = std::min(1.0f, getProgress());
            break;
    }
    
    float maturationFactor = 1.0f / (1.0f + std::exp(-10.0f * (stageFactor - 0.5f)));
    
    float thresholdFactor = 0.3f + 0.7f * maturationFactor;
    float restingPotentialFactor = 0.5f + 0.5f * maturationFactor;
    float timeConstantFactor = 0.4f + 0.6f * maturationFactor;
    
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        const auto& populations = region->getPopulations();
        for (const auto& population : populations) {
            auto neurons = population->getAllNeurons();
            for (Neuron* neuron : neurons) {
                if (!neuron) continue;
                
                MembranePotential currentThreshold = neuron->getThreshold();
                MembranePotential matureThreshold = pImpl->matureThreshold;
                MembranePotential targetThreshold = matureThreshold - thresholdFactor * (matureThreshold - currentThreshold);
                neuron->setThreshold(targetThreshold);
                
                MembranePotential currentRestingPotential = neuron->getRestingPotential();
                MembranePotential matureRestingPotential = pImpl->matureRestingPotential;
                MembranePotential targetRestingPotential = matureRestingPotential - restingPotentialFactor * (matureRestingPotential - currentRestingPotential);
                neuron->setRestingPotential(targetRestingPotential);
                
                float currentLeakConductance = neuron->getLeakConductance();
                float matureTimeConstant = pImpl->matureTimeConstant;
                float targetLeakConductance = matureTimeConstant * timeConstantFactor + (1.0f - timeConstantFactor) * currentLeakConductance;
                neuron->setLeakConductance(targetLeakConductance);
                
                uint32_t currentRefractoryPeriod = neuron->getRefractoryPeriod();
                uint32_t targetRefractoryPeriod = static_cast<uint32_t>(5.0f + 20.0f * maturationFactor);
                neuron->setRefractoryPeriod(targetRefractoryPeriod);
            }
        }
    }
    
    if (devSystemStage && devSystemStage->isCriticalPeriod()) {
    }
    
    if (stage == DevelopmentalStage::Adult) {
        float finalMaturation = std::min(1.0f, (getProgress() - 0.6f) * 2.0f);
        
        const auto& regions = brain->getRegions();
        for (const auto& region : regions) {
            const auto& populations = region->getPopulations();
            for (const auto& population : populations) {
                auto neurons = population->getAllNeurons();
                for (Neuron* neuron : neurons) {
                    if (!neuron) continue;
                    
                    neuron->setThreshold(pImpl->matureThreshold);
                    neuron->setRestingPotential(pImpl->matureRestingPotential);
                    neuron->setLeakConductance(pImpl->matureTimeConstant);
                    neuron->setRefractoryPeriod(static_cast<uint32_t>(25.0f));
                }
            }
        }
    } else if (stage == DevelopmentalStage::Aging) {
        float agingFactor = std::min(1.0f, (getProgress() - 0.8f) * 3.0f);
        const auto& regions = brain->getRegions();
        for (const auto& region : regions) {
            const auto& populations = region->getPopulations();
            for (const auto& population : populations) {
                auto neurons = population->getAllNeurons();
                for (Neuron* neuron : neurons) {
                    if (!neuron) continue;
                    
                    float agingAdjust = 0.9f + 0.2f * agingFactor;
                    MembranePotential currentThreshold = neuron->getThreshold();
                    neuron->setThreshold(currentThreshold * agingAdjust);
                }
            }
        }
    }
}

MembranePotential Maturation::getMatureThreshold() const {
    return pImpl->matureThreshold;
}

MembranePotential Maturation::getMatureRestingPotential() const {
    return pImpl->matureRestingPotential;
}

float Maturation::getMatureTimeConstant() const {
    return pImpl->matureTimeConstant;
}

} // namespace nlm