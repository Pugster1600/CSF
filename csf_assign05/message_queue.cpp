#include <cassert>
#include <ctime>
#include "message_queue.h"
#include "guard.h"
#include <iostream>

MessageQueue::MessageQueue() {
  // TODO: initialize the mutex and the semaphore
  if (sem_init(&this -> m_avail, 0, 0) != 0){ //(sem_t, 0 = shared between threads , 0 initial tokens)
    std::cerr << "semaphore initalization failed" << std::endl;
  }

  //default initialization (NULL arg)
  if (pthread_mutex_init(&this -> m_lock, NULL) != 0) {
    std::cerr << "mutex init failed" << std::endl;
  }
}

MessageQueue::~MessageQueue() {
  // TODO: destroy the mutex and the semaphore
  {
    Guard guard(this->m_lock);
    for (Message * m : m_messages) {
      delete m;
    }
  }

  sem_destroy(&this -> m_avail);
  pthread_mutex_destroy(&this->m_lock);
}

void MessageQueue::enqueue(Message *msg) {
  // -> using guards to always unlock when out of scope pthread_mutex_lock(&this -> m_lock);
  
  {
    Guard guard(this -> m_lock);
    // TODO: put the specified message on the queue
    m_messages.push_back(msg);
  }

  sem_post(&this -> m_avail);

  // be sure to notify any thread waiting for a message to be
  // available by calling sem_post
}

Message *MessageQueue::dequeue() {
  struct timespec ts;

  // get the current time using clock_gettime:
  // we don't check the return value because the only reason
  // this call would fail is if we specify a clock that doesn't
  // exist
  clock_gettime(CLOCK_REALTIME, &ts);

  // compute a time one second in the future
  ts.tv_sec += 1;

  // TODO: call sem_timedwait to wait up to 1 second for a message
  //       to be available, return nullptr if no message is available

  // TODO: remove the next message from the queue, return it
  Message *msg = nullptr;
  if (sem_timedwait(&this->m_avail, &ts) == -1) {
    return nullptr;
  }

  //lock only when message becomes available to prevent locking while blocking
  
  {
    Guard guard(this -> m_lock); //hold mutex for the messagequeue itself
    msg = m_messages.front();
    m_messages.pop_front();
  } 
  return msg;
}
