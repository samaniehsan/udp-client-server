#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "clientchannel.h"

using namespace std;

ClientChannel::ClientChannel(
  const char * szAddr,
  const unsigned short int nPort
):clientSocket(-1) {

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
  initServerAddress(&szAddr[0],nPort);
  addr_size = sizeof(this->serverAddr);
}

ClientChannel::~ClientChannel() {
  close(clientSocket);
}

void ClientChannel::initServerAddress(
  const char * szAddr,
  const unsigned short int port
) {
  
   /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  this->serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  this->serverAddr.sin_port = htons(port);
  /* Set IP address to localhost */
  this->serverAddr.sin_addr.s_addr = inet_addr(szAddr);
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
}

bool ClientChannel::getStudents(
  const Action_Code desiredActionCode,
  vector<Student> & studentList
) {
  Action_Code code = (Action_Code)0;
  if(!SocketHelper::receiveActionCode(clientSocket,&serverAddr,addr_size, code)) {
    return false;
  }

  if(code != desiredActionCode) {
    std::cerr<<"Server Action Code:"<<code<<" desired Action Code:"<<desiredActionCode<<endl;
    return false;
  }

  int nCount = 0;
  if(!SocketHelper::receiveStudentsCount(clientSocket,&serverAddr,addr_size,nCount)) {
    return false;
  }
  
  int i;
  for(i = 0; i < nCount; i++ ) {
    Student student;
    if(!SocketHelper::receiveStudentInfo(clientSocket,&serverAddr,addr_size, student)) {
      std::cerr<<"cannot receive student:"<<i+1<<"of"<<nCount<<endl;
      return false;
    }
    studentList.push_back(student);
  }
  return true;
}

void ClientChannel::printStudents(const vector<Student>& studentList) {
  cout<<"****students****"<<endl;
  for (std::vector<Student>::const_iterator it=studentList.begin(); it!=studentList.end(); it++) {
    cout
    <<"id:"<<it->studentId
    <<" firstName:"<<
    it->firstName
    <<" lastName:"<<it->lastName
    <<" score:"<<(int)it->score<<
    endl;
  }
  cout<<"****----------****"<<endl;
}

bool ClientChannel::addStudents() {
  const int nStudents= 10;
  Student students[nStudents] = {
      {100000,"John","Smith",10},
      {100001,"Jane","Moore",20},
      {100002,"Alex","Holland",30},
      {100003,"Alex","Bear",40},
      {100004,"Cody","White",50},
      {100005,"Kyle","Black",60},
      {100006,"Shannon","O'Reilly",70},
      {100007,"Tim","Duncan",80},
      {100008,"Manu","Ginobili",90},
      {100009,"Tony","Parker",100}     
  };
  int i;
  for(i=0; i<nStudents; i++) {
    const Student & student = students[i];
    if(!SocketHelper::addStudent(
      clientSocket,
      &serverAddr,
      addr_size,
      student.studentId, 
      student.firstName,
      student.lastName,
      student.score)) {
        return false;
      }
  }
  return true;
}

bool ClientChannel::deleteStudentById(int studentId) {
  if(!SocketHelper::deleteStudent(clientSocket,&serverAddr,addr_size,studentId)) {
    std::cerr<<"Failed to sendDeleteStudentRequest"<<endl;
    return false;
  }
  cout<<"deleteStudent By Id:"<<studentId<<endl;
  return true;
}

bool ClientChannel::displayAllStudents() {
  if(!SocketHelper::sendAllStudentsRequest(
    clientSocket,
    &serverAddr,
    addr_size
    )) {
    std::cerr<<"Failed to getStudents"<<endl;
    return false;
  }

  vector<Student> studentList;
  if(!getStudents(Action_Code_Display_All, studentList)) {
    std::cerr<<"Failed to getStudents"<<endl;
    return false;
  }
  cout<<"AllStudents"<<endl;
  printStudents(studentList);
  return true;
}

bool ClientChannel::displayStudentsByScore(BYTE score) {
  if(!SocketHelper::sendStudentsByScoreRequest(
    clientSocket,
    &serverAddr,
    addr_size,
    score)) {
    std::cerr<<"Failed to getStudents"<<endl;
    return false;
  }

  std::vector<Student> studentList;
  if(!getStudents(Action_Code_Display_Score, studentList)) {
    std::cerr<<"Failed to getStudents"<<endl;
    return false;
  }
  cout<<"Students above score:"<<score<<endl;
  printStudents(studentList);
  return true;
}

bool ClientChannel::displayStudentsById(int studentId) {
  if(!SocketHelper::sendStudentsByStudentIdRequest(
    clientSocket,
    &serverAddr,
    addr_size,
    studentId)) {
    std::cerr<<"Failed to getStudents"<<endl;
    return false;
  }

  std::vector<Student> studentList;
  if(!getStudents(Action_Code_Display_Id, studentList)) {
    std::cerr<<"Failed to getStudents"<<endl;
    return false;
  }
  cout<<"Students by Id:"<<studentId<<endl;
  printStudents(studentList);
  return true;
}

int ClientChannel::run(
) {

  cout<<"adding students"<<endl;
  if(!addStudents()) {
    std::cerr<<"AddStudents Failed"<<endl;
    return 4;
  }

  // cout<<"display allstudents"<<endl;
  // if(!displayAllStudents()) { 
  //   std::cerr<<"displayAllStudents Failed"<<endl;
  //   return 5;
  // }

  // cout<<"display students by score"<<endl;
  // if(!displayStudentsByScore(0)) {
  //   std::cerr<<"displayStudentsByScore Failed"<<endl;
  //   return 6;
  // }
  
  // cout<<"display students by id"<<endl;
  // if(!displayStudentsById(100000)) {
  //   std::cerr<<"displayStudentsById Failed"<<endl;
  //   return 7;
  // }

  // cout<<"delete student by id:100,000"<<endl;
  // if(!deleteStudentById(100000)) {
  //   std::cerr<<"deleteStudentByID Failed"<<endl;
  //   return 8;
  // }
  
  // cout<<"display all students"<<endl;
  // if(!displayAllStudents()) {
  //   std::cerr<<"displayAllStudents_2 Failed"<<endl;
  //   return 9;
  // }

  int errorCode = shutdown(clientSocket, 0);
  if(errorCode == -1) {
    SocketHelper::printError("shutdown");
    return errorCode;
  }
  cout<<"$$$$$$$$$$ Success; Exiting! $$$$$$$$$$$"<<endl;
  return 0;
}