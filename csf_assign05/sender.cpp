#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

void getCommand(const std::string &message, std::string &command){
  //join, leave, quit
  if (message.rfind("/join", 0) == 0){
    command = std::string(TAG_JOIN);
  } else if (message.rfind("/leave", 0) == 0){
    command = std::string(TAG_LEAVE);
  } else if (message.rfind("/quit", 0) == 0){
    command = std::string(TAG_QUIT);
  } else {
    command = std::string(TAG_ERR);
  }
}

void getData(const std::string &message, const std::string &command, std::string &data){
  if (command == TAG_JOIN) {
    data = message.substr(6); // /join_
  } else if (command == TAG_LEAVE) {
    data = "leaving"; // /leave_
  } else if (command == TAG_QUIT) {
    data = "quitting"; // /quit_
  }
}

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  // TODO: connect to server
  //1. connect to server
  Connection serverConnection;
  serverConnection.connect(server_hostname, server_port);

  // TODO: send slogin message
  //2. send slogin message
  Message message = Message(TAG_SLOGIN, username);
  serverConnection.send(message);
  serverConnection.receive(message);
  if (message.tag == TAG_ERR) {
    std::cout << "error: " << message.data << std::endl;
    return 1;
  } 

  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate

  //4. loop - including join room
  while (1) {
    std::string line;
    std::getline(std::cin, line);

    if (line[0] != '/') {
      message.tag = TAG_SENDALL;
      message.data = line;
    } else {
      getCommand(line, message.tag);
      getData(line, message.tag, message.data);
    }
    
    if (message.tag == TAG_QUIT) {
      serverConnection.send(message);
      serverConnection.receive(message);
      break;
    }
    serverConnection.send(message);
    serverConnection.receive(message);
    if (message.tag == TAG_ERR) {
      std::cout << "error: " << message.data << std::endl;
      return 1;
    }
  }
  return 0;
}

//./sender localhost 33333 alice
//nc -l -p 33333
//scp jshi61@ugradx.cs.jhu.edu:~/CSF/csf_assign05/solution.zip .