#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QListWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QTreeWidget>

class ListWidget : public QListWidget
{
	Q_OBJECT
public:
	ListWidget(QWidget *parent = 0) : QListWidget(parent) {};

signals:
	void dropEventActivated();

protected:
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);
};

#endif // LISTWIDGET_H
