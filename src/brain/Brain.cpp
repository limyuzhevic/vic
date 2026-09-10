// Get neuromodulation system access
    Dopamine* getDopamine() { return pImpl->dopamine.get(); }
    const Dopamine* getDopamine() const { return pImpl->dopamine.get(); }
    
    Curiosity* getCuriosity() { return pImpl->curiosity.get(); }
    const Curiosity* getCuriosity() const { return pImpl->curiosity.get(); }
    
    Novelty* getNovelty() { return pImpl->novelty.get(); }
    const Novelty* getNovelty() const { return pImpl->novelty.get(); }
    
    PredictionError* getPredictionErrorSignal() { return pImpl->predictionError.get(); }
    const PredictionError* getPredictionErrorSignal() const { return pImpl->predictionError.get(); }
    
    Acetylcholine* getAcetylcholine() { return pImpl->acetylcholine.get(); }
    const Acetylcholine* getAcetylcholine() const { return pImpl->acetylcholine.get(); }
    
    Norepinephrine* getNorepinephrine() { return pImpl->norepinephrine.get(); }
    const Norepinephrine* getNorepinephrine() const { return pImpl->norepinephrine.get(); }
    
    Serotonin* getSerotonin() { return pImpl->serotonin.get(); }
    const Serotonin* getSerotonin() const { return pImpl->serotonin.get(); }
