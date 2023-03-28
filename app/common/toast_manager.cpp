#include "toast_manager.h"
#include "toast.h"

ToastManager* ToastManager::m_instance = nullptr;

ToastManager::ToastManager(QObject *parent) : QObject(parent)
{

}

ToastManager::~ToastManager()
{

}

ToastManager &ToastManager::instance()
{
    if (!m_instance)
    {
        m_instance = new ToastManager();
    }

    return *m_instance;
}

void ToastManager::setRootWidget(std::shared_ptr<QWidget> view)
{
    m_view = view;
}

void ToastManager::show(const QString &message)
{
    if (m_toast && m_toast->isVisible())
    {
        m_toast->close();
    }

    if (auto view = m_view.lock())
    {
        m_toast = std::make_shared<Toast>(message, view.get());
        m_toast->show();
    }
}

void ToastManager::release()
{
    if (m_toast && m_toast->isVisible())
    {
        m_toast->close();
    }
    m_toast = nullptr;
}
