#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

struct ClientInfo {
  Connection *conn;     // Connection object for the client
  Server *server;       // Pointer to server instance
  
  ClientInfo(Connection *conn, Server *server) 
      : conn(conn), server(server) {}
};

namespace {

// Process sender client messages
void chat_with_sender(Connection *conn, Server *server, const std::string &username) {

  
}

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)

  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // TODO: initialize mutex
  if (pthread_mutex_init(&this -> m_lock, NULL) != 0) {
    std::cerr << "mutex init failed" << std::endl;
  }
}

Server::~Server() {
  // destroy mutex
  pthread_mutex_destroy(&this -> m_lock);
    
  // Close server socket if open
  if (m_ssock >= 0) {
      close(m_ssock);
  }
  
  // Free all Room objects
  for (auto it = m_rooms.begin(); it != m_rooms.end(); ++it) {
      delete it->second;
  }
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  char port_str[20]; //so port number is an int, but we need to convert to string for open_listenfd
  //yeah listenfd takes string port number
  //need to convert
  m_ssock = open_listenfd(port_str);
  return m_ssock >= 0;
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  while(true){
    int client_fd = Accept(m_ssock, nullptr, nullptr);
    if (client_fd < 0) {
      std::cerr << "Error accepting connection" << std::endl;
      continue;
    }
        
    // Create Connection Object
    Connection *conn = new Connection(client_fd);
        
    // Create ClientInfo to pass to the client thread
    // need to use aux argument or something
    ClientInfo *info = new ClientInfo(conn, this);
        
    // Create a new thread to handle this client
    pthread_t thread_id;
    int rc = pthread_create(&thread_id, nullptr, worker, info); //last is aux
    if (rc != 0) {
      std::cerr << "Failed to create thread: " << rc << std::endl;
      delete conn;
      delete info;
    }
    
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  std::map<std::string, Room*>::iterator it = m_rooms.find(room_name);
  if (this -> m_rooms.find(room_name) != m_rooms.end()) {
    return this->m_rooms[room_name];
  } 

  Room * newRoom = new Room(room_name);
  this->m_rooms[room_name] = newRoom;

  return newRoom;
}
