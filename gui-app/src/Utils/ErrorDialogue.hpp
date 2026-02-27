#ifndef UTILS_ERROR_DIALOGUE
#define UTILS_ERROR_DIALOGUE

#include <qerrormessage.h>
#include <qtmetamacros.h>
namespace sane_in_the_membrane::utils {

    class CErrorDialogue : public QErrorMessage {
        Q_OBJECT

      private:
        CErrorDialogue(QWidget* parent = nullptr);

      public:
        static void show_message(QString& message, QWidget* parent = nullptr);
        static void show_message(QString&& message, QWidget* parent = nullptr);
    };
}

#endif // !UTILS_ERROR_DIALOGUE
