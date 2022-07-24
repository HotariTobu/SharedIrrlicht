#include "Animators.h"

using namespace irr;
using namespace scene;
using namespace video;
using namespace core;
using namespace gui;
using namespace io;

bool mouseLeft = 0;
bool mouseMiddle = 0;
bool mouseRight = 0;
bool firstattack = 0;
vector3df curmvec;
core::vector3df playerpos;
int frameno;
int framax = 1000;

enum FRAMESTATES {RUN, ATTACK, STAND};

CSceneNodeAnimator3PCamera::CSceneNodeAnimator3PCamera(
	irr::scene::ISceneManager *manager,irr::gui::ICursorControl *cursor,irr::scene::ISceneNode *player,
	irr::f32 initDist,irr::f32 initAngleY,irr::f32 initAngleZ,
	irr::core::vector3df &targetOffset,
	irr::f32 minDist,irr::f32 maxDist,irr::f32 minAngle,irr::f32 maxAngle,
	irr::f32 boxSize,irr::f32 rotationSpeed,irr::f32 zoomSpeed)
	: Active(true), Manager(manager), Cursor(cursor), Player(player),
	Dist(initDist),AngleY(initAngleY),AngleZ(initAngleZ),
	TargetOffset(targetOffset),
	MinDist(minDist), MaxDist(maxDist), MinAngle(-maxAngle), MaxAngle(-minAngle),
	BoxSize(boxSize), RotationSpeed(rotationSpeed), ZoomSpeed(zoomSpeed)
{
	ZoomIn = false;
	ZoomOut = false;

	ZoomInKey = irr::KEY_ADD;
	ZoomOutKey = irr::KEY_SUBTRACT;

	Cursor->setPosition(0.5f,0.5f);

	// Ensure Distance is bounded correctly
	if(Dist < MinDist)
		Dist = MinDist;
	else if(Dist > MaxDist)
		Dist = MaxDist;

	// Bound MinAngle/MaxAngle to avoid problematic areas
	if(MinAngle < -89.0f)
		MinAngle = -89.0f;
	if(MinAngle > 89.0f)
		MinAngle = 89.0f;
	if(MaxAngle < -89.0f)
		MaxAngle = -89.0f;
	if(MaxAngle > 89.0f)
		MaxAngle = 89.0f;
	if(minAngle > maxAngle)
		MaxAngle = MinAngle+1.0f;

	// Ensure Vertical Rotation Angle is bounded correctly
	if(AngleZ < MinAngle)
		AngleZ = MinAngle;
	else if(AngleZ > MaxAngle)
		AngleZ = MaxAngle;
}

//! Destructor
CSceneNodeAnimator3PCamera::~CSceneNodeAnimator3PCamera(void)
{}

//! Puts the cursor back in the box
void CSceneNodeAnimator3PCamera::updateCursorPosition()
{
	irr::core::position2d<irr::f32> pos = Cursor->getRelativePosition();

	if(pos.X < (0.5f-BoxSize))
		pos.X = (0.5f-BoxSize);
	if(pos.X > (0.5f+BoxSize))
		pos.X = (0.5f+BoxSize);

	if(pos.Y < (0.5f-BoxSize))
		pos.Y = (0.5f-BoxSize);
	if(pos.Y > (0.5f+BoxSize))
		pos.Y = (0.5f+BoxSize);

	Cursor->setPosition(pos);
}

//! Process an input event
bool CSceneNodeAnimator3PCamera::OnEvent(irr::SEvent event)
{
	if(!Active)
	{
		return false;
	}

	if(event.EventType == irr::EET_KEY_INPUT_EVENT)
	{
		if(event.KeyInput.Key == ZoomInKey)
		{
			ZoomIn = event.KeyInput.PressedDown;
			return true;
		}
		else if(event.KeyInput.Key == ZoomOutKey)
		{
			ZoomOut = event.KeyInput.PressedDown;
			return true;
		}
	}

	return false;
}

//! Get/set active status
bool CSceneNodeAnimator3PCamera::isActive()
{
	return Active;
}

void CSceneNodeAnimator3PCamera::setActive(bool status)
{
	// reset the cursor only if we are switching back to active from inactive
	if(!Active && status)
	{
		updateCursorPosition();
	}

	Active = status;
}

