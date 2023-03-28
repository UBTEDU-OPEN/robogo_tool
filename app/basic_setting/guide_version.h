#ifndef GUIDE_VERSION_GUIDE_H
#define GUIDE_VERSION_GUIDE_H

#include <QDialog>

namespace Ui {
class guide_ucar_version;
}

class GuideUcarVersion : public QDialog
{
    Q_OBJECT

public:
    explicit GuideUcarVersion(QString strVersion, QWidget *parent = 0);
    ~GuideUcarVersion();

void paintEvent(QPaintEvent *);

private slots:
    void on_pBOk_clicked();


    void on_pb_close_clicked();

private:
    Ui::guide_ucar_version *ui;
};

#endif // GUIDE_VERSION_GUIDE_H
