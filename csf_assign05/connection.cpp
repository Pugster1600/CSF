#include <sstream>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "string"
#include <iostream>
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

  if (msgWrite != (ssize_t)message.size()) { //function returns length of written if successful
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

  //std::cout << buffer << std::endl; //for debugging

  msg.recievedData = true;
  this -> m_last_result = SUCCESS;
  return true;
}
