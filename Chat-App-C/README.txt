README

Server: run ./server [port]

- defaults to port 13000 if left blank
- server allows connection from clients (in main thread)
- server receives messages from clients (in handleConnection())
- server notifies when a client joins or leaves (client announces to server that it has joined)
- uses the SAFE_BOUNDED struct to store messages & socket id of message owner
- SAFE_BOUNDED has been modified to store long long ints instead of ints
- server DOES NOT announce everyone that is currently online to a new client that joins

Client: run ./client [name] [ip] [port]

- defaults to localhost:13000 with name = 'no name' if left blank, respectively
- keyword 'EXIT' terminates the connection
- client displays a welcome message to the user
- client notifies server that it has connected
- client has a thread listening to stdin and sending the message to the server 
- client has a thread listening to the server socket and displays the message on stdout 
- client notifies the server when exiting and closes the socket