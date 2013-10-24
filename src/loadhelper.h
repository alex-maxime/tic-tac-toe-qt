#ifndef LOADHELPER_H
#define LOADHELPER_H

#include <QObject>

// Forward declarations
class QDeclarativeView;


class LoadHelper : public QObject
{
    Q_OBJECT

public:
    explicit LoadHelper(QDeclarativeView *view, QObject *parent = 0);

public slots:
    void loadMainQML();

private: // Data
    QDeclarativeView *m_view;
};


#endif // LOADHELPER_H
