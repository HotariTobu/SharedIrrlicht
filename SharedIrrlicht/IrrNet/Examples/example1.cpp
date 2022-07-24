#include <irrlicht.h>
//#include <enet/enet.h>
#include <irrNet.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


#pragma comment(lib, "Irrlicht.lib")
#pragma comment(lib, "ws2_32.lib")

IrrlichtDevice* device = 0;
scene::ISceneNode* n = 0;

bool keypressed = 0;
bool isclient = 1; // Are we the client or the server?


enum EPLAYER_ACTION
{
	EPA_MOVEUP,
	EPA_MOVEDOWN,
	EPA_MOVELEFT,
	EPA_MOVERIGHT,
	EPA_SPACE,
	EPA_COUNT
};

irr::EKEY_CODE Keys[EPA_COUNT];
bool KeyStates[EPA_COUNT];
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
	Keys[EPA_SPACE] = irr::KEY_SPACE;


	device = createDevice(video::EDT_OPENGL, core::dimension2d<s32>(800, 600),
		32, false, false, true, &receiver);

	device->getFileSystem()->addZipFileArchive("./media/waker.dta");

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();



	// Initialize NetManager
	irr::net::INetManager* netmgr = new irr::net::INetManager(device);

	// Set the NetManager to output info to console
	netmgr->setVerbose(1);

	

	// Set up client or server
	if(isclient)
	{
		netmgr->setUpClient();
	}
	else
	{
		netmgr->setUpServer();
	}



	// sendVelocity may get a little out of sync after a while so we update the position every second
	// it wont use a great deal of bandwidth but keeps our nodes on track
	// Remember that sendPositionAuto only sends when the position changes, so a stationary node uses no bandwidth
	
	ISceneNode* cuby = 0;


	if(isclient)
	{
		// Create some Net Cubes irrNet will handle NetId's now, but you can still access them using various functions
		cuby = netmgr->addNetSphereSceneNode(5,16,vector3df(-50,0,100));
		// sendVelocity may get a little out of sync after a while so we update the position every second
		// it wont use a great deal of bandwidth but keeps our nodes on track
		// Remember that sendPositionAuto only sends when the position changes, so a stationary node uses no bandwidth
		netmgr->sendPositionAuto(cuby,1000);
	}
	else
	{	
		// Do the same for the server
		n = netmgr->addNetSphereSceneNode(5,16,vector3df(50,0,100),vector3df(0,0,0),vector3df(1,1,1));
		netmgr->sendPositionAuto(n,1000);
	}




	vector3df currentvel;
	vector3df sentvel;
	ISceneNodeAnimator* tmpanimate;
	int texid;


	// Create a simple camera
	smgr->addCameraSceneNode();
	device->getCursorControl()->setVisible(false);

	int lastFPS = -1;
	int counterint = 0;
	bool firstrecieve = 1;

	while(device->run())
	{
		currentvel = vector3df(0,0,0);
		if(KeyStates[EPA_MOVEUP])
		{
			currentvel = vector3df(0,2,0);keypressed = true;
		}
		if(KeyStates[EPA_MOVEDOWN])
		{
			currentvel = vector3df(0,-2,0);keypressed = true;
		}
		if(KeyStates[EPA_MOVERIGHT])
		{
			currentvel = vector3df(2,0,0);keypressed = true;
		}
		if(KeyStates[EPA_MOVELEFT])
		{
			currentvel = vector3df(-2,0,0);keypressed = true;
		}

		driver->beginScene(true, true, video::SColor(255,113,113,133));

		smgr->drawAll(); // draw the 3d scene

		driver->endScene();
		



		if(isclient)
		{
			netmgr->updateClient();
			// If our velocity changed send this to peer, and set it locally.
			if(currentvel != sentvel){netmgr->sendVelocity(cuby,currentvel);netmgr->setLocalVelocity(cuby,currentvel);sentvel = currentvel;}
			
		}
		else
		{
			netmgr->updateServer();
			// If space is pressed rotate the texture, we do this by sending a custom variable over the network.
			// This allows us to change any aspect of a node according to one of its custom variables,
			// which are specific to each node (Like a private member of the node that we can retrieve and change at will)
			if(KeyStates[EPA_SPACE])
			{
				printf("1");
			if(netmgr->getCustomVar(n,0) == 0){netmgr->sendCustomVar(n,0,1);}
			else if(netmgr->getCustomVar(n,0) == 1){netmgr->sendCustomVar(n,0,2);}
			else if(netmgr->getCustomVar(n,0) == 2){netmgr->sendCustomVar(n,0,3);}
			else if(netmgr->getCustomVar(n,0) == 3){netmgr->sendCustomVar(n,0,0);}
			}
			// If our velocity changed send this to peer, and set it locally.
			if(currentvel != sentvel){netmgr->sendVelocity(n,currentvel);netmgr->setLocalVelocity(n,currentvel);sentvel = currentvel;}

		}

		if(netmgr->getSceneNodeFromNetId(1))
		{
		if(firstrecieve){netmgr->getSceneNodeFromNetId(1)->setMaterialTexture(0, driver->getTexture("media/lwood.jpg"));netmgr->getSceneNodeFromNetId(1)->setMaterialFlag(EMF_LIGHTING,false);firstrecieve = 0;}
		if(netmgr->getCustomVar(netmgr->getSceneNodeFromNetId(1),0) == 0){netmgr->getSceneNodeFromNetId(1)->setMaterialTexture(0, driver->getTexture("media/lwood.jpg"));}
		else if(netmgr->getCustomVar(netmgr->getSceneNodeFromNetId(1),0) == 1){netmgr->getSceneNodeFromNetId(1)->setMaterialTexture(0, driver->getTexture("media/grass.jpg"));}
		else if(netmgr->getCustomVar(netmgr->getSceneNodeFromNetId(1),0) == 2){netmgr->getSceneNodeFromNetId(1)->setMaterialTexture(0, driver->getTexture("media/stones.jpg"));}
		else if(netmgr->getCustomVar(netmgr->getSceneNodeFromNetId(1),0) == 3){netmgr->getSceneNodeFromNetId(1)->setMaterialTexture(0, driver->getTexture("media/wood.jpg"));}
		}

		int fps = driver->getFPS();

		if (lastFPS != fps)
		{
			wchar_t tmp[1024];
			swprintf_s(tmp, 1024, L"Network Test[%ls] fps: %d (Server can press Space to change Cube colour!)", 
				driver->getName(), fps);
			device->setWindowCaption(tmp);
			lastFPS = fps;
		}

		
	}

	
	//In the end, delete the Irrlicht device.
	
	device->drop();
	

	return 0;
}

