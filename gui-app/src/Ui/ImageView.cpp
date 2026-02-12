#include "ImageView.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
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
#include "Image/ImageToolbar.hpp"
#include <QImage>
#include <QBuffer>
#include <QTransform>
#include <QObject>
#include <utility>
#include <vector>

using namespace sane_in_the_membrane::ui;
using namespace sane_in_the_membrane::utils::pdf;

CImageItem::~CImageItem() {}
CImageItem::CImageItem(std::shared_ptr<utils::CFile> file, uint32_t page_number, QWidget* parent) :
    QWidget(parent), m_file(file), m_layout(new QVBoxLayout(this)), m_image_label(new QLabel(this)), m_pixmap(QString::fromStdString(file->path())),
    m_toolbar(new image::CImageToolbar{page_number}), m_page_number(page_number) {

    QObject::connect(m_toolbar->m_btn_rotate_left, &QPushButton::clicked, this, [this]() {
        m_transform.rotate(-90);
        set_pixmap();
    });
    QObject::connect(m_toolbar->m_btn_rotate_right, &QPushButton::clicked, this, [this]() {
        m_transform.rotate(90);
        set_pixmap();
    });
    QObject::connect(m_toolbar->m_btn_mirror, &QPushButton::clicked, this, [this]() {
        m_transform.scale(-1, 1);
        set_pixmap();
    });

    QObject::connect(m_toolbar->m_btn_delete, &QPushButton::clicked, this, &CImageItem::sl_remove_me);

    QObject::connect(m_toolbar->m_btn_move_prev, &QPushButton::clicked, this, [this]() { emit sig_move_page_by(this, EMovePage::LEFT); });
    QObject::connect(m_toolbar->m_btn_move_next, &QPushButton::clicked, this, [this]() { emit sig_move_page_by(this, EMovePage::RIGHT); });

    m_layout->setContentsMargins(8, 8, 8, 8);
    m_layout->setSpacing(4);

    m_image_label->setAlignment(Qt::AlignCenter);

    set_pixmap();

    m_layout->addWidget(m_image_label);
    m_layout->addWidget(m_toolbar);
}
std::shared_ptr<sane_in_the_membrane::utils::CFile> CImageItem::file() {
    return m_file;
}

void CImageItem::sl_remove_me() {
    emit sig_remove_requested();
}

