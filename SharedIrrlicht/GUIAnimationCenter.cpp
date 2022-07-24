#include "GUIAnimationCenter.h"

namespace irr {
	namespace gui {
		GUIAnimationCenter::~GUIAnimationCenter() {
			removeContainers();
		}

		void GUIAnimationCenter::animate() {
			auto end = Containers.end();
			for (auto ite = Containers.begin(); ite != end; ite++) {
				(*ite)->animateElement(tim->getTime());
			}
		}

		void GUIAnimationCenter::addContainer(GUIAnimationContainer* container) {
			if (container) {
				Containers.push_back(container);
			}
		}

		GUIAnimationContainer* GUIAnimationCenter::getContainer(s32 id) const {
			auto end = Containers.end();
			for (auto ite = Containers.begin(); ite != end; ite++) {
				if ((*ite)->getElement()->getID() == id) {
					return (*ite);
				}
			}

			return 0;
		}

		core::list<GUIAnimationContainer*> GUIAnimationCenter::getContainers() const {
			return Containers;
		}

		void GUIAnimationCenter::removeContainer(GUIAnimationContainer* container) {
			auto end = Containers.end();
			for (auto ite = Containers.begin(); ite != end; ite++) {
				if ((*ite) == container) {
					container->drop();
					Containers.erase(ite);
					return;
				}
			}
		}

		void GUIAnimationCenter::removeContainer(s32 id) {
			auto end = Containers.end();
			for (auto ite = Containers.begin(); ite != end; ite++) {
				if ((*ite)->getElement()->getID() == id) {
					(*ite)->drop();
					Containers.erase(ite);
					return;
				}
			}
		}

		void GUIAnimationCenter::removeContainers() {
			auto end = Containers.end();
			for (auto ite = Containers.begin(); ite != end; ite++) {
				(*ite)->drop();
			}
			Containers.clear();
		}
	}
}