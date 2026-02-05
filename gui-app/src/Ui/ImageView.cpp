#include "ImageView.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qforeach.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include "../Utils/Globals.hpp"
#include <QFileDialog>
#include <string>
#include "../Utils/Pdf/Pdf.hpp"
#include <QImage>
#include <QBuffer>

using namespace sane_in_the_membrane::ui;
using namespace sane_in_the_membrane::utils::pdf;

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
std::shared_ptr<sane_in_the_membrane::utils::CFile> CImageItem::file() {
    return m_file;
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
    QWidget(parent), m_main_layout(new QVBoxLayout(this)), m_image_container(new QWidget()), m_grid(new QHBoxLayout(m_image_container)), m_scroll(new QScrollArea(this)),
    m_save(new QPushButton("Save", this)) {
    QObject::connect(&utils::Globals::get()->m_scan_response_reader, &reader::CScanResponseReader::sig_done, this, &CImageView::sl_sig_done);

    m_scroll->setWidgetResizable(true);
    m_scroll->setWidget(m_image_container);

    m_main_layout->addWidget(m_save);
    m_main_layout->addWidget(m_scroll);

    connect(m_save, &QPushButton::clicked, this, [this]() {
        auto filename = QFileDialog::getSaveFileName(this, "Save pdf");
        if (filename.isEmpty()) {
            std::cout << "Empty, exiting\n";
            return;
        }

        sane_in_the_membrane::utils::pdf::CPdfBuilder builder{};
        builder.set_error_handler(sane_in_the_membrane::utils::pdf::CPdf::error_handler);
        builder.set_compression_mode(HPDF_COMP_ALL);
        auto pdf = builder.build();

        for (const auto& item : m_items) {
            const auto file = item->file();
            pdf.add_jpeg(file->path());
        }

        pdf.save(filename.toStdString());
    });
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

    auto   image_file = utils::Globals::get()->m_file_manager.new_temp_file_with_extension(".jpg");
    auto   file_data  = file->read();

    QImage img{file_data.data(), params->width(), params->height(), params->bytes_per_line, QImage::Format::Format_RGB888};
    img.save(image_file->path().c_str(), "jpg");

    add_image(image_file);
}
