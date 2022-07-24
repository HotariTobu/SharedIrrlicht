#ifndef NANIM_H
#define NANIM_H

#include "NetManager.h"

namespace irr
{
namespace net
{
class AutoAnim :
public irr::scene::ISceneNodeAnimator
{
public:
	AutoAnim(ISceneManager* manager,INetManager* netmgr);
	void updateVel(bool ifv, vector3df velocity);
	void sendPos(bool ifp,u32 interval = 0);
	void sendRot(bool ifr,u32 interval = 0);
	void sendSca(bool ifs,u32 interval = 0);
	void sendFrame(bool iff,u32 interval = 0);
	void setVar(u32 varid, u32 varvalue);
	void sendCustomVar(bool iff,u16 varid = 0, u32 interval = 0);
	u32 getVar(u32 varid);
	virtual ~AutoAnim(void);
	virtual void animateNode(irr::scene::ISceneNode *node, irr::u32 timeMs);
private:
	vector3df myvel;
	ISceneNodeAnimator* tmpanim;
	ISceneManager* smgr;
	ENetHost* host;
	INetManager* netmanager;
	vector3df lastvel;
	vector3df lastpos;
	vector3df lastrot;
	vector3df lastsca;
	vector3df tmpvector;
	f32 tmpfloat;
	u32 intertimepos;
	u32 currentcountpos;
	u32 intertimerot;
	u32 currentcountrot;
	u32 intertimesca;
	u32 currentcountsca;
	u32 intertimeframe;
	u32 currentcountframe;
	u32 currentcountvel;
	u32 customvar[100];
	bool stopsend;
	bool ifvel;
	bool ifpos;
	bool ifrot;
	bool ifsca;
	bool ifframe;
	bool firstrun;
	f32 timedelta;
	u32 lastframe;
	IAnimatedMeshSceneNode* tmpnode;
	c8 buff[18];
	SMaterial* materials[100];
	u16 varIntervals[100];
	bool sendVar[100];
	u32 varcurrent[100];
	u32 vartime[100];
	ISceneNodeAnimator* animators[100];
	IAnimatedMesh* meshes[100];
};

} // Close Net Namespace
} // Close Irr namespace

#endif

// Copyright(C) Ahmed Hilali 2007

/* License:

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgement in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be clearly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  */