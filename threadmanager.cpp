#include "threadmanager.h"

ThreadManager :: ThreadManager(QObject *parent)
    : QObject{parent}
{

}

void ThreadManager :: registerThread(QThread* thread) {

    m_threads.append(thread);

    connect(thread, &QThread::finished, this, [this, thread]() {

        m_threads.removeAll(thread);
        thread->deleteLater();
    });
}

void ThreadManager :: stopAllThreads() {

    for (QThread* thread : m_threads) {

        if (thread->isRunning()) {
            thread->quit();
            thread->wait();
        }
    }
    m_threads.clear();
    qDebug() << "All threads stopped!";
}

void ThreadManager :: rebootPC() {

    QProcess* rebootProcess = new QProcess(this);
    QString rebootCommand = QString("shutdown /r /t 0");

    qDebug() << "rebootCommand" << rebootCommand;

    rebootProcess->start("cmd.exe", QStringList() << "/C" << rebootCommand);
}

ThreadManager& ThreadManager :: instance() {

    static ThreadManager instance;
    return instance;
}


