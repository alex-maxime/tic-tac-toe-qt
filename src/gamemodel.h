/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QAbstractListModel>


const int ROW_COUNT(3);
const int COLUMN_COUNT(3);
const int GRID_COUNT(ROW_COUNT * ROW_COUNT);


class GameModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(TicTacType)

public:
    enum GameRoles {
        MarkRole = Qt::UserRole + 1,
        HighlightRole
    };

    enum TicTacType {
        None = 0,
        Zero,
        Cross
    };

    explicit GameModel(QObject *parent = 0);

public: // From QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = MarkRole) const;

public slots:
    bool setData(int row, QVariant newValue, int role = MarkRole);
    void resetData();

private: // Data
    int mModelData[GRID_COUNT];
    bool mHighlightData[GRID_COUNT];
};

#endif // GAMEMODEL_H
