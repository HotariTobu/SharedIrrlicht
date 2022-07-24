#ifndef NMAG_H
#define NMAG_H


#include <Irrlicht.h>
#include <enet/enet.h>
#include <stdio.h>
#include <wchar.h>
#include <iostream>
#include <string> 
#include <irrXML.h>
#include "Packets.h"


using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Defines
#define FILEPACKETSIZE 200

/** \mainpage

\section intro_sec Introduction

Welcome to irrNet! An easy to use Network Framework for Irrlicht utilizing enet! irrNet provides both high
and low level functionality and aims to fulfil the needs of all programmers using Irrlicht that seek a
powerful, robust and easy to use implementation of network in their application.

To use irrNet you must include enet into your project, then define WIN32 or UNIX corresponding to your operating
system in the preprocessor. You must also remember to link "ws2_32.lib" if you are using windows.

For a good place to get started reading this guide, check out the INetManager class, or the brandnew irr::net
namespace. :D

Here is a simple example of implementing irrNet:

\code

#include <irrlicht.h>
#include <irrNet.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#pragma comment(lib, "Irrlicht.lib")

// Need to include this lib in your project.
#pragma comment(lib, "ws2_32.lib")

IrrlichtDevice* device = 0;
scene::ISceneNode* n = 0;

#define CLIENT; // Define here whether we are the client. Else we are the server.

enum EPLAYER_ACTION
{
	EPA_MOVEUP,
	EPA_MOVEDOWN,
	EPA_MOVELEFT,
	EPA_MOVERIGHT,
	EPA_COUNT
};

irr::EKEY_CODE Keys[EPA_COUNT];
bool KeyStates[EPA_COUNT];

// Input reciever
class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(SEvent event)
	{
		if(event.EventType == irr::EET_KEY_INPUT_EVENT)
		{
			if(event.KeyInput.Key == KEY_ESCAPE)
			{
				device->closeDevice();
			}
			for(int i=0;i<EPA_COUNT;i++)
			{
				if(event.KeyInput.Key == Keys[i])
				{
					KeyStates[i] = event.KeyInput.PressedDown;
					return true;
				}
			}
		}


		return false;
	}
};

int main()
{

	MyEventReceiver receiver;

	Keys[EPA_MOVEUP] = irr::KEY_UP;
	Keys[EPA_MOVEDOWN] = irr::KEY_DOWN;
	Keys[EPA_MOVELEFT] = irr::KEY_LEFT;
	Keys[EPA_MOVERIGHT] = irr::KEY_RIGHT;

	// Create Irrlicht device.
	device = createDevice(video::EDT_OPENGL, core::dimension2d<s32>(800, 600),
		32, false, false, true, &receiver);

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	// Initialize INetManager
	irr::net::INetManager* netmgr = new irr::net::INetManager(device);

	// Set the NetManager to output info to console
	netmgr->setVerbose(1);

	// Set up client or server
	#ifdef CLIENT
	
		netmgr->setUpClient();
	
	#else
	
		netmgr->setUpServer();

	#endif

	// Create a NetCube.
	ISceneNode* cuby = netmgr->addNetCubeSceneNode(10,vector3df(-50,0,100));

	// sendVelocity may get a little out of sync after a while so we update the position every second
	// it wont use a great deal of bandwidth but keeps our nodes on track
	// Remember that sendPositionAuto only sends when the position changes, so a stationary node uses no bandwidth
	netmgr->sendPositionAuto(cuby,1000);

	vector3df currentvel;
	vector3df sentvel;

	// Create a simple camera
	smgr->addCameraSceneNode();

	while(device->run())
	{
		currentvel = vector3df(0,0,0);
		if(KeyStates[EPA_MOVEUP])
		{
			currentvel = vector3df(0,2,0);
		}
		if(KeyStates[EPA_MOVEDOWN])
		{
			currentvel = vector3df(0,-2,0);
		}
		if(KeyStates[EPA_MOVERIGHT])
		{
			currentvel = vector3df(2,0,0);
		}
		if(KeyStates[EPA_MOVELEFT])
		{
			currentvel = vector3df(-2,0,0);
		}

		driver->beginScene(true, true, video::SColor(255,113,113,133));

		smgr->drawAll(); // draw the 3d scene

		driver->endScene();
		
		// If our velocity changed send this to peer, and set it locally.
		if(currentvel != sentvel){netmgr->sendVelocity(cuby,currentvel);netmgr->setLocalVelocity(cuby,currentvel);sentvel = currentvel;}
			


		#ifdef CLIENT

			netmgr->updateClient();
		
		#else
		
			netmgr->updateServer();

		#endif

		int fps = driver->getFPS();

		device->setWindowCaption("Network Example (Use arrow keys to move cube)");
				
	}

	
	//In the end, delete the Irrlicht device.
	
	device->drop();
	

	return 0;
}

\endcode

Thats it!. As you can see irrNet is very easy to use.

See you and have fun!
*/

