#pragma once

#ifndef __C_SCENE_NODE_ANIMATOR_TPS_H_INCLUDED__
#define __C_SCENE_NODE_ANIMATOR_TPS_H_INCLUDED__

#include <ISceneNode.h>
#include <ICameraSceneNode.h>

namespace irr {
	namespace scene {
		class CSceneNodeAnimatorCameraTPS : public ISceneNodeAnimator {
		public:
			CSceneNodeAnimatorCameraTPS(core::vector3df* centerPos = new core::vector3df(), bool movable = false, f32 rotateSpeed = 0.7f, f32 zoomSpeed = 0.3f) : CenterPos(centerPos), Movable(movable), RotateSpeed(rotateSpeed), ZoomSpeed(zoomSpeed), MinZoomIn(0), MaxZoomOut((f32)0xffffffff), Dragging(false), Zooming(false), Invert(false), Wheel(0), Level(0), Shift(false), Ctrl(false) {}

			virtual void animateNode(ISceneNode* node, u32 timeMs) override;

			virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager = 0) override;

			virtual bool isEventReceiverEnabled() const override {
				return true;
			}

			virtual bool OnEvent(const SEvent& event) override;

			core::vector3df* CenterPos;
			bool Movable;
			f32 RotateSpeed, ZoomSpeed;
			f32 MinZoomIn, MaxZoomOut;

		private:
			bool Dragging;
			bool Zooming;
			bool Invert;
			core::vector2di Difference;
			f32 Wheel, Level;
			bool Shift;
			bool Ctrl;
		};
	}
}

#endif // !__C_SCENE_NODE_ANIMATOR_TPS_H_INCLUDED__