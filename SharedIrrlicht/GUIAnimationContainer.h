#pragma once

#ifndef __GUI_ANIMATION_CONTAINER_H_INCLUDED__
#define __GUI_ANIMATION_CONTAINER_H_INCLUDED__

#include <irrList.h>
#include <IGUIElement.h>

#include "IGUIAnimator.h"

namespace irr {
	namespace gui {
		class GUIAnimationContainer : public IReferenceCounted {
		public:
			GUIAnimationContainer() : Element(0), Animators(core::list<IGUIAnimator*>()) {}
			~GUIAnimationContainer();

			void animateElement(u32 timeMs);

			void addAnimator(IGUIAnimator* animator);
			core::list<IGUIAnimator*> getAnimators() const;
			void removeAnimator(IGUIAnimator* animator);
			void removeAnimators();
			void setElement(IGUIElement* element);
			IGUIElement* getElement() const;

		private:
			IGUIElement* Element;
			core::list<IGUIAnimator*> Animators;
		};
	}
}

#endif // !__GUI_ANIMATION_CONTAINER_H_INCLUDED__
