#include "GUIAnimationContainer.h"

namespace irr {
	namespace gui {
		GUIAnimationContainer::~GUIAnimationContainer() {
			removeAnimators();
		}

		void GUIAnimationContainer::animateElement(u32 timeMs) {
			if (Element) {
				auto end = Animators.end();
				for (auto ite = Animators.begin(); ite != end; ite++) {
					(*ite)->animateElement(Element, timeMs);
				}
			}
		}

		void GUIAnimationContainer::addAnimator(IGUIAnimator* animator) {
			if (animator) {
				Animators.push_back(animator);
			}
		}

		core::list<IGUIAnimator*> GUIAnimationContainer::getAnimators() const {
			return Animators;
		}

		void GUIAnimationContainer::removeAnimator(IGUIAnimator* animator) {
			auto end = Animators.end();
			for (auto ite = Animators.begin(); ite != end; ite++) {
				if ((*ite) == animator) {
					animator->drop();
					Animators.erase(ite);
					return;
				}
			}
		}

		void GUIAnimationContainer::removeAnimators() {
			auto end = Animators.end();
			for (auto ite = Animators.begin(); ite != end; ite++) {
				(*ite)->drop();
			}
			Animators.clear();
		}

		void GUIAnimationContainer::setElement(IGUIElement* element) {
			Element = element;
		}

		IGUIElement* GUIAnimationContainer::getElement() const {
			return Element;
		}
	}
}