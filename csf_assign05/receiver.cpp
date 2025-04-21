#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"


// parsed chat message with room, sender, and content
struct ChatMessage {
  std::string room;
  std::string sender;
  std::string content;
};

// method to parse message into room, sender, and string components
bool readMessage(std::string &data, ChatMessage& message){
  //1. separate room : sender + message
  size_t pos = data.find(':');
  // message should use : as a delimiter, if not there throw error
  if (pos == std::string::npos){
    return false;
  }
  // room is in first part of message, room name
  message.room = data.substr(0,pos);

  //2. separate sender : message
  std::string senderAndMessageString = data.substr(pos + 1);
  pos = senderAndMessageString.find(':');
  if (pos == std::string::npos){
    return false; // no delimiter found, invalid format so return false for error
  }

  // uses another delimiter in second half of message to extract sender tag and data
  message.sender = senderAndMessageString.substr(0,pos);
  message.content = senderAndMessageString.substr(pos + 1);
  return true;
}

// main function for receiver client
int main(int argc, char **argv) {
  // checks to see if another arguments provided
  if (argc != 5) {
    // improper call of method
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  // store command line arguments
  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  // Create connection to server
  Connection serverConnection;
  serverConnection.connect(server_hostname, server_port);

  // check to see if connection was successful
  if (!serverConnection.is_open()){
    std::cerr << "failed to connect to server" << std::endl;
    return 1;
  }

  // Send rlogin message to authenticate user
  Message login = Message(TAG_RLOGIN, username);
  if (!serverConnection.send(login)){
    std::cerr << "failed to send login message" << std::endl;
    return 1;
  }
  if (!serverConnection.receive(login)){
    std::cerr << "failed to receive login confirmation" << std::endl;
    return 1;
  }
  if (login.tag == TAG_ERR) {
    std::cerr << login.data;
    return 1;
  }

  //2. Send join message to join the specified room
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

  
  // Main loop to receive and process messages from the server
  //       (which should be tagged with TAG_DELIVERY)
  while (true) {
    Message loopMessage;
    if (!serverConnection.receive(loopMessage)){
      std::cerr << "failed to recieve join confirmation" << std::endl;
      // Continue loop on receive failure
    }

    if (loopMessage.tag == TAG_ERR) {
      std::cerr << loopMessage.data;
      // Continue loop on receive failure
    }

    if (loopMessage.tag != TAG_DELIVERY) {
      std::cerr << "Unexpected message tag: " << loopMessage.tag << '\n';
      continue;
    }

    ChatMessage chat_message;
    // Parse the received message (expected format: room:sender:message)
    bool success = readMessage(loopMessage.data, chat_message);
    if (!success){
      std::cerr << "error in reading message";
      continue; // Skip to next message on parse failure
    }
    // Print the sender and message to console
    std::cout << chat_message.sender << ": " << chat_message.content;
  }
  
  return 0;
}

//./receiver localhost 33333 alice cafe
//nc -l -p 33333
//scp jshi61@ugradx.cs.jhu.edu:~/CSF/csf_assign05/solution.zip .
//delivery:cafe:sender:test message
