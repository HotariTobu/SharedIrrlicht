#pragma once

#ifndef __MAIN_EVENT_RECEIVER_H_INCLUDED__
#define __MAIN_EVENT_RECEIVER_H_INCLUDED__

#include <functional>

#include "Head.h"

namespace irr {
	namespace utils {
		using EventHandler = std::function<bool(const SEvent&)>;

		class MainEventReceiver : IEventReceiver {
		public:
			MainEventReceiver(IrrlichtDevice* device, EventHandler onEvent = 0) : device(device), csr(device->getCursorControl()), fsy(device->getFileSystem()), env(device->getGUIEnvironment()), lgr(device->getLogger()), opr(device->getOSOperator()), rdm(device->getRandomizer()), mgr(device->getSceneManager()), tim(device->getTimer()), drv(device->getVideoDriver()), onEvent(onEvent) {}

			virtual bool OnEvent(const SEvent& event) override {
				if (event.EventType == EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown) {
					switch (event.KeyInput.Key) {
#ifdef __ESC_CLOSE__
					case EKEY_CODE::KEY_ESCAPE:
						device->closeDevice();
						break;
#endif // __ESC_CLOSE__
					case EKEY_CODE::KEY_F1:
						if (env) {
							gui::IGUIElement* root = env->getRootGUIElement();
							root->setVisible(!root->isVisible());
						}
						break;
					case EKEY_CODE::KEY_F2:
						if (drv) {
							video::IImage* image = drv->createScreenShot();
							if (image) {
								ITimer::RealTimeDate dateTime = device->getTimer()->getRealTimeAndDate();

								bool isChnage = fsy->changeWorkingDirectoryTo("./screenshots");
								core::stringc filename(core::stringc(dateTime.Year) + core::stringc(dateTime.Month) + core::stringc(dateTime.Day) + core::stringw("_") + core::stringc(dateTime.Hour) + core::stringc(dateTime.Minute) + core::stringc(dateTime.Second) + core::stringc(device->getTimer()->getTime()) + ".png");
								if (drv->writeImageToFile(image, filename)) {
									device->getLogger()->log((core::stringc("The screenshot was succeeded saving to \"") + fsy->getWorkingDirectory() + "/" + filename + "\"").c_str());
								}
								else {
									device->getLogger()->log((core::stringc("The screenshot was failed saving to \"") + fsy->getWorkingDirectory() + "/" + filename + "\"").c_str());
								}
								if (isChnage) {
									fsy->changeWorkingDirectoryTo("../");
								}
							}
							else {
								device->getLogger()->log("The screenshot failed creation.");
							}
						}
						break;
#ifdef _DEBUG
					case EKEY_CODE::KEY_F3:
						if (DebugBoard) {
							DebugBoard->setVisible(!DebugBoard->isVisible());
						}
						break;
#endif // _DEBUG
					}
				}

				if (onEvent) {
					return onEvent(event);
				}
				else {
					return false;
				}
			}

		private:
			IrrlichtDevice* device;

			gui::ICursorControl* csr;
			io::IFileSystem* fsy;
			gui::IGUIEnvironment* env;
			ILogger* lgr;
			IOSOperator* opr;
			IRandomizer* rdm;
			scene::ISceneManager* mgr;
			ITimer* tim;
			video::IVideoDriver* drv;

			EventHandler onEvent;
		};
	}
}

#endif // !__MAIN_EVENT_RECEIVER_H_INCLUDED__