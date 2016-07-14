#include "listwidget.h"
#include <QDebug>

void ListWidget::dragMoveEvent(QDragMoveEvent *event)
{
	if(event->source() != this) {
		event->accept();
	}
	else
	{
		event->ignore();
	}
	 //event->acceptProposedAction();
}

void ListWidget::dropEvent(QDropEvent *event)
{
	if(event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
	{
		QTreeWidget *tree = dynamic_cast<QTreeWidget *>(event->source());

		QByteArray itemData = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
		QDataStream stream(&itemData, QIODevice::ReadOnly);
		int r, c;
		QMap<int, QVariant> v;
		stream >> r >> c >> v;
		QString text = v[0].toString();
		//qDebug()<<text;
		for(int i = 0;i < this->count();i++)
		{
			if(this->item(i)->text().compare(text) == 0)
				return;
		}
	}
	 QListWidget::dropEvent(event);
}

