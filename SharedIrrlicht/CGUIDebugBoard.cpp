#include "CGUIDebugBoard.h"

namespace irr {
	namespace gui {
		CGUIDebugBoard::CGUIDebugBoard(IGUIEnvironment* environment, core::rect<s32> rectangle, IGUIElement* parent, s32 id) : IGUIElement(EGUIET_ELEMENT, environment, parent ? parent : environment->getRootGUIElement(), id, rectangle) {
			IGUISkin* skin = Environment->getSkin();
			if (skin) {
				Font = skin->getFont();
				Color = skin->getColor(EGDC_BUTTON_TEXT);
			}
			else {
				Font = Environment->getBuiltInFont();
			}
		}

		CGUIDebugBoard::~CGUIDebugBoard() {
			IGUIElement::~IGUIElement();

			if (Font) {
				Font->drop();
			}
		}

		void CGUIDebugBoard::draw() {
			if (!IsVisible) {
				return;
			}

			IGUIElement::draw();

			if (Font) {
				Font->draw(Text, RelativeRect, Color);
			}

			Text = core::stringw();
		}

		void CGUIDebugBoard::setFont(IGUIFont* font) {
			if (font) {
				Font = font;
			}
			else {
				IGUISkin* skin = Environment->getSkin();
				if (skin) {
					Font = skin->getFont();
				}
				else {
					Font = Environment->getBuiltInFont();
				}
			}
		}

		IGUIFont* CGUIDebugBoard::getFont() const {
			return Font;
		}

		void CGUIDebugBoard::setColor(video::SColor color) {
			Color = color;
		}

		video::SColor CGUIDebugBoard::getColor() const {
			return Color;
		}

		void CGUIDebugBoard::appendLine(core::stringw text) {
			if (IsVisible) {
				Text += text + "\r\n";
			}
		}
	}
}