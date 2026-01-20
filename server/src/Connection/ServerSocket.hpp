namespace server::Connection {
    class ServerSocket final {
      public:
        ServerSocket();
        ~ServerSocket();

        void listen_for_connection();
    };
}
