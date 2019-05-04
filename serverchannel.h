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
    
    int receive();
    int handleDisplayAll(sockaddr_storage* serverStorage,socklen_t& addr_size);
    int handleDisplayScore(sockaddr_storage* serverStorage,socklen_t& addr_size);
    int handleDisplayStudentId(sockaddr_storage* serverStorage,socklen_t& addr_size);
    int handleAdd(sockaddr_storage* serverStorage,socklen_t& addr_size);
    int handleDelete(sockaddr_storage* serverStorage,socklen_t& addr_size);    
    int readData(sockaddr_storage* serverStorage,socklen_t& addr_size, Action_Code code);
    void printStudents(const std::vector<Student>& studentList);
private:
int serverSocket;
sockaddr_in serverAddr;
STUDENT_MAP studentDatabase; 
};