//! Get/set box size
irr::f32 CSceneNodeAnimator3PCamera::getBoxSize()
{
	return BoxSize;
}

void CSceneNodeAnimator3PCamera::setBoxSize(irr::f32 newSize)
{
	BoxSize = newSize;
	updateCursorPosition();
}

//! Map zoom in/zoom out buttons
void CSceneNodeAnimator3PCamera::mapZoomIn(irr::EKEY_CODE newCode)
{
	ZoomInKey = newCode;
}

void CSceneNodeAnimator3PCamera::mapZoomOut(irr::EKEY_CODE newCode)
{
	ZoomOutKey = newCode;
}

//! Access the camera's current orientation
irr::f32 CSceneNodeAnimator3PCamera::getOrientation()
{
	return AngleY;
}


//! animates the scene node
void CSceneNodeAnimator3PCamera::animateNode(irr::scene::ISceneNode *node, irr::u32 timeMs)
{
	// make sure you don't go attaching this animator to anything other than a camera
	irr::scene::ICameraSceneNode *camera = (irr::scene::ICameraSceneNode*)node;

	if(Manager->getActiveCamera() != camera)
	{
		return;
	}

	if(Active)
	{
		// Camera is active, rotate as necessary
		irr::core::position2d<irr::f32> pos = Cursor->getRelativePosition();

		if(pos.X < 0.5f-BoxSize-EPSILON)
		{
			AngleY -= (pos.X-(0.5f-BoxSize))*RotationSpeed;//Decrement of this statement gives the correct movement to the mouse.... by smartwhiz
		}

		if(pos.X > 0.5f+BoxSize+EPSILON)
		{
			AngleY -= (pos.X-(0.5f+BoxSize))*RotationSpeed;//Decrement of this statement gives the correct movement to the mouse.... by smartwhiz
		}

		// So we don't get huge rotation numbers
		if(AngleY > 360.0f)
		{
			AngleY -= 360.0f;
		}

		if(AngleY < 0.0f)
		{
			AngleY += 360.0f;
		}

		if(pos.Y < 0.5f-BoxSize-EPSILON)
		{
			AngleZ -= (pos.Y-(0.5f-BoxSize))*RotationSpeed;
		}

		if(pos.Y > 0.5f+BoxSize+EPSILON)
		{
			AngleZ -= (pos.Y-(0.5f+BoxSize))*RotationSpeed;
		}

		// Ensure Vertical Rotation Angle is bounded correctly
		if(AngleZ < MinAngle)
			AngleZ = MinAngle;
		else if(AngleZ > MaxAngle)
			AngleZ = MaxAngle;

		// Zoom only if one or other button is pressed
		if((ZoomIn && !ZoomOut) || (!ZoomIn && ZoomOut))
		{
			int dir = (ZoomIn) ? -1 : 1;
			Dist += ZoomSpeed*dir;
			// Ensure Distance is bounded correctly
			if(Dist < MinDist)
				Dist = MinDist;
			else if(Dist > MaxDist)
				Dist = MaxDist;
		}

		updateCursorPosition();
	}

	// Create translation vector
	irr::core::vector3df translation(Dist,0,0);
	translation.rotateXYBy(-AngleZ,irr::core::vector3df(0,0,0));
	translation.rotateXZBy(AngleY,irr::core::vector3df(0,0,0));

	// Assumes the camera is *not* a child of the player node
	vector3df cameratarget = Player->getPosition() + vector3df(0,30,0);
	camera->setTarget(cameratarget+TargetOffset);
	camera->setPosition(cameratarget+translation+TargetOffset);
}




