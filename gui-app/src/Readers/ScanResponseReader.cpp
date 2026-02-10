#include "ScanResponseReader.hpp"
#include <atomic>
#include <memory>
#include "../Utils/Globals.hpp"
#include "../GlobalLogger.cpp"

using namespace sane_in_the_membrane;

reader::CScanResponseReader::CScanResponseReader(std::shared_ptr<ScannerService::Stub>& stub) : m_stub(stub) {}
reader::CScanResponseReader::~CScanResponseReader() {
    reset_context();
}

void reader::CScanResponseReader::scan(const ScanRequest& request) {
    if (m_in_progress.load(std::memory_order::relaxed)) {
        return;
    }

    m_in_progress.store(true, std::memory_order::relaxed);

    reset();
    start_new_file();
    m_context = new grpc::ClientContext();

    m_stub->async()->Scan(m_context, &request, this);

    StartRead(&m_response);
    StartCall();
}
void reader::CScanResponseReader::OnReadDone(bool ok) {
    g_logger.log(DEBUG, "Read done");
    if (ok) {
        g_logger.log(DEBUG, "Read ok");
        if (m_response.has_parameters()) {
            m_params          = utils::ScannerParameters(m_response.parameters());
            m_hundred_percent = ((uint64_t)m_params.bytes_per_line) * m_params.lines *
                ((m_params.format == utils::SANE_Frame::SANE_FRAME_RGB || m_params.format == utils::SANE_Frame::SANE_FRAME_GRAY) ? 1 : 3);
        }

        if (m_response.has_data()) {
            const auto& data = m_response.data().raw_bytes();
            m_current_file->write(data);
            m_total_bytes += data.size();
        }

        double progress = ((m_total_bytes * 100.) / (double)m_hundred_percent);

        emit   sig_progress(progress);

        StartRead(&m_response);
    }
}

void reader::CScanResponseReader::OnDone(const grpc::Status& s) {
    g_logger.log(DEBUG, "All done");

    m_in_progress.store(false, std::memory_order::relaxed);

    reset();
    emit sig_done(std::make_shared<grpc::Status>(s), m_current_file, std::make_shared<utils::ScannerParameters>(m_params));

    g_logger.log(DEBUG, "Done after sig");
}

void reader::CScanResponseReader::reset() {
    reset_context();
    m_total_bytes     = 0;
    m_hundred_percent = 0;
}

void reader::CScanResponseReader::reset_context() {
    delete m_context;
    m_context = nullptr;
}

void reader::CScanResponseReader::start_new_file() {
    m_current_file = utils::Globals::file_manager()->new_temp_file();
}
