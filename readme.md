# WebSocket Server and Client with UDP Broadcasting

This project demonstrates a WebSocket server and client application pair written in C++. The server broadcasts messages received from clients to all connected clients using WebSocket (TCP) and allows the client to broadcast these messages over UDP to a specified endpoint.

## Requirements

- C++ compiler that supports C++11 or later
- Boost C++ Libraries (specifically Boost.Asio)
- `websocketpp` library

## Setup and Compilation

1. **Install Dependencies:**
   - Ensure you have Boost installed. If not, you can download it from [Boost website](https://www.boost.org/) and follow their installation instructions.
   - `websocketpp` can be downloaded or cloned from its [GitHub repository](https://github.com/zaphoyd/websocketpp).

2. **Compilation:**
   - Compile `websocket_server.cpp` and `websocket_client.cpp` with your C++ compiler. For example, using g++:
     ```bash
     g++ -std=c++11 -I path/to/websocketpp -lboost_system -lboost_thread websocket_server.cpp -o websocket_server
     g++ -std=c++11 -I path/to/websocketpp -lboost_system -lboost_thread websocket_client.cpp -o websocket_client
     ```
   - Compile `websocket_msg_server.cpp` and `websocket_msg_client.cpp` similarly:
     ```bash
     g++ -std=c++11 -I path/to/websocketpp -lboost_system -lboost_thread websocket_msg_server.cpp -o websocket_msg_server
     g++ -std=c++11 -I path/to/websocketpp -lboost_system -lboost_thread websocket_msg_client.cpp -o websocket_msg_client
     ```

## WebSocket Server (websocket_server.cpp)

### Usage

1. **Running the WebSocket Server:**
   - Run the compiled `websocket_server` executable. It listens on port `9002` by default.
     ```bash
     ./websocket_server
     ```
   - The server accepts WebSocket connections from clients and broadcasts received messages to all connected clients.

2. **Connection Method:**
   - The server initializes a WebSocket server using `websocketpp` with Boost.Asio configuration (`asio_no_tls`).
   - It listens for WebSocket connections on port `9002`.

3. **Threading:**
   - Threading is managed internally within the WebSocket server using `std::thread`. The `run()` method starts a separate thread for the WebSocket server event loop (`m_server.run()`).

4. **Protocols Used:**
   - **WebSocket (TCP):** The server communicates with WebSocket clients using the WebSocket protocol over TCP/IP.

### Explanation

- **WebSocket Server Initialization (`WebSocketServer` class in `websocket_server.cpp`):**
  - Initializes a WebSocket server using `websocketpp` and Boost.Asio (`websocketpp::config::asio`).
  - Handles WebSocket connection events (`on_open`, `on_close`, `on_message`) to manage client connections and message broadcasting.

- **Event Handlers (`WebSocketServer` class):**
  - `on_open(connection_hdl hdl)`: Called when a new WebSocket connection is established. Adds the connection to the list of active connections.
  - `on_close(connection_hdl hdl)`: Called when a WebSocket connection is closed. Removes the connection from the list of active connections.
  - `on_message(connection_hdl hdl, server::message_ptr msg)`: Called when a message is received from a WebSocket client. Broadcasts the message to all connected clients.

- **Thread Management:**
  - Uses `std::thread` to manage concurrent execution of WebSocket server operations (`m_server.run()`).

## WebSocket Client (websocket_client.cpp)

### Usage

1. **Running the WebSocket Client:**
   - Run the compiled `websocket_client` executable. Ensure it is provided with appropriate command-line arguments.
     ```bash
     ./websocket_client
     ```
   - The client connects to the WebSocket server (`ws://localhost:9002` by default) and broadcasts received messages over UDP to `127.0.0.1:9003`.

2. **Connection Method:**
   - The client establishes a WebSocket connection (`ws://localhost:9002`) using the `websocketpp` library's `asio_client` configuration.

3. **Threading:**
   - Threading is managed internally within the WebSocket client using `std::thread`. The `run()` method starts a separate thread for the WebSocket client event loop (`m_client.run()`).

4. **Protocols Used:**
   - **WebSocket (TCP):** The client communicates with the WebSocket server using the WebSocket protocol over TCP/IP.
   - **UDP:** Received WebSocket messages are broadcasted over UDP to the specified endpoint (`127.0.0.1:9003`).

### Explanation

- **WebSocket Client Initialization (`WebSocketClient` class in `websocket_client.cpp`):**
  - Initializes a WebSocket client using `websocketpp` and Boost.Asio (`websocketpp::config::asio_client`).
  - Connects to the WebSocket server (`ws://localhost:9002` by default).
  - Sets up UDP socket (`boost::asio::ip::udp::socket`) and endpoint (`boost::asio::ip::udp::endpoint`) for broadcasting received WebSocket messages.

- **Event Handlers (`WebSocketClient` class):**
  - `on_open(websocketpp::connection_hdl hdl)`: Triggered when the WebSocket connection is established.
  - `on_message(websocketpp::connection_hdl hdl, client::message_ptr msg)`: Triggered when a message is received from the WebSocket server. Broadcasts the message over UDP.

- **Thread Management:**
  - Uses `std::thread` to manage concurrent execution of WebSocket client operations (`m_client.run()`).

## WebSocket Server with Message Logging (websocket_msg_server.cpp)

### Usage

1. **Running the WebSocket Message Server:**
   - Run the compiled `websocket_msg_server` executable. It listens on port `9002` by default.
     ```bash
     ./websocket_msg_server
     ```
   - The server accepts WebSocket connections from clients, logs received messages, and broadcasts them to all connected clients.

2. **Logging Messages:**
   - The server logs the body of each message received from clients.

### Explanation

- **WebSocket Server Initialization (`WebSocketMsgServer` class in `websocket_msg_server.cpp`):**
  - Initializes a WebSocket server using `websocketpp` and Boost.Asio (`websocketpp::config::asio`).
  - Handles WebSocket connection events (`on_open`, `on_close`, `on_message`) to manage client connections, log received messages, and broadcast messages to all connected clients.

- **Event Handlers (`WebSocketMsgServer` class):**
  - `on_open(connection_hdl hdl)`: Called when a new WebSocket connection is established. Adds the connection to the list of active connections.
  - `on_close(connection_hdl hdl)`: Called when a WebSocket connection is closed. Removes the connection from the list of active connections.
  - `on_message(connection_hdl hdl, server::message_ptr msg)`: Called when a message is received from a WebSocket client. Logs the message body and broadcasts it to all connected clients.

## WebSocket Client with UDP and WebSocket Broadcasting (websocket_msg_client.cpp)

### Usage

1. **Running the WebSocket Message Client:**
   - Run the compiled `websocket_msg_client` executable.
     ```bash
     ./websocket_msg_client
     ```
   - The client connects to the WebSocket server (`ws://localhost:9002` by default) and broadcasts received messages over UDP to `127.0.0.1:9003`.

2. **Broadcasting Messages:**
   - The client sends input messages over UDP and WebSocket and consumes broadcasted messages from the server.

### Explanation

- **WebSocket Client Initialization (`WebSocketMsgClient` class in `websocket_msg_client.cpp`):**
  - Initializes a WebSocket client using `websocketpp` and Boost.Asio (`websocketpp::config::asio_client`).
  - Connects to the WebSocket server (`ws://localhost:9002` by default).
  - Sets up UDP socket (`boost::asio::ip::udp::socket`) and endpoint (`boost::asio::ip::udp::endpoint`) for broadcasting received WebSocket messages.

- **Event Handlers (`WebSocketMsgClient` class):**
  - `on_open(websocketpp::connection_hdl hdl)`: Triggered when the WebSocket connection is established.
  - `on_message(websocketpp::connection_hdl hdl, client::message_ptr msg)`: Triggered when a message is received from the WebSocket server. Logs and broadcasts the message over UDP.

- **Thread Management:**
  - Uses `std::thread` to manage concurrent execution of WebSocket client operations (`m_client.run()`).
  
- **Additional Functionality:**
  - Logs received messages.
  - Handles input from the console and sends it over both UDP and WebSocket.

## Summary

This project demonstrates a WebSocket server-client pair where the server broadcasts messages to all connected clients, and the client broadcasts received messages over UDP. The enhanced versions (`websocket_msg_server.cpp` and `websocket_msg_client.cpp`) include logging of received messages and functionality to send and consume messages over both.
