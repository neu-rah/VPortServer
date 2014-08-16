#include <Arduino.h>
#include "../Wire/Wire.h"
#include "VPortServer.h"

char vpserver_active_port=-1;

uint8_t CSStatus::mask[nports];

void CSStatus::backupCS() {
  for(int p=0;p<nports;p++)
  	if (portOutputRegister(p)!=NOT_A_PORT)
	    status[p]=*portOutputRegister(p)&mask[p];
}

void CSStatus::restoreCS() {
  for(int p=0;p<nports;p++)
    if (portOutputRegister(p)!=NOT_A_PORT)
    	*portOutputRegister(p)=(*portOutputRegister(p)&~mask[p])|(status[p]&mask[p]);
}

void CSStatus::begin() {
	//aquire CS pins list from currently defined OUTPUT pins
  for(int p=0;p<nports;p++)
  	if (portModeRegister(p)!=NOT_A_PORT)
  		mask[p]=*portModeRegister(p);
}

/************************************************************
VPortServer
Virtual Port Server
Serving local (real and virtual) ports to I2C network
************************************************************/
//there's still space for protocol expansion:
//operation mode uses only 3 of 4 states
//number of ports now 64, we can limit it to 32 and have an extra bit

CSStatus csI;
CSStatus csS;

void vpsRun() {
	//Serial.println("rcv");
	csI.backupCS();
	csS.restoreCS();
	char port=Wire.read();
	char op=port&0b11;//OPeration can be setmode|output|input (00|01|10)
	port>>=2;
	vpserver_active_port=port;
	//TODO: verify we dont override nothing!
	//for(int n=0;n<numBytes-1;n++) {
	for(int n=0;n<Wire.available();n++) {
		long timeout=millis()+500;
		while(!Wire.available()) if (millis()>=timeout) return;
  	*(op+portModeRegister(port+n))=Wire.read();
		vpins_out(port);
	}
	csS.backupCS();
	csI.restoreCS();
}

void shared_rcv(int numBytes) {}

void rcv(int numBytes) {vpsRun();}

void req() {
	vpins_in(vpserver_active_port);
	Wire.write(*portInputRegister(vpserver_active_port));
}

VPortServer::VPortServer(TwoWire & wire):Wire(wire) {}

void VPortServer::begin(uint8_t serverId,bool shared) {
	Serial.print("VPort server");
	Serial.println(shared?" shared mode":"");
	Wire.onReceive(shared?shared_rcv:rcv);
	Wire.onRequest(req);
	Wire.begin(serverId);
	//csI.backupCS();
}

/*void rcv_debug(int numBytes) {
  Serial.print("numBytes: ");Serial.print(numBytes);
  for(int n=0;n<numBytes;n++) {
    Serial.print(" 0x");
    Serial.print(0xff&(int)Wire.read(),HEX);
  }
  Serial.println("\n");
  if (Wire.available()) Serial.print("damn");
}*/

//_VPortServer VPortServer();


