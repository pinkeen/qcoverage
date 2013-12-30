#ifndef GRAPHNODETEXT_H
#define GRAPHNODETEXT_H

#include <QGraphicsTextItem>
#include <QGraphicsItem>
#include <QString>
#include <QFocusEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMenu>

class GraphNodeText : public QGraphicsTextItem
{
	Q_OBJECT

	public:
		GraphNodeText(const QString &text, QGraphicsItem *parent, QGraphicsScene *scene);
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

	signals:
		void keyPressed();
		void lostFocus();

	private:
		void keyPressEvent(QKeyEvent *event);

	protected:
		void focusOutEvent(QFocusEvent *event);
		void focusInEvent(QFocusEvent *event);


};

#endif // GRAPHNODETEXT_H