namespace irr
{
namespace net
{

/// This is the custom operations class, make a class that inherits this one for it to be compatible with the
/// custom operations setting. This is very similar to setting up a custom scene node animator in Irrlicht.
/// You will be passed the scene node and the Irrlicht device currently in use. You can use this then to perform
/// any initial operations on newly created scene nodes (Set custom settings, add more animators, physics, etc)

class CustomOperations
{
public:
	CustomOperations();
	virtual void Operations(IAnimatedMeshSceneNode* node, irr::IrrlichtDevice *device);
};

/**
This is the custom handler class, make a class that inherits this one for it to be compatible with the
custom handling setting. This is very similar to setting up a custom scene node animator in Irrlicht.
Your handler will be passed the InPacket and you can process this InPacket to your desire. You do not have
to destroy or do anything to the InPacket once you are done with it. To use InPackets, you must call
InPacket->getNextItem(var) with any commonly used variable as a parameter. All common Irrlicht typedefs are
supported and vector3dfs' are too. The Irrlicht typedefs supported are c8,s8,u16,s16,u32,s32,f32 and f64.

Here is an example on how to handle a packet using the custom handler:

\code

// Ok so the handler has passed you the InPacket:

c8 packetid;
InPacket->getNextItem(packetid);

// Lets say packet id 2 means a Y angle float followed by a position vector3df.
// You may just use 3 floats instead of a vector3df here if you wish, its custom handling!...
if(packetid == 2)
{

f32 yrotation;
InPacket->getNextItem(yrotation);

vector3df position;
InPacket->getNextItem(position);

// You may set the nodes position or rotation or whatever you want here...and thats it!

\endcode

*/


class CustomHandler
{
public:
	CustomHandler();
	virtual void Handle(InPacket * packet);
};


typedef struct
{
  s32 framestart[50];
  s32 frameend[50];
  s32 framespeed[50];
  bool frameloop[50];
  c8 currentframestate;
} SFrameRange;

typedef struct
{
  char fileData[FILEPACKETSIZE];
  c8 fileId;
  u32 readPos;
} SFilePacket;

typedef struct
{
  SMaterial material;
  IAnimatedMesh* mesh;
  ISceneNodeAnimator* animator;
  CustomOperations* customop;
  SFrameRange nodeframerange;
} NodeType;

static const SFrameRange NULLRANGE = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	0};

/**

This is the main bad-ass class of irrNet. It is similar to Irrlicht's SceneManager, except this one is a 
NetManager and handles everything to do with Networks. For now, to instantiate a new NetManager simply do

\code
irr::net::INetManager* NetManager = createNetManager(device);
// Where "device" is your Irrlicht device, this is IRRnet after all :P
\endcode

Before you begin using the NetManager, however, you will have to do a:

\code
NetManager->setUpServer();
\endcode

or

\code
NetManager->setUpClient();
\endcode

Depending on whether this is a server or client. After this you can use your NetManager to do a multitude of
network related operations, which will be explaned in detail in the specific functions. There are some exceptions
to this rule, however, some diagnostic and other options can be set before the client or server is set up, for
example:

\code
NetManager->setVerbose(1);
\endcode

Can be used to enable debug output.

And thats it for the main class! Please check out all its functions for an indepth look.
Please note that most of the node handling functions in this class do not work if setDefaultHandling
is set to 0.

*/
class INetManager
{
public:
	INetManager(irr::IrrlichtDevice* irrdev);
	virtual ~INetManager(void);

	/**
	 This sets up the client. If setCustomAddress is called before it, it will use the address and port provided,
	 else it will look for a "ip.xml" file in the working directory and get the ip and port information from that.
	*/
	void setUpClient();

	 /**
	 This sets up the server. If setCustomAddress is called before it, it will use the address and port provided,
	 else it will look for a "ip.xml" file in the working directory and get the port information from that.
	 Please note that when setting a custom address on a server to set it to "127.0.0.1" unless you know what you 
	 are doing because this automatically sets the server up on the computer running the program. If setCustomAddress
	 is not called this function will only use the port settings from the config file and automatically set the
	 address to "127.0.0.1". This is for ease of use so that the same "ip.xml" file can be placed in both the client
	 and the server without causing any trouble.
	 */
	void setUpServer();

