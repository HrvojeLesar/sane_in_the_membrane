#include "ImageView.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <qboxlayout.h>
#include <qforeach.h>
#include <qimage.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qwidget.h>
#include "../Utils/Globals.hpp"
#include <QFileDialog>
#include <ranges>
#include <string>
#include "../Utils/Pdf/Pdf.hpp"
#include "Assert.hpp"
#include "Image/ImageToolbar.hpp"
#include "Service/SessionService.hpp"
#include "Utils/File.hpp"
#include <QImage>
#include <QBuffer>
#include <QTransform>
#include <QObject>
#include <utility>
#include <vector>
#include <QTimer>
#include <GLogger.hpp>

using namespace sane_in_the_membrane;
using namespace sane_in_the_membrane::ui;
using namespace sane_in_the_membrane::utils::pdf;

CImageItem::SQtMatrix::SQtMatrix() {}
CImageItem::SQtMatrix::SQtMatrix(QTransform& transform) :
    m11(transform.m11()), m12(transform.m12()), m13(transform.m13()), m21(transform.m21()), m22(transform.m22()), m23(transform.m23()), m31(transform.m31()), m32(transform.m32()),
    m33(transform.m33()) {}

CImageItem::SImageItemSerialized::SImageItemSerialized() {}
CImageItem::SImageItemSerialized::SImageItemSerialized(CImageItem& item) :
    transform_matrix(item.m_transform), page_number(item.m_page_number), path(item.m_file->path().string()) {}

service::Bytes CImageItem::SImageItemSerialized::serialize() const {
    auto           path_size = path.size();
    service::Bytes bytes(sizeof(transform_matrix) + sizeof(page_number) + sizeof(path_size) + path_size);

    auto           data_ptr = bytes.data();

    std::memcpy(data_ptr, &transform_matrix, sizeof(transform_matrix));
    data_ptr += sizeof(transform_matrix);

    std::memcpy(data_ptr, &page_number, sizeof(page_number));
    data_ptr += sizeof(page_number);

    std::memcpy(data_ptr, &path_size, sizeof(path_size));
    data_ptr += sizeof(path_size);

    std::memcpy(data_ptr, path.data(), path_size);

    return bytes;
}

std::optional<CImageItem::SImageItemSerialized> CImageItem::SImageItemSerialized::deserialize(const unsigned char* start, const unsigned char* end) {
    SImageItemSerialized out;

    if (start + sizeof(out.transform_matrix) > end) {
        log::error("Provided bytes cannot be converted into transform_matrix");
        return std::nullopt;
    }
    std::memcpy(&out.transform_matrix, start, sizeof(out.transform_matrix));
    start += sizeof(out.transform_matrix);

    if (start + sizeof(out.page_number) > end) {
        log::error("Provided bytes cannot be converted into page_number");
        return std::nullopt;
    }

    std::memcpy(&out.page_number, start, sizeof(out.page_number));
    start += sizeof(out.page_number);

    std::size_t path_size = 0;
    if (start + sizeof(path_size) > end) {
        log::error("Provided bytes cannot be converted into path_size");
        return std::nullopt;
    }

    std::memcpy(&path_size, start, sizeof(path_size));
    start += sizeof(path_size);

    if (start + path_size > end) {
        log::error("Provided bytes cannot be converted into path");
        return std::nullopt;
    }
    out.path.assign(reinterpret_cast<const char*>(start), path_size);

    return out;
}

std::size_t CImageItem::SImageItemSerialized::size() const {
    return sizeof(transform_matrix) + sizeof(page_number) + sizeof(path) + path.size();
}

CImageItem::~CImageItem() {}

void CImageItem::setup_layout_and_connections() {
    QObject::connect(m_toolbar->m_btn_rotate_left, &QPushButton::clicked, this, [this]() {
        m_transform.rotate(-90);
        set_pixmap();
        emit sig_transform(this);
    });

    QObject::connect(m_toolbar->m_btn_rotate_right, &QPushButton::clicked, this, [this]() {
        m_transform.rotate(90);
        set_pixmap();
        emit sig_transform(this);
    });
    QObject::connect(m_toolbar->m_btn_mirror, &QPushButton::clicked, this, [this]() {
        m_transform.scale(-1, 1);
        set_pixmap();
        emit sig_transform(this);
    });

    QObject::connect(m_toolbar->m_btn_delete, &QPushButton::clicked, this, &CImageItem::sl_remove_me);

    QObject::connect(m_toolbar->m_btn_move_prev, &QPushButton::clicked, this, [this]() { emit sig_move_page_by(this, EMoveDirection::LEFT); });
    QObject::connect(m_toolbar->m_btn_move_next, &QPushButton::clicked, this, [this]() { emit sig_move_page_by(this, EMoveDirection::RIGHT); });

    m_layout->setContentsMargins(8, 8, 8, 8);
    m_layout->setSpacing(4);

    set_pixmap();

    m_toolbar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_layout->addWidget(m_image_label, 0, Qt::AlignHCenter);
    m_layout->addWidget(m_toolbar, 0, Qt::AlignHCenter);
}

