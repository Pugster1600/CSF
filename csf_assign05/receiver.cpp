#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

bool readMessage(std::string &data, std::string& room, std::string &sender, std::string &message){
  //1. separate room : sender + message
  size_t pos = data.find(':');
  if (pos == std::string::npos){
    return false;
  }
  room = data.substr(0,pos);

  //2. seprate sender : message
  std::string senderAndMessageString = data.substr(pos + 1);
  pos = senderAndMessageString.find(':');
  if (pos == std::string::npos){
    return false;
  }

  sender = senderAndMessageString.substr(0,pos);
  message = senderAndMessageString.substr(pos + 1);
  return true;
}

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  // TODO: connect to server
  Connection serverConnection;
  serverConnection.connect(server_hostname, server_port);

  if (!serverConnection.is_open()){
    std::cerr << "failed to connect to server" << std::endl;
    return 1;
  }


  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)
  //1. login
  Message login = Message(TAG_RLOGIN, username);
  if (!serverConnection.send(login)){
    std::cerr << "failed to send login message" << std::endl;
    return 1;
  }
  if (!serverConnection.receive(login)){
    std::cerr << "failed to recieve login confirmation" << std::endl;
    return 1;
  }
  if (login.tag == TAG_ERR) {
    std::cerr << login.data;
    return 1;
  }

  //2. join
  Message join = Message(TAG_JOIN, room_name); //rlogin:username
  if (!serverConnection.send(join)){
    std::cerr << "failed to send join message" << std::endl;
    return 1;
  }
  if (!serverConnection.receive(join)){
    std::cerr << "failed to recieve join confirmation" << std::endl;
    return 1;
  }
  if (join.tag == TAG_ERR) {
    std::cerr << join.data;
    return 1;
  }

  
  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  while (1) {
    Message loopMessage;
    if (!serverConnection.receive(loopMessage)){
      std::cerr << "failed to recieve join confirmation" << std::endl;
    }

    if (loopMessage.tag == TAG_ERR) {
      std::cerr << loopMessage.data;
    }

    std::string room;
    std::string sender;
    std::string message;
    bool success = readMessage(loopMessage.data, room, sender, message);
    if (!success){
      std::cerr << "error in reading message";
      continue;
    }
    std::cout << sender << ": " << message;
  }
  return 0;
}

//./receiver localhost 33333 alice cafe
//nc -l -p 33333
//scp jshi61@ugradx.cs.jhu.edu:~/CSF/csf_assign05/solution.zip .
//delivery:cafe:sender:test message