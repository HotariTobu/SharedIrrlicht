#pragma once

#ifndef __COORDINATE_SYSTEM_CURSOR_H_INCLUDED__
#define __COORDINATE_SYSTEM_CURSOR_H_INCLUDED__

#include <IVideoDriver.h>
#include <ISceneManager.h>
#include <ISceneNode.h>

#include <vector3d.h>
#include <S3DVertex.h>

namespace irr {
	namespace scene {
		class CoordinateSystemCursor : public ISceneNode {
		public:
			CoordinateSystemCursor(ISceneNode* parent, ISceneManager* mgr, s32 id);

			virtual void OnRegisterSceneNode() override;
			virtual void render() override;
			virtual const core::aabbox3d<f32>& getBoundingBox() const override;
			virtual u32 getMaterialCount() const override;
			virtual video::SMaterial& getMaterial(u32 i) override;

		private:
			core::aabbox3d<f32> Box;
			video::S3DVertex Vertices[4];
			video::SMaterial Material;
		};
	}
}

#endif // !__COORDINATE_SYSTEM_CURSOR_H_INCLUDED__