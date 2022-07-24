#include <irrNet.h>

using namespace irr;
using namespace scene;
using namespace video;
using namespace core;
using namespace gui;
using namespace io;
using namespace net;

#define EPSILON 0.001f



class CSceneNodeAnimator3PCamera :
	public irr::scene::ISceneNodeAnimator,
	public irr::IEventReceiver
{

public:

	//! Constructor
	//! player: The object to follow
	//! initDist: The initial distance from the player
	//! initAngleY: The initial horizontal rotatation
	//! initAngleZ: The initial vertical rotation
	//! targetOffset: The offset from the object's center at which the camera looks
	//! minDist/maxDist: Distance bounding
	//! minAngle/maxAngle: Rotation bounding. -89 ==> looking up at player, 89 ==> looking down
	//! boxSize: The size of the box in which mouse movements do not result in camera movements
	//! rotationSpeed/zoomSpeed: Speed of movement, in terms of units/millisecond
	CSceneNodeAnimator3PCamera(
		irr::scene::ISceneManager *manager,irr::gui::ICursorControl *cursor,irr::scene::ISceneNode *player,
		irr::f32 initDist = 100.0f,
		irr::f32 initAngleY = 180.0f,
		irr::f32 initAngleZ = 50.0f,
		irr::core::vector3df &targetOffset = irr::core::vector3df(100,0,0),
		irr::f32 minDist = 50.0f,
		irr::f32 maxDist = 200.0f,
		irr::f32 minAngle = -15.0f,
		irr::f32 maxAngle = 89.0f,
		irr::f32 boxSize = 0.0f,
		irr::f32 rotationSpeed = 80.0f,
		irr::f32 zoomSpeed = 5.0f);

	//! Destructor
	virtual ~CSceneNodeAnimator3PCamera(void);

	//! animates the scene node
	virtual void animateNode(irr::scene::ISceneNode *node, irr::u32 timeMs);

	//! Process an input event
	virtual bool OnEvent(irr::SEvent event);

	//! Get/set active status
	bool isActive();
	void setActive(bool status);

	//! Get/set box size
	irr::f32 getBoxSize();
	void setBoxSize(irr::f32 newSize);

	//! Map zoom in/zoom out buttons
	void mapZoomIn(irr::EKEY_CODE newCode);
	void mapZoomOut(irr::EKEY_CODE newCode);

	//! Access the camera's current orientation
	irr::f32 getOrientation();


private:

	bool Active;

	// current states
	irr::f32 Dist;
	irr::f32 AngleY;
	irr::f32 AngleZ;

	// boundaries
	irr::core::vector3df TargetOffset;
	irr::f32 MinDist;
	irr::f32 MaxDist;
	irr::f32 MinAngle;
	irr::f32 MaxAngle;
	irr::f32 BoxSize;

	// Motion
	irr::f32 RotationSpeed;
	irr::f32 ZoomSpeed;

	// Zooming keys
	irr::EKEY_CODE ZoomInKey;
	irr::EKEY_CODE ZoomOutKey;

	bool ZoomIn;
	bool ZoomOut;

	//! Node to follow
	irr::scene::ISceneNode *Player;

	irr::scene::ISceneManager *Manager;
	irr::gui::ICursorControl *Cursor;

	//! Puts the cursor back in the box
	void updateCursorPosition();
};

enum EPLAYER_ACTION
{
	EPA_MOVE_FORWARD,
	EPA_MOVE_BACKWARD,
	EPA_MOVE_LEFT,
	EPA_MOVE_RIGHT,
	EPA_COUNT,
	EPA_EXIT
};

//! Defines player states
enum EPLAYER_STATE
{
	EPS_RUNNING,
	EPS_STANDING,
	EPS_ATTACKING,
	EPS_NULL
};

class CSceneNodeAnimator3PPlayer :
	public irr::scene::ISceneNodeAnimator,
	public irr::IEventReceiver
{
public:

	//! Constructor
	//! camera: The 3PCamera watching this object (should have a 3PC Animator attached to it)
	//! cameraAnimator: The animator mentioned above
	//! moveSpeed: The speed at which this player moves
	CSceneNodeAnimator3PPlayer(
		irr::IrrlichtDevice *device,
		irr::scene::ICameraSceneNode *camera,
		CSceneNodeAnimator3PCamera *cameraAnimator,
		irr::f32 moveSpeed = 0.2f,irr::scene::ITriangleSelector *selector = 0, irr::net::INetManager* netmgr = 0);

	virtual ~CSceneNodeAnimator3PPlayer(void);

	//! animates the scene node
	virtual void animateNode(irr::scene::ISceneNode *node, irr::u32 timeMs);

	//! Process an input event
	virtual bool OnEvent(irr::SEvent event);

	//! Map movement keys
	void MapKey(EPLAYER_ACTION action, irr::EKEY_CODE newCode);

	//! Retrieve the player's current state
	EPLAYER_STATE getState();

private:

	irr::f32 MoveSpeed;
	irr::u32 PrevTime;

	EPLAYER_STATE State;

	// action keys
	irr::EKEY_CODE Keys[EPA_COUNT];
	bool KeyStates[EPA_COUNT];
	//Define things used in raycasting type crap
	core::line3d<f32> line;
	core::vector3df intersection;
	core::triangle3df tri;
	irr::scene::ICameraSceneNode *Camera;
	CSceneNodeAnimator3PCamera *CameraAnimator;
	irr::scene::ISceneManager *Manager;
	irr::scene::ITriangleSelector *selector;
	irr::net::INetManager* Netmgr;
	irr::IrrlichtDevice *Device;
	double spinspeed;
	float outwardradiance;
	float lradius;
	float rradius;
	ISceneNode * billy;
	bool imded;
	irr::scene::IAnimatedMeshSceneNode *amNode;
	vector3df lastvec;
};
