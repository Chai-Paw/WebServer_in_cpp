#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <iostream>
#include <string>

typedef websocketpp::client<websocketpp::config::asio_client> client;

class WebSocketClient {
public:
    WebSocketClient(const std::string& uri, const std::string& udp_host, uint16_t udp_port)
        : m_udp_socket(m_io_service), m_udp_endpoint(boost::asio::ip::address::from_string(udp_host), udp_port), m_running(true) {
        
        m_udp_socket.open(boost::asio::ip::udp::v4()); // Open the UDP socket

        m_client.init_asio();
        m_client.set_open_handler(std::bind(&WebSocketClient::on_open, this, std::placeholders::_1));
        m_client.set_message_handler(std::bind(&WebSocketClient::on_message, this, std::placeholders::_1, std::placeholders::_2));
        m_client.set_fail_handler(std::bind(&WebSocketClient::on_fail, this, std::placeholders::_1));
        
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        m_hdl = con->get_handle();  // Assign the handle here
        m_client.connect(con);

        std::thread input_thread([this]() {
            handle_console_input();
        });
        input_thread.detach();
    }

    void run() {
        std::thread t([this]() { m_client.run(); });
        t.detach();
    }

    bool is_running() const {
        return m_running;
    }

private:
    void on_open(websocketpp::connection_hdl hdl) {
        std::cout << "Connected to server" << std::endl;
        m_hdl = hdl;  // Properly assign the handle
    }

    void on_fail(websocketpp::connection_hdl hdl) {
        std::cerr << "WebSocket connection failed" << std::endl;
        m_running = false;
    }

    void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
        std::string payload = msg->get_payload();
        std::cout << "Message from server: " << payload << std::endl; // Print received message
        boost::system::error_code ec;
        m_udp_socket.send_to(boost::asio::buffer(payload), m_udp_endpoint, 0, ec);
        if (ec) {
            std::cerr << "UDP send failed: " << ec.message() << std::endl;
        }
    }

    void handle_console_input() {
        while (m_running) {
            std::string input;
            std::getline(std::cin, input);
            
            if (input == "quit") {
                m_running = false;
                break;
            }
            
            boost::system::error_code ec;
            m_udp_socket.send_to(boost::asio::buffer(input), m_udp_endpoint, 0, ec);
            if (ec) {
                std::cerr << "UDP send failed: " << ec.message() << std::endl;
            }
            
            // Send the input message to the WebSocket server
            websocketpp::lib::error_code ws_ec;
            m_client.send(m_hdl, input, websocketpp::frame::opcode::text, ws_ec);
            if (ws_ec) {
                std::cerr << "WebSocket send failed: " << ws_ec.message() << std::endl;
            }
        }

        m_client.stop();
    }

    client m_client;
    websocketpp::connection_hdl m_hdl;
    boost::asio::io_service m_io_service;
    boost::asio::ip::udp::socket m_udp_socket;
    boost::asio::ip::udp::endpoint m_udp_endpoint;
    bool m_running;
};

int main() {
    WebSocketClient client("ws://localhost:9002", "127.0.0.1", 9003); // Connect to WebSocket server and broadcast to UDP
    client.run();
    
    while (client.is_running()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
