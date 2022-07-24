#include "Packets.h"
#include <assert.h>

namespace irr
{
namespace net
{

ENetHost* OutPacket::user = NULL;

OutPacket::OutPacket()
{
  pos = 0;
}

//this ctor is for when i want to forward a packet usuaully
//i can just send this OP a enet_uint8 * which will fill in the data for me
OutPacket::OutPacket(enet_uint8 *buff,const int buff_size)
{
  pos = 0;
  memcpy(this->buff,buff,buff_size);
  pos+=buff_size;
}

void OutPacket::clearData()
{
	pos = 0;
}

c8* OutPacket::getData()
{
	return (c8*)buff;
}

u32 OutPacket::getSize()
{
	return pos;
}


void OutPacket::addData(c8 data)
{
  memcpy(buff + pos,&data,1);
  pos++;
}

void OutPacket::addData(u16 data)
{
  memcpy(buff + pos,&data,2);
  pos += 2;
}

void OutPacket::addData(u32 data)
{
  memcpy(buff + pos,&data,4);
  pos += 4;
}

void OutPacket::addData(s8 data)
{
  memcpy(buff + pos,&data,1);
  pos++;
}

void OutPacket::addData(s16 data)
{
  memcpy(buff + pos,&data,2);
  pos += 2;
}

void OutPacket::addData(s32 data)
{
  memcpy(buff + pos,&data,4);
  pos += 4;
}

void OutPacket::addData(f32 data)
{
  memcpy(buff + pos,&data,4);
  pos += 4;
}

void OutPacket::addData(f64 data)
{
  memcpy(buff + pos,&data,8);
  pos += 8;
}

void OutPacket::addData(vector3df data)
{
  memcpy(buff + pos,&data.X,4);
  pos += 4;
  memcpy(buff + pos,&data.Y,4);
  pos += 4;
  memcpy(buff + pos,&data.Z,4);
  pos += 4;
}

void OutPacket::addData(const char* string)
{
	//if(strlen(string) <= 255)
	// {
	//	 enet_uint8 tmp = (enet_uint8)strlen(string);
	//	 memcpy(buff+pos,&tmp,1);
	//	 pos++;

	//	 memcpy(buff+pos,string,strlen(string));
	//     pos += (int)strlen(string);
	// }
	//else
	//{
		enet_uint16 tmp = (enet_uint16)strlen(string);
		memcpy(buff+pos,&tmp,2);
		pos+=2;

		memcpy(buff+pos,string,strlen(string));
		pos+= (int)strlen(string);
	//}
}

void InPacket::getNextItem(c8 &data)
{
	memcpy(&data,buff+pos,1);
	pos++;
}


void InPacket::setNewData(enet_uint8 *buffer)
{
	buff = buffer;
	pos = 0;
}

void InPacket::setPlayerId(u16 playernumber)
{
	playerid = playernumber;
}

void InPacket::getNextItem(f32 &data)
{
	memcpy(&data,buff+pos,4);
	pos+=4;
}

void InPacket::getNextItem(f64 &data)
{
	memcpy(&data,buff+pos,8);
	pos+=8;
}

void InPacket::getNextItem(u16 &data)
{
	memcpy(&data,buff+pos,2);
	pos+=2;
}

void InPacket::getNextItem(u32 &data)
{
	memcpy(&data,buff+pos,4);
	pos+=4;
}

void InPacket::getNextItem(s8 &data)
{
	memcpy(&data,buff+pos,1);
	pos++;
}

void InPacket::getNextItem(s16 &data)
{
	memcpy(&data,buff+pos,2);
	pos+=2;
}

void InPacket::getNextItem(s32 &data)
{
	memcpy(&data,buff+pos,4);
	pos+=4;
}

void InPacket::getNextItem(vector3df &data)
{
	memcpy(&data.X,buff+pos,4);
	pos+=4;
	memcpy(&data.Y,buff+pos,4);
	pos+=4;
	memcpy(&data.Z,buff+pos,4);
	pos+=4;
}

void InPacket::getNextItem(char *string,const int header_size)
{
	//assert(header_size == 1 || header_size == 2);

	//if(header_size == 1)
	//{
	//	c8 sz;
	//	memcpy(&sz,buff+pos,1);
	//	pos++;

	//	memcpy(string,buff+pos,sz);
	//	pos+=sz;

	//	//dont forget to add newline
	//	string[sz] = '\0';
	//}
	//else 
	//{
		u16 sz;
		memcpy(&sz,buff+pos,2);
		pos+=2;

		memcpy(string,buff+pos,sz);
		pos+=sz;

		//dont forget to add newline!!!
		string[sz] = '\0';
	//}
}

u16 InPacket::getPlayerId()
{
	return playerid;
}

void InPacket::resetPos()
{
	pos = 0;
}

} // Close Net Namespace
} // Close Irr namespace