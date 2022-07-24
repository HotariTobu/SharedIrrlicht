#pragma once

#ifndef __GUI_ANIMATION_CENTER_H_INCLUDED__
#define __GUI_ANIMATION_CENTER_H_INCLUDED__

#include <irrList.h>
#include <ITimer.h>

#include "GUIAnimationContainer.h"

namespace irr {
	namespace gui {
		class GUIAnimationCenter : public IReferenceCounted {
		public:
			GUIAnimationCenter(ITimer* tim) : tim(tim), Containers(core::list<GUIAnimationContainer*>()) {}
			~GUIAnimationCenter();

			void animate();

			void addContainer(GUIAnimationContainer* container);
			GUIAnimationContainer* getContainer(s32 id) const;
			core::list<GUIAnimationContainer*> getContainers() const;
			void removeContainer(GUIAnimationContainer* container);
			void removeContainer(s32 id);
			void removeContainers();

		private:
			ITimer* tim;
			core::list<GUIAnimationContainer*> Containers;
		};
	}
}

#endif // !__GUI_ANIMATION_CENTER_H_INCLUDED__
