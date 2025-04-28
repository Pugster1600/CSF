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
  User* user;
  
  ClientInfo(Connection *_conn, Server *_server, User *_user) 
      : conn(_conn), server(_server),  user(_user){}

  ~ClientInfo() {
    delete conn;
    delete user;
  }
};



struct ChatMessage {
  std::string room;
  std::string sender;
  std::string content;
};

namespace {

void handle_message(Connection* conn, Server* server, User* user, Room* room, 
  const Message& msg, bool is_sender) {

  std::string command = msg.tag;
  std::string data = msg.data;

  //1. initial command must be join
  if (!room && command != TAG_JOIN) {
    conn->send(Message(TAG_ERR, "Must join a room first"));
    return;
  }

  //2. handle join from both sender and reciever
  if (command == TAG_JOIN) {
    if (room) {
      conn->send(Message(TAG_ERR, "Already in a room"));
      return;
    }
    if (data.empty()) {
      conn->send(Message(TAG_ERR, "Room name cannot be empty"));
      return;
    }

    room = server->find_or_create_room(data);
    room->add_member(user);
    conn->send(Message(TAG_OK, "Joined room: " + data));

  } else if (command == TAG_SENDALL && is_sender) { //3. handle sendall from sender
    room->broadcast_message(user->username, data);
    conn->send(Message(TAG_OK, "Message sent"));
  } else if (command == TAG_LEAVE) { //4. handle leave from both sender and reciever
    room->remove_member(user);
    room = nullptr;
    conn->send(Message(TAG_OK, "Left room")); 
  } else if (command == TAG_QUIT) { //5. handle quit from both sender and reciever
    if (room) {
      room->remove_member(user);
    }
    throw std::runtime_error("Client quit");
  } else { //6. unknown command
    conn->send(Message(TAG_ERR, "Unknown command"));
  }
}

// Process sender client messages
void chat_with_sender(Connection *conn, Server *server, User * user) {
  //1. handle join first
  Room * room = nullptr;

  //2. loop
  while (1) {
    //3. read message from ./sender
    Message senderMessage;
    if (!conn->receive(senderMessage)) {
      //server error handle
      if (conn->get_last_result() == Connection::INVALID_MSG) {
        senderMessage = Message(TAG_ERR, "Invalid message");
        conn->send(senderMessage);
      }
      return;
    }

    //4. handle message from sender
    std::string command = senderMessage.tag;
    std::string data = senderMessage.data;

    //5. initial command must be join
    if (!room && command != TAG_JOIN) {
      conn->send(Message(TAG_ERR, "Must join a room first"));
      return;
    }

    //2. handle join
    if (command == TAG_JOIN) {
      if (room) {
        conn->send(Message(TAG_ERR, "Already in a room"));
        return;
      }
      if (data.empty()) {
        conn->send(Message(TAG_ERR, "Room name cannot be empty"));
        return;
      }
      room = server->find_or_create_room(data);
      room->add_member(user);
      conn->send(Message(TAG_OK, "Joined room: " + data));

    } else if (command == TAG_SENDALL) { //3. handle sendall
      room->broadcast_message(user->username, data);
      conn->send(Message(TAG_OK, "Message sent"));
    } else if (command == TAG_LEAVE) { //4. handle leave
      if (room) {
        room->remove_member(user);
        room = nullptr;
        conn->send(Message(TAG_OK, "Left room")); 
      } else {
        conn->send(Message(TAG_ERR, "not in a room!"));
      }
      
    } else if (command == TAG_QUIT) { //5. handle quit
      if (room) {
        room->remove_member(user);
      } 
      conn->send(Message(TAG_OK, "QUIT"));
    } else { //6. unknown command
      conn->send(Message(TAG_ERR, "Unknown command"));
    }
  }
}

// Process reciever client messages
void chat_with_receiver(Connection *conn, Server *server, User * user) {
  Room * room = nullptr;

  //1. join room -> only going to recieve once so need to do this outside the loop
  Message recieverMessage;
  if (!conn->receive(recieverMessage)) {
    //server error handle
    if (conn->get_last_result() == Connection::INVALID_MSG) {
      conn->send(Message(TAG_ERR, "Invalid message format"));
    }
  }

  if (recieverMessage.tag != TAG_JOIN) {
    conn->send(Message(TAG_ERR, "Must join a room first"));
    return;
  }

  room = server -> find_or_create_room(recieverMessage.data);
  room->add_member(user);
  conn->send(Message(TAG_OK, "Joined room: " + recieverMessage.data));

  while (1) {
    //get message
    Message * msg = user->mqueue.dequeue();
    if (!msg) { //queue empty
      continue;
    }

    conn -> send(*msg);
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
  // this is Matthew
  // so since we made the auxilliary data structure ClientInfo we need to cast the void argument to that
  ClientInfo *info = static_cast<ClientInfo*>(arg); //assumes that arg1 = Connection, arg2 = Server, arg3 = User
  Connection *conn = info->conn;
  Server *server = info->server;
  User *user = info->user;

  Message initial_msg;
  bool result = conn->receive(initial_msg);
  if (result){
    //ack reception
    std::string replyMessage = std::string("logged in as ") + std::string(initial_msg.data);
    Message confirmation = Message(TAG_OK, replyMessage);
    conn -> send(confirmation);
    //jump to loop
    if (initial_msg.tag == TAG_SLOGIN) {
      user = new User(initial_msg.data);
      chat_with_sender(conn, server, user);
    } else if (initial_msg.tag == TAG_RLOGIN) {
      user = new User(initial_msg.data);
      chat_with_receiver(conn, server, user);
    } else {
      conn->send(Message("error", "Invalid login message"));
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
    std::cerr << "mutex init failed";
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
  //char port_str[20]; //so port number is an int, but we need to convert to string for open_listenfd
  //yeah listenfd takes string port number
  //need to convert
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
      std::cerr << "Error accepting connection";
      continue;
    }
        
    // Create Connection Object
    Connection *conn = new Connection(client_fd);
        
    // Create ClientInfo to pass to the client thread
    // need to use aux argument or something
    ClientInfo *info = new ClientInfo(conn, this, nullptr);
        
    // Create a new thread to handle this client
    pthread_t thread_id;
    //thread id, attributes, while 1, arguments
    int rc = pthread_create(&thread_id, nullptr, worker, info); //last is aux
    if (rc != 0) {
      std::cerr << "Failed to create thread: " << rc;
      delete conn;
      delete info;
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  Guard guard(m_lock); //for m_rooms
  std::map<std::string, Room*>::iterator it = m_rooms.find(room_name);
  if (it != m_rooms.end()) {
    return this->m_rooms[room_name];
  } 

  Room * newRoom = new Room(room_name);
  this->m_rooms[room_name] = newRoom;

  return newRoom;
}
