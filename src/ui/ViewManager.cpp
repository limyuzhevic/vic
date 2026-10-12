#include "ViewManager.hpp"
#include <iostream>

namespace nlm {

struct ViewManager::Impl {
    std::shared_ptr<ControlPanel> activeView;
    std::vector<std::shared_ptr<ControlPanel>> views;
    std::string layout;
    int layoutMargin;
    int layoutSpacing;
    
    Impl() : layoutMargin(5), layoutSpacing(10) {}
};

ViewManager::ViewManager() : pImpl(std::make_unique<Impl>()) {}

ViewManager::~ViewManager() = default;

void ViewManager::addView(std::shared_ptr<ControlPanel> view) {
    if (view) {
        pImpl->views.push_back(view);
        std::cout << "View added to manager" << std::endl;
    }
}

void ViewManager::removeView(std::shared_ptr<ControlPanel> view) {
    for (size_t i = 0; i < pImpl->views.size(); ++i) {
        if (pImpl->views[i] == view) {
            pImpl->views.erase(pImpl->views.begin() + i);
            std::cout << "View removed from manager" << std::endl;
            break;
        }
    }
}

void ViewManager::clearViews() {
    pImpl->views.clear();
    pImpl->activeView.reset();
    std::cout << "All views cleared from manager" << std::endl;
}

void ViewManager::setActiveView(std::shared_ptr<ControlPanel> view) {
    pImpl->activeView = view;
    std::cout << "Active view set in manager" << std::endl;
}

std::shared_ptr<ControlPanel> ViewManager::getActiveView() const {
    return pImpl->activeView;
}

void ViewManager::setLayout(const std::string& layout) {
    pImpl->layout = layout;
    std::cout << "View manager layout set to: " << layout << std::endl;
}

void ViewManager::setLayoutMargin(int margin) {
    pImpl->layoutMargin = margin;
}

void ViewManager::setLayoutSpacing(int spacing) {
    pImpl->layoutSpacing = spacing;
}

void ViewManager::showAllViews() {
    for (const auto& view : pImpl->views) {
        if (view) {
            view->show();
        }
    }
    std::cout << "All views shown" << std::endl;
}

void ViewManager::hideAllViews() {
    for (const auto& view : pImpl->views) {
        if (view) {
            view->hide();
        }
    }
    std::cout << "All views hidden" << std::endl;
}

void ViewManager::maximizeView(std::shared_ptr<ControlPanel> view) {
    if (view) {
        view->setSize(1920, 1080); // Full screen size
        std::cout << "View maximized" << std::endl;
    }
}

void ViewManager::restoreView(std::shared_ptr<ControlPanel> view) {
    if (view) {
        view->setSize(200, 400); // Normal size
        std::cout << "View restored" << std::endl;
    }
}

void ViewManager::renderAllViews() {
    for (const auto& view : pImpl->views) {
        if (view) {
            view->render();
        }
    }
}

void ViewManager::updateAllViews() {
    for (const auto& view : pImpl->views) {
        if (view) {
            view->update();
        }
    }
}

void ViewManager::setViewTitle(std::shared_ptr<ControlPanel> view, const std::string& title) {
    if (view) {
        view->setTitle(title);
    }
}

void ViewManager::setViewTheme(std::shared_ptr<ControlPanel> view, const std::string& theme) {
    if (view) {
        view->setTheme(theme);
    }
}

void ViewManager::setViewDockPosition(std::shared_ptr<ControlPanel> view, const std::string& position) {
    if (view) {
        view->setDockPosition(position);
    }
}

} // namespace nlm