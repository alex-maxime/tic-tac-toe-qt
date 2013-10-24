/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#include <QtGui/QApplication>
#include <QtDeclarative>
#include <QTimer>

#include "gamemodel.h"
#include "loadhelper.h"
#include "tictacengine.h"



/*!
  \class LoadHelper
  \brief A utility class for loading the main QML file after the splash screen
         has been shown.
*/

LoadHelper::LoadHelper(QDeclarativeView *view, QObject *parent)
    : QObject(parent),
      m_view(view)
{}

void LoadHelper::loadMainQML()
{
    if (m_view) {
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
        m_view->setSource(QUrl::fromLocalFile("symbian/main.qml"));
#else
        m_view->setSource(QUrl("qrc:///main.qml"));
#endif
    }
}



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDeclarativeView view;
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);

    // Register meta-types - required to access enumerations
    char uri[] = "TicTacToeGame";
    qmlRegisterType<TicTacEngine>(uri, 1, 0, "GameEngine");
    qmlRegisterType<GameModel>(uri, 1, 0, "GameModel");

    // Create the game engine and add to declarative engine's context
    TicTacEngine gameEngine;
    GameModel *model = gameEngine.gameData();

    view.rootContext()->setContextProperty("gameModel", model);
    view.rootContext()->setContextProperty("gameEngine", &gameEngine);

    view.rootContext()->setContextProperty("rowCount", ROW_COUNT);
    view.rootContext()->setContextProperty("columnCount", COLUMN_COUNT);

#ifdef Q_OS_SYMBIAN
    view.setAttribute(Qt::WA_NoSystemBackground);
#endif

    view.setSource(QUrl("qrc:///SplashScreen.qml"));
    view.showFullScreen();

    LoadHelper loadHelper(&view);
    QTimer::singleShot(1, &loadHelper, SLOT(loadMainQML()));

    QObject::connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
    return app.exec();
}