void CImageItem::set_pixmap() {
    constexpr int MAX_W = 640;
    constexpr int MAX_H = 640;

    if (m_preview_disabled) {
        m_image_label->setText("Preview disabled");
    } else {
        if (!m_pixmap.isNull()) {
            m_image_label->setPixmap(m_pixmap.transformed(m_transform, Qt::SmoothTransformation).scaled(MAX_W, MAX_H, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            m_image_label->setText("Preview failed");
        }
    }
}

void CImageItem::resizeEvent(QResizeEvent* event) {
    // set_pixmap();
}

void CImageItem::set_page_number(uint32_t page_number) {
    m_page_number = page_number;
    m_toolbar->set_page_number(page_number);
}
uint32_t CImageItem::get_page_number() {
    return m_page_number;
}

CImageView::CImageView(std::string filepath, QWidget* parent) :
    QWidget(parent), m_main_layout(new QVBoxLayout(this)), m_image_container(new QWidget()), m_grid(new QHBoxLayout(m_image_container)), m_scroll(new QScrollArea(this)),
    m_save(new QPushButton("Save", this)) {
    QObject::connect(&utils::Globals::get_instance().proxies()->m_scan_response_reader_proxy, &utils::proxy::CScanResponseReaderProxy::sig_done, this, &CImageView::sl_sig_done);

    m_scroll->setWidgetResizable(true);
    m_scroll->setWidget(m_image_container);

    m_main_layout->addWidget(m_save);
    m_main_layout->addWidget(m_scroll);

    connect(m_save, &QPushButton::clicked, this, [this]() {
        auto filename = QFileDialog::getSaveFileName(this, "Save pdf");
        if (filename.isEmpty()) {
            g_logger.log(DEBUG, "Empty, exiting");
            return;
        }

        sane_in_the_membrane::utils::pdf::CPdfBuilder builder{};
        builder.set_error_handler(sane_in_the_membrane::utils::pdf::CPdf::error_handler);
        builder.set_compression_mode(HPDF_COMP_ALL);
        auto pdf = builder.build();

        for (const auto& item : get_image_items()) {
            const auto file = item.second->file();
            pdf.add_jpeg(file->path());
        }

        pdf.save(filename.toStdString());
    });
}

void CImageView::add_image(std::shared_ptr<utils::CFile> file) {
    auto item_count_guard = m_item_count.access();
    *item_count_guard += 1;
    auto* item = new CImageItem(file, *item_count_guard, this);

    m_grid->addWidget(item);

    connect(item, &CImageItem::sig_remove_requested, this, [this, item]() { remove_item(item); });

    connect(item, &CImageItem::sig_move_page_by, this, [this](CImageItem* item, EMovePage move_to) {
        auto               items = get_image_items();
        std::optional<int> potential_item_index{std::nullopt};

        for (const auto& other_item : items) {
            if (other_item.second == item) {
                potential_item_index = other_item.first;
                break;
            }
        }

        if (!potential_item_index.has_value()) {
            return;
        }

        auto item_index = potential_item_index.value();
        auto increment  = move_to == EMovePage::LEFT ? -1 : 1;

        int  set_item_to_index = item_index + increment;
        if (set_item_to_index < 0 || static_cast<size_t>(set_item_to_index) >= *(m_item_count.shared_access())) {
            return;
        }

        auto* other_item = qobject_cast<CImageItem*>(m_grid->itemAt(set_item_to_index)->widget());

        if (set_item_to_index > item_index) {
            std::swap(set_item_to_index, item_index);
            std::swap(item, other_item);
        }

        auto page_number = item->get_page_number();
        item->set_page_number(other_item->get_page_number());
        other_item->set_page_number(page_number);

        m_grid->insertWidget(set_item_to_index, item);
        m_grid->insertWidget(item_index, other_item);
    });
}

void CImageView::remove_item(CImageItem* item) {
    // TODO: Maybe mark as deleted and delete actual file when closing program ?
    // utils::Globals::get()->m_file_manager.delete_file(item->file());
    m_grid->removeWidget(item);
    item->deleteLater();

    *m_item_count.access() -= 1;

    auto items = get_image_items();
    for (size_t item_index = 0; item_index < items.size(); item_index++) {
        auto item = items.at(item_index).second;
        item->set_page_number(item_index + 1);
    }
}

void CImageView::sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params) {
    if (!status->ok()) {
        return;
    }

    if (static_cast<size_t>(params->pixels_per_line * params->lines) > file->size()) {
        g_logger.log(INFO, "File size does not correspond to params");
        return;
    }

    auto   image_file = utils::Globals::get_instance().file_manager()->new_temp_file_with_extension(".jpg");
    auto   file_data  = file->read();

    QImage img{file_data.data(), static_cast<int>(params->width()), static_cast<int>(params->height()), params->bytes_per_line, QImage::Format::Format_RGB888};
    img.save(image_file->path().c_str(), "jpg");

    add_image(image_file);
}

std::vector<std::pair<int, CImageItem*>> CImageView::get_image_items() {
    std::vector<std::pair<int, CImageItem*>> items{};

    for (int i = 0; i < m_grid->count(); i++) {
        auto potential_item = m_grid->itemAt(i);
        if (potential_item) {
            auto widget = potential_item->widget();
            if (widget) {
                auto* item = qobject_cast<CImageItem*>(widget);
                if (item) {
                    items.emplace_back(i, item);
                }
            }
        }
    }

    return items;
}
