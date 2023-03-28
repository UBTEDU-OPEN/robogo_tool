#ifndef CONNECTION_GUIDELINES_H
#define CONNECTION_GUIDELINES_H

#include <QDialog>
#include <memory>

namespace Ui {
    class ConnectionGuidelinesDialog;
}

class ConnectionGuidelines : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectionGuidelines(QWidget *parent = nullptr);
    ~ConnectionGuidelines();

signals:

public slots:

private:
    std::unique_ptr<Ui::ConnectionGuidelinesDialog> m_ui;
};

#endif // CONNECTION_GUIDELINES_H
