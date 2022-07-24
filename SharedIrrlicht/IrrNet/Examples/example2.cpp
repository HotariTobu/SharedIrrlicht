
// Remember to include both irrlicht and irrNet, and include all the irrNet source files in your project.
#include <irrlicht.h>
#include <irrNet.h>

// For third person camera and player control.
// Player control has been modified slightly for network use, it uses "setVelocity" so I needed to add
// an extra parameter to the class which takes a pointer to NetManager. Thats pretty much it.

// Please note I used setLocalVelocity and sendVelocity in the character movement code to set the velocity
// locally then send it to other peers.
#include "Animators.h"

// We're using some namespaces
using namespace irr;
using namespace scene;
using namespace video;
using namespace core;
using namespace gui;
using namespace io;
using namespace net;

// Irrlicht lib
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(lib, "ws2_32.lib")

// Comment/Uncomment this to define if this is a client or server.
#define CLIENT

// Objects used in the Event Receiver, for camera and player animation.
CSceneNodeAnimator3PCamera *camAnim;
CSceneNodeAnimator3PPlayer *ninjaAnim;

// Irrlicht input reciever class..
class MyEventReceiver : public irr::IEventReceiver
{
public:
	virtual bool OnEvent(irr::SEvent event)
	{
		if(event.EventType == irr::EET_LOG_TEXT_EVENT)
		{
			OutputDebugString(event.LogEvent.Text);
			OutputDebugString("\n");

			return true;
		}
		if(camAnim->OnEvent(event))
			return true;
		if(ninjaAnim->OnEvent(event))
			return true;

		return false;
	}
};


// I declared the selector here so the handler can access it.
irr::scene::ITriangleSelector *selector = 0;


// Here I make a Custom Operations class. This is passed the node and irrlicht device and allows you to perform operations
// on newly created Net nodes. This counts both for nodes created locally and nodes created by request from other peers.
// Here I am using it to set a collision response animator to every new Net node created, you can do pretty much anything
// you want though. The reason to have a Custom Operations class is because you don't know how many or what nodes are going
// to be created dynamically by the server/client, so here you can make sure you get what you want done to them. In essence,
// this is very similar to a scene node animator, except it applys to all net nodes of the node type its set to, and only 
// occurs one time after they are created. If you want to do these things for all new Net nodes you can set this in 
// setGlobalCustomOperations. (Look into Node Types in API document if you are interested)

class CollisionOperator :
	public irr::net::CustomOperations
{
public:
	CollisionOperator();
	virtual void Operations(IAnimatedMeshSceneNode* node, irr::IrrlichtDevice *device);
};

CollisionOperator::CollisionOperator()
{
}

void CollisionOperator::Operations(irr::scene::IAnimatedMeshSceneNode *node, irr::IrrlichtDevice *device)
{
	scene::ISceneNodeAnimator* anim = device->getSceneManager()->createCollisionResponseAnimator(
	selector, node, core::vector3df(10,50,10),
	core::vector3df(0,-3,0), 
	core::vector3df(0,-50,0));
	node->addAnimator(anim);
	anim->drop();
}

// This is an example of how to make a Custom Handler, this handles packets in any way you want. This must be enabled using
// setCustomHandling, look in main function and in API documentation for more info.

/*

class myHandler :
	public irr::net::CustomHandler
{
public:
	myHandler();
	virtual void Handle(InPacket * packet);
};

myHandler::myHandler()
{
}

void myHandler::Handle(InPacket * packet)
{
	c8 display1;
	u16 display2;
	packet->GetNextItem(display1);
	packet->GetNextItem(display2);
	printf("%d %d", display1, display2);
}

*/


// Make some enumerations for our frame states.
enum FRAMESTATES {RUN, ATTACK, STAND};

//! Main game loop

