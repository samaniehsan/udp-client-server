#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include "common.h"

using namespace std;

class ClientChannel {
public:
    ClientChannel(const char * szAddr, const unsigned short port);
    ~ClientChannel();
    int run();
private:
    bool addStudents();
    int run(
    int clientSocket,
    const sockaddr_in& serverAddr
    );

    void initServerAddress(
    const char * szAddr,
    const unsigned short int port
    );
    bool deleteStudentById(int studentId);
    bool getStudents(const Action_Code desiredActionCode,vector<Student>& studentList);
    bool displayAllStudents();
    bool displayStudentsByScore(short score);
    bool displayStudentsById(int studentId);
    void printStudents(const vector<Student>& studentList);
private:
    int clientSocket;
    sockaddr_in serverAddr;
    socklen_t addr_size;
};
