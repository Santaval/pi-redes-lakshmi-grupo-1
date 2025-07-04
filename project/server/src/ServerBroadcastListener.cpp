#include "./ServerBroadcastListener.hpp"
#include "sockets/BroadcastSocket.hpp"
#include "pig/includes/ProtocolParser.h"
#include "pig/includes/Message.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

ServerBroadcastListener::ServerBroadcastListener()  {
    this->clientPort = SERVER_BROADCAST_LISTENER_PORT;
    this->ipAddress = "127.0.0.1";
    this->initSocket = nullptr;
}

ServerBroadcastListener::~ServerBroadcastListener() {
    this->stop();
}

int ServerBroadcastListener::start(const char * /* address */) {
    try {
        this->initSocket = new BroadcastSocket(); // Use BroadcastSocket for UDP broadcast
        if (this->initSocket == nullptr) {
            throw std::runtime_error("ServerBroadcastListener::start - Failed to create broadcast socket");
        }
        static_cast<BroadcastSocket*>(this->initSocket)->EnableBroadcast();

        // Bind to all interfaces to receive broadcasts
        if (this->initSocket->Bind(this->clientPort, "0.0.0.0") == 0) {
            std::cout << "[+] ServerBroadcastListener listening for broadcasts on 0.0.0.0:" << this->clientPort << std::endl;
            
            // Receive loop
            while (true) {
                char buffer[1024];
                struct sockaddr_in clientAddr;
                memset(&clientAddr, 0, sizeof(clientAddr));
                
                size_t bytes = this->initSocket->recvFrom(buffer, sizeof(buffer), &clientAddr);
                if (bytes > 0) {
                    buffer[bytes] = '\0';
                    std::cout << "[UDP] Message: " << buffer << std::endl;
                    
                    // Handle the broadcast message here
                    // You can add your broadcast handling logic
                    this->handleClientConnection(buffer, this->initSocket);
                }
            }
        } else {
            throw std::runtime_error("Failed to bind broadcast socket");
        }
    } catch (const std::exception& e) {
        std::cerr << "[-] Error: " << e.what() << std::endl;
        if (this->initSocket) {
            this->initSocket->Close();
            delete this->initSocket;
            this->initSocket = nullptr;
        }
        return -1;
    }
    
    return 0;
}

int ServerBroadcastListener::stop() {
    if (this->initSocket) {
        this->initSocket->Close();
        delete this->initSocket;
        this->initSocket = nullptr;
    }
    return 0;
}


int ServerBroadcastListener::handleClientConnection(char buffer[256], VSocket* /* socket */) {
    ProtocolParser parser;
   std::shared_ptr<Message> message = parser.parseMessage(buffer);
    // Process the message as needed
    if (message->getType() == MessageType::ANNOUNCEMENT) {
        std::cout << "[+] Announcement from " << message->getIP() 
                  << ":" << message->getPort() << std::endl;

    } else if (message->getType() == MessageType::DISCONNECTION) {
        std::cout << "[+] Disconnection from " << message->getIP() 
                  << ":" << message->getPort() << std::endl;
    } else {
        std::cout << "[+] Other message type received." << std::endl;
    }

    // You can add your logic to process the broadcast message
    return 0; // Return 0 to indicate success
}


int ServerBroadcastListener::run() {
    return this->start(this->ipAddress);
}