int main()
{

	// Set up event reciever.
	MyEventReceiver receiver;

	// Create Device
	irr::IrrlichtDevice *device = irr::createDevice(irr::video::EDT_DIRECT3D9,irr::core::dimension2d<irr::s32>(800,600),32,0,0,1,&receiver);
		
	// Make pointers to irrlicht stuff
	irr::video::IVideoDriver*driver = device->getVideoDriver();
	irr::scene::ISceneManager*smgr = device->getSceneManager();
	irr::gui::IGUIEnvironment*guienv = device->getGUIEnvironment();
	irr::gui::ICursorControl*cursor = device->getCursorControl();
	irr::scene::ICameraSceneNode*camera = smgr->addCameraSceneNode();

	// Make a new INetManager
	irr::net::INetManager* netmgr = new irr::net::INetManager(device);

	
	// Here you can set the NetManager to output info to console...
	// netmgr->setVerbose(1);
	


	// If we're a client set up a client, if we're a server set up a server.
#ifdef CLIENT
	netmgr->setUpClient();
#else
	netmgr->setUpServer();
#endif

	

	// Load file system
	device->getFileSystem()->addZipFileArchive("./media/demo.dta");

	// Load a map, and set up the selector. Dont worry about all this it has nothing to do with irrNet...
	smgr->loadScene("temple.irr");

	scene::IAnimatedMesh* q3levelmesh = smgr->getMesh("temple.dae");
	scene::ISceneNode* q3node = 0;

	if (q3levelmesh)
		q3node = smgr->getSceneNodeFromName("templemap");

	if (q3node)
	{		
		selector = smgr->createOctTreeTriangleSelector(q3levelmesh->getMesh(0), q3node, 128);
		q3node->setTriangleSelector(selector);
		selector->drop();
	}

	q3node->setMaterialFlag(irr::video::EMF_LIGHTING,false);
	q3node->setMaterialTexture(0, driver->getTexture("wood.png"));

	// Here is some fun stuff, creating and modifying the material for our first node type.
	SMaterial nmaterial;
	nmaterial.Texture1 = driver->getTexture("nskinrd.jpg");
	nmaterial.Lighting = false;
	nmaterial.NormalizeNormals = true;

	// Here we load the mesh.
	irr::scene::IAnimatedMesh *ninjaMesh = smgr->getMesh("ninja.ms3d");

	// Here we make a new instance of our CustomOperations class.
	CollisionOperator* coop = new CollisionOperator();
	
	// Here we set the frame ranges for our ninja node.
   SFrameRange ninjaframe;	

   ninjaframe.framestart[RUN] = 40;
   ninjaframe.frameend[RUN] = 560;
   ninjaframe.framespeed[RUN] = 750;
   ninjaframe.frameloop[RUN] = true;

   ninjaframe.framestart[ATTACK] = 1800;
   ninjaframe.frameend[ATTACK] = 2360;
   ninjaframe.framespeed[ATTACK] = 1500;
   ninjaframe.frameloop[ATTACK] = false;

   ninjaframe.framestart[STAND] = 7360;
   ninjaframe.frameend[STAND] = 8180;
   ninjaframe.framespeed[STAND] = 150;
   ninjaframe.frameloop[STAND] = true;

   // Here we set the Node Type 1, with the material, mesh, (no animator), our CustomOperations class, and frame ranges.
   // This will mean that every new node of type 1 created will be made with that material, mesh, animator,
   // and will have our collision operations performed on them. You can use enums and label NINJA as 1 or
   // just remember the numbers like I do, it is not very hard, but you can use enums for big projects to
   // make it easier. So anyway, now you know all nodes of Type 1, are Ninjas! :D
   netmgr->setNodeType(1,nmaterial,ninjaMesh,0,coop,ninjaframe);

	// Here we make our ninja net node, this will create the node locally and also send a request to other connected peers
	// to create it. It will also be kept on the list on the server so it will be created when new clients join on, unless
	// it is destroyed or the client who created it disconnects. Please note the first value, it is very important. It sets
	// the node type for the node. To create a net node you MUST create a node type first like I did and set the mesh,
	// material, etc to it, so that it is able to be created on connected computers. Here I am setting node type 1, so we
	// are creating a ninja. :D Please note that connected computers will need the same node type to have the same mesh,
	// material, etc. So that they are created the same. (You were not actually thinking of sending the mesh were you :P)
	irr::scene::IAnimatedMeshSceneNode *ninja =
	netmgr->addNetSceneNode(1,core::vector3df(800,10,0),core::vector3df(0, -90, 0),core::vector3df(5.0f, 5.0f, 5.0f));

	// Here i am using the auto senders, each node has its personal auto send animator, allowing you to send data at specified
	// intervals. Here I am sending the position every second (1000 ms), the rotation every 50 ms, and the frame every 100 ms.
	// (Frame defaults to 100 ms if no second value is entered.) Please note all others default to 0, or as much as possible.
	netmgr->sendPositionAuto(ninja, 1000);
	netmgr->sendRotationAuto(ninja, 50);
	netmgr->sendFrameAuto(ninja);

	// Create the camera animator (Only attaching locally because we only want camera on our own ninja.)
	camAnim = new CSceneNodeAnimator3PCamera(smgr,cursor,ninja,50,180,10,irr::core::vector3df(0,20,0));
	camera->addAnimator(camAnim);
	camAnim->drop();
	camera->setNearValue(3.5f);
	camera->setFOV(PI / 3.0f);

	// Create the player animator (Same here we only want to control our own ninja.)
	ninjaAnim = new CSceneNodeAnimator3PPlayer(device,camera,camAnim,1.5f,selector, netmgr);
	ninja->addAnimator(ninjaAnim);
	ninjaAnim->drop();

	// Get rid of annoying mouse
	cursor->setVisible(false);


	// Here is another example of irrNet's lower level functionality. You can create a packet and add variable to it very
	// easily (you can even add vector3df). Then we are creating a custom handler and enabling custom handling. We do not
	// need all of this right now, so it is commented. Once you are familiar with this demo and how irrNet works (It wont
	// take long because it is very simple) you can uncomment this, the custom handler above, and the send line in the run
	// loop to see custom handling in effect. (It will simply send some numbers then output them to console when it recieves
	// them.)
	
	// Please note that irrNet uses a c8 at the start of the packet to determine its type. These numbers are all
	// below 30, so use a c8 with a number that is 30 or higher to identify your own packets. Or even better when you 
	// want to use only low level functionality (Custom game classes etc) you can just disable the default handling using
	// setDefaultHandling and be free to do whatever you want, although it is not much trouble either way. Please look
	// into API documentation for CustomHandling as there are some useful functions for handling packets/players etc.


	/*

	OutPacket* packy = netmgr->createOutPacket();

	c8 twenty = 20;
	u16 dude = 56;

	packy->addData(twenty);
	packy->addData(dude);

	myHandler* handler = new myHandler;
	netmgr->setCustomHandling(1,handler);

	*/

	// Main run loop
	while(device->run())
	{
			driver->beginScene(true, true,irr::video::SColor(0,0,0,0));
			smgr->drawAll();
			guienv->drawAll(); // not that we have GUI stuff
			device->setWindowCaption(L"MultiPlayer Demo");
			driver->endScene();

			// Here is the send packet for custom packet handling example, dont uncomment this until you are ready for
			// the custom handling and have commented the other two parts. (Read paragraphs before loop for more info)
			// netmgr->sendOutPacket(packy);

			// ALWAYS remember to update the client or server.

#ifdef CLIENT 
			netmgr->updateClient();
#else
			netmgr->updateServer();
#endif

	}

	// Kill renderer
	if(device)
	{
		device->drop();
		device = NULL;
	}
	return 0;
}

// And thats it! Easy peasy stuff...
