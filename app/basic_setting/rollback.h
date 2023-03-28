#ifndef ROLLBACK_H
#define ROLLBACK_H

#include <QWidget>

namespace Ui {
class rollback;
}

class rollback : public QWidget
{
    Q_OBJECT

public:
    explicit rollback(QWidget *parent = 0);
    ~rollback();
signals:
    void backToPreviousView();
private slots:
    void onGuideButtonClicked();
    void onBackButtonClicked();
    void onLeftClicked();
    void onRightClicked();
    void onReduceClicked();
    void onAddClicked();
    void onDriveTestClicked();
    void onCancelAdjust();
    void onDriveFoward();
    void onDriveStop();
    void onComebackCenter();
public slots:
    void onUcarDisconnet();
private:
    bool setDefaultOffset(int nDir);
    int  getDefaultOffset(int nDir);
    bool setRollbackStype(int nType);
    void showdisconnectDlg();
    void DriveTestFail();
    bool onCheckUcarVersion();

private:
    Ui::rollback *ui;
    QWidget* m_view;
    int m_nCurType;
    int m_nLeftDefault; 
    int m_nRightDefault;
    bool m_bIslineTest;//是否进行直线测试
};

#endif // ROLLBACK_H
