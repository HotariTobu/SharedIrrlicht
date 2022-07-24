#include "CoordinateSystemCursor.h"

namespace irr {
	namespace scene {
		CoordinateSystemCursor::CoordinateSystemCursor(ISceneNode *parent, ISceneManager *mgr, s32 id) : ISceneNode(parent, mgr, id)
		{
			Material.Wireframe = false;
			Material.Lighting = false;

			Vertices[0] = video::S3DVertex(0, 0, 0, 0, 0, 0, (255, 0, 0, 0), 0, 0);
			Vertices[1] = video::S3DVertex(1, 0, 0, 0, 0, 0, video::SColor(255, 255, 0, 0), 0, 0);
			Vertices[2] = video::S3DVertex(0, 1, 0, 0, 0, 0, video::SColor(255, 0, 255, 0), 0, 0);
			Vertices[3] = video::S3DVertex(0, 0, 1, 0, 0, 0, video::SColor(255, 0, 0, 255), 0, 0);

			Box.reset(Vertices[0].Pos);
			for (s32 i = 1; i < 4; ++i)
			{
				Box.addInternalPoint(Vertices[i].Pos);
			}
		}

		void CoordinateSystemCursor::OnRegisterSceneNode()
		{
			if (IsVisible)
			{
				SceneManager->registerNodeForRendering(this);
			}

			ISceneNode::OnRegisterSceneNode();
		}

		void CoordinateSystemCursor::render()
		{
			video::IVideoDriver *drv = SceneManager->getVideoDriver();

			drv->setMaterial(Material);
			drv->setTransform(video::ETS_WORLD, AbsoluteTransformation);
			drv->draw3DLine(core::vector3df(), core::vector3df(1, 0, 0), video::SColor(255, 255, 0, 0));
			drv->draw3DLine(core::vector3df(), core::vector3df(0, 1, 0), video::SColor(255, 0, 255, 0));
			drv->draw3DLine(core::vector3df(), core::vector3df(0, 0, 1), video::SColor(255, 0, 0, 255));
			/*u16 indices[] = { 0,1,0,2,0,3 };
			drv->drawVertexPrimitiveList(&Vertices[0], 4, &indices[0], 3, EVT_STANDARD, EPT_LINES, EIT_16BIT);*/
		}

		const core::aabbox3d<f32> &CoordinateSystemCursor::getBoundingBox() const
		{
			return Box;
		}

		u32 CoordinateSystemCursor::getMaterialCount() const
		{
			return 1;
		}

		video::SMaterial &CoordinateSystemCursor::getMaterial(u32 i)
		{
			return Material;
		}
	}
}