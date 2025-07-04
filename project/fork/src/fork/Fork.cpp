#include <sys/socket.h>         // sockaddr_in
#include <arpa/inet.h>          // ntohs
#include <unistd.h>				// write, read
#include <cstring>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>
#include "./includes/Fork.hpp"
#include "http/HttpParser.h"
#include "sockets/includes/BroadcastSocket.hpp"
#include "includes/Router.hpp"
#include "includes/ForkRoutes.hpp"

Fork::Fork(int clientPort, const char* ip) 
    : serverManager()  // Initialize ServerManager directly, not as a pointer
    , router(new Router()) {
	this->clientPort = clientPort; // 5051
	this->myAddress = ip;
	memset( &this->clientAddr, 0, sizeof( this->clientAddr ) );
	this->clientAddr.sin_family = AF_INET;
	this->clientAddr.sin_port = htons(this->clientPort);

	ForkRoutes::registerRoutes(this);
}

int Fork::handleClientConnection(char buffer[256], VSocket* socket) {
	static std::string accumulatedRequest;
	accumulatedRequest += buffer;
	if (accumulatedRequest.find("\r\n\r\n") != std::string::npos) {
		if (accumulatedRequest.find("exit") != std::string::npos) {
			socket->sendTo("\0", 1, (void *) &this->clientAddr);
			accumulatedRequest.clear();
			return -1;
		}
		HttpParser parser;
		HttpParser::HttpRequest request = parser.parseRequest(accumulatedRequest);
		bool routed = this->router->handle(request.method, request.path, accumulatedRequest, socket);
		if (!routed) {
			std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nUnknown request.";
			socket->Write(response.c_str(), response.length());
		}
		socket->Close(); // Close the socket after handling the request
		accumulatedRequest.clear();
	}
	return 0;
}

int Fork::start(const char * address) {
	// init broadcast listener
	this->broadcastListener = new ForkBroadcastListener(this->serverManager);
	if (this->broadcastListener == nullptr) {
		throw std::runtime_error("Fork::start - Failed to create broadcast listener");
		return -1;
	}
	// start broadcast listener in a separate thread
	if (this->broadcastListener->Thread::start() != 0) {
		std::cerr << "[-] Failed to start broadcast listener." << std::endl;
		return -1;
	}

	// this->initSocket = new SSLSocket(true);
	this->initSocket = new Socket('s');
	if (this->initSocket == nullptr) {
		std::runtime_error( "Server::start" );
		return -1;
	}
	// prepare port to listen for connections
	if (this->initSocket->Bind(this->clientPort, "0.0.0.0") == 0) {

		if (this->initSocket->MarkPassive() == 0) {
			std::cout << "[+] Server listening on 0.0.0.0:" << this->clientPort << "\n";

			while (true) {	// loop for clients connections
				VSocket* newSock = this->initSocket->AcceptConnection(&this->clientAddr);
				if (newSock == nullptr) {
					return -1;
				}

				printf("[+] New client connected from %s\n", 
				inet_ntoa(this->clientAddr.sin_addr));

				// set a child process to handle the incoming request
				pid_t childpid = fork();
				if (childpid == 0) {

					// loop for taking requests for certain client
					while (true) {
						char buffer[250];
						memset(buffer, '\0', sizeof(buffer));


						int bytes = newSock->recvFrom((void*)buffer, 
							sizeof(buffer), (void *) & this->clientAddr);
						if ( bytes > 0) { // if not empty, manage response
							buffer[bytes] = '\0';
							if (this->handleClientConnection(buffer, newSock) == -1) {			
								break;
							}		
						} else { // empty response from client
							break;
						}
					}
					printf("[+] Client disconnected\n");
				}
				newSock->Close();
			}
			this->initSocket->Close();	
		} else { // Listening for connections failed
			return -1;
		}
	} else { // Binding socket failed
		return -1;
	}
	return 0;
}

Fork::~Fork() {
	if (this->broadcastListener) {
		delete this->broadcastListener;
	}
	if (this->router) {
		delete this->router;
	}
	if (this->initSocket) {
		delete this->initSocket;
	}
}