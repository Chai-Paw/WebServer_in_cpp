// websocket_client.cpp
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <iostream>

typedef websocketpp::client<websocketpp::config::asio_client> client;

class WebSocketClient {
public:
    WebSocketClient(const std::string& uri, const std::string& udp_host, uint16_t udp_port)
        : m_udp_socket(m_io_service), m_udp_endpoint(boost::asio::ip::address::from_string(udp_host), udp_port) {
        m_client.init_asio();
        m_client.set_open_handler(bind(&WebSocketClient::on_open, this, std::placeholders::_1));
        m_client.set_message_handler(bind(&WebSocketClient::on_message, this, std::placeholders::_1, std::placeholders::_2));
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        m_client.connect(con);
    }

    void run() {
        std::thread t([this]() { m_client.run(); });
        t.detach();
    }

private:
    void on_open(websocketpp::connection_hdl hdl) {
        std::cout << "Connected to server" << std::endl;
    }

    void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
        std::string payload = msg->get_payload();
        boost::system::error_code ec;
        m_udp_socket.send_to(boost::asio::buffer(payload), m_udp_endpoint, 0, ec);
        if (ec) {
            std::cerr << "UDP send failed: " << ec.message() << std::endl;
        }
    }

    client m_client;
    boost::asio::io_service m_io_service;
    boost::asio::ip::udp::socket m_udp_socket;
    boost::asio::ip::udp::endpoint m_udp_endpoint;
};

int main() {
    WebSocketClient client("ws://localhost:9002", "127.0.0.1", 9003); // Connect to WebSocket server and broadcast to UDP
    client.run();
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