	/// Make sure to stick this in your run loop, or at the very least update it very often. (Run loop recommended)
	void updateClient();

	/// Make sure to stick this in your run loop, or at the very least update it very often. (Run loop recommended)
	void updateServer();

	 /**
	 This sends the velocity to other computers. The first parameter is the scenenode, the second is a vector3df of
	 the velocity and the third sets wether the packet is reliable or not. Reliable is recommended for this operation.
	 Please note that this only sends the velocity to other computers, if you want to set the velocity on this computer
	 I have provided the function "setLocalVelocity". Here is an example:
	 \code
	 if(Keys[RIGHT]) // Just do your general event reciever stuff here
	 vector3df velocity = vector3df(2,0,0); // make a velocity vector.
	 NetManager->setLocalVelocity // Set the velocity on the local computer.
	 NetManager->sendVelocity(mynode, velocity) // 3rd option defaults to reliable so can be left blank.
	 \endcode

	 And thats it! The NetManager takes care of the velocity kind of like how a physics lib or a flystraight animator
	 would take care of it. The node will continue to travel at that speed until the velocity is changed. If you want
	 the node to stop moving set the velocity to (0,0,0).
	 */
	void sendVelocity(ISceneNode* node, vector3df velocity, bool reliable = 1);

	/**
	This sends a nodes' position to other computers. The first parameter is the scenenode and the second sets
	wether the packet is reliable or not. The position sent will be the current position of the scenenode.
	Reliable is recommended for this operation.
	*/
	void sendPosition(ISceneNode* node, bool reliable = 1);

	/**
	This sends a nodes' position to other computers. The first parameter is the scenenode and the second sets
	the interval in milliseconds between each auto send. The position sent will be the current position of the
	scenenode at every interval. The interval defaults to 0 (Every update).
	*/
	void sendPositionAuto(ISceneNode* node, u32 interval = 0);

	/// Use this to stop sending a nodes' position automatically, if sendPositionAuto was called.
	void sendPositionAutoStop(ISceneNode* node);

	/**
	This sends a nodes' rotation to other computers. The first parameter is the scenenode and the second sets
	wether the packet is reliable or not. The rotation sent will be the current rotation of the scenenode.
	Reliable is recommended for this operation.
	*/
	void sendRotation(ISceneNode* node, bool reliable = 1);

	/**
	This sends a nodes' rotation to other computers. The first parameter is the scenenode and the second sets
	the interval in milliseconds between each auto send. The rotation sent will be the current rotation of the
	scenenode at every interval. The interval defaults to 0 (Every update).
	*/
	void sendRotationAuto(ISceneNode* node, u32 interval = 0);

	/// Use this to stop sending a nodes' rotation automatically, if sendRotationAuto was called.
	void sendRotationAutoStop(ISceneNode* node);

	/**
	This sends a nodes' scale to other computers. The first parameter is the scenenode and the second sets
	wether the packet is reliable or not. The scale sent will be the current scale of the scenenode.
	Reliable is recommended for this operation.
	*/
	void sendScale(ISceneNode* node, bool reliable = 1);

	/**
	This sends a nodes' scale to other computers. The first parameter is the scenenode and the second sets
	the interval in milliseconds between each auto send. The scale sent will be the current scale of the
	scenenode at every interval. The interval defaults to 0 (Every update).
	*/
	void sendScaleAuto(ISceneNode* node, u32 interval = 0);

	/// Use this to stop sending a nodes' scale automatically, if sendScaleAuto was called.
	void sendScaleAutoStop(ISceneNode* node);

	/**
	This sends a nodes' current frame of animation to other computers. The first parameter is the scenenode
	and the second sets	wether the packet is reliable or not. The frame sent will be the current frame
	of the scenenode. Reliable is recommended for this operation.
	*/
	void sendFrame(IAnimatedMeshSceneNode* node, bool reliable = 1);

	/**
	This sends a nodes' current frame to other computers. The first parameter is the scenenode and the second sets
	the interval in milliseconds between each auto send. The frame sent will be the current frame of the
	scenenode at every interval. The interval defaults to 100 ms, I have found this to be the optimum balance
	between quality and bandwidth efficiency. It is recommended to merely have animation states using setFrameLoop
	and send them using custom handling methods to save a shitload more bandwidth. Or even better use one of the nodes'
	custom vars as an animation state value. But just using this is easier. :P
 	*/
	void sendFrameAuto(IAnimatedMeshSceneNode* node, u32 interval = 100);

