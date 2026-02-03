#include "ScanResponseReader.hpp"
#include "image/MagickImageWrapper.hpp"
#include <fstream>

using namespace sane_in_the_membrane;

reader::CScanResponseReader::CScanResponseReader(ScannerService::Stub& stub) : m_stub(stub) {}
reader::CScanResponseReader::~CScanResponseReader() {
    reset_context();
}

void reader::CScanResponseReader::scan(const ScanRequest& request) {
    reset();
    m_context = new grpc::ClientContext();

    m_stub.async()->Scan(m_context, &request, this);

    StartRead(&m_response);
    StartCall();
}
void reader::CScanResponseReader::OnReadDone(bool ok) {
    std::cout << "Read done\n";
    if (ok) {
        std::cout << "Read ok\n";
        if (m_response.has_parameters()) {
            m_params = ScannerParameters(m_response.parameters());
        }

        if (m_response.has_data()) {
            const auto& data = m_response.data().raw_bytes();
            m_byte_data.insert(m_byte_data.end(), data.begin(), data.end());
        }

        std::cout << "Read from: \n" << m_response.scanner_name() << "\n" << "Size: " << m_response.data().raw_bytes().size() << "\n";
        StartRead(&m_response);
    }
}

void reader::CScanResponseReader::OnDone(const grpc::Status& s) {
    std::cout << "All done\n";

    delete m_context;
    m_context = nullptr;

    if (s.ok()) {
        std::cout << "Writing image\n";

        std::ofstream outfile("scan-out/scan-net.raw", std::ios::out | std::ios::binary);
        if (outfile) {
            outfile.write(reinterpret_cast<const char*>(m_byte_data.data()), m_byte_data.size());
            outfile.close();
        }
        std::cout << "Read from: \n" << m_response.scanner_name() << "\n" << "Size: " << m_response.data().raw_bytes().size() << "\n";
        std::cout << "Params: "
                  << "Bytes per line: " << m_params.bytes_per_line << "\n"
                  << "Pixels per line: " << m_params.pixels_per_line << "\n"
                  << "Lines: " << m_params.lines << "\n"
                  << "Depth: " << m_params.depth << "\n"
                  << "Format: " << m_params.format << "\n"
                  << "Last frame: " << m_params.last_frame << "\n";

        // sane_in_the_membrane::utils::write_image("scan.png", 1648, 2291, m_byte_data.data());
        sane_in_the_membrane::utils::write_image("scan.png", m_params.pixels_per_line, m_params.lines, m_byte_data.data());
    } else {
        std::cout << "Not ok\n";
        std::cout << "Error details: " << s.error_details() << "\n";
    }

    reset();
    emit sig_done(s);
}

void reader::CScanResponseReader::reset() {
    reset_context();
    m_byte_data = std::vector<char>{};
}

void reader::CScanResponseReader::reset_context() {
    delete m_context;
    m_context = nullptr;
}
