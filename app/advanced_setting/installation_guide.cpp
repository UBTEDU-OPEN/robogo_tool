#include "installation_guide.h"
#include "ui_installation_guide.h"
#include <QLoggingCategory>
#include <assert.h>

Q_LOGGING_CATEGORY(installation_guide, "installation_guide")

InstallationGuide::InstallationGuide(QWidget* parent)
    : QDialog(parent),
      m_ui(new Ui::InstallationGuideUi)
{
    qCDebug(installation_guide()) << __func__;
    m_ui->setupUi(this);

    setWindowModality(Qt::ApplicationModal);
    Qt::WindowFlags flags = Qt::FramelessWindowHint;
    this->setWindowFlags(flags);

    connect(m_ui->pb_close, &QPushButton::released,
            this, [this](){
        this->close();
     }
    );

    if(parent)
    {
        move(parent->width()/2 - this->width()/2, parent->height()/2 - this->height()/2);
    }
}

InstallationGuide::~InstallationGuide()
{
    qCDebug(installation_guide()) << __func__;
}
