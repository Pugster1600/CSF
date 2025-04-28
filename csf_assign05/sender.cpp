#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"


// extracts command from user input if it starts with a slash command
// parameters: message (input string), command (output string)
// Returns: true if valid command found, false otherwise
bool getCommand(const std::string &message, std::string &command){
  // Check if message starts with /join, /leave, or /quit
  if (message.rfind("/join", 0) == 0){
    command = std::string(TAG_JOIN); // set command to join tag
  } else if (message.rfind("/leave", 0) == 0){
    command = std::string(TAG_LEAVE); // set command to leave tag
  } else if (message.rfind("/quit", 0) == 0){
    command = std::string(TAG_QUIT); // set command to quit tag
  } else {
    return false;
  }
  return true;
}

// gets data from user input based on command type
// message (input string), command (command type), data (output string)
void getData(const std::string &message, const std::string &command, std::string &data){
  if (command == TAG_JOIN) {
    data = message.substr(6);     // Extract channel name after "/join "
  } else if (command == TAG_LEAVE) {
    data = "leaving";             // Set fixed data for leave command
  } else if (command == TAG_QUIT) {
    data = "quitting";            // Set fixed data for quit command
  }
}



// sends message to server and gets response
bool sendAndReadMessage(Message & message, Connection & serverConnection){
  if (!serverConnection.send(message)){
    std::cerr << "failed to send message" << std::endl;
    return false; // message send failed
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

// main function for sender client
int main(int argc, char **argv) {
  // checks number of command line arguments
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


  // connects to server using hostname and port
  Connection serverConnection;
  serverConnection.connect(server_hostname, server_port);

  // check if connection established, aka file descriptor >= 0 
  if (!serverConnection.is_open()){
    std::cerr << "failed to connect to server" << std::endl;
    return 1;
  }

  // Needs to send slogin message in order to initiate connection w/ server
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

  // Main loop for sender
  // the loop reads commands from user, sending messages to server as appropriate
  // loop - including join room
  while (1) {
    Message loopMessage;
    std::string line;
    std::getline(std::cin, line); // read user input

    if (line[0] != '/') { // not starting w/ / means non command message
      loopMessage.tag = TAG_SENDALL;
      loopMessage.data = line;
    } else { // else is a command message, join, leave, etc
      bool success = getCommand(line, loopMessage.tag);
      if (!success) {
        std::cerr << "invalid command";
        continue;
      }
      getData(line, loopMessage.tag, loopMessage.data);
    }
    
    // handle quit command
    if (loopMessage.tag == TAG_QUIT) {
      serverConnection.send(loopMessage);
      serverConnection.receive(loopMessage);
      break; // exit loop
    }

    if (!serverConnection.send(loopMessage)) { // send message to server
      // send() returned error
      std::cerr << "sending error";
    }
    
    if (!serverConnection.receive(loopMessage)) {
      // didn't get message back, aka no handshake on other end
      std::cerr << "recieving error";
    }

    // tag is changed to error on server side if something happened
    if (loopMessage.tag == TAG_ERR) {
      std::cerr << loopMessage.data;
    }
  }

  return 0;
}

//./sender localhost 12333 alice
//./sender localhost 33333 alice
//nc -l -p 33333
//scp jshi61@ugradx.cs.jhu.edu:~/CSF/csf_assign05/solution.zip .
//set breakpoint in the worker