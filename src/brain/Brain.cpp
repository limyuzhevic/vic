// Add these missing getter methods in Brain.cpp after the other getXXX methods

// Add getter for acetylcholine
Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

Acetylcholine* Brain::getAcetylcholine() const {
    return pImpl->acetylcholine.get();
}

// Add getter for norepinephrine
Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Norepinephrine* Brain::getNorepinephrine() const {
    return pImpl->norepinephrine.get();
}

// Add getter for serotonin
Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
}

Serotonin* Brain::getSerotonin() const {
    return pImpl->serotonin.get();
}