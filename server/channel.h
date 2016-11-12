#ifndef CHANNEL_H
#define CHANNEL_H

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

class Channel
{
public:
    Channel(const QString &name, QTabWidget *tabParent);

private:
    QString name;
    QWidget *tab;

    QWidget* initializeTab(QTabWidget *tabParent);
};

#endif // CHANNEL_H
