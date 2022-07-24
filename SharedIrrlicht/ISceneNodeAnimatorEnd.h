#pragma once

#ifndef __I_SCENE_NODE_ANIMATOR_END_H_INCLUDED__
#define __I_SCENE_NODE_ANIMATOR_END_H_INCLUDED__

#include <ISceneNode.h>

namespace irr {
	namespace scene {
		class ISceneNodeAnimatorEnd : public ISceneNodeAnimator {
		public:
			/*
			! constructor
			\param endTimeMs The animation time. If it passed the time over, the animator will be removed from the scene node.
			*/
			ISceneNodeAnimatorEnd(u32 endTimeMs) : LastTimeMs(0), StartTimeMs(0), EndTimeMs(endTimeMs), DifferenceTimeMs(0), ElapsedTimeMs(0), LastElapsedTimeMs(0), RelativeTimeMs(0), IsFirst(true) {}

			/*
			! If you call ISceneNodeAnimatorEnd::check() in animateNode(), this will be called at first of animation.
			\param node Node to animate.
			*/
			virtual void begin(ISceneNode* node) = 0;
			/*
			! If you call ISceneNodeAnimatorEnd::check() in animateNode(), this will be called at last of animation.
			\param node Node to animate.
			*/
			virtual void end(ISceneNode* node) = 0;

			/*
			! Check if the animation begins or ends, and update protected member values.
			For example:
			\code
			void animateNode(ISceneNode* node, u32 timeMs) {
				if (check(node, timeMs)) {
					// animate the scene node here
				}
			}
			\endcode
			\param node Node to animate.
			\param timeMs Current time in milli seconds.
			\return False if the animation begins or end, otherwise true.
			*/
			bool check(ISceneNode* node, u32 timeMs) {
				if (IsFirst) {
					IsFirst = false;

					StartTimeMs = timeMs;
					LastTimeMs = timeMs;

					begin(node);

					return false;
				}
				else if (timeMs > (StartTimeMs + EndTimeMs)) {
					node->removeAnimator(this);
					end(node);
					return false;
				}
				else {
					DifferenceTimeMs = timeMs - LastTimeMs;
					ElapsedTimeMs = timeMs - StartTimeMs;
					LastElapsedTimeMs = LastTimeMs - StartTimeMs;
					RelativeTimeMs = (f32)DifferenceTimeMs / (f32)EndTimeMs;
					LastTimeMs = timeMs;
					return true;
				}
			}

			/*
			\param targetValue The target value to animate.
			\return The change value from deltaTimeMs.
			*/
			template<class T>
			T modify(T targetValue) {
				return (T)(targetValue * RelativeTimeMs);
			}

		protected:
			u32 LastTimeMs, StartTimeMs, EndTimeMs, DifferenceTimeMs, ElapsedTimeMs, LastElapsedTimeMs;
			f32 RelativeTimeMs;

		private:
			bool IsFirst;

		};
	}
}

#endif // !__I_SCENE_NODE_ANIMATOR_END_H_INCLUDED__