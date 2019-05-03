#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "clientchannel.h"

using namespace std;

int main(int argc, char *argv[]) {
  if(argc < 3) {
    cerr<<"command line arguments needed for client to connect to server {ip} {port}."<<endl;
    cerr<<"example:./client 127.0.0.1 10999"<<endl;
    return 1;
  }
  //ipv6 maximum plus null terminator
  char szAddr[46] = {0}; 
  strcpy(&szAddr[0], argv[1]);

  int nPort = atoi(argv[2]);
  unsigned short maxPort = 0xFFFF;
  if(nPort > maxPort) {
    cerr<<"Port:"<<nPort<<" is larger than maximum allowed "<<maxPort<<"."<<endl;
    return 2;
  }
  
  ClientChannel client(szAddr, nPort);
  int errorCode = client.run();
  
  return errorCode;
}
