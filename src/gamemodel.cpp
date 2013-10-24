/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#include "gamemodel.h"
#include <QHash>


/*!
  \class GameModel
  \brief Model representing the state of the game.
*/


/*!
  Constructor.
*/
GameModel::GameModel(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[MarkRole] = "mark";
    roles[HighlightRole] = "highlight";
    setRoleNames(roles);

    beginInsertRows(QModelIndex(), 0, GRID_COUNT - 1);
    insertRows(0, GRID_COUNT);
    endInsertRows();

    resetData();
}


/*!
  Resets the data model data
*/
void GameModel::resetData()
{
    for (int i = 0; i < GRID_COUNT; ++i) {
        setData(i, None);
        setData(i, false, HighlightRole);
    }
}


/*!
  Returns the number of items in the model
*/
int GameModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return GRID_COUNT;
}


/*!
  Returns the model data value corresponding to \a index and \a role.
*/
QVariant GameModel::data(const QModelIndex &index, int role) const
{
    int value = None;
    bool highlight = false;

    if (index.row() < GRID_COUNT) {
        if (role == MarkRole) {
            value = mModelData[index.row()];
        }
        else {
            highlight = mHighlightData[index.row()];
        }
    }

    return (role == MarkRole) ? value : highlight;
}


/*!
  Update the data corresponding to \a index, with the \a value for the
  specified \a role. Returns true, if data was modified.
 */
bool GameModel::setData(int row, QVariant newValue, int role)
{
    if (role == MarkRole) {
        bool status = false;
        int theValue = newValue.toInt(&status);

        if (status) {
            mModelData[row] = theValue;
            emit dataChanged(index(row, 0), index(row, 0));
            return true;
        }
    }
    else if (role == HighlightRole) {
        bool theValue = newValue.toBool();
        mHighlightData[row] = theValue;
        emit dataChanged(index(row, 0), index(row, 0));
        return true;
    }

    return false;
}
