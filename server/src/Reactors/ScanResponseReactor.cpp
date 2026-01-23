#include "ScanResponseReactor.hpp"

using namespace reactor;

CScanResponseReactor::CScanResponseReactor(std::shared_ptr<sane::CSaneDevice> m_device) : m_device(m_device) {
    if (m_device.get() != nullptr) {
        next_packet();
    } else {
        std::cout << "Device not found exiting\n";
        Finish(grpc::Status(grpc::StatusCode::ABORTED, "Device not found"));
    }
}

CScanResponseReactor::~CScanResponseReactor() {}

void CScanResponseReactor::OnDone() {
    delete this;
}

void CScanResponseReactor::OnCancel() {
    std::cout << "Cancel\n";
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

void CScanResponseReactor::next_packet() {
    std::cout << "Next packet\n";
    Finish(grpc::Status::OK);
}
