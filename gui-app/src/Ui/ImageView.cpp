#include "ImageView.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <qboxlayout.h>
#include <qforeach.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qwidget.h>
#include "../Utils/Globals.hpp"
#include <QFileDialog>
#include <ranges>
#include <string>
#include "../Utils/Pdf/Pdf.hpp"
#include "Image/ImageToolbar.hpp"
#include <QImage>
#include <QBuffer>
#include <QTransform>
#include <QObject>
#include <utility>
#include <vector>
#include <GLogger.hpp>

using namespace sane_in_the_membrane;
using namespace sane_in_the_membrane::ui;
using namespace sane_in_the_membrane::utils::pdf;

CImageItem::~CImageItem() {}
CImageItem::CImageItem(std::shared_ptr<utils::CFile>& file, std::size_t page_number, QWidget* parent) :
    QWidget(parent), m_file(file), m_layout(new QVBoxLayout(this)), m_image_label(new QLabel(this)), m_pixmap(QString::fromStdString(file->path().string())),
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

    set_pixmap();

    m_toolbar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_layout->addWidget(m_image_label, 0, Qt::AlignHCenter);
    m_layout->addWidget(m_toolbar, 0, Qt::AlignHCenter);
}

const std::shared_ptr<utils::CFile>& CImageItem::file_ref() const {
    return m_file;
}

std::shared_ptr<utils::CFile> CImageItem::file() const {
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

void CImageItem::set_page_number(std::size_t page_number) {
    m_page_number = page_number;
    m_toolbar->set_page_number(page_number);
}

std::size_t CImageItem::get_page_number() {
    return m_page_number;
}

CImageView::CImageView(std::string filepath, QWidget* parent) :
    QWidget(parent), m_main_layout(new QVBoxLayout(this)), m_image_container(new QWidget()), m_grid(new QHBoxLayout(m_image_container)),
    m_scroll(new image::CImageHorizontalScroll(this)), m_save(new QPushButton("Save", this))
#ifdef OCR
    ,
    m_ocr_checkbox(new QCheckBox("OCR", this))
#endif

{
    QObject::connect(&utils::Globals::get_instance().proxies()->m_scan_response_reader_proxy, &utils::proxy::CScanResponseReaderProxy::sig_done, this, &CImageView::sl_sig_done);

    m_scroll->setWidget(m_image_container);

    auto save_layout = new QHBoxLayout();
    save_layout->addWidget(m_save);
#ifdef OCR
    if (m_ocr_processor.is_ocrmypdf_installed())
        save_layout->addWidget(m_ocr_checkbox);
    else
        m_ocr_checkbox->setVisible(false);
#endif

    m_main_layout->addLayout(save_layout);
    m_main_layout->addWidget(m_scroll);


    connect(m_save, &QPushButton::clicked, this, &CImageView::sl_save_pdf);
}

void CImageView::sl_save_pdf() {
    QFileDialog dialog(this, "Save pdf", QString(), "Pdf (*.pdf)");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("pdf");

    if (dialog.exec() != QDialog::Accepted)
        return;

    auto        selected_files     = dialog.selectedFiles();
    const auto& selected_file_name = selected_files.first();
    if (selected_file_name.isEmpty()) {
        log::debug("Empty, exiting");
        return;
    }

    auto pdf         = sane_in_the_membrane::utils::pdf::CPdfBuilder::build_default();
    auto image_paths = std::views::transform(image_items(), [](const CImageItem* item) { return item->file_ref()->path().string(); });
    pdf.add_jpegs(image_paths.begin(), image_paths.end());
    pdf.save(selected_file_name.toStdString());

#ifdef OCR
    if (m_ocr_processor.is_ocrmypdf_installed() && m_ocr_checkbox->checkState() == Qt::CheckState::Checked)
        m_ocr_processor.ocr(selected_file_name);
#endif
}

void CImageView::add_image(std::shared_ptr<utils::CFile>& file) {
    auto  items = image_items();
    auto* item  = new CImageItem(file, items.size() + 1, this);

    m_grid->addWidget(item);

    log::debug("Grid count: {}", m_grid->count());

    connect(item, &CImageItem::sig_remove_requested, this, [this, item]() { remove_item(item); });

    connect(item, &CImageItem::sig_move_page_by, this, [this](CImageItem* item, EMovePage move_to) {
        auto page = item->get_page_number();
        if (page <= 0)
            return;

        auto swap_with_page = move_to == EMovePage::LEFT ? page - 1 : page + 1;
        if (swap_with_page == 0 || swap_with_page > m_grid->count())
            return;

        auto page_index           = page - 1;
        auto swap_with_page_index = swap_with_page - 1;

        if (swap_with_page_index > page_index)
            std::swap(page_index, swap_with_page_index);

        auto* last_item  = qobject_cast<CImageItem*>(m_grid->itemAt(page_index)->widget());
        auto* first_item = qobject_cast<CImageItem*>(m_grid->itemAt(swap_with_page_index)->widget());

        {
            auto page = last_item->get_page_number();
            last_item->set_page_number(first_item->get_page_number());
            first_item->set_page_number(page);
        }

        m_grid->insertWidget(swap_with_page_index, last_item);
        m_grid->insertWidget(page_index, first_item);
    });
}

void CImageView::remove_item(CImageItem* item) {
    // TODO: Maybe mark as deleted and delete actual file when closing program ?
    // utils::Globals::get()->m_file_manager.delete_file(item->file());
    m_grid->removeWidget(item);
    item->deleteLater();

    auto items = image_items();
    std::ranges::for_each(items | std::views::drop(item->get_page_number()), [](CImageItem* item) { item->set_page_number(item->get_page_number() - 1); });
}

void CImageView::sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params) {
    if (!status->ok()) {
        return;
    }

    if (static_cast<size_t>(params->pixels_per_line * params->lines) > file->size()) {
        log::info("File size does not correspond to params");
        return;
    }

    auto   image_file = utils::Globals::get_instance().file_manager()->new_temp_file_with_extension(".jpg");
    auto   file_data  = file->read();

    QImage img{file_data.data(), static_cast<int>(params->width()), static_cast<int>(params->height()), params->bytes_per_line, QImage::Format::Format_RGB888};
    img.save(image_file->path().string().c_str(), "jpg");

    add_image(image_file);
}

QList<CImageItem*> CImageView::image_items() const {
    return QWidget::findChildren<CImageItem*>();
}
