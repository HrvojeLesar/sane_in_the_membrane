#include "ScanResponseReactor.hpp"

using namespace reactor;

CScanResponseReactor::CScanResponseReactor(std::shared_ptr<sane::CSaneDevice> m_device) : m_device(m_device) {
    if (m_device.get() != nullptr) {
        m_byte_data.reserve(10240);
        start_scan();
    } else {
        std::cout << "Device not found exiting\n";
        Finish(grpc::Status(grpc::StatusCode::ABORTED, "Device not found"));
    }
}

CScanResponseReactor::~CScanResponseReactor() {}

void CScanResponseReactor::OnDone() {
    m_device->close();

    delete this;
}

void CScanResponseReactor::OnCancel() {
    std::cout << "Cancel\n";
    m_device->cancel();
}

void CScanResponseReactor::OnWriteDone(bool ok) {
    std::cout << "Write done\n";
    if (!ok) {
        std::cout << "Write done not ok\n";
        Finish(grpc::Status(grpc::StatusCode::UNKNOWN, "Write failed"));
    } else {
        next_packet();
    }
}

void CScanResponseReactor::write_response() {
    m_byte_data_string->assign(m_byte_data.begin(), m_byte_data.end());
    m_data->set_allocated_data(m_byte_data_string);
    m_response.set_allocated_data(m_data);

    StartWrite(&m_response);
}

void CScanResponseReactor::next_packet() {
    std::cout << "Next packet\n";
    reset_response();

    while (true) {
        auto status = m_device->read();
        if ((status.is_ok() || status == SANE_STATUS_EOF) && m_buffer.read_len > 0) {
            m_byte_data.insert(m_byte_data.end(), m_buffer.m_data.begin(), m_buffer.m_data.begin() + m_buffer.read_len);
        }

        if (m_byte_data.size() >= 10240) {
            write_response();
            return;
        }

        if (!status.is_ok()) {
            if (status == SANE_STATUS_EOF) {
                write_response();
                break;
            }

            Finish(grpc::Status(grpc::StatusCode::ABORTED, std::format("Failed to read from device: {}", status.str_status())));
            return;
        }
    }

    Finish(grpc::Status::OK);
}

void CScanResponseReactor::write_sane_parameters(const SANE_Parameters& parameters) {
    m_parameters = new ScanParameters();
    m_parameters->set_format(parameters.format);
    m_parameters->set_last_frame(parameters.last_frame);
    m_parameters->set_bytes_per_line(parameters.bytes_per_line);
    m_parameters->set_pixels_per_line(parameters.pixels_per_line);
    m_parameters->set_lines(parameters.lines);
    m_parameters->set_depth(parameters.depth);

    m_response.set_allocated_parameters(m_parameters);
}

void CScanResponseReactor::reset_response() {
    std::cout << "Resetting response\n";
    m_response.Clear();
    m_byte_data.clear();
    m_data             = new ScanResponseData();
    m_byte_data_string = new std::string();
    std::cout << "Reset response\n";
}

void CScanResponseReactor::start_scan() {
    auto status = m_device->open();
    if (!status.is_ok()) {
        return Finish(grpc::Status(grpc::StatusCode::CANCELLED, std::format("Failed to open device: {}", status.str_status())));
    }

    status = m_device->start();
    if (!status.is_ok()) {
        return Finish(grpc::Status(grpc::StatusCode::CANCELLED, std::format("Failed to start device: {}", status.str_status())));
    }

    status = m_device->refresh_parameters();
    if (!status.is_ok()) {
        std::cout << std::format("Failed to refresh parameters: {}\n", status.str_status());
        return Finish(grpc::Status(grpc::StatusCode::CANCELLED, std::format("Failed to start device: {}", status.str_status())));
    }

    write_sane_parameters(m_device->parameters());

    StartWrite(&m_response);
}

void CScanResponseReactor::Finish(grpc::Status status) {
    auto finished = m_finished.access();
    if (*finished) {
        return;
    }
    *finished = true;

    grpc::ServerWriteReactor<ScanResponse>::Finish(status);
}
