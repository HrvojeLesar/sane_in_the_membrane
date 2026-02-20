#include "ProgressBar.hpp"
#include "../../Utils/Globals.hpp"

using namespace sane_in_the_membrane::ui::scan;

CScanProgress::CScanProgress(QWidget* parent) : QProgressBar(parent) {
    setMinimum(0);
    setMaximum(100);

    connect(&utils::Globals::get_instance().proxies()->m_scan_response_reader_proxy, &utils::proxy::CScanResponseReaderProxy::sig_progress, this,
            [this](double progress) { setValue(progress); });
};
