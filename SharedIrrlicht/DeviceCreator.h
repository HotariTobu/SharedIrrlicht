#pragma once

#ifndef __DEVICE_CREATOR_H_INCLUDED__
#define __DEVICE_CREATOR_H_INCLUDED__

#include "irrlicht.h"
#include "Converter.h"

namespace irr {
	namespace utils {
		inline IrrlichtDevice* getDevice() {
			core::dimension2du windowSize(640, 480);
			u32 bits = 16;
			bool fullScreen = false;
			bool stencilbuffer = false;
			bool vsync = false;

			IrrlichtDevice* nullDevice = createDevice(video::EDT_NULL);
			ILogger* logger = nullDevice->getLogger();
			io::IReadFile* file = nullDevice->getFileSystem()->createAndOpenFile("option.txt");
			if (file) {
				logger->log(R"(Succeed to open "option.txt")");

				u32 size = file->getSize();
				c8* buffer = new c8[size];
				size = file->read(buffer, size);
				core::stringc str(buffer);
				delete[] buffer;

				if (str.lastChar() != '\n') {
					str.append('\n');
				}

				core::array<core::stringc> lines;
				s32 lineCount = str.split(lines, "\n");
				for (s32 i = 0; i < lineCount; i++) {
					if (lines[i].empty() || lines[i].findFirst('#') != -1) {
						continue;
					}

					core::array<core::stringc> blocks;
					s32 blockCount = lines[i].split(blocks, "=");
					if (blockCount > 1) {
						core::stringc item = blocks[0].trim();
						core::stringc content = blocks[1].trim();

						logger->log(core::stringc("\t").append(item).c_str());
						core::stringc logText("\t\t");

						if (item.operator==("windowSize")) {
							core::array<core::stringc> values;
							s32 valueCount = content.split(values, "x");
							if (valueCount > 1) {
								windowSize = core::dimension2du(str_to_u32(values[0].trim()), str_to_u32(values[1].trim()));
							}

							logText += dim2d_to_str<c8>(windowSize);
						}
						else if (item.operator==("bits")) {
							bits = str_to_u32(content);

							logText += rn_to_str<c8>(bits);
						}
						else if (item.operator==("fullScreen")) {
							fullScreen = content.operator==("true");

							logText += bool_to_str<c8>(fullScreen);
						}
						else if (item.operator==("stencilbuffer")) {
							stencilbuffer = content.operator==("true");

							logText += bool_to_str<c8>(stencilbuffer);
						}
						else if (item.operator==("vsync")) {
							vsync = content.operator==("true");

							logText += bool_to_str<c8>(vsync);
						}
						else {
							continue;
						}

						logger->log(logText.c_str());
					}
				}

				if (fullScreen) {
					windowSize = nullDevice->getVideoModeList()->getDesktopResolution();
				}
				file->drop();

				logger->log(R"(Closed "option.txt")");
			}
			else {
				logger->log(R"(Failed to open "option.txt")");

				io::IWriteFile* newFile = nullDevice->getFileSystem()->createAndWriteFile("option.txt");
				if (newFile) {
					logger->log(R"(Succeed to create new "option.txt")");

					core::stringc lines;
					lines += "#This is a comment line.\n";
					lines += core::stringc("windowSize = ") + dim2d_to_str<c8>(windowSize) + "\n";
					lines += core::stringc("bits = ") + rn_to_str<c8>(bits) + "\n";
					lines += core::stringc("fullScreen = ") + bool_to_str<c8>(fullScreen) + "\n";
					lines += core::stringc("stencilbuffer = ") + bool_to_str<c8>(stencilbuffer) + "\n";
					lines += core::stringc("vsync = ") + bool_to_str<c8>(vsync) + "\n";

					newFile->write(lines.c_str(), lines.size());
					newFile->drop();

					logger->log(R"(Closed new "option.txt")");
				}
				else {
					logger->log(R"(Failed to create new "option.txt")");
				}
			}
			nullDevice->drop();

			return createDevice(video::EDT_OPENGL, windowSize, bits, fullScreen, stencilbuffer, vsync);
		}
	}
}

#endif // !__DEVICE_CREATOR_H_INCLUDED__