CSceneNodeAnimator3PPlayer::CSceneNodeAnimator3PPlayer(
	irr::IrrlichtDevice *device,
	irr::scene::ICameraSceneNode *camera,
	CSceneNodeAnimator3PCamera *cameraAnimator,
	irr::f32 moveSpeed,
	irr::scene::ITriangleSelector *selector,
	irr::net::INetManager* netmgr)
	: Device(device), Camera(camera), CameraAnimator(cameraAnimator), MoveSpeed(moveSpeed),	selector(selector), Netmgr(netmgr)
{

	PrevTime = -1;
	lastvec = vector3df(0,0,0);
	
	Manager = device->getSceneManager();

	// Set initial key map
	Keys[EPA_MOVE_FORWARD] = irr::KEY_KEY_W;
	Keys[EPA_MOVE_BACKWARD] = irr::KEY_KEY_S;
	Keys[EPA_MOVE_LEFT] = irr::KEY_KEY_A;
	Keys[EPA_MOVE_RIGHT] = irr::KEY_KEY_D;

	// Init states
	for(int i=0;i<EPA_COUNT;i++)
	{
		KeyStates[i] = false;
	}
	State = EPS_STANDING;
	imded = 0;
}

CSceneNodeAnimator3PPlayer::~CSceneNodeAnimator3PPlayer(void)
{}

void movepla(vector3df)
{}
//! Map movement keys
void CSceneNodeAnimator3PPlayer::MapKey(EPLAYER_ACTION action, irr::EKEY_CODE newCode)
{
	Keys[action] = newCode;
}

//! Retrieve the player's current state
EPLAYER_STATE CSceneNodeAnimator3PPlayer::getState()
{
	return State;
}

bool eventpassed = 0;

