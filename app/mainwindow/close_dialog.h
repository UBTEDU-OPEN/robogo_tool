#ifndef CONNECTION_GUIDELINES_H
#define CONNECTION_GUIDELINES_H

#include <QDialog>
#include <memory>

namespace Ui {
    class CloseDialogUi;
}

class CloseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CloseDialog(QWidget *parent = nullptr);
    ~CloseDialog();

    void setMessage(const QString &message);
    void setTitileText(const QString &text);
    void setOkButtonText(const QString &text);
    void setCancelButtonText(const QString &text);

protected:
    void paintEvent(QPaintEvent *);
private:
    std::unique_ptr<Ui::CloseDialogUi> m_ui;
};

#endif // CONNECTION_GUIDELINES_H
