#include "BrainPrediction.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainPrediction::initializePredictionSystem(Brain::Impl& pImpl) {
    try {
        NLM_LOG_INFO("Initializing prediction system...");
        
        // Validate Brain::Impl
        if (!pImpl->config) {
            NLM_LOG_ERROR("Failed to initialize prediction system: Configuration is null");
            return;
        }
        
        // Initialize prediction system with current brain state
        if (pImpl->predictionSystem) {
            try {
                pImpl->predictionSystem->initialize(pImpl.get());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize prediction system: " + e.what());
                return;
            }
        }
        
        NLM_LOG_INFO("Prediction system initialization complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during prediction system initialization: ") + e.what());
    }
}

void BrainPrediction::updatePredictionSystem(Brain::Impl& pImpl) {
    if (pImpl->predictionSystem) {
        // Update prediction system with current state
        pImpl->predictionSystem->update(pImpl.get());
        
        // Get prediction error and update neuromodulation
        if (pImpl->predictionError) {
            float predictionError = pImpl->predictionError->getErrorLevel();
            
            // Apply prediction error to neural processing
            // Negative prediction error increases exploration
            if (pImpl->curiosity) {
                pImpl->curiosity->setErrorLevel(std::abs(predictionError));
            }
        }
    }
}

void BrainPrediction::logPredictionStats(const Brain::Impl& pImpl) const {
    if (pImpl->predictionSystem) {
        // Log prediction system statistics
        // This would include prediction accuracy, error rates, etc.
        NLM_LOG_INFO("Prediction system active: computing sensory predictions");
    }
}

void BrainPrediction::integratePredictionWithMemory(Brain::Impl& pImpl) {
    if (pImpl->predictionSystem && pImpl->workingMemory) {
        // Integrate predictions with working memory for better temporal processing
        pImpl->predictionSystem->integrateWithWorkingMemory(pImpl->workingMemory.get());
    }
}

void BrainPrediction::updatePredictionError(Brain::Impl& pImpl) {
    if (pImpl->predictionError) {
        pImpl->predictionError->update(pImpl.get());
    }
}

} // namespace nlm
