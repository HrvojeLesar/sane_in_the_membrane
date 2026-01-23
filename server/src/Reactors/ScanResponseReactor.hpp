#ifndef SCAN_RESPONSE_REACTOR
#define SCAN_RESPONSE_REACTOR

#include <grpcpp/grpcpp.h>
#include <memory>
#include "SaneDevice.hpp"
#include "scanner/v1/scanner.pb.h"

namespace reactor {
    using namespace scanner::v1;
    class CScanResponseReactor : public grpc::ServerWriteReactor<ScanResponse> {
      public:
        CScanResponseReactor(std::shared_ptr<sane::CSaneDevice> device);
        ~CScanResponseReactor();

        void OnDone() override;
        void OnCancel() override;
        void OnWriteDone(bool ok) override;

      protected:
        void next_packet();

      private:
        std::shared_ptr<sane::CSaneDevice> m_device;
        ScanResponse                       m_response{};
        ScanResponseData                   m_data{};
    };
}

#endif // !SCAN_RESPONSE_REACTOR
