#include <sstream>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "string"
#include <iostream>

//default constructor, initializes file descriptor (m_fd) to -1 (invalid) and sets last result to success
Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

//constructor when file descriptor is given
Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // initialize rio_t object with given file descriptor value and sets up buffer
  rio_readinitb(&this->m_fdbuf, this->m_fd);
}

//establishes connection to given hostname and port
void Connection::connect(const std::string &hostname, int port) {
  // open_clientfd given in csapp.cpp, returns integer file descriptor
  this->m_fd = open_clientfd(hostname.c_str(), std::to_string(port).c_str());
  
  // Initialize rio_t object with given buffer and file descriptor
  rio_readinitb(&this->m_fdbuf, this->m_fd);
}

//destructor, makes sure socket is truly closed
Connection::~Connection() {
  // use close method below to close if still open
  this->close();
}

bool Connection::is_open() const {
  // returns an integer value if file descriptor is non-negative
  // negative file descriptor implies invalid, aka closed or never opened in first place
  return this->m_fd >= 0;
}

// closes connection if open
void Connection::close() {
  // needs to check if the socket is open
  // checks using is_open() helper method, basically returns true if non-negative integer value
  if (this->is_open()) {
    Close(this->m_fd);
    this->m_fd = -1; // Mark as closed
  }
}

// send a message over socket
bool Connection::send(const Message &msg) {
  // format the message string as "tag:data\n" for transmission
  std::string message = msg.tag + ":" + msg.data;

  if (message.find('\n') != std::string::npos) {
    //std::cout << "new line character dawg: " << message << std::endl;
    //return false;
  } else {
    //std::cout <<"no new line character" << std::endl;
    message += '\n';
  }
  //message += '\n';
  // write message to file descriptor using outside method and unbuffered I/O
  ssize_t msgWrite = rio_writen(this->m_fd, message.c_str(), message.size()); //write n bytes NOT buffered

  //std::cout << "sent: "<< message;
  //std::cout <<"size of message sent" << msgWrite;

  //msgWrite returns the string written, so need to check the size. If not equal, error
  if (msgWrite != (ssize_t)message.size()) { //function returns length of written if successful
    // Set error status for last result in connection
    this->m_last_result = EOF_OR_ERROR;
    return false;
  }

  this->m_last_result = SUCCESS;
  return true;
}

// Receives a message from the connection and stores it in the provided Message object
bool Connection::receive(Message &msg) {

  // create buffer w/ size of message
  char buffer[msg.MAX_LEN + 1];
  ssize_t msgLength = rio_readlineb(&this->m_fdbuf, buffer, msg.MAX_LEN); //read line


  // check if read failed
  if (msgLength <= 0) { //msgRead >= 0 success according to function
    this -> m_last_result = EOF_OR_ERROR;
    return false;
  }

  
  buffer[msgLength] = '\0'; //buffer it just in case
  std::string receivedMsg = std::string(buffer);
  //std::cout << "recieved" << buffer;
  // we need to split message up into tag, data, etc
  // therefore have to find location of ":" separator
  size_t pos = receivedMsg.find(':');
  // case where ":" is not found, aka error
  if (pos == std::string::npos) {
    this -> m_last_result = EOF_OR_ERROR;
    return false;
  }

  // extract tag and message using substring
  msg.tag = receivedMsg.substr(0, pos);
  msg.data = receivedMsg.substr(pos + 1);

  //std::cout << buffer << std::endl; //for debugging

  msg.recievedData = true;
  this -> m_last_result = SUCCESS;
  return true;
}
