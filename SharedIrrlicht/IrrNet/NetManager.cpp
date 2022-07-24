#include "NetAnim.h"

/*
List of Packet Identifiers:
3 - Connect Info Request
4 - PlayerID
5 - sendPosition
6 - sendRotation
7 - sendScale
8 - sendFrame
9 - sendVisible
10 - sendVelocity
11 - removeAnimators
12 - NodeCountRequest
13 - CustomVar
14 - Change in CustomVar
15 - Make a Cube
16 - Make a Sphere
17 - Make a custom node
18 - send file request
19 - send file accept/decline
20 - send file data
21 - sendFrameState
25 - Update CustomVar
30 - Remove Node
*/




namespace irr
{
	namespace net
	{

		AutoAnim* NetAnim[10000];

		NodeType NodeTypes[100]; 

		INetManager::INetManager(irr::IrrlichtDevice* irrdev)
			: device(irrdev)
		{
			device = irrdev;
			printf_s("Initializing Enet!\n");
			if (enet_initialize() != 0)
			{
				printf_s ("An error occurred while initializing ENet.\n");
			}
			verbose = 0;
			established = 0;
			firstupdate = 1;
			customops = 0;
			defaulthandling = 1;
			customhandling = 0;
			customconnect = 0;
			inpacket = new InPacket(0);
			recievefiles = 1;
			filePriority = 5;
			netIterations = 20;
			PacketIterator = 0;

			//device->getTimer()->stop();
			//device->getTimer()->setTime(0);
			//device->getTimer()->start();

		}



		INetManager::~INetManager(void)
		{
			enet_deinitialize();
		}

		void INetManager::setUpClient()
		{
			isserver = 0;
			
			core::stringc ipaddress;
			u32 port = 0;

			if(customconnect)
			{
				ipaddress = customaddress;
				port = customport;
			}
			else
			{
			IXMLReader* xml = device->getFileSystem()->createXMLReader("ip.xml");
			while(xml && xml->read())
			{ 
				if (core::stringw("config") == xml->getNodeName())
					ipaddress = xml->getAttributeValue(L"address");
				port = xml->getAttributeValueAsInt(L"port");
			}
			}

			printf("Creating client!\n");
			host = enet_host_create(NULL, 1,57600 / 8, 14400 / 8);

			ENetEvent event;

			/* Connect to some.server.net:1234. */
			//printf("Please enter the ip address.\n");
			//std::cin >> i;
			enet_address_set_host (& address, ipaddress.c_str());
			address.port = port;

			/* Initiate the connection, allocating the two channels 0 and 1. */
			printf("Connecting to ");
			printf(ipaddress.c_str());
			printf(":%d\n",port);
			peer = enet_host_connect (host, & address, 2);    

			if (peer == NULL)
			{
				fprintf (stderr, 
					"No available peers for initiating an ENet connection.\n");
				exit (EXIT_FAILURE);
			}

			/* Wait up to 5 seconds for the connection attempt to succeed. */
			if (enet_host_service(host, & event, 5000) > 0 &&
				event.type == ENET_EVENT_TYPE_CONNECT)
			{
				printf("Connection succeeded.\n");

			}
			else
			{
				/* Either the 5 seconds are up or a disconnect event was */
				/* received. Reset the peer in the event the 5 seconds   */
				/* had run out without any significant event.            */
				enet_peer_reset (peer);

				printf ("Connection failed.\n");
			}
			//c8 buffer[2];
			//u8 packid = 4;
			//memcpy(buffer,(char*)&packid,1);
			//ENetPacket * packet;
			//packet = enet_packet_create (buffer,1,ENET_PACKET_FLAG_RELIABLE);
			//enet_host_broadcast (host, 0, packet);
			if (enet_host_service(host, & event, 5000) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE)
			{
				buff = event.packet->data;
				u8 packiden = 0;
				memcpy((char*)&packiden,buff,1);
				if (packiden == 4){
					memcpy((char*)&playernumber,buff+1,2);
					printf ("Player ID is %d\n", playernumber);
				}
			}
			else
			{
				printf ("Failed to recieve Player ID.\n");
			}
		}







		void INetManager::updateClient()
		{

			if(firstupdate)
			{
				c8 buffer[2];
				ENetPacket * packet;
				u8 packid = 3;
				memcpy(buffer,(char*)&packid,1);
				packet = enet_packet_create (buffer,1,ENET_PACKET_FLAG_RELIABLE);
				enet_host_broadcast (host, 0, packet);
				firstupdate = 0;
			}

			ENetEvent event;
			//if(device->getTimer()->getTime() > (timeset + 10))
			//{
			for(PacketIterator = 0; PacketIterator < netIterations; PacketIterator++)
			{
				if(!(enet_host_service (host, & event, 1) > 0))
				break;
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					printf ("A new client connected from %x:%u.\n", 
						event.peer -> address.host,
						event.peer -> address.port);

					/* Store any relevant client information here. */
					event.peer -> data = "Server";
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					{
						if(verbose)
						{
							printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
								event.packet -> dataLength,
								event.packet -> data,
								event.peer -> data,
								event.channelID);
						}

						buff = event.packet->data;

						if(defaulthandling)
						{

						

						packiden = 0;
						memcpy((char*)&packiden,buff,1);
						currentpackiden = packiden;
						
						if(packiden == 3){established = 1;} 

						if(packiden == 5){
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							//if(originarray[netid] == 0)
							//{
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 3,4);
							memcpy((char*)&floaty,buff + 7,4);
							memcpy((char*)&floatz,buff + 11,4);
							if(verbose)
								printf("Packet conversion: %d %d %f x %f x %f",packiden,netid,floatx,floaty,floatz);
							if(nodearray[netid] > 0)
								device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->setPosition(vector3df(floatx,floaty,floatz));
							//}
						}
						if(packiden == 6){
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							//if(originarray[netid] == 0)
							//{
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 3,4);
							memcpy((char*)&floaty,buff + 7,4);
							memcpy((char*)&floatz,buff + 11,4);
							if(verbose)
								printf("Packet conversion: %d %d %f x %f x %f",packiden,netid,floatx,floaty,floatz);
							if(nodearray[netid] > 0)
								device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->setRotation(vector3df(floatx,floaty,floatz));
							//}
						}
						if(packiden == 7){
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							//if(originarray[netid] == 0)
							//{
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 3,4);
							memcpy((char*)&floaty,buff + 7,4);
							memcpy((char*)&floatz,buff + 11,4);
							if(verbose)
								printf("Packet conversion: %d %d %f x %f x %f",packiden,netid,floatx,floaty,floatz);
							if(nodearray[netid] > 0)
								device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->setScale(vector3df(floatx,floaty,floatz));
							//}
						}
						if(packiden == 8){
							u16 netid = 0;
							u32 frameno;
							memcpy((char*)&netid,buff+1,2);
							//if(originarray[netid] == 0)
							//{
							memcpy((char*)&frameno,buff + 3,4);
							//frameno -= 1441000;
							if(verbose)
								printf("Packet conversion: %d %d %d",packiden,netid,frameno);
							if(nodearray[netid] > 0)
							{
								tmpnode = dynamic_cast<IAnimatedMeshSceneNode*>(device->getSceneManager()->getSceneNodeFromId(nodearray[netid]));
								//if(tmpnode)
								tmpnode->setCurrentFrame(frameno);	
							}
							//}
						}

						if(packiden == 9)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							u8 isvisible;
							memcpy((char*)&isvisible,buff + 3,1);
							if(verbose)
								printf("Packet conversion: %d %d",packiden,netid,isvisible);
							if(nodearray[netid] > 0)
								device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->setVisible(isvisible);
						}

