#include <irrlicht.h>
//#include <enet/enet.h>
#include <irrNet.h>
#include "chatQue.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


#pragma comment(lib, "Irrlicht.lib")
#pragma comment(lib, "ws2_32.lib")

IrrlichtDevice* device = 0;
ISceneNode* cuby = 0;
ChatQue* chatQue;

bool isclient = 1; // Are we the client or the server?


enum EPLAYER_ACTION
{
	EPA_MOVEUP,
	EPA_MOVEDOWN,
	EPA_MOVELEFT,
	EPA_MOVERIGHT,
	EPA_RETURN,
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






class myCustomHandler :
            public irr::net::CustomHandler
{
public:
    myCustomHandler();
    virtual ~myCustomHandler(void);
    virtual void Handle(irr::net::InPacket * packet);
private:
	char* buffer[1000]; // This is a buffer for storing the char string.
	c8 packetid;	// This is a single 8-bit char for storing the packet id.
};

myCustomHandler::myCustomHandler()
{}

myCustomHandler::~myCustomHandler(void)
{}

//! animates the scene node
void myCustomHandler::Handle(irr::net::InPacket * packet)
{
	packet->getNextItem(packetid);
	// Lets say packet id 50 is a chat message.
	if(packetid == 50)
	{

	// String size handling is integrated into the packets. The default header size is u16, which would support up to
	// 65500-ish characters. If you wish to optimize the size to c8 (Which would limit you to 255 chars.), you would
	// have to uncomment the lines in Packets.cpp to use a header size of 1 and also find a way to get this across.
	// Personally I recommend not caring too much about that and leave it u16, its less of a headache that way.
	packet->getNextItem((char*)buffer);

	// And display it! (Dont forget to "(char*)" it!)
	chatQue->addMessage((char*)buffer);
	}
}



int main()
{


	MyEventReceiver receiver;

	Keys[EPA_MOVEUP] = irr::KEY_UP;
	Keys[EPA_MOVEDOWN] = irr::KEY_DOWN;
	Keys[EPA_MOVELEFT] = irr::KEY_LEFT;
	Keys[EPA_MOVERIGHT] = irr::KEY_RIGHT;
	Keys[EPA_RETURN] = irr::KEY_RETURN;

	device = createDevice(video::EDT_OPENGL, core::dimension2d<s32>(800, 600),
		32, false, false, true, &receiver);

	device->getFileSystem()->addZipFileArchive("./media/waker.dta");

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();


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

	
	

	// Seed the random variable.
	srand((unsigned int) device->getTimer()->getRealTime());

	// Create a Net Sphere. IrrNet will handle NetId's now, but you can still access them using various functions.
	// We will give the sphere a random X and Y position so it doesnt end up in the same place as the others.
	cuby = netmgr->addNetSphereSceneNode(5,16,vector3df((rand()%120)-60,(rand()%120)-60,100));
	// sendVelocity may get a little out of sync after a while so we update the position every second
	// it wont use a great deal of bandwidth but keeps our nodes on track
	// Remember that sendPositionAuto only sends when the position changes, so a stationary node uses no bandwidth
	netmgr->sendPositionAuto(cuby,1000);
	
	vector3df currentvel;
	vector3df sentvel;

	//netmgr->setDefaultHandling(false);
	// Enable our custom handler.
	myCustomHandler* myHandler = new myCustomHandler();
	netmgr->setCustomHandling(true,myHandler);

	// Create a simple camera
	smgr->addCameraSceneNode();

	// Make the GUI elements. (Thanks to Rambus for his chatQue class.)
	// (Else this demo will spend too long covering the visual side!)
	// Remember to have a font in the proper directory!!!
	IGUIFont* guiFont = device->getGUIEnvironment()->getFont("media/font.bmp");
	chatQue = new ChatQue();
	chatQue->initialise(device,core::rect<s32>(20,400,700,520),guiFont,10000,500);
	chatQue->addMessage("Welcome to the Custom Handling with chat demo.");
	guienv->getSkin()->setFont(guienv->getFont("media/font.bmp"));
	IGUIEditBox* editbox = guienv->addEditBox(L"",rect<s32>(20,500,800,600),false);
	editbox->setOverrideColor(SColor(255,255,255,255));
	guienv->setFocus(editbox);
	
	int lastFPS = -1;
	int ping;
	bool switch1 = 1;
	irr::net::OutPacket* myOutPacket = netmgr->createOutPacket(); // Make an OutPacket.
	stringc chatstring; // Create a chat string. (stringc for non wide chars.)

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

		if(KeyStates[EPA_RETURN])
		{
			if(switch1)
			{
			chatQue->addMessage(editbox->getText());
			

			// Make sure to clear this if you are re-using it.
			myOutPacket->clearData();

			// Use our chat packet id of 50.
			c8 packetid = 50; 
			myOutPacket->addData(packetid);

			chatstring = editbox->getText();

			// Follow that by the char array itself.
			myOutPacket->addData(chatstring.c_str());

			// Send the packet.
			netmgr->sendOutPacket(myOutPacket);

			// Clear the edit box. (There is also a switch incase send button is held down.)
			editbox->setText(L"");
			switch1 = 0;
			}
		}
		else
		{
			switch1 = 1;
		}

		driver->beginScene(true, true, video::SColor(255,50,50,150));

		smgr->drawAll(); // draw the 3d scene
		guienv->drawAll();
		chatQue->draw();

		driver->endScene();
		



		if(isclient)
		{
			netmgr->updateClient();
		}
		else
		{
			netmgr->updateServer();
		}

		// If our velocity changed send this to peer, and set it locally.
		if(currentvel != sentvel){netmgr->sendVelocity(cuby,currentvel);netmgr->setLocalVelocity(cuby,currentvel);sentvel = currentvel;}
			


		int fps = driver->getFPS();

		// Use the new feature "getPing" to retrieve the first ENet peer and use its "rountTripTime" variable as the ping :)
		// Please note that the ping may take a few seconds to stabilize after connection.
		ping = netmgr->getPing();

		if (lastFPS != fps)
		{
			wchar_t tmp[1024];
			swprintf_s(tmp, 1024, L"Network Test[%ls] fps: %d ping: %d", 
				driver->getName(), fps, ping);
			device->setWindowCaption(tmp);
			lastFPS = fps;
		}

		
	}

	
	//In the end, delete the Irrlicht device.
	
	device->drop();
	

	return 0;
}

