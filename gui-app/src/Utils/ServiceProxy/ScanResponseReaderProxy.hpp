#ifndef UTILS_SERVICE_PROXY_SCAN_RESPONSE_READER_PROXY
#define UTILS_SERVICE_PROXY_SCAN_RESPONSE_READER_PROXY

#include "ServiceProxyBase.hpp"
#include "../../Readers/ScanResponseReader.hpp"

namespace sane_in_the_membrane::utils::proxy {
    class CScanResponseReaderProxy : public CServiceProxyBase<reader::CScanResponseReader> {
        Q_OBJECT

      public:
        CScanResponseReaderProxy(std::shared_ptr<reader::CScanResponseReader>& service) : CServiceProxyBase(service) {}

        virtual void set_connections() override {
            QObject::connect(m_service.get(), &reader::CScanResponseReader::sig_done, this, &CScanResponseReaderProxy::sig_done);
            QObject::connect(m_service.get(), &reader::CScanResponseReader::sig_progress, this, &CScanResponseReaderProxy::sig_progress);
        }

      signals:
        void sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params);
        void sig_progress(double progress);
    };
}

#endif // !UTILS_SERVICE_PROXY_SCAN_RESPONSE_READER_PROXY
