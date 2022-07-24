#include "NetAnim.h"

namespace irr
{
namespace net
{

// This one here is an internal function, dont attempt to call it (You probably cant anyway :P)
AutoAnim::AutoAnim(ISceneManager* manager,INetManager* netmgr)
: netmanager(netmgr), smgr(manager)
{
	currentcountpos = 0;
	currentcountrot = 0;
	currentcountsca = 0;
	currentcountframe = 0;
	currentcountvel = 0;
	memset(varcurrent,0,100);
	myvel = vector3df(0,0,0);
	host = netmanager->getHost();
}

AutoAnim::~AutoAnim(void)
{
}

//! animates the scene node
void AutoAnim::animateNode(irr::scene::ISceneNode *node, irr::u32 timeMs)
{

if(ifvel)
{
//if(timeMs >= (currentcountvel + 10))
//{
if(currentcountvel == 0){currentcountvel = timeMs;}
vector3df myvelocity = vector3df(myvel *((f32)(timeMs - currentcountvel) / 10.0f));
node->setPosition(node->getPosition() + myvelocity);
currentcountvel = timeMs;
//printf("%f %d\n", myvelocity.X, timeMs - currentcountvel);
//}
}


if(ifpos)
{
	if(timeMs >= (currentcountpos + intertimepos))
	{
	u8 packid = 5;
	//if(device->getTimer()->getTime() > (timeset + interval))
	//{
	tmpvector = node->getPosition();
	if(tmpvector != lastpos)
	{
	lastpos = tmpvector;
	memcpy(buff,(void*)&packid,1);
	u16 netid = netmanager->getNetId(node);
	memcpy(buff+1,(void*)&netid,2);
	tmpfloat = tmpvector.X;
	memcpy(buff+3,(void*)&tmpfloat,sizeof(tmpfloat));
	tmpfloat = tmpvector.Y;
	memcpy(buff+7,(void*)&tmpfloat,sizeof(tmpfloat));
	tmpfloat = tmpvector.Z;
	memcpy(buff+11,(void*)&tmpfloat,sizeof(tmpfloat));
	ENetPacket * packet = enet_packet_create (buff,15,ENET_PACKET_FLAG_RELIABLE);
	
	/* Send the packet to the peer over channel id 0. */
	/* One could also broadcast the packet by         */
	/* enet_host_broadcast (host, 0, packet);         */
	enet_host_broadcast (host, 0, packet);
	currentcountpos = timeMs;
	//int timeset = device->getTimer()->getTime();
	//}
	}
	}
}

if(ifrot)
{
	if(timeMs >= (currentcountrot + intertimerot))
	{
	u8 packid = 6;
	//if(device->getTimer()->getTime() > (timeset + interval))
	//{
	tmpvector = node->getRotation();
	if(tmpvector != lastrot)
	{
	lastrot = tmpvector;
	memcpy(buff,(void*)&packid,1);
	u16 netid = netmanager->getNetId(node);
	memcpy(buff+1,(void*)&netid,2);
	tmpfloat = tmpvector.X;
	memcpy(buff+3,(void*)&tmpfloat,sizeof(tmpfloat));
	tmpfloat = tmpvector.Y;
	memcpy(buff+7,(void*)&tmpfloat,sizeof(tmpfloat));
	tmpfloat = tmpvector.Z;
	memcpy(buff+11,(void*)&tmpfloat,sizeof(tmpfloat));

	ENetPacket * packet = enet_packet_create (buff,15,ENET_PACKET_FLAG_RELIABLE);
	
	/* Send the packet to the peer over channel id 0. */
	/* One could also broadcast the packet by         */
	/* enet_host_broadcast (host, 0, packet);         */
	enet_host_broadcast (host, 0, packet);
	currentcountrot = timeMs;
	//int timeset = device->getTimer()->getTime();
	//}
	}
	}
}

if(ifsca)
{
	if(timeMs >= (currentcountsca + intertimesca))
	{
	u8 packid = 7;
	//if(device->getTimer()->getTime() > (timeset + interval))
	//{
	tmpvector = node->getScale();
	if(tmpvector != lastsca)
	{
	lastsca = tmpvector;
	memcpy(buff,(void*)&packid,1);
	u16 netid = netmanager->getNetId(node);
	memcpy(buff+1,(void*)&netid,2);
	tmpfloat = tmpvector.X;
	memcpy(buff+3,(void*)&tmpfloat,sizeof(tmpfloat));
	tmpfloat = tmpvector.Y;
	memcpy(buff+7,(void*)&tmpfloat,sizeof(tmpfloat));
	tmpfloat = tmpvector.Z;
	memcpy(buff+11,(void*)&tmpfloat,sizeof(tmpfloat));
	ENetPacket * packet = enet_packet_create (buff,15,0);
	
	/* Send the packet to the peer over channel id 0. */
	/* One could also broadcast the packet by         */
	/* enet_host_broadcast (host, 0, packet);         */
	enet_host_broadcast (host, 0, packet);
	currentcountsca = timeMs;
	//int timeset = device->getTimer()->getTime();
	//}
	}
	}
}

if(ifframe)
{
	if(timeMs >= (currentcountframe + intertimeframe))
	{
	u8 packid = 8;
	u32 frameno = 0;
	//if(device->getTimer()->getTime() > (timeset + interval))
	//{
	tmpnode = dynamic_cast<IAnimatedMeshSceneNode*>(node);
	frameno = tmpnode->getFrameNr();
	if(frameno != lastframe)
	{
	lastframe = frameno;
	memcpy(buff,(char*)&packid,1);
	u16 netid = netmanager->getNetId(node);
	memcpy(buff+1,(char*)&netid,2);
	memcpy(buff+3,(char*)&frameno,4);

	ENetPacket * packet = enet_packet_create (buff,7,ENET_PACKET_FLAG_RELIABLE);
	
	/* Send the packet to the peer over channel id 0. */
	/* One could also broadcast the packet by         */
	/* enet_host_broadcast (host, 0, packet);         */
	enet_host_broadcast (host, 0, packet);
	currentcountframe = timeMs;
	//int timeset = device->getTimer()->getTime();
	//}
	}
	}
}

for(int i = 0;i <= 100; i++)
{
	if(sendVar[i] == 1)
	{
	if(timeMs >= (varcurrent[i] + vartime[i]))
	{
		netmanager->sendCustomVar(node,i,customvar[i]);
		varcurrent[i] = timeMs;
	}
	}
}



}

void AutoAnim::sendPos(bool ifp, u32 interval)
{
	ifpos = ifp;
	intertimepos = interval;
}

void AutoAnim::sendRot(bool ifr, u32 interval)
{
	ifrot = ifr;
	intertimerot = interval;
}

void AutoAnim::sendSca(bool ifs, u32 interval)
{
	ifsca = ifs;
	intertimesca = interval;
}

void AutoAnim::sendFrame(bool iff, u32 interval)
{
	ifframe = iff;
	intertimeframe = interval;
}

void AutoAnim::sendCustomVar(bool iff,u16 varid, u32 interval)
{
	sendVar[varid] = iff;
	vartime[varid] = interval;
}

void AutoAnim::updateVel(bool ifv, vector3df velocity)
{
	ifvel = ifv;
	myvel= velocity;
}

void AutoAnim::setVar(u32 varid, u32 varvalue)
{
	customvar[varid] = varvalue;
}

u32 AutoAnim::getVar(u32 varid)
{
	return customvar[varid];
}

} // Close Net Namespace
} // Close Irr namespace


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