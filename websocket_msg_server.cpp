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
        m_server.init_asio();
        m_server.set_open_handler(bind(&WebSocketServer::on_open, this, std::placeholders::_1));
        m_server.set_close_handler(bind(&WebSocketServer::on_close, this, std::placeholders::_1));
        m_server.set_message_handler(bind(&WebSocketServer::on_message, this, std::placeholders::_1, std::placeholders::_2));
    }

    void run(uint16_t port) {
        m_server.listen(port);
        m_server.start_accept();
        m_server.run();
    }

private:
    void on_open(connection_hdl hdl) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.insert(hdl);
    }

    void on_close(connection_hdl hdl) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.erase(hdl);
    }

    void on_message(connection_hdl hdl, server::message_ptr msg) {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Log the received message
        std::cout << "Message received: " << msg->get_payload() << std::endl;

        // Broadcast the message to all connected clients
        for (auto& conn : m_connections) {
            m_server.send(conn, msg);
        }
    }

    server m_server;
    std::set<connection_hdl, std::owner_less<connection_hdl>> m_connections;
    std::mutex m_mutex;
};

int main() {
    WebSocketServer server;
    server.run(9002);
    return 0;
}
