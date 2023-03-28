#ifndef INSTALLATION_GUIDE_H
#define INSTALLATION_GUIDE_H

#include <QDialog>
#include <memory>

namespace Ui {
class InstallationGuideUi;
}

class InstallationGuide : public QDialog
{
    Q_OBJECT
public:
    explicit InstallationGuide(QWidget *parent = nullptr);
    ~InstallationGuide();

private:
    std::unique_ptr<Ui::InstallationGuideUi> m_ui;
};

#endif // INSTALLATION_GUIDE_H