CImageItem::CImageItem(std::shared_ptr<utils::CFile>& file, std::size_t page_number, QWidget* parent) :
    QWidget(parent), m_file(file), m_layout(new QVBoxLayout(this)), m_image_label(new QLabel(this)), m_pixmap(QString::fromStdString(file->path().string())),
    m_toolbar(new image::CImageToolbar{page_number}), m_page_number(page_number) {
    setup_layout_and_connections();
}

CImageItem::CImageItem(SImageItemSerialized& serialized_item, QWidget* parent) :
    QWidget(parent), m_file(std::make_shared<utils::CFile>(serialized_item.path)), m_layout(new QVBoxLayout(this)), m_image_label(new QLabel(this)),
    m_pixmap(QString::fromStdString(serialized_item.path)), m_toolbar(new image::CImageToolbar{serialized_item.page_number}), m_page_number(serialized_item.page_number) {
    m_transform.setMatrix(serialized_item.transform_matrix.m11, serialized_item.transform_matrix.m12, serialized_item.transform_matrix.m13, serialized_item.transform_matrix.m21,
                          serialized_item.transform_matrix.m22, serialized_item.transform_matrix.m23, serialized_item.transform_matrix.m31, serialized_item.transform_matrix.m32,
                          serialized_item.transform_matrix.m33);
    setup_layout_and_connections();
}

const std::shared_ptr<utils::CFile>& CImageItem::file_ref() const {
    return m_file;
}

std::shared_ptr<utils::CFile> CImageItem::file() const {
    return m_file;
}

void CImageItem::sl_remove_me() {
    emit sig_remove_requested(this);
}

void CImageItem::set_pixmap() {
    constexpr int MAX_W = 600;
    constexpr int MAX_H = 600;

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

CImageItemManager::~CImageItemManager() {
    std::ranges::for_each(m_items, [](CImageItem* item) { item->deleteLater(); });
}

CImageItem* CImageItemManager::add_item(CImageItem* item) {
    m_items.push_back(item);

    return item;
}

void CImageItemManager::remove_item(CImageItem* item) {
    auto erased = std::erase(m_items, item);
    SITM_ASSERT(erased == 1, "Tried erasing a non existant item");

    item->deleteLater();
}

const std::vector<CImageItem*>& CImageItemManager::items() const {
    return m_items;
}

CImageItemContainer::CImageItemContainer(QWidget* parent) : QHBoxLayout(parent) {}

CImageItemContainer::~CImageItemContainer() {}

CImageItem* CImageItemContainer::add_image(std::shared_ptr<utils::CFile>& file, QWidget* parent) {
    auto item = new CImageItem{file, m_manager.items().size() + 1, parent};
    add_image(item);

    return item;
}

CImageItem* CImageItemContainer::add_image(CImageItem* item) {
    m_manager.add_item(item);
    addWidget(item);

    return item;
}

CImageItem* CImageItemContainer::add_image(CImageItem::SImageItemSerialized& serialized_item, QWidget* parent) {
    auto item = new CImageItem(serialized_item, parent);
    add_image(item);

    return item;
}

void CImageItemContainer::remove_image(CImageItem* item) {
    removeWidget(item);

    auto items = m_manager.items();
    std::ranges::for_each(items | std::views::drop(item->get_page_number()), [](CImageItem* item) { item->set_page_number(item->get_page_number() - 1); });

    m_manager.remove_item(item);
}

void CImageItemContainer::move_image(CImageItem* item, EMoveDirection direction) {
    auto page = item->get_page_number();
    if (page <= 0)
        return;

    auto swap_with_page = direction == EMoveDirection::LEFT ? page - 1 : page + 1;
    if (swap_with_page == 0 || swap_with_page > count())
        return;

    auto page_index           = page - 1;
    auto swap_with_page_index = swap_with_page - 1;

    if (swap_with_page_index > page_index)
        std::swap(page_index, swap_with_page_index);

    auto* last_item  = qobject_cast<CImageItem*>(itemAt(page_index)->widget());
    auto* first_item = qobject_cast<CImageItem*>(itemAt(swap_with_page_index)->widget());

    {
        auto page = last_item->get_page_number();
        last_item->set_page_number(first_item->get_page_number());
        first_item->set_page_number(page);
    }

    insertWidget(swap_with_page_index, last_item);
    insertWidget(page_index, first_item);
}

std::size_t CImageItemContainer::image_count() const {
    return m_manager.items().size();
}

const std::vector<CImageItem*>& CImageItemContainer::items() const {
    return m_manager.items();
}

CImageView::CImageView(std::string filepath, QWidget* parent) :
    QWidget(parent), m_main_layout(new QVBoxLayout(this)), m_image_container(new QWidget()), m_grid(new CImageItemContainer(m_image_container)),
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
    auto image_paths = std::views::transform(m_grid->items(), [](const CImageItem* item) { return item->file_ref()->path().string(); });
    pdf.add_jpegs(image_paths.begin(), image_paths.end());
    pdf.save(selected_file_name.toStdString());

#ifdef OCR
    if (m_ocr_processor.is_ocrmypdf_installed() && m_ocr_checkbox->checkState() == Qt::CheckState::Checked)
        m_ocr_processor.ocr(selected_file_name);
#endif

    emit sig_document_saved();

    m_session.remove_session();
}

