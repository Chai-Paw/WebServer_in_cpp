// websocket_client is a client app that connects to the server on a specified port using a hybrid approach of TCP and UDP.
// The WebSocket client uses TCP for establishing the initial connection with the server (WebSocket protocol),
// and then all communication of data payloads is done over UDP.

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <iostream>

typedef websocketpp::client<websocketpp::config::asio_client> client; // Define WebSocket client type

class WebSocketClient {
public:
    WebSocketClient(const std::string& uri, const std::string& udp_host, uint16_t udp_port)
        : m_udp_socket(m_io_service), m_udp_endpoint(boost::asio::ip::address::from_string(udp_host), udp_port) {
        
        // Initialize WebSocket client with Boost.Asio
        m_client.init_asio();
        
        // Set handler functions for WebSocket events
        m_client.set_open_handler(bind(&WebSocketClient::on_open, this, std::placeholders::_1));
        m_client.set_message_handler(bind(&WebSocketClient::on_message, this, std::placeholders::_1, std::placeholders::_2));
        
        // Establish TCP connection to the WebSocket server
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        m_client.connect(con);
    }

    void run() {
        // Run the WebSocket client's IO loop in a separate thread
        std::thread t([this]() { m_client.run(); });
        t.detach(); // Detach the thread to run independently
    }

private:
    // WebSocket event handler for when the connection to the server is opened
    void on_open(websocketpp::connection_hdl hdl) {
        std::cout << "Connected to server" << std::endl;
    }

    // WebSocket event handler for when a message is received from the server
    void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
        std::string payload = msg->get_payload(); // Get the payload (message content) from the WebSocket message
        
        // Send the payload via UDP to the specified UDP endpoint
        boost::system::error_code ec;
        m_udp_socket.send_to(boost::asio::buffer(payload), m_udp_endpoint, 0, ec);
        
        // Handle any errors that occur during UDP send operation
        if (ec) {
            std::cerr << "UDP send failed: " << ec.message() << std::endl;
        }
    }

    client m_client; // WebSocket client object
    boost::asio::io_service m_io_service; // Boost.Asio IO service for UDP socket
    boost::asio::ip::udp::socket m_udp_socket; // UDP socket for sending data
    boost::asio::ip::udp::endpoint m_udp_endpoint; // UDP endpoint (IP address and port)
};

int main() {
    // Create an instance of WebSocketClient to connect to the WebSocket server on port 9002
    // and broadcast received WebSocket messages to UDP endpoint "127.0.0.1:9003"
    WebSocketClient client("ws://localhost:9002", "127.0.0.1", 9003);
    
    // Start the WebSocket client
    client.run();
    
    // Keep the main thread running indefinitely (in this case, with a sleep interval)
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
