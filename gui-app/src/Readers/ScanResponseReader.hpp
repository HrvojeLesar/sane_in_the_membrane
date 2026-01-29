#include <grpcpp/client_context.h>
#include <grpcpp/support/client_callback.h>
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
#include <scanner/v1/scanner.pb.h>
#include <vector>
#include "scanner/v1/scanner.grpc.pb.h"

namespace reader {
    using namespace scanner::v1;

    struct ScannerParameters {
        ScannerParameters() {}
        ScannerParameters(const ScanParameters& scan) :
            format(scan.format()), last_frame(scan.last_frame()), bytes_per_line(scan.bytes_per_line()), pixels_per_line(scan.pixels_per_line()), lines(scan.lines()),
            depth(scan.depth()) {}

        int64_t format;
        bool    last_frame;
        int64_t bytes_per_line;
        int64_t pixels_per_line;
        int64_t lines;
        int64_t depth;
    };

    class CScanResponseReader : public QObject, public grpc::ClientReadReactor<scanner::v1::ScanResponse> {
        Q_OBJECT
      public:
        CScanResponseReader(ScannerService::Stub& stub);
        ~CScanResponseReader();

        void scan(const ScanRequest& request);
        void OnReadDone(bool ok) override;
        void OnDone(const grpc::Status& s) override;
        void reset();
        void reset_context();

      signals:
        void sig_done(const grpc::Status& status);

      private:
        ScannerService::Stub& m_stub;
        grpc::ClientContext*  m_context;
        std::vector<char>     m_byte_data{};
        ScanResponse          m_response{};
        ScannerParameters     m_params{};
    };

    inline std::shared_ptr<CScanResponseReader> g_scan_response_reader{nullptr};
}
