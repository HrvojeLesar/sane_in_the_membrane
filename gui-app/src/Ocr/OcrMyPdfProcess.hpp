#include <qtmetamacros.h>
#ifdef OCR
#ifndef OCR_OCR_MY_PDF_PROCESS
#define OCR_OCR_MY_PDF_PROCESS

#include <QProcess>
#include <qobject.h>

namespace sane_in_the_membrane::ocr {
    class COcrMyPdfNotifier : public QObject {
        Q_OBJECT

      public:
        COcrMyPdfNotifier(QProcess& process);
        void disconnect_connections();
        void connect();

      private:
        QProcess& m_process;
    };

    class COcrMyPdfProcess {

      public:
        COcrMyPdfProcess();
        bool is_ocrmypdf_installed() const;
        void ocr(QString& input_file);

      private:
        void start(QString& input_file);
        void stop();

      private:
        QProcess          m_process{};
        COcrMyPdfNotifier m_notifier;
        bool              m_installed{};
    };
}

#endif // !OCR_OCR_MY_PDF_PROCESS
#endif
