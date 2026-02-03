#include "SaneStatus.hpp"

using namespace sane_in_the_membrane;

sane::CSaneStatus::CSaneStatus(SANE_Status status) : m_status(status) {}
bool sane::CSaneStatus::is_ok() const {
    return m_status == SANE_STATUS_GOOD;
}

bool sane::CSaneStatus::operator==(const CSaneStatus& other) const {
    return m_status == other.m_status;
}

bool sane::CSaneStatus::operator!=(const CSaneStatus& other) const {
    return m_status != other.m_status;
}

bool sane::CSaneStatus::operator==(const SANE_Status& other) const {
    return m_status == other;
}

bool sane::CSaneStatus::operator!=(const SANE_Status& other) const {
    return m_status != other;
}

SANE_String_Const sane::CSaneStatus::str_status() const {
    return sane_strstatus(m_status);
}
