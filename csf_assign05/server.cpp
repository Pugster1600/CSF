
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
  User * user;
  Connection *conn;     // Connection object for the client
  Server *server;       // Pointer to server instance
  
  ClientInfo(User *_user, Connection *_conn, Server *_server) 
      : user(_user), conn(_conn), server(_server){}

  ~ClientInfo() {
    delete user;
    delete conn;
  }
};


namespace {

// Process sender client messages
void chat_with_sender(Connection *conn, Server * server, User * user) {
  //1. handle join first
  Room * room = nullptr;
  while (1) {
    //2. read message from ./sender
    Message senderMessage;
    if (!conn->receive(senderMessage)) {
      //server error handle
      if (conn->get_last_result() == Connection::INVALID_MSG) {
        senderMessage = Message(TAG_ERR, "Invalid message");
        conn->send(senderMessage);
      }
      return;
    }

    //3. handle message from sender
    std::string command = senderMessage.tag;
    std::string data = senderMessage.data;

    //4. initial command must be join or quit
    if (!room && command != TAG_JOIN) {
      if (command != TAG_QUIT) {
        conn->send(Message(TAG_ERR, "Must join a room first"));
        return;
      }
    }

    //5. handle join
    if (command == TAG_JOIN) {
      if (room) {
        conn->send(Message(TAG_ERR, "Already in a room"));
        return;
      }
      if (data.empty()) {
        conn->send(Message(TAG_ERR, "Room name cannot be empty"));
        return;
      }
      //remove new line character from data
      if (data.length() >= 1) {
        data = data.substr(0, data.length() - 1);
      }   
      room = server->find_or_create_room(data);
      room->add_member(user);
      conn->send(Message(TAG_OK, "Joined room: " + data));
    } 
    
    //6. handle sendall
    else if (command == TAG_SENDALL) { 
      room->broadcast_message(user->username, data);
      conn->send(Message(TAG_OK, "Message sent"));
    } 
    
    //7. handle leave
    else if (command == TAG_LEAVE) { 
      if (room) {
        room->remove_member(user);
        room = nullptr;
        conn->send(Message(TAG_OK, "Left room")); 
      } else {
        conn->send(Message(TAG_ERR, "not in a room!"));
      }
    } 
    
    //8. handle quit
    else if (command == TAG_QUIT) { 
      if (room) {
        room->remove_member(user);
      } 
      delete user;
      conn->send(Message(TAG_OK, "QUIT"));
    } 
    
    //9. unknown command
    else { 
      conn->send(Message(TAG_ERR, "Unknown command"));
    }
  }
}

// Process reciever client messages
void chat_with_receiver(Connection *conn, Server * server, User * user) {
  Room * room = nullptr;
  //1. get join room message
  Message recieverMessage;
  if (!conn->receive(recieverMessage)) {
    //server error handle
    if (conn->get_last_result() == Connection::INVALID_MSG) {
      conn->send(Message(TAG_ERR, "Invalid message format"));
    }
  }

  //2. if not join room, reply back
  if (recieverMessage.tag != TAG_JOIN) {
    conn->send(Message(TAG_ERR, "Must join a room first"));
    return;
  } 

  //delete new line character from room
  if (recieverMessage.data.length() >= 1) {
    recieverMessage.data = recieverMessage.data.substr(0, recieverMessage.data.length() - 1);
  } 

  //3. join room
  room = server -> find_or_create_room(recieverMessage.data);
  room->add_member(user);
  conn->send(Message(TAG_OK, "Joined room: " + recieverMessage.data));

  while (1) {
    //4. loop and read message
    Message * msg = user->mqueue.dequeue();
    if (!msg) { //queue empty
      continue;
    }

    conn -> send(*msg);
    delete msg;
  }
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
 
  ClientInfo *info = static_cast<ClientInfo*>(arg);
  Message initial_msg;

  //1. read login message
  bool result = info->conn->receive(initial_msg);
  if (result){
    //2. reply back to login message
    std::string replyMessage = "logged in as " + initial_msg.data;
    Message confirmation = Message(TAG_OK, replyMessage);
    if (!info->conn -> send(confirmation)) {
      std::cerr << "error sending back login in confirmation";
    }
    std::string username = initial_msg.data;

    //delete new line character
    if (username.length() >= 1) {
      username = username.substr(0, username.length() - 1);
    } 
    if (initial_msg.tag == TAG_SLOGIN) { //sender login
      info->user = new User(username);
      chat_with_sender(info -> conn, info -> server, info -> user);
    } else if (initial_msg.tag == TAG_RLOGIN) { //reciever login
      info->user = new User(username);
      chat_with_receiver(info -> conn, info -> server, info -> user);
    } else {
      info->conn->send(Message("error", "Invalid login message"));
    }
  }
  
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

  std::string str = std::to_string(this -> m_port);  // Convert integer to string
  const char* port_str = str.c_str();
  m_ssock = open_listenfd(port_str);
  return m_ssock >= 0;
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  while(true){
    int client_fd = Accept(m_ssock, nullptr, nullptr); //something wrong here
    if (client_fd < 0) {
      std::cerr << "Error accepting connection" << std::endl;
      continue;
    }
        
    // Create Connection Object
    Connection *conn = new Connection(client_fd);
        
    // Create ClientInfo to pass to the client thread
    ClientInfo *info = new ClientInfo(nullptr, conn, this);
        
    // Create a new thread to handle this client
    pthread_t thread_id;
    int rc = pthread_create(&thread_id, nullptr, worker, static_cast<void*>(info));
    if (rc != 0) {
      std::cerr << "Failed to create thread: " << rc << std::endl;
      delete info;
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary

  Guard guard(m_lock);
  std::map<std::string, Room*>::iterator it = m_rooms.find(room_name);
  //if room exists, return room
  if (it != m_rooms.end()) {
    return this->m_rooms[room_name];
  } 

  //if room does not exist, create room
  Room * newRoom = new Room(room_name);
  this->m_rooms[room_name] = newRoom;

  return newRoom;
}

//VALGRIND_ENABLE=1 ./test_sequential.sh 33333 seq_send_1.in seq_send_2.in seq_recv
//valgrind --leak-check=full --show-leak-kinds=all -s ./test_sequential.sh 33333 seq_send_1.in seq_send_2.in seq_recv