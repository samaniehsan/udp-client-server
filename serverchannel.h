#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <vector> 

#include "common.h"

typedef std::map<unsigned int, Student> STUDENT_MAP;
class ServerChannel {
public:
    ServerChannel(const unsigned short port);
    ~ServerChannel();
    int run();

private:
    bool addStudents(int clientSocket);
    int run(
    int clientSocket,
    const sockaddr_in& serverAddr
    );
    void initServerAddress(const unsigned short int port);
    
    int handleDisplayAll(sockaddr_storage* serverStorage);
    int handleDisplayScore(sockaddr_storage* serverStorage);
    int handleDisplayStudentId(sockaddr_storage* serverStorage);
    int handleAdd(sockaddr_storage* serverStorage);
    int handleDelete(sockaddr_storage* serverStorage);    
    int receive();
    int readData(sockaddr_storage* serverStorage, Action_Code code);
    void printStudents(const std::vector<Student>& studentList);
private:
int serverSocket;
sockaddr_in serverAddr;
STUDENT_MAP studentDatabase; 
};