	/// Use this to stop sending a nodes' current animation frame automatically, if sendFrameAuto was called.
	void sendFrameAutoStop(IAnimatedMeshSceneNode* node);

	/// Sends whether the node is visible or not. The first parameter is the node, the second is the visible value
	/// (true or false) and the last is whether it should be a reliable packet. Reliable is recommended for this
	/// operation.
	void sendVisible(ISceneNode* node, bool visible, bool reliable = 1);

	/// This removes animators from the node set in the first parameter from all connected computers. Does not remove
	/// them locally, use node->removeAnimators() for that.
	void removeAnimators(ISceneNode* node);

	/// If set true NetManager will output debug info to console. Stuff like
	///
	/// "Packet recieved from blah containing blah".
	///
	///	   and
	///
	/// "Packet conversion: 5 4 34.3454 x 23.2354 x 0.4357932" 
	///
	/// It gets pretty annoying so don't enable it unless you are having some weird network problems and know what your
	/// doing.
	void setVerbose(bool isverbose);

	/** 
	Sets the velocity of the node only on the local computer. Use this inconjunction with sendVelocity to set the
	nodes' velocity both locally and on all connected peers. Look at sendVelocity for more info. This takes the
	node as the first parameter and the velocity as the second.
	*/
	void setLocalVelocity(ISceneNode* node, vector3df velocity);

	/** 
	Sets a custom var of the var id of the node. Each Net node has its own private array of 100 ints. These can be used for
	various things like Health, Mana, states, etc. The first parameter is the node, the second is the ID of the var, 0 to 100,
	and the last is the value which is a standard u32. These can be used in conjunction with getCustomVar and may prove useful
	in changing pretty much any attribute of an object. For example you can set 4 textures to correspond with var id 5 and
	each value from 1 to 4 of varid 5 of that node represents a texture. Then you can change the textures remotely for
	whatever purpose. Or it can be used to set the health of the node. Or what state its in (Attacking, dead, etc). Please
	see getCustomVar for more info. Please look into ENUMS which can be used here for more comfortable usage.
	*/
	void sendCustomVar(ISceneNode* node, u16 varid, u32 varvalue);

	/// This sends the custom var to all peers. There is not parameter to change the value in this function, so use setCustomVarLocally instead of sendCustomVar to set the var itself because you don't need to send it when you are sending it automatically. The third parameter is the interval in milliseconds between the CustomVar being sent.
	void sendCustomVarAuto(ISceneNode* node, u16 varid, u16 interval);

	/// Stops sending the specified custom var.
	void sendCustomVarAutoStop(ISceneNode* node, u16 varid);

	/// This sends a change in a custom var, for example instead of setting it to 90 from 100 you would enter a value of
	/// -10 and this will send the change only. This is necessary if for example you want to decrease the health of a player,
	/// but you dont know their health at their given instance or there is someone also changing it at the same time. So you
	/// can just choose to decrease it by 10 and you're good. Please see sendCustomVar for more info.
	void sendChangeInCustomVar(ISceneNode* node, u16 varid, u32 varvalue);

	/// CustomVars owned by remote scene nodes are only updated automatically if explicitly sent by the remote peer. 
	/// To force a specific CustomVar to update, use this function. Please note the effect will take place after a
	/// frame or two, due to packets being sent back and forth to retrieve this information. (Should feel instaniace real-time, so this will not really have an effect unless you really need that variable the same frame you request it. Average time is around 500-600ms before getting updated in most situations)
	void updateCustomVar(ISceneNode* node, u16 varid);

	/// Asks the peer that owns the specified NetSceneNode to automatically send  the custom var. This is probably better than simply "updateCustomVar" in most cases, but can contribute to unnecessary lag if the variable does not need to be updated that often. You can however adjust the interval parameter (Third one) to compensate.
	void updateCustomVarAuto(ISceneNode* node, u16 varid, u16 interval);

	/** 
	This is rather obsolete, because sendCustomVar sets it locally also, but if for whatever reason you only want to
	change a var locally then you can use this. Please see sendCustomVar for more info.
	*/
	void setCustomVarLocally(ISceneNode* node, u32 varid, u32 varvalue);
	
