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
    struct sockaddr_in* serverAddr,
    const socklen_t addr_size,
    unsigned int studentId,
    const char * firstName,
    const char * lastName,
    BYTE score
) {
    cout<<"sending student:"<<studentId<<endl;
    RequestHeader header = {.code=Action_Code_Display_Add};
    int nCode = sendto(
        socket,
        &header,
        sizeof(RequestHeader),
        0,
        (struct sockaddr *)serverAddr,
        addr_size);
    if(nCode != -1) {
        Student student;
        student.studentId = studentId;
        strncpy(student.firstName,firstName,11);
        strncpy(student.lastName,lastName,11);
        student.score = score;
        int nCode = sendto(
            socket,
            &student,
            sizeof(Student),
            0,
            (struct sockaddr *)serverAddr,
            addr_size);
        if(nCode != -1) {
            return true;
        } else {
            SocketHelper::printError("addStudent_2");
        }
    }
    SocketHelper::printError("addStudent_1");
    return false;
}



bool
SocketHelper::deleteStudent(
    int socket,
    struct sockaddr_in* serverAddr,
    const socklen_t addr_size,
    unsigned int studentId
) {
    cout<<"sending delete student:"<<studentId<<endl;
    RequestHeader header = {.code=Action_Code_Display_Delete};
    int nCode = sendto(
        socket,
        &header,
        sizeof(RequestHeader),
        0,
        (struct sockaddr *)serverAddr,
        addr_size);

    if(nCode != -1) {
        StudentRequest student = {studentId};
        int nCode = sendto(
            socket,
            &student,
            sizeof(Student),
            0,
            (struct sockaddr *)serverAddr,
            addr_size);
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
SocketHelper::sendAllStudentsRequest(
    int socket,
    struct sockaddr_in* serverAddr,
    const socklen_t addr_size
) {
    cout<<"sending display all students"<<endl;
    RequestHeader header = {.code=Action_Code_Display_All};
    int nCode = sendto(
        socket,
        &header,
        sizeof(RequestHeader),
        0,
        (struct sockaddr *)serverAddr,
        addr_size);
    if(nCode != -1) {
        return true;
    }
    SocketHelper::printError("sendAllStudentsRequest");
    return false;
}

bool
SocketHelper::sendStudentsByScoreRequest(
    int socket,
    struct sockaddr_in* serverAddr,
    const socklen_t addr_size,
    BYTE score
) {
    cout<<"sending display all students above score:"<<score<<endl;
    RequestHeader header = {.code=Action_Code_Display_Score};
    int nCode = sendto(
        socket,
        &header,
        sizeof(RequestHeader),
        0,
        (struct sockaddr *)serverAddr,
        addr_size);
    if(nCode != -1) {
        ScoreRequest request = {.score=score};
        nCode = sendto(
            socket,
            &request,
            sizeof(ScoreRequest),
            0,
            (struct sockaddr *)serverAddr,
            addr_size);
        if(nCode != -1) {
            return true;
        }
    }
    SocketHelper::printError("sendStudentsByScoreRequest");
    return false;
}

bool
SocketHelper::sendStudentsByStudentIdRequest(
    int socket,
    struct sockaddr_in* serverAddr,
    const socklen_t addr_size,
    const unsigned int studentId
) {
    cout<<"sending display student with id:"<<studentId<<endl;
    RequestHeader header = {.code=Action_Code_Display_Id};
    int nCode = sendto(
        socket,
        &header,
        sizeof(RequestHeader),
        0,
        (struct sockaddr *)serverAddr,
        addr_size);
    if(nCode != -1) {
        StudentRequest request = {.studentId=studentId};
        nCode = sendto(
            socket,
            &request,
            sizeof(StudentRequest),
            0,
            (struct sockaddr *)serverAddr,
            addr_size);
        if(nCode != -1) {
            return true;
        }
    }
    SocketHelper::printError("sendStudentsByStudentIdRequest");
    return false;
}


bool
SocketHelper::receiveActionCode(
    int socket,
    struct sockaddr_in* serverAddr,
    const socklen_t addr_size,
    Action_Code& actionCode
) {
    RequestHeader header = {0};
    int errorCode = recvfrom(socket,&header,sizeof(RequestHeader),0, NULL, NULL);
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

bool
SocketHelper::receiveStudentsCount(
    int socket,
    struct sockaddr_in* serverAddr,
    const socklen_t addr_size,
    int & nCount
) {
    int n = 0;
    int nCode = recvfrom(socket,&n,sizeof(int),0,NULL, NULL);
    if(nCode != -1) {
        cout<<"receieved Students Count:"<<n<<endl;
        nCount = n;
        return true;
    }
    SocketHelper::printError("receiveStudentsCount");
    return false;
}

bool
SocketHelper::receiveStudentInfo(
    int socket,
    struct sockaddr_in* serverAddr,
    const socklen_t addr_size,
    Student & student
) {
    int nCode = recvfrom(socket,&student,sizeof(Student),0,NULL, NULL);
    cout<<"received "<<nCode<<" bytes."<<endl;
    if(nCode != -1) {
        cout<<"student.id"<<student.studentId<<endl;
        return true;
    }
    SocketHelper::printError("receiveStudentInfo");
    return false;
}
