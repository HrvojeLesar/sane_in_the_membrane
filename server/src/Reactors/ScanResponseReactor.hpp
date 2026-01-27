#ifndef SCAN_RESPONSE_REACTOR
#define SCAN_RESPONSE_REACTOR

#include <grpcpp/grpcpp.h>
#include <memory>
#include <sane/sane.h>
#include <string>
#include <vector>
#include "SaneDevice.hpp"
#include "SaneDeviceBuffer.hpp"
#include "scanner/v1/scanner.pb.h"

namespace reactor {
    using namespace scanner::v1;
    class CScanResponseReactor : public grpc::ServerWriteReactor<ScanResponse> {
      public:
        CScanResponseReactor() = delete;
        CScanResponseReactor(std::shared_ptr<sane::CSaneDevice> device);
        ~CScanResponseReactor();

        void OnDone() override;
        void OnCancel() override;
        void OnWriteDone(bool ok) override;

      protected:
        void next_packet();
        void start_scan();
        void reset_response();
        void write_response();

        void write_sane_parameters(const SANE_Parameters& parameters);

      private:
        std::shared_ptr<sane::CSaneDevice> m_device;
        ScanResponse                       m_response{};
        ScanResponseData                   m_data{};
        ScanParameters                     m_parameters{};
        std::vector<unsigned char>         m_byte_data{};
        sane::CSaneDeviceBuffer<2048>      m_buffer{};
        std::string                        m_byte_data_string{};
    };
}

#endif // !SCAN_RESPONSE_REACTOR
