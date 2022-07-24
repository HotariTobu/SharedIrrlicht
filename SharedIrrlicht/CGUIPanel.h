#pragma once

#ifndef __C_GUI_PANEL_H_INCLUDED__
#define __C_GUI_PANEL_H_INCLUDED__

#include <IGUIEnvironment.h>
#include <IGUIElement.h>

namespace irr {
	namespace gui {
		class CGUIPanel : public IGUIElement {
		public:
			CGUIPanel(IGUIEnvironment* environment, core::rect<s32> rectangle, IGUIElement* parent = 0, s32 id = -1);
			virtual ~CGUIPanel();

			virtual void draw() override;

			void setBorderColor(video::SColor color) {
				BorderColor = color;
			}

			video::SColor getBorderColor() const {
				return BorderColor;
			}

			void setBackgroundColor(video::SColor color) {
				BackgroundColor = color;
			}

			video::SColor getBackgeoundColor() const {
				return BackgroundColor;
			}

			void setBorderThickness(u32 value) {
				BorderThickness = value;
			}

			u32 getBorderThickness() const {
				return BorderThickness;
			}

			core::recti getClientRect() {
				ClientRect = AbsoluteRect;
				ClientRect.UpperLeftCorner += BorderThickness;
				ClientRect.LowerRightCorner -= BorderThickness;
				return ClientRect;
			}

		protected:
			video::SColor BorderColor, BackgroundColor;
			u32 BorderThickness;
			core::recti ClientRect;
		};
	}
}

#endif // !__C_GUI_PANEL_H_INCLUDED__
