Sample README.txt

MS1
Johnny worked on sender and connection
Matthew worked on reciever and connection

MS2:

Eventually your report about how you implemented thread synchronization
in the server should go here


How thread synchronization in our chat server worked:

Critical sections are areas in the code that are accessed by multiple threads. Threads are characterized by their ability to
access shared data. AKA, each thread is not mapped to its own separate virtual memory, unlike processes. Therefore, 
there will be sections that need to be locked in order not to have race conditions, memory corruption, and otherwise unknown results. 

In the chat server, there are multiple worker threads each representing a sender or a receiver. We use threads so that we can concurrently receive and send messages for many clients. 
Otherwise, you would basically only be able to talk with yourself. 

m_rooms:
m_rooms is a map containing the room names and room objects. This will be accessed my multiple threads as they join rooms, for example. 

In order to avoid race conditions and competing threads, we have to lock this section until a thread is finished. We do this using the Guard() class,
a wrapper on mutex that automatically destructs when going out of scope. We used this synchronization primitive due to the low chance for user error, and the fact
that this operation will take very little time. 


