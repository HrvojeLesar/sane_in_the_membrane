#ifndef READER_SCAN_RESPONSE_READER
#define READER_SCAN_RESPONSE_READER

#include <atomic>
#include <grpcpp/client_context.h>
#include <grpcpp/support/client_callback.h>
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
#include <scanner/v1/scanner.pb.h>
#include "scanner/v1/scanner.grpc.pb.h"
#include "../Utils/ScannerUtils.hpp"
#include "../Utils/File.hpp"

namespace sane_in_the_membrane::reader {
    using namespace scanner::v1;

    class CScanResponseReader : public QObject, public grpc::ClientReadReactor<scanner::v1::ScanResponse> {
        Q_OBJECT
      public:
        CScanResponseReader(std::shared_ptr<ScannerService::Stub>& stub);
        ~CScanResponseReader();

        void scan(const ScanRequest& request);
        void OnReadDone(bool ok) override;
        void OnDone(const grpc::Status& s) override;
        void reset();
        void reset_context();

      signals:
        void sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params);
        void sig_progress(double progress);

      private:
        void start_new_file();

      private:
        std::shared_ptr<ScannerService::Stub> m_stub;
        grpc::ClientContext*                  m_context{nullptr};
        ScanResponse                          m_response{};
        utils::ScannerParameters              m_params{};
        uint64_t                              m_hundred_percent{0};
        uint64_t                              m_total_bytes{0};
        std::atomic<bool>                     m_in_progress{false};
        std::shared_ptr<utils::CFile>         m_current_file{nullptr};
    };
}

#endif // !READER_SCAN_RESPONSE_READER
