#pragma once

#ifndef __CONVERTER_H_INCLUDED__
#define __CONVERTER_H_INCLUDED__

#include <irrString.h>

namespace irr {
	namespace utils {
		inline s32 c8_to_s32(c8 c) {
			s32 result = c - '0';
			if (result <= 9 && result >= 0) {
				return result;
			}
			else {
				return -1;
			}
		}

		template <typename StrT>
		inline s32 str_to_s32(core::string<StrT> str) {
			s32 result = 0;

			u32 size = str.size();
			if (size > 0) {
				u32 i = 0;

				bool sign = str[i] == '-';
				if (sign) {
					i++;
				}

				for (; i < size; i++) {
					s32 c = c8_to_s32(str[i]);
					if (c != -1) {
						result *= 10;
						result += c;
					}
					else {
						break;
					}
				}

				if (sign) {
					result = -result;
				}
			}

			return result;
		}

		template <typename StrT>
		inline u32 str_to_u32(core::string<StrT> str) {
			u32 result = 0;

			u32 size = str.size();
			if (size > 0) {
				for (u32 i = 0; i < size; i++) {
					s32 c = c8_to_s32(str[i]);
					if (c != -1) {
						result *= 10;
						result += c;
					}
					else {
						break;
					}
				}
			}

			return result;
		}

		template <typename StrT>
		inline f32 str_to_f32(core::string<StrT> str) {
			f32 result = 0;

			u32 size = str.size();
			if (size > 0) {
				u32 i = 0;

				bool sign = str[i] == '-';
				if (sign) {
					i++;
				}

				bool down = false;

				u32 decimal = 0;
				for (; i < size; i++) {
					s32 c = c8_to_s32(str[i]);
					if (c != -1) {
						result *= 10;
						result += c;
					}
					else if (str[i] == '.') {
						decimal = i + 1;
						down = true;
					}
					else {
						break;
					}
				}

				if (down) {
					for (; decimal < i; decimal++) {
						result /= 10;
					}
				}

				if (sign) {
					result = -result;
				}
			}

			return result;
		}

		inline s32 f32_to_s32(f32 value) {
			if ((s32)(value * 10) % 10 >= 5) {
				value += 1;
			}

			return (s32)value;
		}

		template <typename StrT>
		inline core::string<StrT> bool_to_str(bool value) {
			return core::string<StrT>(value ? "true" : "false");
		}

		template <typename StrT, typename T>
		inline core::string<StrT> rn_to_str(T value) {
			return core::string<StrT>(value);
		}

		template <typename StrT, typename T>
		inline core::string<StrT> vec2d_to_str(core::vector2d<T> value, core::string<StrT> delimiter = core::string<StrT>(", ")) {
			return core::string<StrT>("(") + rn_to_str<StrT>(value.X) + delimiter + rn_to_str<StrT>(value.Y) + core::string<StrT>(")");
		}

		template <typename StrT, typename T>
		inline core::string<StrT> vec3d_to_str(core::vector3d<T> value, core::string<StrT> delimiter = core::string<StrT>(", ")) {
			return core::string<StrT>("(") + rn_to_str<StrT>(value.X) + delimiter + rn_to_str<StrT>(value.Y) + delimiter + rn_to_str<StrT>(value.Z) + core::string<StrT>(")");
		}

		template <typename StrT, typename T>
		inline core::string<StrT> pos2d_to_str(core::position2d<T> value, core::string<StrT> delimiter = core::string<StrT>(", ")) {
			return vec2d_to_str<StrT>(value, delimiter);
		}

		template <typename StrT, typename T>
		inline core::string<StrT> dim2d_to_str(core::dimension2d<T> value, core::string<StrT> delimiter = core::string<StrT>(" x ")) {
			return rn_to_str<StrT>(value.Width) + delimiter + rn_to_str<StrT>(value.Height);
		}

		template <typename StrT, typename T>
		inline core::string<StrT> rect_to_str_UL_LR(core::rect<T> value, core::string<StrT> delimiter1 = core::string<StrT>(", "), core::string<StrT> delimiter2 = core::string<StrT>(", "), core::string<StrT> delimiter3 = core::string<StrT>(", ")) {
			return vec2d_to_str<StrT>(value.UpperLeftCorner, delimiter1) + delimiter2 + vec2d_to_str<StrT>(value.LowerRightCorner, delimiter3);
		}

		template <typename StrT, typename T>
		inline core::string<StrT> rect_to_str_Pos_Size(core::rect<T> value, core::string<StrT> delimiter1 = core::string<StrT>(", "), core::string<StrT> delimiter2 = core::string<StrT>(" - "), core::string<StrT> delimiter3 = core::string<StrT>(" x ")) {
			return pos2d_to_str<StrT>(value.UpperLeftCorner, delimiter1) + delimiter2 + dim2d_to_str(value.getSize(), delimiter3);
		}

		template <typename StrT, typename T>
		inline core::string<StrT> line2d_to_str_Pos_Size(core::line2d<T> value, core::string<StrT> delimiter1 = core::string<StrT>(", "), core::string<StrT> delimiter2 = core::string<StrT>(", "), core::string<StrT> delimiter3 = core::string<StrT>(", ")) {
			return vec2d_to_str<StrT>(value.start, delimiter1) + delimiter2 + vec2d_to_str<StrT>(value.end, delimiter3);
		}

		template <typename StrT, typename T>
		inline core::string<StrT> line3d_to_str_Pos_Size(core::line3d<T> value, core::string<StrT> delimiter1 = core::string<StrT>(", "), core::string<StrT> delimiter2 = core::string<StrT>(", "), core::string<StrT> delimiter3 = core::string<StrT>(", ")) {
			return vec3d_to_str<StrT>(value.start, delimiter1) + delimiter2 + vec3d_to_str<StrT>(value.end, delimiter3);
		}

		template <typename StrT, typename T>
		inline core::string<StrT> tri3d_to_str_Pos_Size(core::triangle3d<T> value, core::string<StrT> delimiter1 = core::string<StrT>(", "), core::string<StrT> delimiter2 = core::string<StrT>(", "), core::string<StrT> delimiter3 = core::string<StrT>(", "), core::string<StrT> delimiter4 = core::string<StrT>(", "), core::string<StrT> delimiter5 = core::string<StrT>(", ")) {
			return vec3d_to_str<StrT>(value.pointA, delimiter1) + delimiter2 + vec3d_to_str<StrT>(value.pointB, delimiter3) + delimiter4 + vec3d_to_str<StrT>(value.pointC, delimiter5);
		}
	}
}

#endif // !__CONVERTER_H_INCLUDED__