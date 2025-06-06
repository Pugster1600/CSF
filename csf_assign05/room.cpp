#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"
#include <iostream>

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // TODO: initialize the mutex
  if (pthread_mutex_init(&this -> lock, nullptr) != 0) {
    std::cerr << "mutex init failed" << std::endl;
  }
}

Room::~Room() {
  // TODO: destroy the mutex
  pthread_mutex_destroy(&this->lock);
}

void Room::add_member(User *user) {
  // TODO: add User to the room
  {
    Guard guard(this -> lock);
    members.insert(user);
  }
}

void Room::remove_member(User *user) {
  // TODO: remove User from the room
  {
    Guard guard(this -> lock);
    members.erase(user);
  }
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // TODO: send a message to every (receiver) User in the room
  
  std::string combinedMessage = this->room_name + ":" + sender_username + ":" + message_text; //delivery:room:sender:message
  
  {
    Guard guard(this -> lock);
    
    //add message to queue of all members
    for (User * user : members) {
      Message * messageStruct = new Message(TAG_DELIVERY, combinedMessage);
      user->mqueue.enqueue(messageStruct);
    }
  }
}
