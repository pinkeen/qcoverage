#include "graphnodetext.h"

#include <QGraphicsScene>
#include <QTextCursor>

GraphNodeText::GraphNodeText(const QString &text, QGraphicsItem *parent, QGraphicsScene *scene)
	: QGraphicsTextItem(text, parent, scene)
{
}

void GraphNodeText::focusOutEvent(QFocusEvent *event)
{
	QTextCursor cursor(textCursor());
	cursor.clearSelection();
	setTextCursor(cursor);

	setTextInteractionFlags(Qt::NoTextInteraction);

	emit lostFocus();
	QGraphicsTextItem::focusOutEvent(event);
}

void GraphNodeText::focusInEvent(QFocusEvent *event)
{
	QGraphicsTextItem::focusInEvent(event);
}

void GraphNodeText::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if(textInteractionFlags() == Qt::NoTextInteraction)
		setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);

	QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void GraphNodeText::keyPressEvent(QKeyEvent *event)
{
	QGraphicsTextItem::keyPressEvent(event);
	emit keyPressed();
}


