#include <enet/enet.h>
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifndef SMART_PACKET_H
#define SMART_PACKET_H

namespace irr
{
namespace net
{

/// An OutPacket class, used for sending packets.
class OutPacket
{

	public:
 
		OutPacket();
		OutPacket(enet_uint8 *buff,const int buff_size);
		~OutPacket(){}
				 
		/// Adds data to the packet.
		void addData(c8  data);
		/// Adds data to the packet.
		void addData(u16 data);
		/// Adds data to the packet.
		void addData(u32 data);
		/// Adds data to the packet.
		void addData(s8  data);
		/// Adds data to the packet.
		void addData(s16 data);
		/// Adds data to the packet.
		void addData(s32 data);
		/// Adds data to the packet.
		void addData(f32 data);
		/// Adds data to the packet.
		void addData(f64 data);
		/// Adds data to the packet.
		void addData(vector3df data);
		/// Clears the data in the packet. (Resets the write position)
		void clearData();
		/// Adds data to the packet.
		void addData(const char *string);

		/// Gets the data in the packet.
		c8* getData();
		/// Returns the size in bytes of the packet.
		u32 getSize();

				  
		static void Set_User(ENetHost *host){user = host;}
  
  private:
  
		u32 pos;
		c8 buff[1024];

		//set this to the server in the server and client in the client
		static ENetHost *user;
};

#endif
#ifndef INPACKET_H
#define INPACKET_H

/// An InPacket class, used for handling recieved packets.
class InPacket
{
  public:
		  
	    //init pos to 1, skipping the first byte  of buff, since its the header of the packet 
		InPacket(enet_uint8 *buff) : buff(buff),pos(0){}
		~InPacket(){}

		void setNewData(enet_uint8 *buffer);

		/// Gets the next item in the packet based on the size of the variable.
		void getNextItem(c8 &data);
		/// Gets the next item in the packet based on the size of the variable.
		void getNextItem(u16 &data);
		/// Gets the next item in the packet based on the size of the variable.
		void getNextItem(u32 &data);
		/// Gets the next item in the packet based on the size of the variable.
		void getNextItem(s8 &data);
		/// Gets the next item in the packet based on the size of the variable.
		void getNextItem(s16 &data);
		/// Gets the next item in the packet based on the size of the variable.
		void getNextItem(s32 &data);
		/// Gets the next item in the packet based on the size of the variable.
		void getNextItem(f32 &data);
		/// Gets the next item in the packet based on the size of the variable.
		void getNextItem(f64 &data);
		/// Gets the next item in the packet based on the size of the variable.
		void getNextItem(vector3df &data);
		/// Gets the next item in the packet based on the size of the variable.
		/// Please do not modify the header_size parameter, just leave it out
		/// unless you read thoroughly through the source code of the packets
		/// and know what you are doing.
		void getNextItem(char *string,const int header_size = 2);

		/// Resets the read position.
		void resetPos();

		/// Returns the player ID of the player that sent this packet. (Only for servers)
		u16 getPlayerId();
		void setPlayerId(u16 playernumber);

   private:
		 
		int pos;
		enet_uint8 *buff;  
		u16 playerid;
};

} // Close Net Namespace
} // Close Irr namespace

#endif
