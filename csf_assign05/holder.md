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

in essence, we send stuff via command line to the reciever/sender program aka client
the reciever/sender will connect to the socket ie the fd
the program will send stuff to the connection/server based on the command line inputs!

we used COnnection class to get and send data to and from the server

we are acting like server when testing with netcat
though netcat can act as client or server
*/

-----------------------
1. try to get a background on everything

1. messgae broadcast

sending thread iterates through all the users in a room and pushes a message into each message queue
> this event wakes up the reciever thread, allowing it to dequeue the messages at itsleisure

sem_post increments the semaphore (giving up token)
> ie signals a resource has become available

sema_wait decreaes the semaphore (taking a token)

binary semaphore

so # of available counters = # of available messages
> so if we have 0 tokens available, we have 0 available message in the queue
> when we get a message, sem post
> when we read a message and thus dequeu, we sem_wait 
> sem_wait basically sleeps the thread (1 thread = either a receiver or sender client)

when all threads (sender and reciever) have gotten the message ie sem_post, a sender can send a message
> it does not need to wait for the message to be sem_wait (ie dequeued) and sent to the reciever client
> but it should not return a status until the message is done being enqueued.

message queue is shared BY ALL THREADS
> becuase of this, we must use the heap as each thread has its own stack that must not be shared 

> so basically to send a message, a sender will enqueue onto the queue
> to read a message, a reciever will dequeue. 

use sem_timedwait() instead of just sem_wait to not indefiently block a thread

make critical sections short to improve performance (think of it as us locking out all other threads which we dont want to do for a long time)

server, messagequeue and room objects need synchronization

timing
1. reciever finishes processing and calls sem_wait for new message
> if no mesages, semaphore goes to 0 and client thread sleeps 
2. sender thread recieves a sendall messsage
> here, the sender should claim a lock to prevent any other threads from doing things (so clients cannot leave/join when we are equeuing or dequeuing)
> so semaphore to basically keep track of items in the queue (semaphore on queue)
> locks to prevent mutex (lock on room)
3. message queue::enqueue calls sem_post to notify a new thing
> the sender thread still holds the synch primitive on the room!
> so holding the synch primitive on the room basically prevents say a reciving thread or sneding thread from doing things in the room until lock is release
> server manages all the messgae queues??
4a. client threda wakes up as semaphore is now avaialble and sends message to client (ie the program finally gets it and can now print it to terminal)
> returns 1 after sending the messgae
4b. happens concurrently with 4a
> sender thread transmits a status message to the sender and waits for the next message

1. create threads
> 

synchroniztion primitives
> p_thread_mutex_init()
> sem_init();

client vs the actually sender
> like the client sender just needs to stype "message"
> whent he sender program gets it, it will send "sendall:message"
> so basically we are emualting the program it self rather than the user
> so we know this becuase the command we invoked was NOT ./sender
> if its ./sender, we are emutaling the user becuase we have the sender running!

gurad is just a wrapper around a mutex 
> its a class that calls the lock when initalizied
> when out of scope, destructor invoked which unlocks
> so responsibility goes to the compiler

reciever reads from a socket
> the server will dequeue each message
> it will then write to the socket (so the reciever can read it)


sender writes to a socket
> server will read the socket for the message
> the server will enqueue each message (so the reciever can read it)

server talks to client!! so not std::err only send stuff via the sockets

//scp jshi61@ugradx.cs.jhu.edu:~/CSF/csf_assign05/solution.zip .

set args 33333

  // -> using guards to always unlock when out of scope pthread_mutex_lock(&this -> m_lock);
  