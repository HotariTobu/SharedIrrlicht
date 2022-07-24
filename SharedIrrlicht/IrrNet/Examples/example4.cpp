#include <irrlicht.h>
#include <irrNet.h>

#include "Animators.h"
#include "chatQue.h"

using namespace irr;
using namespace scene;
using namespace video;
using namespace core;
using namespace gui;
using namespace io;
using namespace net;

#pragma comment(lib, "irrlicht.lib")
#pragma comment(lib, "ws2_32.lib")

// Comment/Uncomment this to define if this is a client or server.
//#define CLIENT

// Objects used in the Event Receiver, for camera and player animation.
CSceneNodeAnimator3PCamera *camAnim;
CSceneNodeAnimator3PPlayer *ninjaAnim;

IrrlichtDevice* device = 0;
irr::gui::IGUIEnvironment* guienv;
irr::net::INetManager* netmgr;
IGUIEditBox* editbox;
ChatQue* chatQue;
irr::net::OutPacket* myOutPacket;
bool switch2;
stringc chatstring; // Create a chat string. (stringc for non wide chars.)
int ping;

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

      if(!guienv->hasFocus(editbox))   
      {
         if(camAnim->OnEvent(event))
            return true;
         if(ninjaAnim->OnEvent(event))
            return true;
      }

      if(event.EventType == irr::EET_KEY_INPUT_EVENT)
      {
         
         if(event.KeyInput.Key == KEY_KEY_T &&! event.KeyInput.PressedDown)
         { 
            guienv->setFocus(editbox);
         }

		if(guienv->hasFocus(editbox))   
		 {
         if(event.KeyInput.Key == KEY_RETURN)
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

               // Clear the edit box and remove the focus.
               guienv->removeFocus(editbox);
               editbox->setText(L"");
              
   
            
           
			
			}
}
      }
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

class myCustomHandler :
            public irr::net::CustomHandler
{
public:
    myCustomHandler();
    virtual ~myCustomHandler(void);
    virtual void Handle(irr::net::InPacket * packet);
private:
   char* buffer[1000]; // This is a buffer for storing the char string.
   c8 packetid;   // This is a single 8-bit char for storing the packet id.
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

// Make some enumerations for our frame states.
enum FRAMESTATES {RUN, ATTACK, STAND};

//! Main game loop
int main()
{

   // Set up event reciever.
   MyEventReceiver receiver;

   // Create Device
   device = irr::createDevice(irr::video::EDT_DIRECT3D9,irr::core::dimension2d<irr::s32>(800,600),32,0,0,1,&receiver);
      
   // Make pointers to irrlicht stuff
   irr::video::IVideoDriver*driver = device->getVideoDriver();
   irr::scene::ISceneManager*smgr = device->getSceneManager();
   guienv = device->getGUIEnvironment();
   irr::gui::ICursorControl*cursor = device->getCursorControl();
   irr::scene::ICameraSceneNode*camera = smgr->addCameraSceneNode();

   // Make a new INetManager
   netmgr = createNetManager(device);

   
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
   nmaterial.Texture1 = driver->getTexture("nskinbr.jpg");
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
   // intervals. Here I am sending the position every second (1000 ms), and the rotation every 50 ms.
   // (Frame sending obsolete now that we have frame states.) Please note all others default to 0, or as frequent as possible.
   netmgr->sendPositionAuto(ninja, 1000);
   netmgr->sendRotationAuto(ninja, 50);

   // Create the camera animator (Only attaching locally because we only want camera on our own ninja.)
   camAnim = new CSceneNodeAnimator3PCamera(smgr,cursor,ninja,50,180,10,irr::core::vector3df(0,20,0));
   camera->addAnimator(camAnim);
   camAnim->drop();
   camera->setNearValue(3.5f);
   camera->setFOV(PI / 3.0f);

   // Create the player animator (Same here we only want to control our own ninja.)
   ninjaAnim = new CSceneNodeAnimator3PPlayer(device,camera,camAnim,1.5f,selector,netmgr);
   ninja->addAnimator(ninjaAnim);
   ninjaAnim->drop();

   // Get rid of annoying mouse
   cursor->setVisible(false);

   // Enable our custom handler.
   myCustomHandler* myHandler = new myCustomHandler();
   netmgr->setCustomHandling(true,myHandler);

   // Make the GUI elements. (Thanks to Rambus for his chatQue class.)
   // (Else this demo will spend too long covering the visual side!)
   // Remember to have a font in the proper directory!!!
   IGUIFont* guiFont = device->getGUIEnvironment()->getFont("media/font.bmp");
   chatQue = new ChatQue();
   chatQue->initialise(device,core::rect<s32>(20,400,700,520),guiFont,10000,500);
   chatQue->addMessage("Welcome to the Custom Handling with chat demo.");
   chatQue->addMessage("Press T to talk.");
   guienv->getSkin()->setFont(guienv->getFont("media/font.bmp"));
   editbox = guienv->addEditBox(L"",rect<s32>(20,500,800,600),false);
   editbox->setOverrideColor(SColor(255,255,255,255));
   
   int lastFPS = -1;
   
   myOutPacket = netmgr->createOutPacket(); // Make an OutPacket.
        
   // Main run loop
   while(device->run())
   {
         driver->beginScene(true, true,irr::video::SColor(0,0,0,0));
         smgr->drawAll();
         guienv->drawAll(); // not that we have GUI stuff
         chatQue->draw();
         driver->endScene();


#ifdef CLIENT
         netmgr->updateClient();
#else
         netmgr->updateServer();
#endif
      int fps = driver->getFPS();

      // Use the new feature "getPing" to retrieve the first ENet peer and use its "roundTripTime" variable as the ping :)
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

   // Kill renderer
   if(device)
   {
      device->drop();
      device = NULL;
   }
   return 0;
}

// And thats it! Easy peasy stuff... 