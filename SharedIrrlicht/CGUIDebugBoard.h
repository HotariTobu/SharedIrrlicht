#pragma once

#ifndef __C_GUI_DEBUGBOARD_H_INCLUDED__
#define __C_GUI_DEBUGBOARD_H_INCLUDED__

#include <IGUIEnvironment.h>
#include <IGUIElement.h>
#include <IGUIFont.h>
#include <irrString.h>

namespace irr {
	namespace gui {
		class CGUIDebugBoard : public IGUIElement {
		public:
			CGUIDebugBoard(IGUIEnvironment* environment, core::rect<s32> rectangle, IGUIElement* parent = 0, s32 id = -1);
			virtual ~CGUIDebugBoard();

			virtual void draw() override;

			void setFont(IGUIFont* font = 0);
			IGUIFont* getFont() const;
			void setColor(video::SColor color);
			video::SColor getColor() const;

			void appendLine(core::stringw str);

		private:
			IGUIFont* Font;
			video::SColor Color;
		};
	}
}

#endif // !__C_GUI_DEBUGBOARD_H_INCLUDED__