	/**
	Sets the parameters of a node type. A node type is a struct private to the NetManager that contains an SMaterial,
	an IAnimatedMesh, an ISceneNodeAnimator, frame ranges and a CustomOperations instance. With setNodeType you can set all of
	these to a node type id (0 to 100) and then use that node type to remotely (and locally) create a new
	IAnimatedMesh scene node that will automatically be set up with the material, mesh, animator and CustomOperations
	instance specified in the setNodeType of the node type ID used. The animator, frame ranges and CustomOperations are optional,
	so as long as you set a Material and a Mesh you are ok (After all what else do you need to create a new IAnimatedMesh
	Scene Node?) Please look into addNetSceneNode for more info about utilizing the power of node types.
	*/
	void setNodeType(u16 type, SMaterial material, IAnimatedMesh* mesh, ISceneNodeAnimator* animator = 0, CustomOperations* customop = 0, SFrameRange framerange = NULLRANGE);
	
	/** 
	This sets a global custom operations instance. First make an appropriate class inhereting CustomOperations,
	instantiate it, and set it here to perform the operations on ALL newly created Net scene nodes. Please look into the
	CustomOperations class for more info. The first parameter enables the use of a global operations instance, and the
	second sets the instance to be used. To disable the use of GlobalCustomOperations later on, you may invoke this
	function with the first parameter false and the second parameter blank, eg.:

	\code

	NetManager->setGlobalCustomOperations(true,MyOperations);

	Do some stuff here...

	Now you want to disable it for whatever reason so you do:

	NetManager->setGlobalCustomOperations(false);

	\endcode

	*/
	void setGlobalCustomOperations(bool enable, CustomOperations* operations = 0);
	
	/**
	This enables custom handling and sets a CustomHandler. You must first make an appropriate class inhereting CustomHandler,
	instantiate it, and then set it here to handle all the packets youself. The first parameter enables the use of a custom 
	handler instance, and the second sets the instance to be used. You may disable custom handling later on if you wish
	by invoking this function with the first parameter false and the second parameter left out. Please note when using 
	custom handling that the NetManagers default handling uses packets that start with a c8 of a value between 0 and 20.
	So when using custom handling either use a c8 at the start and stay out of that range, or just disable default handling.
	(Recommended if you plan to only use custom handling.)
	*/
	void setCustomHandling(bool enable, CustomHandler* handler = 0);

	/// This disables/enables the use of the NetManagers' default handling. On by default. If this is disabled all high level
	/// operations involving Net scene nodes are disabled. Disabling default handling is only recommended when you wish to
	/// only use custom handling.
	void setDefaultHandling(bool enable);

	/// This sends the OutPacket specified. Please look at createOutPacket for more info.
	void sendOutPacket(OutPacket* outpacket);

	/// This sends the OutPacket specified to a specific player denoted by the player number. Players are numbered
	/// automatically as they connect to the server. It is possible to get the player number from an InPacket in a custom
	/// handler using getPlayerId() which makes this function a little more useful. IMPORTANT: This feature is only valid
	/// if you are the server. This is because clients can only send packets to the server, making this function unusable.
	/// However, using your own handling and custom packet identification it is not hard to set up a system where a packet
	/// is forwarded to the desired player. Please look at createOutPacket for more info.
	void sendOutPacket(OutPacket* outpacket, u16 playernumber);

	/// This sends the OutPacket specified unreliably. This is not recommended unless you really know what you are doing.
	/// Please look at createOutPacket for more info.
	void sendOutPacketUnreliable(OutPacket* outpacket);

	/// This sends the OutPacket specified unreliably to the specified player. This is not recommended unless you really
	/// know what you are doing. Please look at sendOutPacket(OutPacket* outpacket, u16 playernumber) and createOutPacket
	/// for more info. IMPORTANT: This feature is only valid if you are the server. This is because clients can only send 
	/// packets to the server, making this function unusable. However, using your own handling and custom packet identification
	/// it is not hard to set up a system where a packet is forwarded to the desired player. 
	void sendOutPacketUnreliable(OutPacket* outpacket, u16 playernumber);

	/// This sets a custom address and port for the client to connect to or for the server to set up on. Please note that
	/// servers should only use the address of "127.0.0.1" unless you really know what you are doing. If this option is not
	/// called then the standard method of reading the "ip.xml" file in the working directory will be used. Please see
	/// setUpClient and setUpServer for more info.
	void setCustomAddress(stringc address, u32 port);

	/// Gets the material of the specified node type ID denoted by the first parameter. Please see setNodeType for more info.
	SMaterial getNodeTypeMaterial(u16 type);

