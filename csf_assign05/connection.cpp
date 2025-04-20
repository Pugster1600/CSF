#include <sstream>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "string"
#include <iostream>

/**

we are writing the code for the client
meaning that when we type ./sender [server_address] [port] [username]
this then sends us to be a sender client

now that we are a sender client, we can login to the server with slogin
once we login, the server needs to ok

test1 - netcat as the server itself and a terminal as the sende 
NO RECIEVER
so we run netcat as the server and we host it on the port 33333
> (nc -l -p 33333)
BASICALLY SENDER TEST
> 1. we type ./sender
> 1.1. netcat recieves it and must respond with ok:[message]
in the video, we need to manually type it in the netcat terminal
> 2. now that necat ok, we can /join [room]
> 2.1. netcat ok:ok
so basically we talk to the server aka netcat and netcat will respond 

test2 - netcat as the reciever, so we have the server set up
another terminal as the sender
NO SERVER (server already implemented)
BASICALLY SERVER TEST
> 1. sender terminal, ./sender
> 1.1 netcat reciver, start as reciever (nc localhost 33333)
> 2.1 sender ./join
> 2.2 reciever in netcat rlogin
> 3. sender sends a messgae 
> the server should relay it to everyone, 
so the netcat terminal should be able to see the message

test3 - netcat as both sender and reciever
nc localhost 33333
> 1. slogin and rloing in both terminals
> 2. test send message in slogin
> you should see the server respond back with ok:message sent
> NOTE: server only responds back for netcat since we are in netcat!

so netcat emulates a client
so if it acts as a server, we must manually respond with what the server should respond with
if it acts as reciever, we must manually respond with what the reciever client does etc

When the port is passed as a char* (string), it's usually used by higher-level APIs like getaddrinfo() that can take port numbers as strings or even service names.

so the way it works is the port aka 33333 is opened as the file
> we basically write to the port and its treated as a fd

client means you are initiaing connection either as sender or reciever

nc localhost 1234 -> client (becuase you are connecting to a port/machine ie fd/socket)
nc -l 1234 -> server (becuase you are listening to the port ie fd/socket)
> so we are hosting the server on port 1234 ie all data is being fed to that file descriptor/socket


You create a socket

Connect it to localhost:1234

Send data over the socket

just spam write code dont care about optimizaiton until later!
get the code working first!
this principle applies everywhere
*/
Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&this->m_fdbuf, this->m_fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // TODO: call open_clientfd to connect to the server
  this->m_fd = open_clientfd(hostname.c_str(), std::to_string(port).c_str());
  
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&this->m_fdbuf, this->m_fd);
}

Connection::~Connection() {
  // TODO: close the socket if it is open
  this->close();
}

bool Connection::is_open() const {
  // TODO: return true if the connection is open
  return this->m_fd >= 0;
}

void Connection::close() {
  // TODO: close the connection if it is open
  if (this->is_open()) {
    Close(this->m_fd);
    this->m_fd = -1; // Mark as closed
  }

}

bool Connection::send(const Message &msg) {
  // TODO: send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  std::string message = msg.tag + ":" + msg.data + "\n";
  ssize_t msgWrite = rio_writen(this->m_fd, message.c_str(), message.size()); //write n bytes NOT buffered

  if (msgWrite != (ssize_t)message.size()) { //function returns length of written if successful (read it!)
    this->m_last_result = EOF_OR_ERROR;
    return false;
  }

  this->m_last_result = SUCCESS;
  return true;
}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  char buffer[msg.MAX_LEN + 1];
  //ssize_t msgLength = rio_readnb(&this -> m_fdbuf, buffer, msg.MAX_LEN); //read n bytes buffered -> buffer is basically for us to use while m_fdbuf is like the actual one
  //ssize_t msgLength = rio_readn(this -> m_fd, buffer, msg.MAX_LEN); //read n bytes not buffered
  ssize_t msgLength = rio_readlineb(&this->m_fdbuf, buffer, msg.MAX_LEN); //read line

  if (msgLength <= 0) { //msgRead >= 0 success according to function
    this -> m_last_result = EOF_OR_ERROR;
    return false;
  }

  buffer[msgLength] = '\0'; //buffer it just in case
  std::string receivedMsg = std::string(buffer);
  size_t pos = receivedMsg.find(':');
  if (pos == std::string::npos) {
    this -> m_last_result = EOF_OR_ERROR;
    return false;
  }

  msg.tag = receivedMsg.substr(0, pos);
  msg.data = receivedMsg.substr(pos + 1);

  std::cout << buffer << std::endl; //NOTE: DONT NEED THIS JUST FOR DEBUG REASONS!

  msg.recievedData = true;
  this -> m_last_result = SUCCESS;
  return true;
}
