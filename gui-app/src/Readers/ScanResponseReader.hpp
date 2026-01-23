#include <grpcpp/client_context.h>
#include <grpcpp/support/client_callback.h>
#include <scanner/v1/scanner.pb.h>
#include "scanner/v1/scanner.grpc.pb.h"

namespace reader {
    using namespace scanner::v1;

    class CScanResponseReader : public grpc::ClientReadReactor<scanner::v1::ScanResponse> {
      public:
        CScanResponseReader(ScannerService::Stub& stub, grpc::ClientContext& context) : m_stub(stub), m_context(context) {}

        void scan(const ScanRequest& request) {
            std::cout << "Requesting scan\n";
            m_stub.async()->Scan(&m_context, &request, this);

            std::cout << "Start Read\n";
            StartRead(&m_response);
            std::cout << "Start call\n";
            StartCall();
            std::cout << "After call\n";
        }

        void OnReadDone(bool ok) override {
            std::cout << "Read done\n";
            if (ok) {
                std::cout << "Read: \n" << m_response.scanner_name() << "\n" << m_response.data().success() << "\n" << m_response.data().data() << "\n";
                StartRead(&m_response);
            }
        }

        void OnDone(const grpc::Status& s) override {
            std::cout << "All done\n";
        }

      private:
        ScannerService::Stub& m_stub;
        grpc::ClientContext&  m_context;

        ScanResponse          m_response{};
    };
}
