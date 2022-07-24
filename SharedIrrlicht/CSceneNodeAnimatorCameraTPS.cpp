#include "CSceneNodeAnimatorCameraTPS.h"

namespace irr {
	namespace scene {
		void CSceneNodeAnimatorCameraTPS::animateNode(ISceneNode* node, u32 timeMs) {
			if (!node || node->getType() != ESNT_CAMERA) {
				return;
			}

			if (!CenterPos) {
				return;
			}

			ICameraSceneNode* camera = static_cast<ICameraSceneNode*>(node);
			core::vector3df pos(camera->getPosition());

			core::vector2df XZ(pos.X, pos.Z);
			core::vector2df centerXZ(CenterPos->X, CenterPos->Z);
			f32 dis, theta;

			if (Dragging) {
				if (Invert) {
					pos.rotateXZBy(Difference.X * (f64)RotateSpeed, *CenterPos);
				}
				else {
					pos.rotateXZBy(Difference.X * (f64)-RotateSpeed, *CenterPos);
				}

				XZ.X = pos.X;
				XZ.Y = pos.Z;
				dis = XZ.getDistanceFrom(centerXZ);
				if (dis != 0) {
					if (Invert) {
						dis = -dis;
					}

					core::vector2df Y(core::vector2df(dis, pos.Y - CenterPos->Y).rotateBy(Difference.Y * (f64)RotateSpeed));

					bool modify = false;
					if (Invert && Y.X > 0) {
						modify = true;
					}
					else if (!Invert && Y.X < 0) {
						modify = true;
					}

					if (modify) {
						Invert = !Invert;
						camera->setUpVector(-camera->getUpVector());
					}

					pos.Y = Y.Y + CenterPos->Y;
					XZ -= centerXZ;
					XZ *= Y.X / dis;
					XZ += centerXZ;
					pos.X = XZ.X;
					pos.Z = XZ.Y;
				}

				Dragging = false;
			}

			if (Zooming) {
				f32 move = Wheel * ZoomSpeed;
				core::vector3df addition;

				if (Movable && (Shift || Ctrl)) {
					if (Shift) {
						dis = XZ.getDistanceFrom(centerXZ);
						if (dis != 0) {
							if (Invert) {
								dis = -dis;
							}

							theta = atan2f(pos.Y - CenterPos->Y, dis);
							addition.Y = move * cosf(theta);
							XZ -= centerXZ;
							XZ *= move * sinf(theta) / dis;
							addition.X = -XZ.X;
							addition.Z = -XZ.Y;
							XZ += centerXZ;

							pos += addition;
							*CenterPos += addition;
						}
					}

					if (Ctrl) {
						XZ -= centerXZ;
						theta = atan2f(XZ.Y, XZ.X);

						if (Invert) {
							addition.X = -move * sinf(theta);
							addition.Z = move * cosf(theta);
						}
						else {
							addition.X = move * sinf(theta);
							addition.Z = -move * cosf(theta);
						}

						XZ += centerXZ;

						pos += addition;
						*CenterPos += addition;
					}
				}
				else {
					dis = pos.getDistanceFrom(*CenterPos);
					f32 result = dis - move;
					if (dis != 0 && result > 0) {
						if (result <= MaxZoomOut && result >= MinZoomIn) {
							pos -= *CenterPos;
							pos *= result / dis;
							pos += *CenterPos;
						}
					}
				}

				Zooming = false;
			}

			camera->setPosition(pos);
			camera->setTarget(*CenterPos);
		}

		ISceneNodeAnimator* CSceneNodeAnimatorCameraTPS::createClone(ISceneNode* node, ISceneManager* newManager) {
			return new CSceneNodeAnimatorCameraTPS(CenterPos, RotateSpeed, ZoomSpeed);
		}

		bool CSceneNodeAnimatorCameraTPS::OnEvent(const SEvent& event) {
			if (event.EventType == EET_MOUSE_INPUT_EVENT) {
				static core::vector2di lastPos;
				static bool RButton = false;

				core::vector2di pos(event.MouseInput.X, event.MouseInput.Y);
				Difference = pos - lastPos;
				lastPos = pos;

				Wheel = event.MouseInput.Wheel;

				if (Movable) {
					Shift = event.MouseInput.Shift;
					Ctrl = event.MouseInput.Control;
				}

				switch (event.MouseInput.Event) {
				case EMIE_RMOUSE_PRESSED_DOWN:
					RButton = true;
					break;

				case EMIE_RMOUSE_LEFT_UP:
					RButton = false;
					break;

				case EMIE_MOUSE_MOVED:
					if (RButton) {
						Dragging = true;
					}
					break;

				case EMIE_MOUSE_WHEEL:
					Zooming = true;
					break;
				}
			}

			return false;
		}
	}
}