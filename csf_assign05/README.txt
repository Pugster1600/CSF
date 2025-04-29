Sample README.txt

MS1
Johnny worked on sender and connection
Matthew worked on reciever and connection

MS2:
Johnny worked on server, message_queue, and room.cpp
Matthew worked on server and README

Eventually your report about how you implemented thread synchronization
in the server should go here


How thread synchronization in our chat server worked:

Critical sections are areas in the code that are accessed by multiple threads. Threads are characterized by their ability to access shared data. AKA, each thread is not mapped to its own separate virtual memory, unlike processes. Therefore, there will be sections that need to be locked in order not to have race conditions, memory corruption, and otherwise unknown results. We identified critical sections by analyzing where shared resources, such as data structures or message queues, are accessed or modified by multiple threads concurrently.
In the chat server, there are multiple worker threads each representing a sender or a receiver. We use threads so that we can concurrently receive and send messages for many clients. Otherwise, you would basically only be able to talk with yourself.


m_rooms:
m_rooms is a map containing the room names and room objects. This will be accessed by multiple threads as they join rooms, for example. This was identified as a critical section because multiple threads may attempt to read or modify the m_rooms map simultaneously, such as when users join or leave rooms, which could lead to inconsistent state or data corruption without synchronization.
In order to avoid race conditions and competing threads, we have to lock this section until a thread is finished. We do this using the Guard() class and mutex, a wrapper on mutex that automatically destructs when going out of scope. We used this synchronization primitive due to the low chance for user error, and the fact that this operation will take very little time. Therefore, it will go out of scope quickly and not lead to deadlock (where two threads need each others resources).
The Guard class was chosen because its automatic unlocking mechanism reduces the risk of forgetting to release the mutex, which could otherwise cause deadlocks. By ensuring the mutex is held only for the duration of the critical section, we prevent race conditions while minimizing the risk of prolonged resource contention that could lead to deadlocks.


message_queue:
There are many critical sections in message_queue.cpp due to its access by many clients simultaneously. 
It is responsible for receiving and holding messages for clients. 
These critical sections were determined by identifying operations that modify the shared message queue, such as enqueuing or dequeuing messages, which could be accessed concurrently by multiple sender and receiver threads.


Thread-safe message enqueue:
{
  Guard(this -> m_lock);
  m_messages.push_back(msg);
  sem_post(&this -> m_avail);
}
 
A Guard lock is needed on this critical section due to the potential for many senders and recievers trying to mutate the queue. We need to ensure that when enqueuing,
the queue is not being modified. This could lead to conflict and undefined behavior. 
To mitigate this, the Guard lock uses mutex to lock this resource and also uses a semaphore to notify each thread that a new message is available.
Guard lock is also there to prevent deadlocks. 

The semaphore was chosen as an additional synchronization primitive because it efficiently signals waiting threads without requiring them to hold the mutex,
reducing contention and preventing unnecessary blocking. 
The semaphore notification allow threads to proceed only when a message is available.


Thread-safe message dequeue:
{
  Guard(this -> m_lock);
  msg = m_messages.front();
  m_messages.pop_front();
}

Guard lock is needed for this dequeue in message_queue due to the potential for many threads to be accessing this resource at the same time. 
In order to prevent blocking while waiting for a message, we only lock a resource when a message is confirmed to have been received. 
This confirmation is given by a semaphore, another synchronization primitive that we use in order to signal when a message has been received. 
We determined this as a critical section because multiple threads may attempt to dequeue messages concurrently, which could corrupt the queue's state without synchronization. 
The combination of a mutex (via Guard) and semaphore ensures that only one thread modifies the queue at a time, preventing race conditions, while the semaphore avoids deadlocks by ensuring threads only lock the mutex when a message is ready, thus reducing contention and wait times.


room.cpp:
There are also multiple critical sections in room.cpp due to the fact there are multiple threads operating in a single room. Therefore, there are shared resources and processes that need to be protected, aka only used by a single thread at a time. 
These critical sections were identified by examining operations that modify shared room state, such as adding members or broadcasting messages, which could be accessed by multiple threads simultaneously, potentially leading to inconsistent room state.


Adding members in room.cpp
void Room::add_member(User *user) {
  Guard(this -> lock);
  members.insert(user);
}
As can be seen, each time a member is added by a thread the members vector needs to be locked. 
This prevents other threads from accessing or editing the members in this resource until the current thread is done. 
We chose to use the Guard wrapper of mutex for our synchronization primitive due to its low risk of user error. 
Due to it being a class, it will destruct when it goes out of scope, releasing the resource. 
This helps prevent us from accidentally not releasing a resource, potentially blocking other threads and leading to a deadlock. 
This critical section ensures synchronization by guaranteeing that only one thread can modify the members set at a time, preventing race conditions. 
The Guard class minimizes the risk of deadlocks by automatically releasing the mutex, ensuring that the resource is not held indefinitely, even in the presence of exceptions or early returns.
A pthread mutex is initialized in the constructor of room.cpp, and is released in the class destructor. The guard class is able to lock/unlock it for critical sections.


Broadcasting message in room.cpp:
Guard guard(this -> lock);
for (User * user : members) {
  Message * messageStruct = new Message(TAG_DELIVERY, combinedMessage);
  user->mqueue.enqueue(messageStruct);
}

As can be seen, this critical section used for broadcasting messages is an example of block scoped guard/mutex. This is useful due to the guard unlocking when it goes out of scope, aka at the end of the method. 
A critical section is needed due to the potential for different threads to access shared data.
Otherwise, multiple threads could try to access the same resource at the same time, leading to a race condition.
This section was identified as critical because broadcasting involves iterating over the shared members set and enqueuing messages, which could be disrupted by concurrent modifications to the set.
The Guard mutex ensures that the members set is not modified during iteration, preventing race conditions. By keeping the critical section short and using Guard’s automatic unlocking, we prevent deadlocks.


Overall, our synchronization strategy ensures that all critical sections are protected using mutexes (via Guard) and semaphores where appropriate, meeting the requirement for thread-safe operations. 
We carefully chose synchronization primitives to balance safety and performance: Guard minimizes user error and deadlock risks, while semaphores optimize notification and reduce contention. 
By locking only the necessary resources for the shortest possible time and using automatic resource management, we prevent race conditions and deadlocks.

