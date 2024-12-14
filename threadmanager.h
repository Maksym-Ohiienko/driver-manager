#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QThread>
#include <QVector>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QProcess>

// Клас-сінглтон для управління потоками
class ThreadManager : public QObject {

    Q_OBJECT

public:

    static ThreadManager& instance();

    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;

    void registerThread(QThread* thread);
    Q_INVOKABLE void stopAllThreads();
    Q_INVOKABLE void rebootPC();

private:
    QVector<QThread*> m_threads;

    explicit ThreadManager(QObject* parent = nullptr);
};


#endif // THREADMANAGER_H