						if(packiden == 10)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 3,4);
							memcpy((char*)&floaty,buff + 7,4);
							memcpy((char*)&floatz,buff + 11,4);
							if(verbose)
								printf("Packet conversion: %d %d %f x %f x %f",packiden,netid,floatx,floaty,floatz);
							//AutoVel* AutoVelocity = new AutoVel(vector3df(floatx,floaty,floatz),device->getSceneManager());
							//device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->removeAnimators();
							//device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->addAnimator(AutoVelocity);
							if(NetAnim[netid] > 0)
								NetAnim[netid]->updateVel(1,vector3df(floatx,floaty,floatz));
						}

						if(packiden == 11)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							if(verbose)
								printf("Packet conversion: %d %d",packiden,netid);
							if(nodearray[netid] > 0)
								device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->removeAnimators();
						}

						if(packiden == 13)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							u16 varid = 0;
							memcpy((char*)&varid,buff+3,2);
							u32 varvalue = 0;
							memcpy((char*)&varvalue,buff+5,4);
							if(verbose)
								printf("Packet conversion: %d %d %d %d",packiden,netid,varid,varvalue);
							if(NetAnim[netid] > 0)
								NetAnim[netid]->setVar(varid,varvalue);
						}

						if(packiden == 14)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							u16 varid = 0;
							memcpy((char*)&varid,buff+3,2);
							u32 varvalue = 0;
							memcpy((char*)&varvalue,buff+5,4);
							if(verbose)
								printf("Packet conversion: %d %d %d %d",packiden,netid,varid,varvalue);
							if(NetAnim[netid] > 0)
								NetAnim[netid]->setVar(varid,NetAnim[netid]->getVar(varid) + varvalue);
						}

							if(packiden == 25)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							if(ownedby[netid] == getPlayerNumber())
							{
							u16 varid = 0;
							memcpy((char*)&varid,buff+3,2);
							c8 selectedProcess = 0;
							memcpy((char*)&selectedProcess,buff+5,1);
							
							if(selectedProcess)
							{
								u16 interval = 0;
								memcpy((char*)&interval,buff+6,2);
								sendCustomVarAuto(getSceneNodeFromNetId(netid),varid,interval);
							}
							else
							{
								sendCustomVar(getSceneNodeFromNetId(netid),varid,getCustomVar(getSceneNodeFromNetId(netid),varid));
							}
							}
						}

						if(packiden == 30)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
						
							getSceneNodeFromNetId(netid)->removeAnimators();
							getSceneNodeFromNetId(netid)->setVisible(false);
							nodearray[netid] = 0;
							nodetype[netid] = 0;
							originarray[netid] = 0;
							ownedby[netid] = 0;
							if(NetAnim[netid] != NULL)
							NetAnim[netid]->drop();
							NetAnim[netid] = 0;
						}

						if(packiden == 15)
						{

							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							if(!(nodearray[netid] > 0))
							{
								f32 size = 0;
								memcpy((char*)&size,buff+3,4);
								f32 floatx = 0;
								f32 floaty = 0;
								f32 floatz = 0;
								memcpy((char*)&floatx,buff + 7,4);
								memcpy((char*)&floaty,buff + 11,4);
								memcpy((char*)&floatz,buff + 15,4);
								vector3df position(floatx,floaty,floatz);
								memcpy((char*)&floatx,buff + 19,4);
								memcpy((char*)&floaty,buff + 23,4);
								memcpy((char*)&floatz,buff + 27,4);
								vector3df rotation(floatx,floaty,floatz);
								memcpy((char*)&floatx,buff + 31,4);
								memcpy((char*)&floaty,buff + 35,4);
								memcpy((char*)&floatz,buff + 39,4);
								vector3df scale(floatx,floaty,floatz);
								tmpstatnode = device->getSceneManager()->addCubeSceneNode(size,0,netid,position,rotation,scale);
								setNetId(tmpstatnode,netid);
								if(verbose)
									nodetype[netid] = 1001;
								printf("\n Net Node Created: %d %d Position: %f x %f x %f Rotation: %f x %f x %f Scale: %f x %f x %f \n",
									packiden,netid,position.X,position.Y,position.Z,rotation.X,rotation.Y,rotation.Z,scale.X,scale.Y,scale.Z);
							}
							else
							{
								printf("\n Duplicate Node Recieved \n");
							}

						}


						if(packiden == 16)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							if(!(nodearray[netid] > 0))
							{
								nodetype[netid] = 1001;
								f32 radius = 0;
								memcpy((char*)&radius,buff+3,4);
								cubesize[netid] = radius;
								f32 floatx = 0;
								f32 floaty = 0;
								f32 floatz = 0;
								memcpy((char*)&floatx,buff + 7,4);
								memcpy((char*)&floaty,buff + 11,4);
								memcpy((char*)&floatz,buff + 15,4);
								vector3df position(floatx,floaty,floatz);
								memcpy((char*)&floatx,buff + 19,4);
								memcpy((char*)&floaty,buff + 23,4);
								memcpy((char*)&floatz,buff + 27,4);
								vector3df rotation(floatx,floaty,floatz);
								memcpy((char*)&floatx,buff + 31,4);
								memcpy((char*)&floaty,buff + 35,4);
								memcpy((char*)&floatz,buff + 39,4);
								vector3df scale(floatx,floaty,floatz);
								s32 polycount = 0;
								memcpy((char*)&polycount,buff + 43,4);
								polyarray[netid] = polycount;
								tmpstatnode = device->getSceneManager()->addSphereSceneNode(radius,polycount,0,netid,position,rotation,scale);
								setNetId(tmpstatnode,netid);
								nodetype[netid] = 1002;
								if(verbose)
									printf("Net Node Created: %d %d Position: %f x %f x %f Rotation: %f x %f x %f Scale: %f x %f x %f Type: 1002",
									packiden,netid,position.X,position.Y,position.Z,rotation.X,rotation.Y,rotation.Z,scale.X,scale.Y,scale.Z);
							}
							else
							{
								printf("\n Duplicate Node Recieved \n");
							}

						}

						if(packiden == 17)
						{
							u16 netid = 0;
							if(!(nodearray[netid] > 0))
							{
								u16 ntype = 0;
								memcpy((char*)&netid,buff+1,2);
								memcpy((char*)&ntype,buff+3,2);
								nodetype[netid] = ntype;
								f32 floatx = 0;
								f32 floaty = 0;
								f32 floatz = 0;
								memcpy((char*)&floatx,buff + 5,4);
								memcpy((char*)&floaty,buff + 9,4);
								memcpy((char*)&floatz,buff + 13,4);
								vector3df position(floatx,floaty,floatz);
								memcpy((char*)&floatx,buff + 17,4);
								memcpy((char*)&floaty,buff + 21,4);
								memcpy((char*)&floatz,buff + 25,4);
								vector3df rotation(floatx,floaty,floatz);
								memcpy((char*)&floatx,buff + 29,4);
								memcpy((char*)&floaty,buff + 33,4);
								memcpy((char*)&floatz,buff + 37,4);
								vector3df scale(floatx,floaty,floatz);

								tmpnode = device->getSceneManager()->addAnimatedMeshSceneNode(NodeTypes[ntype].mesh,0,netid,position,rotation,scale);
								if(NodeTypes[ntype].animator)
									tmpnode->addAnimator(NodeTypes[ntype].animator);
								tmpnode->getMaterial(0) = NodeTypes[ntype].material;
								setNetId(tmpnode,netid);
								nodetype[netid] = ntype;

								if(NodeTypes[ntype].customop)
								NodeTypes[ntype].customop->Operations(tmpnode,device);

								setFrameRange(tmpnode,NodeTypes[ntype].nodeframerange);

							if(customops){customoperations->Operations(tmpnode, device);}

								if(verbose)
									printf("Net Node Created: %d %d Position: %f x %f x %f Rotation: %f x %f x %f Scale: %f x %f x %f Type: 1002",
									packiden,netid,position.X,position.Y,position.Z,rotation.X,rotation.Y,rotation.Z,scale.X,scale.Y,scale.Z);
							}
							else
							{
								if(verbose)
									printf("\n Duplicate Node Recieved \n");
							}

						
						}


	
						if(packiden == 18)
						{
							InPacket* filepacket = new InPacket(buff);
							filepacket->getNextItem((c8 &)packiden);
							s32 filesize;
							filepacket->getNextItem(filesize);
							c8 fileid;
							filepacket->getNextItem(fileid);
							c8 filename[255];
							filepacket->getNextItem((char*)filename);
							bool filexists = device->getFileSystem()->existFile(filename);
							OutPacket* fileresponse = createOutPacket();
							fileresponse->addData((c8)19);
							c8 i = 0;
							while(writefileuse[i] > 0 && i <= 100)
							{
								i++;
							}
							if(filexists == 1 || i > 100)
							{
								fileresponse->addData((c8)0);
							}
							else
							{
							fileresponse->addData((c8)1);
							writefileuse[i] = 1;
							writefile[i] = device->getFileSystem()->createAndWriteFile(filename);
							writefilesize[i] = filesize;
							writefilepos[i] = 0;
							}
							fileresponse->addData((c8)fileid);
							fileresponse->addData((c8)i);
							
							sendOutPacket(fileresponse);
						}

						if(packiden == 19)
						{
							InPacket* filepacket = new InPacket(buff);
							filepacket->getNextItem((c8 &)packiden);
							c8 usefile;
							filepacket->getNextItem(usefile);
							if(usefile == 1)
							{
							c8 fileid;
							filepacket->getNextItem((c8)fileid);
							filepacket->getNextItem((c8)readfileid[fileid]);
							readfileuse[fileid] = 3;
							}
						}

						if(packiden == 21)
						{
							InPacket* filepacket = new InPacket(buff);
							filepacket->getNextItem((c8 &)packiden);
							u16 nodeid;
							filepacket->getNextItem(nodeid);
							c8 framestate;
							filepacket->getNextItem(framestate);
							if(nodearray[nodeid] > 0)
							{
							tmpnode = dynamic_cast<IAnimatedMeshSceneNode*>(getSceneNodeFromNetId(nodeid));
							tmpnode->setFrameLoop(NetFrameRanges[nodeid].framestart[framestate],NetFrameRanges[nodeid].frameend[framestate]);
							if(NetFrameRanges[nodeid].framespeed[framestate] > 0)
							tmpnode->setAnimationSpeed(NetFrameRanges[nodeid].framespeed[framestate]);
							tmpnode->setLoopMode(NetFrameRanges[nodeid].frameloop[framestate]);
							}
						}



						if(recievefiles && packiden == 20)
						{
							//enet_uint8 newbuff[200];
							//memset(newbuff,0,sizeof(newbuff));
							//memcpy((char*)newbuff,(char*)buff,sizeof(buff));
							//packetbuffer.push_back(newbuff);	
							InPacket* filepacket = new InPacket(buff);
							filepacket->getNextItem((c8 &)packiden);
							printf("\n Packet Info: Iden: %d",packiden);

							c8 usefile;
							filepacket->getNextItem(usefile);
							printf(" File ID: %d ",usefile);
							
							u32 filepos;
							filepacket->getNextItem((u32)filepos);
							printf("File Pos: %d ",filepos);	

							
							filepacket->getNextItem((char*)filecontents);
							//printf("\n\n File Packet Contents:");
							//printf(filecontents);
							//printf("\n\n");

							// Make a new File Packet struct and input the information we got from the packet.
							SFilePacket FPacket;
							FPacket.fileId = usefile;
							FPacket.readPos = filepos;
							memcpy((char*)FPacket.fileData,(char*)filecontents,sizeof(filecontents));
	
							// Chuck the struct into the file packet buffer.
							packetbuffer.push_back(FPacket);
						}
						
						
						}

						if(customhandling)
						{

						inpacket->setNewData(buff);
						inpacket->setPlayerId((u16)event.peer->data);

						if(customhandler)
							customhandler->Handle(inpacket);
						}

						/* Clean up the packet now that we're done using it. */
						buff = 0;

						enet_packet_destroy (event.packet);

						break;
					}
				case ENET_EVENT_TYPE_DISCONNECT:
					printf ("%s disconected.\n", event.peer -> data);

					/* Reset the peer's client information. */

					event.peer -> data = NULL;
				}


			}

			// File Handler
						
					int i = 0;
					int z = 0;
							while(i <= 100)
							{
								z = 0;
								while(z < filePriority)
								{
									if(readfileuse[i] == 3)
									{
										c8 filebuffer[FILEPACKETSIZE];
										memset((char*)filebuffer,0,sizeof(filebuffer));

										OutPacket* filedata = createOutPacket();
										filedata->addData((c8)20);
										filedata->addData((c8)readfileid[i]);
										filedata->addData((u32)readfile[i]->getPos());
										if(readfile[i]->getSize() - readfile[i]->getPos() < FILEPACKETSIZE)
										{
											readfile[i]->read(filebuffer,readfile[i]->getSize() - readfile[i]->getPos());
											//readfile[i]->seek(readfile[i]->getSize() - readfile[i]->getPos(), true);
										}
										else
										{
											readfile[i]->read(filebuffer,FILEPACKETSIZE);
											//readfile[i]->seek(FILEPACKETSIZE, true);
										}
										filedata->addData((char*)filebuffer);
										//if(readfilepid[i] == 0)
										sendOutPacket(filedata);
										//else
										//sendOutPacket(filedata,readfilepid[i]);
										if(readfile[i]->getPos() >= readfile[i]->getSize())
										{
											readfileuse[i] = 0;
											readfile[i]->drop();
										}
									}
									z++;
								}
								i++;
							}






							i = 0;
							while(i <= 100)
							{
								if(writefileuse[i] == 1)
								{
									z = 0;
									while(z < packetbuffer.size())
									{
										if(packetbuffer[z].fileId == i)
										{
										printf("\n\n PacketPos%d \n",packetbuffer[z].readPos);
										printf(" Z:%d \n",z);
										printf(" FilePos :%d \n\n",writefilepos[i]);
										if(packetbuffer[z].readPos == writefilepos[i])
										{
											if((s32)(writefilesize[i] - writefilepos[i]) < FILEPACKETSIZE)
											{
											writefile[i]->write(packetbuffer[z].fileData,writefilesize[i] - writefilepos[i]);
											writefilepos[i] += writefilesize[i] - writefilepos[i];
											}
											else
											{
											writefile[i]->write(packetbuffer[z].fileData,FILEPACKETSIZE);
											writefilepos[i] += FILEPACKETSIZE;
											}
											packetbuffer.erase(z);
											z--;
											if(writefilesize[i] == writefilepos[i])
											{
												writefile[i]->drop();
												writefileuse[i] = 0;
												writefilepos[i] = 0;
											}
										}
										}
										
										z++;
									}
									
								}
							i++;
							}
			//int timeset = device->getTimer()->getTime();
			//}
		}









		void INetManager::setUpServer()
		{
			isserver = 1;
			/* Bind the server to the default localhost.     */
			/* A specific host address can be specified by   */
			/* enet_address_set_host (& address, "x.x.x.x"); */

			
			u32 port = 0;

			if(customconnect)
			{
				port = customport;
				printf("Setting server address to ");
				printf(customaddress.c_str());
				printf("\n");
				enet_address_set_host(&address, customaddress.c_str());
			}
			else
			{
			IXMLReader* xml = device->getFileSystem()->createXMLReader("ip.xml");
			while(xml && xml->read())
			{ 
				if (core::stringw("config") == xml->getNodeName())
				port = xml->getAttributeValueAsInt(L"port");
			}
			printf("Setting server address to 127.0.0.1\n");
			enet_address_set_host(&address, "127.0.0.1");
			}

			

			address.host = ENET_HOST_ANY;
			/* Bind the server to port 1234. */
			address.port = port;



			printf("Creating server!\n");
			host = enet_host_create (& address /* the address to bind the server host to */,
				90      /* allow up to 32 clients and/or outgoing connections */,
				0     /* assume any amount of incoming bandwidth */,
				0      /* assume any amount of outgoing bandwidth */);
			if (host == NULL)
			{
				printf ("An error occurred while trying to create an ENet server host.\n");

				exit (EXIT_FAILURE);
			} 

		}




		void INetManager::updateServer()
		{

			ENetEvent event;

			//if(device->getTimer()->getTime() > (timeset + 10))
			//{


			
			for(PacketIterator = 0; PacketIterator < netIterations; PacketIterator++)
			{
				if(!(enet_host_service (host, & event, 1) > 0))
				break;
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					{
						printf ("A new client connected from %x:%u.\n", 
							event.peer -> address.host,
							event.peer -> address.port);

						/* Store any relevant client information here. */
						u16 i = 1;
						while(players[i] == 1)
						{
							i++;
						}
						players[i] = 1;
						event.peer -> data = (void*)i;
						c8 buffer[3];
						u8 packid = 4;
						memcpy(buffer,(char*)&packid,1);
						memcpy(buffer+1,(char*)&i,2);
						ENetPacket * packet;
						packet = enet_packet_create (buffer,3,ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send (event.peer, 0, packet);
						enet_host_flush (host);
						break;
					}





				case ENET_EVENT_TYPE_RECEIVE:
					{
						if(verbose)
						{
							printf ("A packet of length %u containing %s was received from %d on channel %u.\n",
								event.packet -> dataLength,
								event.packet -> data,
								event.peer -> data,
								event.channelID);
						}

						
						buff = event.packet->data;

						packiden = 0;

						if(defaulthandling)
						{
					
						memcpy((char*)&packiden,buff,1);
						currentpackiden = packiden;



						if(packiden == 3)
						{
							ENetPacket * packet;
							u16 i = 0;
							c8 buffer[64];
							u8 packid = 0;

							while(i <= 10000)
							{
								if((nodearray[i] > 0) && !((nodearray[i] >= ((u16)event.peer->data * 100) + 1000) && (nodearray[i] < ((u16)event.peer->data * 100) + 1100)))
								{
									printf("\n \n %d \n \n",i);
									if(nodetype[i] == 1001)
									{
										packid = 15;
										memcpy(buffer,(char*)&packid,1);
										memcpy(buffer+1,(char*)&i,2);
										f32 size = cubesize[i];
										memcpy(buffer+3,(char*)&size,4);
										tmpvector = device->getSceneManager()->getSceneNodeFromId(nodearray[i])->getPosition();
										tmpfloat = tmpvector.X;
										memcpy(buffer+7,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Y;
										memcpy(buffer+11,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Z;
										memcpy(buffer+15,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpvector = device->getSceneManager()->getSceneNodeFromId(nodearray[i])->getRotation();
										tmpfloat = tmpvector.X;
										memcpy(buffer+19,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Y;
										memcpy(buffer+23,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Z;
										memcpy(buffer+27,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpvector = device->getSceneManager()->getSceneNodeFromId(nodearray[i])->getScale();
										tmpfloat = tmpvector.X;
										memcpy(buffer+31,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Y;
										memcpy(buffer+35,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Z;
										memcpy(buffer+39,(char*)&tmpfloat,sizeof(tmpfloat));
										packet = enet_packet_create (buffer,43,ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(event.peer,1,packet);
									}
									if(nodetype[i] == 1002)
									{
										packid = 16;
										memcpy(buffer,(char*)&packid,1);
										memcpy(buffer+1,(char*)&i,2);
										memcpy(buffer+3,(char*)&cubesize[i],4);
										tmpvector = device->getSceneManager()->getSceneNodeFromId(nodearray[i])->getPosition();
										tmpfloat = tmpvector.X;
										memcpy(buffer+7,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Y;
										memcpy(buffer+11,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Z;
										memcpy(buffer+15,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpvector = device->getSceneManager()->getSceneNodeFromId(nodearray[i])->getRotation();
										tmpfloat = tmpvector.X;
										memcpy(buffer+19,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Y;
										memcpy(buffer+23,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Z;
										memcpy(buffer+27,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpvector = device->getSceneManager()->getSceneNodeFromId(nodearray[i])->getScale();
										tmpfloat = tmpvector.X;
										memcpy(buffer+31,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Y;
										memcpy(buffer+35,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Z;
										memcpy(buffer+39,(char*)&tmpfloat,sizeof(tmpfloat));
										memcpy(buffer+43,(char*)&polyarray[i],4);
										packet = enet_packet_create (buffer,47,ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(event.peer,1,packet);
									}
									if(nodetype[i] <= 1000)
									{
										packid = 17;
										memcpy(buffer,(char*)&packid,1);
										memcpy(buffer+1,(char*)&i,2);
										memcpy(buffer+3,(char*)&nodetype[i],2);
										tmpvector =  device->getSceneManager()->getSceneNodeFromId(nodearray[i])->getPosition();
										tmpfloat = tmpvector.X;
										memcpy(buffer+5,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Y;
										memcpy(buffer+9,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Z;
										memcpy(buffer+13,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpvector =  device->getSceneManager()->getSceneNodeFromId(nodearray[i])->getRotation();
										tmpfloat = tmpvector.X;
										memcpy(buffer+17,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Y;
										memcpy(buffer+21,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Z;
										memcpy(buffer+25,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpvector =  device->getSceneManager()->getSceneNodeFromId(nodearray[i])->getScale();
										tmpfloat = tmpvector.X;
										memcpy(buffer+29,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Y;
										memcpy(buffer+33,(char*)&tmpfloat,sizeof(tmpfloat));
										tmpfloat = tmpvector.Z;
										memcpy(buffer+37,(char*)&tmpfloat,sizeof(tmpfloat));

										packet = enet_packet_create (buffer,41,ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(event.peer,1,packet);
										enet_host_flush (host);

										if(getFrameRange((IAnimatedMeshSceneNode *)getSceneNodeFromNetId(i)).currentframestate != -1)
										sendFrameState((IAnimatedMeshSceneNode *)getSceneNodeFromNetId(i),getFrameRange((IAnimatedMeshSceneNode *)getSceneNodeFromNetId(i)).currentframestate);

									}


								}
								i++;
							}

							packid = 3;
							memcpy(buffer,(char*)&packid,1);
							packet = enet_packet_create (buffer,1,ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(event.peer,1,packet);
							established = 1;
						}

							


						if(packiden == 5){
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 3,4);
							memcpy((char*)&floaty,buff + 7,4);
							memcpy((char*)&floatz,buff + 11,4);
							if(verbose)
								printf("Packet conversion: %d %d %f x %f x %f",packiden,netid,floatx,floaty,floatz);
							if(nodearray[netid] > 0)
							{
								device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->setPosition(vector3df(floatx,floaty,floatz));
							}
						}
						if(packiden == 6){
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 3,4);
							memcpy((char*)&floaty,buff + 7,4);
							memcpy((char*)&floatz,buff + 11,4);
							if(verbose)
								printf("Packet conversion: %d %d %f x %f x %f",packiden,netid,floatx,floaty,floatz);
							if(nodearray[netid] > 0)
							{
								device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->setRotation(vector3df(floatx,floaty,floatz));
							}
						}
						if(packiden == 7){
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 3,4);
							memcpy((char*)&floaty,buff + 7,4);
							memcpy((char*)&floatz,buff + 11,4);
							if(verbose)
								printf("Packet conversion: %d %d %f x %f x %f",packiden,netid,floatx,floaty,floatz);
							if(nodearray[netid] > 0)
							{
								device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->setScale(vector3df(floatx,floaty,floatz));
							}
						}
						if(packiden == 8){
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							u32 frameno;
							memcpy((char*)&frameno,buff + 3,4);
							if(verbose)
								printf("Packet conversion: %d %d %d",packiden,netid,(s32)frameno);
							if(nodearray[netid] > 0)
							{
								tmpnode = dynamic_cast<IAnimatedMeshSceneNode*>(device->getSceneManager()->getSceneNodeFromId(nodearray[netid]));
								tmpnode->setCurrentFrame(frameno);	
							}
						}

						if(packiden == 9)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							u8 isvisible;
							memcpy((char*)&isvisible,buff + 3,1);
							if(verbose)
								printf("Packet conversion: %d %d",packiden,netid,isvisible);
							if(nodearray[netid] > 0)
							{
								device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->setVisible(isvisible);
							}
						}

						if(packiden == 10)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 3,4);
							memcpy((char*)&floaty,buff + 7,4);
							memcpy((char*)&floatz,buff + 11,4);
							if(verbose)
								printf("Packet conversion: %d %d %f x %f x %f",packiden,netid,floatx,floaty,floatz);
							//AutoVel* AutoVelocity = new AutoVel(vector3df(floatx,floaty,floatz),device->getSceneManager());
							//device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->removeAnimators();
							//device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->addAnimator(AutoVelocity);
							if(nodearray[netid] > 0)
							{
								NetAnim[netid]->updateVel(1,vector3df(floatx,floaty,floatz));
							}
						}

						if(packiden == 11)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							if(verbose)
								printf("Packet conversion: %d %d",packiden,netid);
							if(nodearray[netid] > 0)
							{
								device->getSceneManager()->getSceneNodeFromId(nodearray[netid])->removeAnimators();
							}
						}

						if(packiden == 12)
						{
							if(verbose)
								printf("Packet conversion: %d",packiden);
							u16 i = ((u16)event.peer->data * 100) + 1000;
							while(nodearray[i] > 0)
							{
								i++;
							}
							c8 buffer[3];
							u8 packid = 12;
							memcpy(buffer,(char*)&packid,1);
							memcpy(buffer+1,(char*)&i,2);
							ENetPacket * packet;
							packet = enet_packet_create (buffer,3,ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send (event.peer, 0, packet);
							break;
						}

						if(packiden == 13)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							u16 varid = 0;
							memcpy((char*)&varid,buff+3,2);
							u32 varvalue = 0;
							memcpy((char*)&varvalue,buff+5,4);
							if(verbose)
								printf("Packet conversion: %d %d %d %d",packiden,netid,varid,varvalue);
							if(nodearray[netid] > 0)
							{
								NetAnim[netid]->setVar(varid,varvalue);
							}
						}

						if(packiden == 14)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							u16 varid = 0;
							memcpy((char*)&varid,buff+3,2);
							u32 varvalue = 0;
							memcpy((char*)&varvalue,buff+5,4);
							if(verbose)
								printf("Packet conversion: %d %d %d %d",packiden,netid,varid,varvalue);
							if(NetAnim[netid] > 0)
								NetAnim[netid]->setVar(varid,NetAnim[netid]->getVar(varid) + varvalue);
						}

						if(packiden == 25)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							if(ownedby[netid] == getPlayerNumber())
							{
							u16 varid = 0;
							memcpy((char*)&varid,buff+3,2);
							c8 selectedProcess = 0;
							memcpy((char*)&selectedProcess,buff+5,1);
							
							if(selectedProcess)
							{
								u16 interval = 0;
								memcpy((char*)&interval,buff+6,2);
								sendCustomVarAuto(getSceneNodeFromNetId(netid),varid,interval);
							}
							else
							{
								sendCustomVar(getSceneNodeFromNetId(netid),varid,getCustomVar(getSceneNodeFromNetId(netid),varid));
							}
							}
						}

						if(packiden == 30)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
						
							getSceneNodeFromNetId(netid)->removeAnimators();
							getSceneNodeFromNetId(netid)->setVisible(false);
							nodearray[netid] = 0;
							nodetype[netid] = 0;
							originarray[netid] = 0;
							ownedby[netid] = 0;
							if(NetAnim[netid] != NULL)
							NetAnim[netid]->drop();
							NetAnim[netid] = 0;
						}

						if(packiden == 15)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							nodetype[netid] = 1001;
							f32 size = 0;
							memcpy((char*)&size,buff+3,4);
							cubesize[netid] = size;
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 7,4);
							memcpy((char*)&floaty,buff + 11,4);
							memcpy((char*)&floatz,buff + 15,4);
							vector3df position(floatx,floaty,floatz);
							memcpy((char*)&floatx,buff + 19,4);
							memcpy((char*)&floaty,buff + 23,4);
							memcpy((char*)&floatz,buff + 27,4);
							vector3df rotation(floatx,floaty,floatz);
							memcpy((char*)&floatx,buff + 31,4);
							memcpy((char*)&floaty,buff + 35,4);
							memcpy((char*)&floatz,buff + 39,4);
							vector3df scale(floatx,floaty,floatz);
							tmpstatnode = device->getSceneManager()->addCubeSceneNode(size,0,netid,position,rotation,scale);
							setNetId(tmpstatnode,netid);
							nodetype[netid] = 1001;
							if(verbose)
								printf("Net Node Created: %d %d Position: %f x %f x %f Rotation: %f x %f x %f Scale: %f x %f x %f Type: 1001",
								packiden,netid,position.X,position.Y,position.Z,rotation.X,rotation.Y,rotation.Z,scale.X,scale.Y,scale.Z);
							//nodepacket[netid] = enet_packet_create (event.packet->data,event.packet->dataLength,event.packet->flags);
							ownedby[netid] = (u16)event.peer->data;

						}

						if(packiden == 16)
						{
							u16 netid = 0;
							memcpy((char*)&netid,buff+1,2);
							nodetype[netid] = 1001;
							f32 radius = 0;
							memcpy((char*)&radius,buff+3,4);
							cubesize[netid] = radius;
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 7,4);
							memcpy((char*)&floaty,buff + 11,4);
							memcpy((char*)&floatz,buff + 15,4);
							vector3df position(floatx,floaty,floatz);
							memcpy((char*)&floatx,buff + 19,4);
							memcpy((char*)&floaty,buff + 23,4);
							memcpy((char*)&floatz,buff + 27,4);
							vector3df rotation(floatx,floaty,floatz);
							memcpy((char*)&floatx,buff + 31,4);
							memcpy((char*)&floaty,buff + 35,4);
							memcpy((char*)&floatz,buff + 39,4);
							vector3df scale(floatx,floaty,floatz);
							s32 polycount = 0;
							memcpy((char*)&polycount,buff + 43,4);
							polyarray[netid] = polycount;
							tmpstatnode = device->getSceneManager()->addSphereSceneNode(radius,polycount,0,netid,position,rotation,scale);
							setNetId(tmpstatnode,netid);
							nodetype[netid] = 1002;
							if(verbose)
								printf("Net Node Created: %d %d Position: %f x %f x %f Rotation: %f x %f x %f Scale: %f x %f x %f Type: 1002",
								packiden,netid,position.X,position.Y,position.Z,rotation.X,rotation.Y,rotation.Z,scale.X,scale.Y,scale.Z);
							//nodepacket[netid] = enet_packet_create (event.packet->data,event.packet->dataLength,event.packet->flags);
							ownedby[netid] = (u16)event.peer->data;

						}

						if(packiden == 17)
						{
							u16 netid = 0;
							u16 ntype = 0;
							memcpy((char*)&netid,buff+1,2);
							memcpy((char*)&ntype,buff+3,2);
							nodetype[netid] = ntype;
							f32 floatx = 0;
							f32 floaty = 0;
							f32 floatz = 0;
							memcpy((char*)&floatx,buff + 5,4);
							memcpy((char*)&floaty,buff + 9,4);
							memcpy((char*)&floatz,buff + 13,4);
							vector3df position(floatx,floaty,floatz);
							memcpy((char*)&floatx,buff + 17,4);
							memcpy((char*)&floaty,buff + 21,4);
							memcpy((char*)&floatz,buff + 25,4);
							vector3df rotation(floatx,floaty,floatz);
							memcpy((char*)&floatx,buff + 29,4);
							memcpy((char*)&floaty,buff + 33,4);
							memcpy((char*)&floatz,buff + 37,4);
							vector3df scale(floatx,floaty,floatz);

							tmpnode = device->getSceneManager()->addAnimatedMeshSceneNode(NodeTypes[ntype].mesh,0,netid,position,rotation,scale);
							if(NodeTypes[ntype].animator)
								tmpnode->addAnimator(NodeTypes[ntype].animator);
							tmpnode->getMaterial(0) = NodeTypes[ntype].material;
							setNetId(tmpnode,netid);
							nodetype[netid] = ntype;
							if(verbose)
								printf("Net Node Created: %d %d Position: %f x %f x %f Rotation: %f x %f x %f Scale: %f x %f x %f Type: 1002",
								packiden,netid,position.X,position.Y,position.Z,rotation.X,rotation.Y,rotation.Z,scale.X,scale.Y,scale.Z);
							//nodepacket[netid] = enet_packet_create (event.packet->data,event.packet->dataLength,event.packet->flags);
							ownedby[netid] = (u16)event.peer->data;

							if(NodeTypes[ntype].customop)
							NodeTypes[ntype].customop->Operations(tmpnode,device);
							setFrameRange(tmpnode,NodeTypes[ntype].nodeframerange);

							if(customops){customoperations->Operations(tmpnode, device);}

						}

						if(packiden == 18)
						{
							InPacket* filepacket = new InPacket(buff);
							filepacket->getNextItem((c8 &)packiden);
							s32 filesize;
							filepacket->getNextItem(filesize);
							c8 fileid;
							filepacket->getNextItem(fileid);
							c8 filename[255];
							filepacket->getNextItem((char*)filename);
							bool filexists = device->getFileSystem()->existFile(filename);
							OutPacket* fileresponse = createOutPacket();
							fileresponse->addData((c8)19);
							c8 i = 0;
							while(writefileuse[i] > 0 && i <= 100)
							{
								i++;
							}
							if(filexists == 1 || i > 100)
							{
								fileresponse->addData((c8)0);
							}
							else
							{
							fileresponse->addData((c8)1);
							writefileuse[i] = 1;
							writefile[i] = device->getFileSystem()->createAndWriteFile(filename);
							writefilesize[i] = filesize;
							}
							fileresponse->addData((c8)fileid);
							fileresponse->addData((c8)i);
							
							sendOutPacket(fileresponse);
						}

						if(packiden == 19)
						{
							InPacket* filepacket = new InPacket(buff);
							filepacket->getNextItem((c8 &)packiden);
							c8 usefile;
							filepacket->getNextItem(usefile);
							if(usefile == 1)
							{
							c8 fileid;
							filepacket->getNextItem(fileid);
							filepacket->getNextItem(readfileid[fileid]);
							readfileuse[fileid] = 3;
							printf("DUDE");
							}
							printf("BOOO");
						}

						if(packiden == 21)
						{
							InPacket* filepacket = new InPacket(buff);
							filepacket->getNextItem((c8 &)packiden);
							u16 nodeid;
							filepacket->getNextItem(nodeid);
							c8 framestate;
							filepacket->getNextItem(framestate);
							if(nodearray[nodeid] > 0)
							{
							tmpnode = dynamic_cast<IAnimatedMeshSceneNode*>(getSceneNodeFromNetId(nodeid));
							tmpnode->setFrameLoop(NetFrameRanges[nodeid].framestart[framestate],NetFrameRanges[nodeid].frameend[framestate]);
							if(NetFrameRanges[nodeid].framespeed[framestate] > 0)
							tmpnode->setAnimationSpeed(NetFrameRanges[nodeid].framespeed[framestate]);
							tmpnode->setLoopMode(NetFrameRanges[nodeid].frameloop[framestate]);
							}
						}

						


						if(recievefiles && packiden == 20)
						{
							//enet_uint8 newbuff[200];
							//memset(newbuff,0,sizeof(newbuff));
							//memcpy((char*)newbuff,(char*)buff,sizeof(buff));
							//packetbuffer.push_back(newbuff);	
							InPacket* filepacket = new InPacket(buff);
							filepacket->getNextItem((c8 &)packiden);
							printf("\n Packet Info: Iden: %d",packiden);

							c8 usefile;
							filepacket->getNextItem(usefile);
							printf("\n File ID: %d ",usefile);
							
							u32 filepos;
							filepacket->getNextItem((u32)filepos);
							printf("\n File Pos: %d",filepos);	

							filepacket->getNextItem((char*)filecontents);
							//printf("\n\n File Packet Contents:");
							//printf(filecontents);
							//printf("\n\n");
							printf("\n Doo");


							// Make a new File Packet struct and input the information we got from the packet.
							SFilePacket FPacket;
							FPacket.fileId = usefile;
							FPacket.readPos = filepos;
							printf("\n Doo");
							memcpy((char*)FPacket.fileData,(char*)filecontents,sizeof(filecontents));

							printf("\n Doo \n\n");

							// Chuck the struct into the file packet buffer.
							packetbuffer.push_back(FPacket);
						}


						}

						if(customhandling)
						{

						inpacket->setNewData(buff);
						inpacket->setPlayerId((u16)event.peer->data);

						if(customhandler)
							customhandler->Handle(inpacket);
						}

						/* Clean up the packet now that we're done using it. */

						ENetPacket * packet = enet_packet_create (event.packet->data,event.packet->dataLength,event.packet->flags);

						if((packiden != 12 && packiden != 3) || !(defaulthandling))
						{
							for (currentPeer = host -> peers; currentPeer < & host -> peers [host -> peerCount];    ++ currentPeer)
							{
								if (currentPeer->data == event.peer->data)
									continue;
								if (currentPeer -> state != ENET_PEER_STATE_CONNECTED)
									continue;
								enet_peer_send (currentPeer, 0, packet);
							}
						}

						buff = 0;

						enet_packet_destroy (event.packet);

						break;
					}
				case ENET_EVENT_TYPE_DISCONNECT:
					//if(event.peer -> data != NULL)
					//printf ("%d disconected.\n", (u16)event.peer -> data);

					/* Reset the peer's client information. */

					u16 i = ((u16)event.peer->data * 100) + 1000;
					while(i < (((u16)event.peer->data * 100) + 1100))
					{
						if(nodearray[i] > 0)
						{
							getSceneNodeFromNetId(i)->setVisible(false);
							sendVisible(getSceneNodeFromNetId(i),0);
							nodearray[i] = 0;
							nodetype[i] = 0;	
							originarray[i] = 0;
						}
						i++;
					}
					if(event.peer->data){players[(u16)event.peer -> data] = 0;}
					event.peer -> data = NULL;
				}



			}


			// File Handler
						
						
							int i = 0;
							int z = 0;
							while(i <= 100)
							{
								z = 0;
								while(z < filePriority)
								{
									if(readfileuse[i] == 3)
									{
										char filebuffer[FILEPACKETSIZE];
										memset((char*)filebuffer,0,sizeof(filebuffer));

										OutPacket* filedata = createOutPacket();
										filedata->addData((c8)20);
										filedata->addData((c8)readfileid[i]);
										filedata->addData((u32)readfile[i]->getPos());
										printf("\n %d \n",(u32)readfile[i]->getPos());
										if(readfile[i]->getSize() - readfile[i]->getPos() < FILEPACKETSIZE)
										{
											readfile[i]->read(filebuffer,readfile[i]->getSize() - readfile[i]->getPos());
											//readfile[i]->seek(readfile[i]->getSize() - readfile[i]->getPos(), true);
										}
										else
										{
											readfile[i]->read(filebuffer,FILEPACKETSIZE);
											//readfile[i]->seek(FILEPACKETSIZE, true);
										}
										filedata->addData((char*)filebuffer);
										if(readfilepid[i] == 0)
											sendOutPacket(filedata);
										else
											sendOutPacket(filedata,readfilepid[i]);
										if(readfile[i]->getPos() >= readfile[i]->getSize())
										{
											readfileuse[i] = 0;
											readfile[i]->drop();
										}
									}
									z++;
								}
								i++;
							}






							i = 0;
							while(i <= 100)
							{
								if(writefileuse[i] == 1)
								{
									z = 0;
									while(z < packetbuffer.size())
									{
										if(packetbuffer[z].fileId == i)
										{
										printf("\n\n PacketPos%d \n",packetbuffer[z].readPos);
										printf(" Z:%d \n",z);
										printf(" FilePos :%d \n\n",writefilepos[i]);
										if(packetbuffer[z].readPos == writefilepos[i])
										{
											if((s32)(writefilesize[i] - writefilepos[i]) < FILEPACKETSIZE)
											{
											writefile[i]->write(packetbuffer[z].fileData,writefilesize[i] - writefilepos[i]);
											writefilepos[i] += (writefilesize[i] - writefilepos[i]);
											}
											else
											{
											writefile[i]->write(packetbuffer[z].fileData,FILEPACKETSIZE);
											writefilepos[i] += FILEPACKETSIZE;
											}
											packetbuffer.erase(z);
											z--;
											if(writefilesize[i] == writefilepos[i])
											{
												writefile[i]->drop();
												writefileuse[i] = 0;
												writefilepos[i] = 0;
											}
										}
										}
										
										z++;
									}
									
								}
							i++;
							}


			//int timeset = device->getTimer()->getTime();
			//}
		}




		u16 INetManager::setNetId(ISceneNode* node, u16 netid, bool origin)
		{
			//int i = 0;
			//while(nodearray[i] > 0)
			//{
			//	i++;
			//}
			//nodearray[i] = node->getID();
			//originarray[i] = origin;
			nodearray[netid] = node->getID();
			originarray[netid] = origin;
			NetAnim[netid] = new AutoAnim(device->getSceneManager(),this);
			node->addAnimator(NetAnim[netid]); 
			return netid;
		}

		u16 INetManager::getNetId(ISceneNode* node)
		{
			s32 nodeid = node->getID();
			u16 i = 0;
			while(nodearray[i] != nodeid && i <= 10000)
			{
				i++;
			}
			return i;
		}

		ISceneNode* INetManager::getSceneNodeFromNetId(u16 netid)
		{
			return device->getSceneManager()->getSceneNodeFromId(nodearray[netid]);
		}

		void INetManager::removeAnimators(ISceneNode* node)
		{
			c8 buffer[6];
			u8 packid = 11;
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			memcpy(buffer+1,(char*)&netid,2);
			ENetPacket * packet;
			packet = enet_packet_create (buffer,6,ENET_PACKET_FLAG_RELIABLE);

			/* Send the packet to the peer over channel id 0. */
			/* One could also broadcast the packet by         */
			/* enet_host_broadcast (host, 0, packet);         */
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::setVerbose(bool isverbose)
		{
			verbose = isverbose;
		}

		void INetManager::sendVelocity(ISceneNode* node, vector3df velocity, bool reliable)
		{
			c8 buffer[15];
			u8 packid = 10;
			tmpvector = velocity;
			memcpy(buffer,(char*)&packid,1);
			u16 netid = 0;
			if(node)
				netid = getNetId(node);
			else
				netid = 1006;
			memcpy(buffer+1,(char*)&netid,2);
			tmpfloat = tmpvector.X;
			memcpy(buffer+3,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+7,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+11,(char*)&tmpfloat,sizeof(tmpfloat));
			ENetPacket * packet;
			if(reliable)
			{
				packet = enet_packet_create (buffer,15,ENET_PACKET_FLAG_RELIABLE);
			}
			else
			{
				packet = enet_packet_create (buffer,15,0);
			}
			/* Send the packet to the peer over channel id 0. */
			/* One could also broadcast the packet by         */
			/* enet_host_broadcast (host, 0, packet);         */
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::sendPosition(ISceneNode* node, bool reliable)
		{
			c8 buffer[15];
			u8 packid = 5;
			tmpvector = node->getPosition();
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			memcpy(buffer+1,(char*)&netid,2);
			tmpfloat = tmpvector.X;
			memcpy(buffer+3,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+7,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+11,(char*)&tmpfloat,sizeof(tmpfloat));
			ENetPacket * packet;
			if(reliable)
			{
				packet = enet_packet_create (buffer,15,ENET_PACKET_FLAG_RELIABLE);
			}
			else
			{
				packet = enet_packet_create (buffer,15,0);
			}
			/* Send the packet to the peer over channel id 0. */
			/* One could also broadcast the packet by         */
			/* enet_host_broadcast (host, 0, packet);         */
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::sendRotation(ISceneNode* node, bool reliable)
		{
			c8 buffer[15];
			u8 packid = 6;
			tmpvector = node->getRotation();
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			memcpy(buffer+1,(char*)&netid,2);
			tmpfloat = tmpvector.X;
			memcpy(buffer+3,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+7,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+11,(char*)&tmpfloat,sizeof(tmpfloat));
			ENetPacket * packet;
			if(reliable)
			{
				packet = enet_packet_create (buffer,15,ENET_PACKET_FLAG_RELIABLE);
			}
			else
			{
				packet = enet_packet_create (buffer,15,0);
			}
			/* Send the packet to the peer over channel id 0. */
			/* One could also broadcast the packet by         */
			/* enet_host_broadcast (host, 0, packet);         */
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::sendScale(ISceneNode* node, bool reliable)
		{
			c8 buffer[15];
			u8 packid = 7;
			tmpvector = node->getScale();
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			memcpy(buffer+1,(char*)&netid,2);
			tmpfloat = tmpvector.X;
			memcpy(buffer+3,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+7,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+11,(char*)&tmpfloat,sizeof(tmpfloat));
			ENetPacket * packet;
			if(reliable)
			{
				packet = enet_packet_create (buffer,15,ENET_PACKET_FLAG_RELIABLE);
			}
			else
			{
				packet = enet_packet_create (buffer,15,0);
			}
			/* Send the packet to the peer over channel id 0. */
			/* One could also broadcast the packet by         */
			/* enet_host_broadcast (host, 0, packet);         */
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::sendFrame(IAnimatedMeshSceneNode* node, bool reliable)
		{
			c8 buffer[7];
			u8 packid = 8;
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			memcpy(buffer+1,(char*)&netid,2);
			u32 frameno = node->getFrameNr();
			memcpy(buffer+3,(char*)&frameno,4);
			ENetPacket * packet;
			if(reliable)
			{
				packet = enet_packet_create (buffer,7,ENET_PACKET_FLAG_RELIABLE);
			}
			else
			{
				packet = enet_packet_create (buffer,7,0);
			}
			/* Send the packet to the peer over channel id 0. */
			/* One could also broadcast the packet by         */
			/* enet_host_broadcast (host, 0, packet);         */
			enet_host_broadcast (host, 0, packet);
		}

		ENetPeer* INetManager::getPlayerById(u16 id)
		{
				for (currentPeer = host -> peers; currentPeer < & host -> peers [host -> peerCount];    ++ currentPeer)
							{
								if (currentPeer -> state != ENET_PEER_STATE_CONNECTED)
									continue;
								if (currentPeer->data == (void*)id)
									break;
							}
				return currentPeer;
		}

		ENetPeer* INetManager::getPeer()
		{
			if(established)
				return &host -> peers [0];
			else
				return 0;
		}

		u32 INetManager::getPing()
		{
			if(getPeer())
			return getPeer()->roundTripTime;
			else
			return 0;
		}


		void INetManager::sendPositionAuto(ISceneNode* node, u32 interval)
		{
			NetAnim[getNetId(node)]->sendPos(1,interval);
		}

		void INetManager::sendRotationAuto(ISceneNode* node, u32 interval)
		{
			NetAnim[getNetId(node)]->sendRot(1,interval);
		}

		void INetManager::sendScaleAuto(ISceneNode* node, u32 interval)
		{
			NetAnim[getNetId(node)]->sendSca(1,interval);
		}


		void INetManager::sendFrameAuto(IAnimatedMeshSceneNode* node, u32 interval)
		{
			NetAnim[getNetId(node)]->sendFrame(1,interval);
		}


		void INetManager::sendPositionAutoStop(ISceneNode* node)
		{
			NetAnim[getNetId(node)]->sendPos(0);
		}

		void INetManager::sendRotationAutoStop(ISceneNode* node)
		{
			NetAnim[getNetId(node)]->sendRot(0);
		}

		void INetManager::sendScaleAutoStop(ISceneNode* node)
		{
			NetAnim[getNetId(node)]->sendSca(0);
		}

		void INetManager::setLocalVelocity(ISceneNode* node, vector3df velocity)
		{

			NetAnim[getNetId(node)]->updateVel(1,velocity);
		}


		void INetManager::sendFrameAutoStop(IAnimatedMeshSceneNode* node)
		{
			NetAnim[getNetId(node)]->sendFrame(0);
		}


		void INetManager::sendCustomVar(ISceneNode* node, u16 varid, u32 varvalue)
		{
			c8 buffer[9];
			u8 packid = 13;
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			NetAnim[netid]->setVar(varid,varvalue);
			memcpy(buffer+1,(char*)&netid,2);
			memcpy(buffer+3,(char*)&varid,2);
			memcpy(buffer+5,(char*)&varvalue,4);
			ENetPacket * packet;
			packet = enet_packet_create (buffer,9,ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::sendChangeInCustomVar(ISceneNode* node, u16 varid, u32 varvalue)
		{
			c8 buffer[9];
			u8 packid = 14;
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			NetAnim[netid]->setVar(varid,varvalue);
			memcpy(buffer+1,(char*)&netid,2);
			memcpy(buffer+3,(char*)&varid,2);
			memcpy(buffer+5,(char*)&varvalue,4);
			ENetPacket * packet;
			packet = enet_packet_create (buffer,9,ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::sendCustomVarAuto(ISceneNode* node, u16 varid, u16 interval)
		{
			if(NetAnim[getNetId(node)] != NULL)
			NetAnim[getNetId(node)]->sendCustomVar(1,varid,interval);
		}

		void INetManager::sendCustomVarAutoStop(ISceneNode* node, u16 varid)
		{
			NetAnim[getNetId(node)]->sendCustomVar(0);
		}

		void INetManager::updateCustomVar(ISceneNode* node, u16 varid)
		{
			c8 buffer[6];
			u8 packid = 25;
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			memcpy(buffer+1,(char*)&netid,2);
			memcpy(buffer+3,(char*)&varid,2);
			memcpy(buffer+5,(char*)0,1);
			ENetPacket * packet;
			packet = enet_packet_create (buffer,6,ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::updateCustomVarAuto(ISceneNode* node, u16 varid, u16 interval)
		{
			c8 buffer[8];
			u8 packid = 25;
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			memcpy(buffer+1,(char*)&netid,2);
			memcpy(buffer+3,(char*)&varid,2);
			memcpy(buffer+5,(char*)1,1);
			memcpy(buffer+6,(char*)&interval,2);
			ENetPacket * packet;
			packet = enet_packet_create (buffer,8,ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::setCustomVarLocally(ISceneNode* node, u32 varid, u32 varvalue) // This is unneccesary, as sendCustomVar sets it locally too, but its nice to have an option for this.
		{
			NetAnim[getNetId(node)]->setVar(varid,varvalue);
		}


		u32 INetManager::getCustomVar(ISceneNode* node, u16 varid)
		{
			return NetAnim[getNetId(node)]->getVar(varid);
		}

		void INetManager::sendVisible(ISceneNode* node, bool visible, bool reliable)
		{
			c8 buffer[8];
			u8 packid = 9;
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			memcpy(buffer+1,(char*)&netid,2);
			memcpy(buffer+3,(char*)&visible,1);
			ENetPacket * packet;
			if(reliable)
			{
				packet = enet_packet_create (buffer,4,ENET_PACKET_FLAG_RELIABLE);
			}
			else
			{
				packet = enet_packet_create (buffer,4,0);
			}
			enet_host_broadcast (host, 0, packet);
		}

	void INetManager::sendFrameState(IAnimatedMeshSceneNode* node, c8 framestate)
	{
			c8 buffer[8];
			u8 packid = 21;
			memcpy(buffer,(char*)&packid,1);
			u16 netid = getNetId(node);
			memcpy(buffer+1,(char*)&netid,2);
			memcpy(buffer+3,(char*)&framestate,1);

			ENetPacket* packet = enet_packet_create (buffer,4,ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (host, 0, packet);
			
			node->setFrameLoop(NetFrameRanges[getNetId(node)].framestart[framestate],NetFrameRanges[getNetId(node)].frameend[framestate]);
			if(NetFrameRanges[getNetId(node)].framespeed[framestate] > 0)
			node->setAnimationSpeed(NetFrameRanges[getNetId(node)].framespeed[framestate]);
			node->setLoopMode(NetFrameRanges[getNetId(node)].frameloop[framestate]);

			getFrameRange(node).currentframestate = framestate;
	}


		u16 INetManager::requestNetId()
		{
			c8 buffer[1];
			u8 packid = 12;
			memcpy(buffer,(char*)&packid,1);
			ENetPacket * packet;

			packet = enet_packet_create (buffer,1,ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(host, 0, packet);
			enet_host_flush (host);

			u16 netid = 0;
			u8 packiden = 0;
			ENetEvent event;
			while (enet_host_service(host, & event, 1000) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE && packiden != 12)
			{
				buff = event.packet->data;

				memcpy((char*)&packiden,buff,1);
				if (packiden == 12)
				{
					memcpy((char*)&netid,buff+1,2);
				}
			}

			if(verbose)
				printf("%d", netid);

			return netid;
		}


		u32 INetManager::getPeerCount()
		{
			return host->peerCount;
		}

		u16 INetManager::getPlayerNumber()
		{
			return playernumber;
		}

		ENetHost* INetManager::getHost()
		{
			return host;
		}

		void INetManager::removeNetNode(ISceneNode* node)
		{
			node->removeAnimators();
			node->setVisible(false);
			u16 netid = getNetId(node);
			nodearray[netid] = 0;
			nodetype[netid] = 0;
			originarray[netid] = 0;
			ownedby[netid] = 0;
			NetAnim[netid]->drop();
			NetAnim[netid] = 0;

			c8 buffer[3];
			u8 packid = 30;
			memcpy(buffer,(char*)&packid,1);
			memcpy(buffer+1,(char*)&netid,2);
		
			ENetPacket* packet = enet_packet_create (buffer,3,ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (host, 0, packet);


			
		}

		ISceneNode* INetManager::addNetCubeSceneNode(f32 size,core::vector3df position, core::vector3df rotation, core::vector3df scale)
		{
			u16 netid = 0;
			if(isserver)
			{
				u16 i = 1;
				while(nodearray[i] > 0)
				{
					i++;
				}
				netid = i;
			}
			else
			{
				netid = requestNetId();
			}
			tmpstatnode = device->getSceneManager()->addCubeSceneNode(size,0,netid,position,rotation,scale);
			setNetId(tmpstatnode,netid);
			originarray[netid] = 1;
			cubesize[netid] = size;
			nodetype[netid] = 1001;
			c8 buffer[43];
			u8 packid = 15;
			memcpy(buffer,(char*)&packid,1);
			memcpy(buffer+1,(char*)&netid,2);
			memcpy(buffer+3,(char*)&size,4);
			tmpvector = position;
			tmpfloat = tmpvector.X;
			memcpy(buffer+7,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+11,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+15,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpvector = rotation;
			tmpfloat = tmpvector.X;
			memcpy(buffer+19,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+23,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+27,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpvector = scale;
			tmpfloat = tmpvector.X;
			memcpy(buffer+31,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+35,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+39,(char*)&tmpfloat,sizeof(tmpfloat));
			ENetPacket * packet;
			//nodepacket[netid] = enet_packet_create (buffer,43,ENET_PACKET_FLAG_RELIABLE);
			packet = enet_packet_create (buffer,43,ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (host, 0, packet);
			return tmpstatnode;

		}

		ISceneNode* INetManager::addNetSphereSceneNode(f32 radius, s32 polycount,core::vector3df position, core::vector3df rotation, core::vector3df scale)
		{
			u16 netid = 0;
			if(isserver)
			{
				u16 i = 1;
				while(nodearray[i] > 0)
				{
					i++;
				}
				netid = i;
			}
			else
			{
				netid = requestNetId();
			}
			tmpstatnode = device->getSceneManager()->addSphereSceneNode(radius,polycount,0,netid,position,rotation,scale);
			setNetId(tmpstatnode,netid);
			originarray[netid] = 1;
			cubesize[netid] = radius;
			polyarray[netid] = polycount;
			nodetype[netid] = 1002;
			c8 buffer[47];
			u8 packid = 16;
			memcpy(buffer,(char*)&packid,1);
			memcpy(buffer+1,(char*)&netid,2);
			memcpy(buffer+3,(char*)&radius,4);
			tmpvector = position;
			tmpfloat = tmpvector.X;
			memcpy(buffer+7,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+11,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+15,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpvector = rotation;
			tmpfloat = tmpvector.X;
			memcpy(buffer+19,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+23,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+27,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpvector = scale;
			tmpfloat = tmpvector.X;
			memcpy(buffer+31,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+35,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+39,(char*)&tmpfloat,sizeof(tmpfloat));
			memcpy(buffer+43,(char*)&polycount,4);
			ENetPacket * packet;
			//nodepacket[netid] = enet_packet_create (buffer,43,ENET_PACKET_FLAG_RELIABLE);
			packet = enet_packet_create (buffer,47,ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (host, 0, packet);
			return tmpstatnode;

		}


		IAnimatedMeshSceneNode* INetManager::addNetSceneNode(u16 node_type , core::vector3df position, core::vector3df rotation, core::vector3df scale)
		{
			u16 netid = 0;
			if(isserver)
			{
				u16 i = 1;
				while(nodearray[i] > 0)
				{
					i++;
				}
				netid = i;
			}
			else
			{
				netid = requestNetId();
			}
			tmpnode = device->getSceneManager()->addAnimatedMeshSceneNode(NodeTypes[node_type].mesh,0,netid,position,rotation,scale);
			if(NodeTypes[node_type].animator)
				tmpnode->addAnimator(NodeTypes[node_type].animator);
			tmpnode->getMaterial(0) = NodeTypes[node_type].material;
			setNetId(tmpnode,netid);
			originarray[netid] = 1;
			nodetype[netid] = node_type;
			c8 buffer[41];
			u8 packid = 17;
			memcpy(buffer,(char*)&packid,1);
			memcpy(buffer+1,(char*)&netid,2);
			memcpy(buffer+3,(char*)&node_type,2);
			tmpvector = position;
			tmpfloat = tmpvector.X;
			memcpy(buffer+5,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+9,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+13,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpvector = rotation;
			tmpfloat = tmpvector.X;
			memcpy(buffer+17,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+21,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+25,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpvector = scale;
			tmpfloat = tmpvector.X;
			memcpy(buffer+29,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Y;
			memcpy(buffer+33,(char*)&tmpfloat,sizeof(tmpfloat));
			tmpfloat = tmpvector.Z;
			memcpy(buffer+37,(char*)&tmpfloat,sizeof(tmpfloat));
			ENetPacket * packet;
			packet = enet_packet_create (buffer,41,ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (host, 0, packet);

			if(NodeTypes[node_type].customop)
			NodeTypes[node_type].customop->Operations(tmpnode,device);
			if(customops){customoperations->Operations(tmpnode, device);}

			setFrameRange(tmpnode,NodeTypes[node_type].nodeframerange);
			getFrameRange(tmpnode).currentframestate = -1;

			return tmpnode;

		}



		void INetManager::setNodeType(u16 type, SMaterial material, IAnimatedMesh* mesh, ISceneNodeAnimator* animator, CustomOperations* customop, SFrameRange framerange)
		{
			NodeTypes[type].material = material;
			NodeTypes[type].mesh = mesh;
			NodeTypes[type].animator = animator;
			NodeTypes[type].customop = customop;
			NodeTypes[type].nodeframerange = framerange;
		}

		SMaterial getNodeTypeMaterial(u16 type)
		{
			return NodeTypes[type].material;
		}

		ISceneNodeAnimator* getNodeTypeAnimator(u16 type)
		{
			return NodeTypes[type].animator;
		}

		IAnimatedMesh* getNodeTypeMesh(u16 type)
		{
			return NodeTypes[type].mesh;
		}


		bool INetManager::getEstablished()
		{
			return established;
		}

		void INetManager::setGlobalCustomOperations(bool enable, CustomOperations* operations)
		{
			customops = enable;
			if(operations)
				customoperations = operations;
		}
		
		void INetManager::setCustomHandling(bool enable, CustomHandler* handler)
		{
			customhandler = handler;
			customhandling = enable;
		}

		void INetManager::setDefaultHandling(bool enable)
		{
			defaulthandling = enable;
		}


		void INetManager::sendOutPacket(OutPacket* outpacket)
		{
			ENetPacket * packet;
			packet = enet_packet_create ((char*)outpacket->getData(),outpacket->getSize(),ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::sendOutPacket(OutPacket* outpacket, u16 playerno)
		{
			ENetPacket * packet;
			packet = enet_packet_create ((char*)outpacket->getData(),outpacket->getSize(),ENET_PACKET_FLAG_RELIABLE);

			for (currentPeer = host -> peers; currentPeer < & host -> peers [host -> peerCount];    ++ currentPeer)
			{
				if (currentPeer->data != (void*)playerno)
					continue;
				if (currentPeer -> state != ENET_PEER_STATE_CONNECTED)
					continue;
				enet_peer_send (currentPeer, 0, packet);
			}
		}

		void INetManager::sendOutPacketUnreliable(OutPacket* outpacket)
		{
			ENetPacket * packet;
			packet = enet_packet_create ((char*)outpacket->getData(),outpacket->getSize(),0);
			enet_host_broadcast (host, 0, packet);
		}

		void INetManager::sendOutPacketUnreliable(OutPacket* outpacket, u16 playerno)
		{
			ENetPacket * packet;
			packet = enet_packet_create ((char*)outpacket->getData(),outpacket->getSize(),0);
			for (currentPeer = host -> peers; currentPeer < & host -> peers [host -> peerCount];    ++ currentPeer)
			{
				if (currentPeer->data != (void*)playerno)
					continue;
				if (currentPeer -> state != ENET_PEER_STATE_CONNECTED)
					continue;
				enet_peer_send (currentPeer, 0, packet);
			}
		}

		void INetManager::setCustomAddress(stringc address, u32 port)
		{
			customconnect = 1;
			customaddress = address;
			customport = port;
		}
		
		u16 INetManager::getNetSceneNodeType(ISceneNode* node)
		{
			return nodetype[getNetId(node)];
		}

		core::array<ISceneNode*> INetManager::getArrayOfAllNetNodes()
		{
			core::array<ISceneNode*> allNodesArray;
			int i = 0;
			while(i <= 10000)
			{
				if(nodearray[i] > 0)
				{
					allNodesArray.push_back(getSceneNodeFromNetId(i));
				}
				i++;
			}
			return allNodesArray;
		}

		core::array<ISceneNode*> INetManager::getArrayOfAllMyNetNodes()
		{
			core::array<ISceneNode*> allNodesArray;
			int i = 0;
			while(i <= 10000 && originarray[i] == 1)
			{
				if(nodearray[i] > 0)
				{
					allNodesArray.push_back(getSceneNodeFromNetId(i));
				}
				i++;
			}
			return allNodesArray;
		}



core::array<ISceneNode*> INetManager::getArrayOfAllNetNodesForPlayedId(u16 playerno)
{
	core::array<ISceneNode*> allNodesArray;
			int i = 0;
			while(i <= 10000 && ((nodearray[i] >= (playerno * 100) + 1000) && (nodearray[i] < (playerno * 100) + 1100)))
			{
				if(nodearray[i] > 0)
				{
					allNodesArray.push_back(getSceneNodeFromNetId(i));
				}
				i++;
			}
			return allNodesArray;
}



		void INetManager::setRecieveFiles(bool recieve)
		{
			recievefiles = recieve;
		}

		void INetManager::sendFile(c8* filename, c8* destfilename, u16 playerid)
		{
			u8 i = 0;
			while(readfileuse[i] > 0 && i <= 100)
			{
				i++;
			}
			if(i > 100){return;}
			readfile[i] = device->getFileSystem()->createAndOpenFile(filename);
			OutPacket* filerequest = createOutPacket();
			filerequest->addData((c8)18); 
			filerequest->addData(readfile[i]->getSize());
			filerequest->addData((c8)i); 
			if(destfilename == 0)
			{
				stringc mystring = filename;
				if(mystring.find("/") != -1)
				{
				mystring = mystring.subString(mystring.findLast(47) + 1,mystring.size() - mystring.findLast(47) - 1);
				destfilename = (char*)mystring.c_str();
				printf(mystring.c_str());	
				}
				else
				{
				mystring = mystring.subString(mystring.findLast(134) + 1,mystring.size() - mystring.findLast(134) - 1);
				destfilename = (char*)mystring.c_str();
				printf(mystring.c_str());	
				}
			}
			filerequest->addData((char*)destfilename);
			printf(destfilename);
			if(playerid == 0)
			sendOutPacket(filerequest);
			else
			sendOutPacket(filerequest, playerid);

			readfilepid[i] = playerid;
			readfileuse[i] = 1;


		}

	SFrameRange& INetManager::getFrameRange(IAnimatedMeshSceneNode* node)
	{
		return NetFrameRanges[getNetId(node)];
	}


	void INetManager::setFrameRange(IAnimatedMeshSceneNode* node, SFrameRange rangestruct)
	{
		NetFrameRanges[getNetId(node)] = rangestruct;
	}

	OutPacket* INetManager::createOutPacket()
		{
			return new OutPacket();
		}

		OutPacket* INetManager::createOutPacket(enet_uint8 *buff,const int buff_size)
		{
		 	return new OutPacket(buff,buff_size);
		}
		

		CustomOperations::CustomOperations()
		{
		}

		void CustomOperations::Operations(IAnimatedMeshSceneNode* node, irr::IrrlichtDevice *device)
		{
		}

		CustomHandler::CustomHandler()
		{
		}

		void CustomHandler::Handle(InPacket * packet)
		{
		}

		INetManager* createNetManager(irr::IrrlichtDevice* irrdev)
		{
			return new irr::net::INetManager(irrdev);
		}



		




	} // Close Net Namespace
} // Close Irr namespace

// Copyright(C) Ahmed Hilali 2007

/* License:

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgement in the product documentation would be
appreciated but is not required.
2. Altered source versions must be clearly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/
