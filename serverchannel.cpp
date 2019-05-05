#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"

#include "serverchannel.h"

using namespace std;
//typedef STUDENT_MAP::size_type size_type;

ServerChannel::ServerChannel(
  const unsigned short int nPort
):serverSocket(-1){
  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
 serverSocket = socket(PF_INET, SOCK_DGRAM, 0);
 initServerAddress(nPort);
}

ServerChannel::~ServerChannel() {
  if(serverSocket != -1 )  
    close(serverSocket);
}

void ServerChannel::initServerAddress(
  const unsigned short int port
) {
  
   /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(port);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
}


int ServerChannel::run(
) {
  int nCode = bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  std::cout<<"binded"<<endl;
  if(nCode != 0) {
    SocketHelper::printError("Binding");
    return -3;
  }
  return receive();
}

int ServerChannel::receive() {
    /*---- Accept call creates a new socket for the incoming connection ----*/
  
  cout<<"studentDatabase:empty"<<studentDatabase.empty()<<endl;
  for(;;) {
    std::cout<<"blocking data reception:"<<endl;
    socklen_t addr_size = sizeof (sockaddr_storage);
    struct sockaddr_storage serverStorage;

    RequestHeader header;
    memset(&header,  0, sizeof(RequestHeader));

    /*---- Send message to the socket of the incoming connection ----*/
    
    int nBytes = recvfrom(
        serverSocket,
        &header,
        sizeof(RequestHeader),
        MSG_WAITALL,
        (struct sockaddr *)&serverStorage,
        &addr_size);
    if(nBytes > 0) {
        cout<<"requestHeader:"<<(int)header.code<<endl;
        cout<<"invoke readData"<<endl;
        int errorCode = readData(&serverStorage,addr_size, (Action_Code)header.code);
        if(errorCode != 0) {
          return errorCode;
        }
      } else {
        SocketHelper::printError("recvFrom");
        return -6;
      }
      
  }
  return 0;
}

int ServerChannel::handleDisplayAll(
  sockaddr_storage* serverStorage,
  socklen_t& addr_size
) {
  cout<<"handle display All"<<endl;
  RequestHeader header = {Action_Code_Display_All};
  int errorCode = sendto(
        serverSocket,
        &header,
        sizeof(RequestHeader),
        0,
        (struct sockaddr *)serverStorage,
        addr_size);

  cout<<"error code:"<<errorCode<<endl;
  if(errorCode == -1) {
    SocketHelper::printError("DisplayStudentId_send1");
    return -8;
  }
  
  int n = (int)studentDatabase.size();
  std::cout<<"sending database length"<<n<<endl;
  errorCode = 
    sendto(
      serverSocket,
      &n,
      sizeof(int),
      0,
      (struct sockaddr *)serverStorage,
      addr_size);

  if(errorCode == -1) {
      SocketHelper::printError("DisplayStudentId_send2");
      return -9;
  
  }
  STUDENT_MAP::const_iterator it;
  for (it=studentDatabase.begin(); it!=studentDatabase.end(); ++it) {
      cout<<"sending student id:"<<it->second.studentId<<endl;
      errorCode = sendto(
                    serverSocket,
                    &it->second,
                    sizeof(Student),
                    0,
                    (struct sockaddr *)serverStorage,
                    addr_size);
      if(errorCode == -1) {
        SocketHelper::printError("DisplayStudentId_send3");
        return -10;
      }
  }
  return 0;
}

int ServerChannel::handleDisplayScore(
  sockaddr_storage* serverStorage,
  socklen_t& addr_size
) {
  cout<<"handle display Score"<<endl;
  ScoreRequest scoreRequest = {0};
  int errorCode = recvfrom(
        serverSocket,
        &scoreRequest,
        sizeof(ScoreRequest),
        MSG_WAITALL,
        (struct sockaddr *)serverStorage,
        &addr_size);    
    
    if(errorCode == -1) {
      SocketHelper::printError("handleDisplayScore_recv");
      return -20;
    }

    STUDENT_MAP tempDb;
    for (STUDENT_MAP::const_iterator it=studentDatabase.begin(); it!=studentDatabase.end(); ++it) {
        if(it->second.score > scoreRequest.score) {
            tempDb.insert(pair<int, Student>(it->first, it->second)); 
        }
    }
    
    RequestHeader header = {.code=Action_Code_Display_Score};
    errorCode = sendto(
      serverSocket, 
      &header, 
      sizeof(RequestHeader),
       0,
       (struct sockaddr *)serverStorage,
      addr_size);
    
    if(errorCode == -1) {
      SocketHelper::printError("handleDisplayScore_recv");
      return -21;
    }    
    
    int n = (int)tempDb.size();
    errorCode = sendto(
      serverSocket,
      &n,
      sizeof(int),
      0,
      (struct sockaddr *)serverStorage,
      addr_size);
    
    if(errorCode == -1) {
      SocketHelper::printError("handleDisplayScore_recv");
      return -22;
    }

    for (STUDENT_MAP::iterator it=tempDb.begin(); it!=tempDb.end(); ++it) {
        Student student = it->second;
        errorCode = sendto(
                      serverSocket,
                      &student,
                      sizeof(Student),
                      0,
                      (struct sockaddr *)serverStorage,
                      addr_size);
        
        if(errorCode == -1) {
          SocketHelper::printError("handleDisplayScore_recv");
          return -23;
        }
    }
    return 0;
}


