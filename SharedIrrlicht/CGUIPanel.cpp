#include "CGUIPanel.h"
#include <IVideoDriver.h>

namespace irr {
	namespace gui {
		CGUIPanel::CGUIPanel(IGUIEnvironment* environment, core::rect<s32> rectangle, IGUIElement* parent, s32 id) : IGUIElement(gui::EGUIET_ELEMENT, environment, parent ? parent : environment->getRootGUIElement(), id, rectangle), BorderColor(video::SColor(0)), BackgroundColor(video::SColor(0)), BorderThickness(1) {
			IsVisible = false;
		}

		CGUIPanel::~CGUIPanel() {
			IGUIElement::~IGUIElement();
		}

		void CGUIPanel::draw() {
			if (!IsVisible) {
				return;
			}

			video::IVideoDriver* drv = Environment->getVideoDriver();
			if (drv) {
				drv->draw2DRectangle(BorderColor, AbsoluteRect, &AbsoluteClippingRect);
				drv->draw2DRectangle(BackgroundColor, getClientRect());
			}

			IGUIElement::draw();
		}
	}
}