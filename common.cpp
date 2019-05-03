#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "common.h"

using namespace std; 

int SocketHelper::printError(const char * errTitle) {
  char * pszErrorString = strerror(errno);
  std::cerr<<"Error"<<" "<<errTitle<<":"<<pszErrorString<<endl;
}

bool
SocketHelper::addStudent(
  int socket,
  unsigned int studentId,
  const char * firstName,
  const char * lastName,
  BYTE score) {
    cout<<"sending student:"<<studentId<<endl;
    RequestHeader header = {.code=Action_Code_Display_Add};
    int nCode = send(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        Student student;
        student.studentId = studentId;
        strncpy(student.firstName,firstName,11);
        strncpy(student.lastName,lastName,11);
        student.score = score;
        int nCode = send(socket,&student,sizeof(Student),0);
        if(nCode != -1) {
            return true;
        } else {
            SocketHelper::printError("addStudent_2");
        }
    }
    SocketHelper::printError("addStudent_1");
    return false;
}



bool SocketHelper::deleteStudent(int socket,  unsigned int studentId) {
    cout<<"sending delete student:"<<studentId<<endl;
    RequestHeader header = {.code=Action_Code_Display_Delete};
    int nCode = send(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        StudentRequest student = {studentId};
        int nCode = send(socket,&student,sizeof(Student),0);
        if(nCode != -1) {
            return true;
        } else {
            SocketHelper::printError("deleteStudent_2");
        }
    }
    SocketHelper::printError("deleteStudent_1");
    return false;
}


bool
SocketHelper::sendAllStudentsRequest(int socket) {
    cout<<"sending display all students"<<endl;
    RequestHeader header = {.code=Action_Code_Display_All};
    int nCode = send(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        return true;
    }
    SocketHelper::printError("sendAllStudentsRequest");
    return false;
}

bool SocketHelper::sendStudentsByScoreRequest(int socket, BYTE score) {
    cout<<"sending display all students above score:"<<score<<endl;
    RequestHeader header = {.code=Action_Code_Display_Score};
    int nCode = send(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        ScoreRequest request = {.score=score};
        nCode = send(socket,&request,sizeof(ScoreRequest),0);
        if(nCode != -1) {
            return true;
        }
    }
    SocketHelper::printError("sendStudentsByScoreRequest");
    return false;
}

bool SocketHelper::sendStudentsByStudentIdRequest(int socket,const unsigned int studentId) {
    cout<<"sending display student with id:"<<studentId<<endl;
    RequestHeader header = {.code=Action_Code_Display_Id};
    int nCode = send(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        StudentRequest request = {.studentId=studentId};
        nCode = send(socket,&request,sizeof(StudentRequest),0);
        if(nCode != -1) {
            return true;
        }
    }
    SocketHelper::printError("sendStudentsByStudentIdRequest");
    return false;
}


bool SocketHelper::receiveActionCode(int socket, Action_Code& actionCode) {
    RequestHeader header = {0};
    int errorCode = recv(socket,&header,sizeof(RequestHeader),0);
    cout<<"receiveActionCode errorCode:"<<errorCode<<" actionCode:"<<(int)header.code<<endl;
    if(errorCode != -1) {
        actionCode = (Action_Code)header.code;
        if(actionCode >= Action_Code_Display_All ) {
            return true;
        }
        std::cerr<<"wrong action code:"<<actionCode<<endl;
        return false;
    }
    SocketHelper::printError("receiveActionCode");
    return false;
}

bool SocketHelper::receiveStudentsCount(int socket, int & nCount) {
    int n = 0;
    int nCode = recv(socket,&n,sizeof(int),0);
    if(nCode != -1) {
        cout<<"receieved Students Count:"<<n<<endl;
        nCount = n;
        return true;
    }
    SocketHelper::printError("receiveStudentsCount");
    return false;
}

bool SocketHelper::receiveStudentInfo(int socket,Student & student) {
    int nCode = recv(socket,&student,sizeof(Student),0);
    cout<<"received "<<nCode<<" bytes."<<endl;
    if(nCode != -1) {
        cout<<"student.id"<<student.studentId<<endl;
        return true;
    }
    SocketHelper::printError("receiveStudentInfo");
    return false;
}