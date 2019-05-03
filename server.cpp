#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "serverchannel.h"

using namespace std; 

int main(int argc, char *argv[]){
  if(argc < 2) {
    cout<<"command line arguments needed for running server {port}."<<endl;
    cout<<"example:./server 10999"<<endl;
    return 1;
  }
  int nPort = atoi(argv[1]);
  unsigned short int maxPort = 0xFFFF;
  if(nPort > maxPort) {
    cout<<"Port:"<<nPort<<" is larger than maximum allowed "<<maxPort<<"."<<endl;
    return 2;
  }
  unsigned short port = nPort;
  
  ServerChannel serverChannel(nPort);
  return serverChannel.run();
}
