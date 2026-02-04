#include "ImageView.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include "../Utils/Globals.hpp"
#include <image/MagickImageWrapper.hpp>

using namespace sane_in_the_membrane::ui;

CImageItem::~CImageItem() {}
CImageItem::CImageItem(std::shared_ptr<utils::CFile> file, QWidget* parent) :
    QWidget(parent), m_file(file), m_layout(new QVBoxLayout(this)), m_image_label(new QLabel(this)), m_pixmap(QString::fromStdString(file->path())),
    m_close_button(new QPushButton("x", this)) {
    m_layout->setContentsMargins(8, 8, 8, 8);
    m_layout->setSpacing(4);

    m_image_label->setScaledContents(true);
    m_image_label->setAlignment(Qt::AlignCenter);

    set_pixmap();

    m_close_button->setFixedSize(28, 28);
    m_close_button->setStyleSheet("QPushButton { background: rgba(0,0,0,140); color: white; border-radius: 14px; font-weight: bold; }"
                                  "QPushButton:hover { background: red; }");

    m_close_button->raise();

    m_layout->addWidget(m_image_label);

    connect(m_close_button, &QPushButton::clicked, this, &CImageItem::sl_remove_me);
}

void CImageItem::sl_remove_me() {
    emit sig_remove_requested();
}

void CImageItem::set_pixmap() {
    m_image_label->setMaximumSize(720, 1280);
    if (!m_pixmap.isNull()) {
        m_image_label->setPixmap(m_pixmap.scaled(720, 1280, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_image_label->setText("Preview failed");
    }
}

void CImageItem::resizeEvent(QResizeEvent* event) {
    set_pixmap();
}

CImageView::CImageView(std::string filepath, QWidget* parent) :
    QWidget(parent), m_main_layout(new QVBoxLayout(this)), m_image_container(new QWidget()), m_grid(new QHBoxLayout(m_image_container)), m_scroll(new QScrollArea(this)) {
    QObject::connect(&utils::Globals::get()->m_scan_response_reader, &reader::CScanResponseReader::sig_done, this, &CImageView::sl_sig_done);

    m_scroll->setWidgetResizable(true);
    m_scroll->setWidget(m_image_container);

    m_main_layout->addWidget(m_scroll);
}

void CImageView::add_image(std::shared_ptr<utils::CFile> file) {
    auto* item = new CImageItem(file, this);

    m_grid->addWidget(item);
    m_items.push_back(item);

    connect(item, &CImageItem::sig_remove_requested, this, [this, item, file]() {
        m_items.erase(std::remove(m_items.begin(), m_items.end(), item), m_items.end());
        utils::Globals::get()->m_file_manager.delete_file(file);
        item->deleteLater();
    });
}

void CImageView::sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params) {
    if (!status->ok()) {
        return;
    }

    if (static_cast<size_t>(params->pixels_per_line * params->lines) > file->size()) {
        std::cout << "File size does not correspond to params\n";
        return;
    }

    auto image_file = utils::Globals::get()->m_file_manager.new_temp_file_with_extension(".png");
    sane_in_the_membrane::utils::write_image(image_file->path(), params->pixels_per_line, params->lines, file->read().data());

    add_image(image_file);
}