	/// Gets the Animator of the specified node type ID denoted by the first parameter. Please see setNodeType for more info.
	ISceneNodeAnimator* getNodeTypeAnimator(u16 type);

	/// Gets the Mesh of the specified node type ID denoted by the first parameter. Please see setNodeType for more info.
	IAnimatedMesh* getNodeTypeMesh(u16 type);

	/// Creates a new Net cube both locally and remotely on all connected peers. NetCubes are not subject to a node type,
	/// they are simply cubes, but you may specify the size, position, rotation, and scale on creation. Net Cubes are not
	/// subject to CustomOperators. Returns a pointer to the node.
	ISceneNode* addNetCubeSceneNode
		(
			f32 size=10.0f,
			core::vector3df position = core::vector3df(0,0,0),
			core::vector3df rotation = core::vector3df(0,0,0),
			core::vector3df scale = core::vector3df(1.0f, 1.0f, 1.0f)
		);

	/// Creates a new Net sphere both locally and remotely on all connected peers. Net spheres are not subject to a node type,
	/// they are simply cubes, but you may specify the radius, polycount, position, rotation, and scale on creation. Net
	/// spheres are not subject to CustomOperators. Returns a pointer to the node.
	ISceneNode* addNetSphereSceneNode
		(
			f32 radius=5.0f,
			s32 polycount = 16,
			core::vector3df position = core::vector3df(0,0,0),
			core::vector3df rotation = core::vector3df(0,0,0),
			core::vector3df scale = core::vector3df(1.0f, 1.0f, 1.0f)
		);
	
	/** 
	This is a very exciting function, it creates a new IAnimatedMeshSceneNode both locally and on all connected players.
	The scene node will have all the attributes set in the node type ID and the position, rotation and scale specified here.
	When a client disconnects, all the net nodes created by them are removed. The pointer returned is to a (seemingly) normal
	Irrlicht IAnimatedMeshSceneNode, but because it has been set up with a net id already, it can be inputted into many
	NetManager functions to perform a variety of operations. Please note all NetManager functions like setPosition, etc. Must
	be used on a Net node (or cube or sphere). Sending stuff like positions and rotations etc on normal nodes will have no
	effect because they do not exist on the other server! Please look at all the functions in NetManager, and specifically
	setNodeType for more info.
	*/
	IAnimatedMeshSceneNode* addNetSceneNode(u16 node_type , core::vector3df position, core::vector3df rotation, core::vector3df scale);
	
	/** 
	This retrieves the value of the custom var specified by the scene node and var id first set by sendCustomVar or
	setCustomVarLocally. Please see those two functions for more info. Please note this only retrieves the value as it
	is locally, so if the client that created the node does not update the value regularly to other peers, or changes it
	using setCustomVarLocally, the number obtained using this method will not actually yeild the same value as the Net node
	on that clients' computer.
	*/
	u32 getCustomVar(ISceneNode* node, u16 varid);

	/// This is a rather advanced function, and setNetId is already taken care of in addNetSceneNode, but advanced users may
	/// wish to create special net nodes. Please note that using this method is not recommended as it is troublesome, but if
	/// you really want to use it all you have to do is create an instance of the node on each player locally, then set the
	/// same netid to it (Please try to keep that under 1000 as to not interfere with NetManager's dynamically created nodes)
	/// Once again, you shouldn't have to use this function, it is mostly for internal use. But if you have it its there.
	/// (Please only use it after looking thoroughly through the
	/// source code of irrNet and understanding its internal workings.)
	/// (Owner will not be held responsible for inappropriate use of this function otherwise. :)
	u16 setNetId(ISceneNode* node, u16 netid, bool origin = 0);
	
	/// For Advanced use only. This returns the scene node that corresponds to the specified Net Id.
	ISceneNode* getSceneNodeFromNetId(u16 netid);

	/// This returns the ENet Host used by NetManager. Only for very advanced use.
	ENetHost* getHost();
	u16 requestNetId();
	u16 getNetId(ISceneNode* node);

	/// This gets the number of players connected. This is only valid for servers.
	u32 getPeerCount();

	/// This returns the playerID. Only valid for Clients.
	u16 getPlayerNumber();

	/// This returns a bool that is set to true if the connection is established and false if it is still pending or
	/// failed to connect.
	bool getEstablished();
	
