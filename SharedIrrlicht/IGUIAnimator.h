#pragma once

#ifndef __I_GUI_ANIMATOR_H_INCLUDED__
#define __I_GUI_ANIMATOR_H_INCLUDED__

#include <IGUIElement.h>

namespace irr {
	namespace gui {
		class IGUIAnimator : public IReferenceCounted {
		public:
			//! Animates a GUI element.
			/** \param element Element to animate.
			\param timeMs Current time in milli seconds. */
			virtual void animateElement(IGUIElement* element, u32 timeMs) = 0;

			//! Returns if the animator has finished.
			/** This is only valid for non-looping animators with a discrete end state.
			\return true if the animator has finished, false if it is still running. */
			virtual bool hasFinished(void) const
			{
				return false;
			}
		};
	}
}

#endif // !__I_GUI_ANIMATOR_H_INCLUDED__
