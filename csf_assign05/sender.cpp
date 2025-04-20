#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

bool getCommand(const std::string &message, std::string &command){
  //join, leave, quit
  if (message.rfind("/join", 0) == 0){
    command = std::string(TAG_JOIN);
  } else if (message.rfind("/leave", 0) == 0){
    command = std::string(TAG_LEAVE);
  } else if (message.rfind("/quit", 0) == 0){
    command = std::string(TAG_QUIT);
  } else {
    return false;
  }
  return true;
}

void getData(const std::string &message, const std::string &command, std::string &data){
  if (command == TAG_JOIN) {
    data = message.substr(6);     // /join_
  } else if (command == TAG_LEAVE) {
    data = "leaving";             // /leave_
  } else if (command == TAG_QUIT) {
    data = "quitting";            // /quit_
  }
}

bool sendAndReadMessage(Message & message, Connection & serverConnection){
  if (!serverConnection.send(message)){
    std::cerr << "failed to send message" << std::endl;
    return false;
  }

  if (!serverConnection.receive(message)){
    std::cerr << "failed to recieve message" << std::endl;
    return false;
  }

  if (message.tag == TAG_ERR) {
    std::cerr << message.data;
    return false;
  }

  return true;
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

  // TODO: 1. connect to server
  Connection serverConnection;
  serverConnection.connect(server_hostname, server_port);

  if (!serverConnection.is_open()){
    std::cerr << "failed to connect to server" << std::endl;
    return 1;
  }

  // TODO: 2. send slogin message
  Message login = Message(TAG_SLOGIN, username);
  if (!serverConnection.send(login)) {
    std::cerr << "failed to send login message" << std::endl;
    return 1;
  }
  if (!serverConnection.receive(login)) {
    std::cerr << "failed to reciever login confirmation" << std::endl;
  }
  if (login.tag == TAG_ERR) {
    std::cerr << login.data;
    return 1;
  } 

  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
  //3. loop - including join room
  while (1) {
    Message loopMessage;
    std::string line;
    std::getline(std::cin, line);

    if (line[0] != '/') {
      loopMessage.tag = TAG_SENDALL;
      loopMessage.data = line;
    } else {
      bool success = getCommand(line, loopMessage.tag);
      if (!success) {
        std::cerr << "invalid command";
        continue;
      }
      getData(line, loopMessage.tag, loopMessage.data);
    }
    
    if (loopMessage.tag == TAG_QUIT) {
      serverConnection.send(loopMessage);
      serverConnection.receive(loopMessage);
      break;
    }

    if (!serverConnection.send(loopMessage)) {
      std::cerr << "sending error";
    }
    
    if (!serverConnection.receive(loopMessage)) {
      std::cerr << "recieving error";
    }

    if (loopMessage.tag == TAG_ERR) {
      std::cerr << loopMessage.data;
    }
  }

  return 0;
}

//./sender localhost 33333 alice
//nc -l -p 33333
//scp jshi61@ugradx.cs.jhu.edu:~/CSF/csf_assign05/solution.zip .