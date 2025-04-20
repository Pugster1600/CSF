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