int ServerChannel::handleDisplayStudentId(
  sockaddr_storage* serverStorage,
  socklen_t& addr_size
) {
  cout<<"handle display Student by Id"<<endl;
  StudentRequest studentRequest = {0};
  int errorCode = recvfrom(
    serverSocket,
    &studentRequest,
    sizeof(StudentRequest),
    MSG_WAITALL,
    (struct sockaddr *)serverStorage,
     &addr_size);
  
  if(errorCode == -1) {
      SocketHelper::printError("handleDisplayStudentId_recv");
      return -30;
  }
  
  STUDENT_MAP::iterator it;
  it = studentDatabase.find(studentRequest.studentId);

  if (it != studentDatabase.end()) {
      socklen_t addr_size = sizeof (sockaddr_storage);
      RequestHeader studentRequest = {.code=Action_Code_Display_Id};
      
      int errorCode = sendto(
        serverSocket,
        &studentRequest,
        sizeof(RequestHeader),
        0,
        (struct sockaddr *)serverStorage,
        addr_size);
      
      if(errorCode == -1) {
        SocketHelper::printError("DisplayStudentId_send1");
        return -31;
      }
        
      int n = 1;
      errorCode = sendto(
        serverSocket,
        &n,
        sizeof(int),
        0,
        (struct sockaddr *)serverStorage,
        addr_size);

      if(errorCode == -1) {
          SocketHelper::printError("DisplayStudentId_Send2");
          return 31;
      }
      
      sendto(
        serverSocket,
        &it->second,
        sizeof(Student),
        0,
        (struct sockaddr *)serverStorage,
        addr_size);
      if(errorCode == -1) {
        SocketHelper::printError("DisplayStudentId_Send3");
        return -32;
      }
        
  } else {
    int n = 0;
    errorCode = sendto(
        serverSocket,
        &n,
        sizeof(int),
        0,
        (struct sockaddr *)serverStorage,
        addr_size);

    if(errorCode == -1) {
      SocketHelper::printError("DisplayStudentId_Send4");
      return -33;
    }
  }
  return 0;
}

int ServerChannel::handleAdd(
  sockaddr_storage* serverStorage,
  socklen_t& addr_size
) {
  Student student = {0};
  memset(&student, 0 ,sizeof(Student));
  std::cout<<"reading student add"<<endl;
  int errorCode = recvfrom(
    serverSocket,
    &student,
    sizeof(Student),
    MSG_WAITALL,
    (struct sockaddr *)serverStorage,
    &addr_size);
  cout<<"studentDatabase:empty"<<studentDatabase.empty()<<endl;
  std::cout<<"student add data was read addr_size:"<<addr_size<<endl;
  if(errorCode == -1) {
    SocketHelper::printError("handleAdd_Send");
    return -40;
  }
  cout<<"stduentid:"<<(unsigned int)student.studentId<<endl;
  cout<<"score:"<<(int)student.score<<endl;
  cout<<"firstName:"<<student.firstName<<endl;
  cout<<"lastName:"<<student.lastName<<endl;
  
  STUDENT_MAP::iterator it = this->studentDatabase.find(student.studentId);
  cout<<"done search map"<<endl;
  if(it == studentDatabase.end()) {
    cout<<"student not found"<<endl;
    if(student.score <=  100) {
      cout<<"trying to insert"<<endl;
      studentDatabase.insert(
            pair<unsigned int, Student>(
              student.studentId, student));
      std::cout<<"student was addded"<<endl;
    }
  } else {
    if(student.score <= 100) {
      cout<<"setting repeat student."<<endl;
      it->second = student;
    }
  }
  return 0;
}

int ServerChannel::handleDelete(
  sockaddr_storage* serverStorage,
  socklen_t& addr_size
) {
  StudentRequest studentRequest = {0};
  int errorCode = recvfrom(
    serverSocket,
    &studentRequest,
    sizeof(StudentRequest),
    MSG_WAITALL,
    (struct sockaddr *)serverStorage,
     &addr_size);
  
  if(errorCode == -1) {
    SocketHelper::printError("handleDelete_Recv");
    return -41;
  }
  STUDENT_MAP::iterator it;
  it = studentDatabase.find(studentRequest.studentId);
  if (it != studentDatabase.end()) {
      studentDatabase.erase(it);
  }
  return 0;
}

int ServerChannel::readData(
  sockaddr_storage* serverStorage,
  socklen_t& addr_size,
  Action_Code code
) {
  cout<<"studentDatabase:empty"<<studentDatabase.empty()<<endl;
    switch(code) {
        case Action_Code_Display_All:
            return handleDisplayAll(serverStorage,addr_size);
            break;
        case Action_Code_Display_Score:
            return handleDisplayScore(serverStorage,addr_size);
            break;
        case Action_Code_Display_Id:
            return handleDisplayStudentId(serverStorage,addr_size);
            break;
        case Action_Code_Display_Add:
            return handleAdd(serverStorage,addr_size);
            break;
        case Action_Code_Display_Delete:
            return handleDelete(serverStorage,addr_size);
            break;
    }
    return -8;
}