void CImageView::add_image(std::shared_ptr<utils::CFile>& file) {
    auto item = m_grid->add_image(file, this);
    init_image_connetions(item);
}

void CImageView::add_image(CImageItem::SImageItemSerialized& serialized_item) {
    auto item = m_grid->add_image(serialized_item, this);
    init_image_connetions(item);
}

void CImageView::init_image_connetions(CImageItem* item) {
    connect(item, &CImageItem::sig_remove_requested, this, &CImageView::sl_remove_image);
    connect(item, &CImageItem::sig_move_page_by, this, &CImageView::sl_move_image);
    connect(item, &CImageItem::sig_transform, this, &CImageView::sl_transform);

    emit sig_document_changed(m_grid->image_count());

    save_session();
}

void CImageView::sl_remove_image(CImageItem* item) {
    m_grid->remove_image(item);

    emit sig_document_changed(m_grid->count());

    save_session();
}

void CImageView::sl_transform(CImageItem* item) {
    save_session();
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

void CImageView::sl_move_image(CImageItem* item, EMoveDirection direction) {
    m_grid->move_image(item, direction);

    save_session();
}

service::Bytes CImageView::serialize_items() {
    service::Bytes data{};
    for (auto& item : m_grid->items()) {
        CImageItem::SImageItemSerialized serializable_item{*item};
        auto                             serialized_data = serializable_item.serialize();
        auto                             size            = serialized_data.size();

        data.resize(data.size() + sizeof(size));
        std::memcpy(data.data() + data.size() - sizeof(size), &size, sizeof(size));

        data.insert(data.end(), serialized_data.begin(), serialized_data.end());
    }

    return data;
}

std::expected<std::vector<CImageItem::SImageItemSerialized>, std::string> CImageView::deserialize_items(service::Bytes& data) {
    auto                                          start = data.data();
    auto                                          end   = start + data.size();

    std::vector<CImageItem::SImageItemSerialized> items{};
    while (start < end) {
        std::size_t item_length = *reinterpret_cast<std::size_t*>(start);
        auto        item_start  = start + sizeof(std::size_t);
        auto        item_end    = item_start + item_length;

        if (item_end > end)
            return std::unexpected{"Item end is outside of allowed range"};
        // SITM_ASSERT(item_end <= end, "Item end is outside of allowed range");
        auto item_data = CImageItem::SImageItemSerialized::deserialize(item_start, item_end);
        if (item_data)
            items.push_back(std::move(item_data).value());

        start = item_end;
    }

    return items;
}

void CImageView::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    QTimer::singleShot(0, this, [this]() {
        auto session = m_session.load_session();
        if (!session.has_value())
            return;

        auto items = deserialize_items(session.value().data);
        if (!items.has_value())
            return;

        QMessageBox session_detected_box{};
        session_detected_box.setText("Detected previous scan session. Load session ?");

        session_detected_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        session_detected_box.setDefaultButton(QMessageBox::Ok);

        auto exec_status = session_detected_box.exec();

        std::sort(items.value().begin(), items.value().end(),
                  [](const CImageItem::SImageItemSerialized& a, const CImageItem::SImageItemSerialized& b) { return a.page_number < b.page_number; });

        switch (exec_status) {
            case QMessageBox::Ok:
            case QMessageBox::Close:
                for (auto& item : items.value())
                    add_image(item);

            default: break;
        }

        m_session_saving_disabled = false;
    });
}

void CImageView::save_session() {
    if (m_session_saving_disabled)
        return;

    auto data = serialize_items();
    m_session.save_session(data);
}

void CImageView::remove_session() {
    m_session.remove_session();
}
