// websocket_server creates a websocket server wherein it assigns each new client a new thread for connecting and processing.
// the websocketpp library is used to create a websocket server and client, the no tls file is used as we are implementing UDP protocol on client side.
// thread, set, mutex, iostream are all built in libs of cpp for assigning threads, creating sets of objects, mutually exclusive threading or async threading, and basic input output respectively.

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <thread>
#include <set>
#include <mutex>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::connection_hdl connection_hdl;

class WebSocketServer {
public:
    WebSocketServer() {
        //async io initialized
        m_server.init_asio();
        m_server.set_open_handler(bind(&WebSocketServer::on_open, this, std::placeholders::_1));
        m_server.set_close_handler(bind(&WebSocketServer::on_close, this, std::placeholders::_1));
        m_server.set_message_handler(bind(&WebSocketServer::on_message, this, std::placeholders::_1, std::placeholders::_2));
    }
    //listener initialized
    void run(uint16_t port) {
        m_server.listen(port);
        m_server.start_accept();
        m_server.run();
    }

private:
    // Event handler for when a new WebSocket connection is opened
    void on_open(connection_hdl hdl) {
        std::lock_guard<std::mutex> lock(m_mutex); // Lock the mutex to protect shared resources, this way each thread is unique to the connection/set
        m_connections.insert(hdl); // Add the new connection handle to the set of connections
    }

    // Event handler for when a WebSocket connection is closed
    void on_close(connection_hdl hdl) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.erase(hdl); // Remove the closed connection handle from the set
    }

    // Event handler for when a message is received from a WebSocket client
    void on_message(connection_hdl hdl, server::message_ptr msg) {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Iterate through all connected clients and send the received message
        for (auto& conn : m_connections) {
            m_server.send(conn, msg); // Send the message to each connected client
        }
    }

    server m_server; // WebSocket server object
    std::set<connection_hdl, std::owner_less<connection_hdl>> m_connections; // Set of active connection handlers
    std::mutex m_mutex; // Mutex for thread-safety
};

int main() {
    WebSocketServer server; // Create an instance of WebSocketServer
    server.run(9002); // Start the WebSocket server, listening on port 9002
    return 0;
}