	/// This returns the index number of the node type of the specified scene node.
	u16 getNetSceneNodeType(ISceneNode* node);

		
	/**
	This is an important function for CustomHandling. It creates a new OutPacket to be used in sending your own data.
	Conversions etc are all taken care of by irrNet. All you have to do is create some kind of variable then add it,
	then read the data from the InPacket on the recieving end. Here is an example of adding data to an OutPacket:

	\code

	OutPacket* MyOutPacket = NetManager->createOutPacket();
	float myvalue = 0.238723;
	MyOutPacket->addData(myvalue);
	vector3df myvector = vector3df(23,354,35);
	MyOutPacket->addData(myvector);
	int myint = 43988434;
	MyOutPacket->addData(myint);

	// And now to send.
	
	NetManager->sendOutPacket(MyOutPacket);

	\endcode

	Tada! Thats it, as you can see you dont have to do much but call addData from the packet. Unless you reuse the OutPacket
	using OutPacket->clearData, it is recommended to delete it to conserve memory usage. Please refer to CustomHandler for
	more information on what types of variables an OutPacket can handle, as they can handle the same types as an InPacket.
	*/
	OutPacket* createOutPacket();

	/// This is for very advanced custom handling, when you already have all your data in a buffer and know the buffer size
	/// you can use this overloader to directly input them into an OutPacket. For the size you can just do "sizeof(buffer)".
	OutPacket* createOutPacket(enet_uint8 *buff,const int buff_size);

	/// This returns the ENet peer of the connected player with the specified player ID. This is only valid for servers.
	ENetPeer* getPlayerById(u16 id);

	/// This returns the first connected ENet peer. This returns the server if you are a client.
	/// Returns 0 if not established.
	ENetPeer* getPeer(); 

	/// This returns the ping. Useful for clients as it returns the ping to the server. For servers it returns
	/// the ping to the first connected player.
	u32 getPing();

	/**
	Do not use this function unless you want to directly modify a specific nodes frame ranges locally.
	(Which will not effect remote instances of the node.)
	Instead, the proper way is to create an SFrameRange struct and pass it to a nodetype.

	eg.
	\code

	enum FRAMESTATE {WALK = 0, RUN, STAND};
	
	SFrameRange myframerange;

	myframerange.framestart[WALK] = 100;
	myframerange.frameend[WALK] = 1000;
	myframerange.framespeed[WALK] = 100;
	myframerange.frameloop[WALK] = true;

	myframerange.framestart[RUN] = 2000;
	myframerange.frameend[RUN] = 2600;
	myframerange.framespeed[RUN] = 100;
	myframerange.frameloop[RUN] = true;

	myframerange.framestart[STAND] = 3000;
	myframerange.frameend[STAND] = 3800;
	myframerange.framespeed[STAND] = 100;
	myframerange.frameloop[STAND] = false;

	NetManager->->setNodeType(1,Material,Mesh,Animator,CustomOps,myframerange); // The last parameter here.

	\endcode
	*/
	SFrameRange& getFrameRange(IAnimatedMeshSceneNode* node);

	/**
	Do not use this function, instead create an SFrameRange struct and pass it to a nodetype.

	eg.
	\code

	enum FRAMESTATE {WALK = 0, RUN, STAND};
	
	SFrameRange myframerange;

	myframerange.framestart[WALK] = 100;
	myframerange.frameend[WALK] = 1000;
	myframerange.framespeed[WALK] = 100;
	myframerange.frameloop[WALK] = true;

	myframerange.framestart[RUN] = 2000;
	myframerange.frameend[RUN] = 2600;
	myframerange.framespeed[RUN] = 100;
	myframerange.frameloop[RUN] = true;

	myframerange.framestart[STAND] = 3000;
	myframerange.frameend[STAND] = 3800;
	myframerange.framespeed[STAND] = 100;
	myframerange.frameloop[STAND] = false;

	NetManager->->setNodeType(1,Material,Mesh,Animator,CustomOps,myframerange); // The last parameter here.

	\endcode
	*/
	void setFrameRange(IAnimatedMeshSceneNode* node, SFrameRange rangestruct);

	/// Sends the frame state numerical value as assigned using getFrameStruct()]
	/// For more info see getFrameStruct() and setFrameStruct()
	void sendFrameState(IAnimatedMeshSceneNode* node, c8 framestate);

	/// Removes the Net node from the world.
	void removeNetNode(ISceneNode* node);

	/// Enables file reception.
	void setRecieveFiles(bool recieve);