//! Process an input event
bool CSceneNodeAnimator3PPlayer::OnEvent(irr::SEvent event)
{
	eventpassed = 0;
	if(event.EventType == irr::EET_KEY_INPUT_EVENT)
	{
		if(event.KeyInput.Key == KEY_ESCAPE){Device->closeDevice();}
		for(int i=0;i<EPA_COUNT;i++)
		{
			if(event.KeyInput.Key == Keys[i])
			{
				KeyStates[i] = event.KeyInput.PressedDown;
				eventpassed = 1;
			}
		}
	}
	if(event.EventType == irr::EET_MOUSE_INPUT_EVENT)
	{
		if(event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			mouseLeft = 1;
			eventpassed = 1;
		}
		if(event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
		{
			mouseRight = 1;
			eventpassed = 1;
		}
	}
	return eventpassed;
}

int initrun2 = 2;

//! animates the scene node
void CSceneNodeAnimator3PPlayer::animateNode(irr::scene::ISceneNode *node, irr::u32 timeMs)
{
	// This would have to be modified for an unanimated node
	amNode = (irr::scene::IAnimatedMeshSceneNode*)node;
	
	// Create movement directions based on camera orientation
	irr::core::vector3df forward = Camera->getTarget()-Camera->getPosition();
	forward.Y = 0;
	irr::core::vector3df left = forward.crossProduct(Camera->getUpVector());
	forward = forward.normalize();
	left = left.normalize();
	frameno = amNode->getFrameNr();
	playerpos = amNode->getPosition();

	// first loop
	if(PrevTime == -1)
	{
		PrevTime = timeMs;
		Netmgr->sendFrameState((irr::scene::IAnimatedMeshSceneNode*)node,STAND);
	}

	//line.start = amNode->getPosition()+vector3df(0,50,0);
	//line.end = amNode->getPosition()-vector3df(0,1000,0);
	//if (Manager->getSceneCollisionManager()->getCollisionPoint(line, selector, intersection,tri))
	//{
	//	amNode->setPosition(intersection + vector3df(0,5,0));
	//	if(initrun2 == 2){amNode->setPosition(intersection + vector3df(1000,5,2300));}
	//}

	if(initrun2 == 2)
	{
		initrun2 = 1;
	}


	// Timer operations
	irr::f32 timeMsFloat = (irr::f32)(timeMs-PrevTime);
	PrevTime = timeMs;
	// Auto-stop attack
	if(frameno >= framax && frameno < 7000)
	{
		State = EPS_NULL;
		amNode->setLoopMode(true);
		firstattack = 0;
		mouseLeft = 0;
		mouseRight = 0;
	}

	if(mouseLeft == 1 && State != EPS_ATTACKING)
	{
		State = EPS_ATTACKING;
		Netmgr->sendFrameState((irr::scene::IAnimatedMeshSceneNode*)node,ATTACK);
		framax = 2360;
		spinspeed = 0.05;
		firstattack = 1;
		mouseLeft = 0;
		curmvec = vector3df(0,0,0);
	}
	else if (mouseRight == 1 && State != EPS_ATTACKING)
	{
		State = EPS_ATTACKING;
		Netmgr->sendFrameState((irr::scene::IAnimatedMeshSceneNode*)node,ATTACK);
		framax = 2360;
		mouseRight = 0;
		curmvec = vector3df(0,0,0);
	}
	else if((KeyStates[EPA_MOVE_FORWARD] || KeyStates[EPA_MOVE_BACKWARD] ||
		KeyStates[EPA_MOVE_LEFT]|| KeyStates[EPA_MOVE_RIGHT]) && State != EPS_ATTACKING)
	{
		if(State != EPS_RUNNING)
		{
			State = EPS_RUNNING;
			Netmgr->sendFrameState((irr::scene::IAnimatedMeshSceneNode*)node,RUN);
			firstattack = 0;
		}
	}

	else if(State != EPS_STANDING && State != EPS_ATTACKING)
	{
		State = EPS_STANDING;
		Netmgr->sendFrameState((irr::scene::IAnimatedMeshSceneNode*)node,STAND);
		curmvec = vector3df(0,0,0);
		firstattack = 0;
		return; // no movement
	}
	

	if(State != EPS_ATTACKING)
	{
		// Rotation must be set differently for all 8 possible movement directions.
		if(KeyStates[EPA_MOVE_FORWARD] && KeyStates[EPA_MOVE_LEFT])
		{
			irr::core::vector3df movementVector = forward+left;
			movementVector = movementVector.normalize();


			curmvec = ((movementVector*MoveSpeed));
			amNode->setRotation(irr::core::vector3df(0,45.0f-CameraAnimator->getOrientation(),0));
		}
		else if(KeyStates[EPA_MOVE_FORWARD] && KeyStates[EPA_MOVE_RIGHT])
		{

			irr::core::vector3df movementVector = forward-left;
			movementVector = movementVector.normalize();

			curmvec = ((movementVector*MoveSpeed));

			amNode->setRotation(irr::core::vector3df(0,135.0f-CameraAnimator->getOrientation(),0));

		}
		else if(KeyStates[EPA_MOVE_BACKWARD] && KeyStates[EPA_MOVE_LEFT])
		{

			irr::core::vector3df movementVector = left-forward;
			movementVector = movementVector.normalize();

			curmvec = ((movementVector*MoveSpeed));


			amNode->setRotation(irr::core::vector3df(0,315.0f-CameraAnimator->getOrientation(),0));
		}
		else if(KeyStates[EPA_MOVE_BACKWARD] && KeyStates[EPA_MOVE_RIGHT])
		{
			irr::core::vector3df movementVector = (forward*-1)-left;
			movementVector = movementVector.normalize();

			curmvec = ((movementVector*MoveSpeed));
			amNode->setRotation(irr::core::vector3df(0,225.0f-CameraAnimator->getOrientation(),0));
		}
		else if(KeyStates[EPA_MOVE_FORWARD])
		{

			curmvec = ((forward*MoveSpeed));

			amNode->setRotation(irr::core::vector3df(0,90.0f-CameraAnimator->getOrientation(),0));
		}
		else if(KeyStates[EPA_MOVE_BACKWARD])
		{
			curmvec = ((-forward*MoveSpeed));


			amNode->setRotation(irr::core::vector3df(0,270.0f-CameraAnimator->getOrientation(),0));
		}
		else if(KeyStates[EPA_MOVE_LEFT])
		{

			curmvec = ((left*MoveSpeed));


			amNode->setRotation(irr::core::vector3df(0,-CameraAnimator->getOrientation(),0));
		}
		else if(KeyStates[EPA_MOVE_RIGHT])
		{

			curmvec = ((-left*MoveSpeed));

			amNode->setRotation(irr::core::vector3df(0,180-CameraAnimator->getOrientation(),0));
		}
		
	}

	if(lastvec != curmvec)
		{
		Netmgr->setLocalVelocity(amNode,curmvec);
		Netmgr->sendVelocity(amNode,curmvec);
		lastvec = curmvec;
		}
}