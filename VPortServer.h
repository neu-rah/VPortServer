//#include "../VPinsSPI/VPinsSPI.h"

#define nports 5 //TODO: get this from somewhere else

void vpsRun();

class CSStatus {
  public://store csPins
  static uint8_t mask[nports];
  uint8_t status[nports];
  CSStatus() {for(int s=0;s<nports;s++) status[s]=0;}
  void backupCS();
  void restoreCS();
  void begin();
};

class VPortServer {
protected:
	TwoWire &Wire;
public:
	VPortServer(TwoWire & wire);
	void begin(uint8_t serverID,bool shared=false);
};

