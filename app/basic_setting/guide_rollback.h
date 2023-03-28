#ifndef CONNECTION_GUIDELINES_H
#define CONNECTION_GUIDELINES_H

#include <QDialog>
#include <memory>

namespace Ui {
    class RollbackGuideDialog;
}

class RollbackGuidelines : public QDialog
{
    Q_OBJECT
public:
    explicit RollbackGuidelines(QWidget *parent = nullptr);
    ~RollbackGuidelines();

private:
    void paintEvent(QPaintEvent *);

private:
    std::unique_ptr<Ui::RollbackGuideDialog> m_ui;
};

#endif // CONNECTION_GUIDELINES_H