	/** Sends a file,
	The first parameter is the path of the file you wish to send.
	The second is the desired name of the file on the recieving end. 
	(Note that this name will be checked for availability on the peer,
	if the name is taken the peer will deny the request and the file will
	not be sent, so make sure that the filename is not already taken on the
	other computer.)
	The third parameter is strictly for servers and allows you to choose
	which player to send the file to. (Experimental, but should work.)
	*/
	void sendFile(c8* filename, c8* destfilename = 0, u16 playerid = 0);
	
	/// Returns the file size in bytes of a file thats being sent. The parameter is a number indicating the index of the file.
	/// For example if this is the first file to be sent out of all the files that are being sent right now then its
	/// index is 0. They are currently tracked using an array sized 100, so please do not exceed this number.
	int getSendFileSize(int whichfile){return readfile[whichfile]->getSize();}

	/// Returns the file size in bytes of a file thats being recieved. The parameter is a number indicating the index of the file.
	/// For example if this is the first file to be sent out of all the files that are being sent right now then its
	/// index is 0. They are currently tracked using an array sized 100, so please do not exceed this number.
	int getRecieveFileSize(int whichfile){return writefilesize[whichfile];}

	/// Returns the progress in bytes of a file thats being sent. The parameter is a number indicating the index of the file.
	/// For example if this is the first file to be sent out of all the files that are being sent right now then its
	/// index is 0. They are currently tracked using an array sized 100, so please do not exceed this number.
	int getSendFileProgress(int whichfile){return readfile[whichfile]->getPos();}

	/// Returns the progress in bytes of a file thats being recieved. The parameter is a number indicating the index of the file.
	/// For example if this is the first file to be sent out of all the files that are being sent right now then its
	/// index is 0. They are currently tracked using an array sized 100, so please do not exceed this number.
	int getRecieveFileProgress(int whichfile){return writefilepos[whichfile];}

	/// Sets the priority of file packet sending. (Iterations per frame)
	void setFilePriority(int priority){filePriority = priority;}

	/// Sets the number of packet processing iterations per frame
	void setNetIterations(u16 iterations){netIterations = iterations;}

	/// Returns an array populated with all current net nodes. (Everything in the current world) (This list is not updated automatically, you need to call this function again when a new node is created or something.) (Or just call it alot...)
	core::array<ISceneNode*> getArrayOfAllNetNodes();

	/// Returns an array populated with all current net nodes that belong to this peer. (Peer means connected computer.)
	core::array<ISceneNode*> getArrayOfAllMyNetNodes();
	
	/// Returns an array populated with all current net nodes that belong to the peer with this Played Id. (Will return an empty array if the peer specified does not own any nodes or if there is no peer with the specified Player Id.) (Peer means connected computer.)
	core::array<ISceneNode*> getArrayOfAllNetNodesForPlayedId(u16 playerno);


private:
	bool defaulthandling;
	bool customhandling;
	bool customconnect;
	stringc customaddress;
	u32 customport;
	ENetHost* host;
	irr::IrrlichtDevice* device;
	enet_uint8 *buff;
	ENetPeer *peer;
	ENetPeer * currentPeer;
	u16 nodetype[10000];
	f32 tmpfloat;
	ENetAddress address;
	InPacket* inpacket;
	vector3df tmpvector;
	IAnimatedMeshSceneNode* tmpnode;
	ISceneNode* tmpstatnode;
	s32 nodearray[10000];
	f32 cubesize[10000];
	s32 polyarray[10000];
	SFrameRange NetFrameRanges[10000];
	u8 currentpackiden;
	u16 playernumber;
	bool originarray[10000];
	bool players[90];
	CustomOperations* customoperations;
	CustomHandler* customhandler;
	bool verbose;
	u16 servernetidcount;
	bool established;
	bool firstupdate;
	bool isserver;
	bool recievefiles;
	u16 ownedby[10000];
	bool customops;
	io::IReadFile* readfile[100];
	io::IWriteFile* writefile[100];
	c8 readfileuse[100];
	c8 writefileuse[100];
	u32 writefilesize[100];
	c8 readfileid[100];
	char filecontents[FILEPACKETSIZE];
	u16 readfilepid[100];
	c8 writefileid[100];
	s32 writefilepos[100];
	u16 filePriority;
	u16 netIterations;
	u16 PacketIterator;
	u8 packiden;
	core::array<SFilePacket> packetbuffer; // PS: Thx MidNight for helping me fix file transfer. :D
};




INetManager* createNetManager(irr::IrrlichtDevice* irrdev);

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