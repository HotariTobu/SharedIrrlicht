#pragma once

#ifndef __I_SCENE_H_INCLUDED__
#define __I_SCENE_H_INCLUDED__

#include <IReferenceCounted.h>

namespace irr {
	class IScene : public IReferenceCounted {
	public:
		virtual void init() = 0;
		virtual void update(f32 delta) = 0;
		virtual void draw() = 0;
	};
}

#endif // !__I_SCENE_H_INCLUDED__
