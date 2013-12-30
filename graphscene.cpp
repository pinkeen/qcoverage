#include "graphscene.h"

#include <QPoint>
#include <QDebug>
#include <QMutableListIterator>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMessageBox>

GraphScene::GraphScene(QMenu *nodeMenu, QObject *parent)
	: QGraphicsScene(parent), m_mode(Manipulate), m_nodeMenu(nodeMenu)
{
	initSettings();
	m_currentID = 0;
	m_line = 0;

	QColor tmpColor(m_insertingLineColor);
	tmpColor.setAlpha(220);
	m_insertingLinePen = QPen(QBrush(tmpColor), 4, Qt::SolidLine, Qt::RoundCap);
}

void GraphScene::initSettings()
{
	m_insertingLineColor = m_settings.value("insertingLineColor").toString();
}

void GraphScene::setMode(Mode mode)
{
	m_mode = mode;
}

void GraphScene::editorLostFocus(GraphNode *node)
{
	if(node->label().isEmpty())
	{
		removeItem(node);
		node->deleteLater();
	}
}

void GraphScene::addNode(GraphNode *node)
{
	connect(node, SIGNAL(lostFocus(GraphNode*)), this, SLOT(editorLostFocus(GraphNode*)));
	connect(node, SIGNAL(dragFinish()), this, SLOT(nodeDragged()));

	addItem(node);
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if(mouseEvent->button() == Qt::LeftButton)
	{
		QPointF pos(mouseEvent->scenePos());
		GraphNode *node;

		switch(m_mode)
		{
			case InsertNode:

				m_currentID++;

				node = new GraphNode(QString::number(m_currentID));
				pos -= node->boundingRect().center();
				node->setPos(pos);

				addNode(node);

				emit changed();
			break;

			case InsertEdge:

				m_line = new QGraphicsLineItem(QLineF(pos, pos));
				m_line->setPen(m_insertingLinePen);

				addItem(m_line);

				emit changed();
			break;

			default: ;

		}
	}

	QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if(m_mode == InsertEdge && m_line != 0)
	{
		m_line->setLine(QLineF(m_line->line().p1(), mouseEvent->scenePos()));
	}
	else if(m_mode == Manipulate)
	{
		QGraphicsScene::mouseMoveEvent(mouseEvent);
	}
}

void GraphScene::filterNonNodeItems(QList<QGraphicsItem*> &itemList)
{
	QMutableListIterator<QGraphicsItem*> itemsIterator(itemList);

	while(itemsIterator.hasNext())
	{
		QGraphicsItem *item = itemsIterator.next();

		if(item->type() != GraphNode::Type)
			itemsIterator.remove();
	}
}

void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if(m_line != 0 && m_mode == InsertEdge)
	{
		QList<QGraphicsItem*> startItems = items(m_line->line().p1());
		QList<QGraphicsItem*> endItems = items(m_line->line().p2());

		filterNonNodeItems(startItems);
		filterNonNodeItems(endItems);

		if(startItems.count() && endItems.count())
			if(startItems.first()->type() == GraphNode::Type &&
			   endItems.first()->type() == GraphNode::Type)
			{

				GraphNode *startNode = qgraphicsitem_cast<GraphNode*>(startItems.first());
				GraphNode *endNode = qgraphicsitem_cast<GraphNode*>(endItems.first());

				if(startNode->hasEdge(endNode))
				{
					QMessageBox::warning(0, tr("Warning"), tr("Such edge already exists!"));
				}
				else
				{
					GraphEdge *edge = new GraphEdge(startNode, endNode);

					startNode->addEdge(edge);
					endNode->addEdge(edge);

					addItem(edge);

					edge->updatePosition();

					emit changed();
				}
			}

		removeItem(m_line);
		delete m_line;

	}

	m_line = 0;

	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void GraphScene::storeToMemento(GraphSceneMemento &memento)
{
	QList<GraphNode*> nodes;
	QList<GraphEdge*> edges;

	foreach(QGraphicsItem *item, items())
	{
		if(item->type() == GraphNode::Type)
			nodes.append(qgraphicsitem_cast<GraphNode*>(item));
		else if(item->type() == GraphEdge::Type)
			edges.append(qgraphicsitem_cast<GraphEdge*>(item));
	}

	memento.storeItems(edges, nodes);
}

void GraphScene::restoreFromMemento(GraphSceneMemento &memento)
{
	clear();

	QList<GraphNode*> nodes;
	QList<GraphEdge*> edges;

	memento.createItems(edges, nodes);

	foreach(GraphNode *node, nodes)
	{
		addNode(node);
	}

	foreach(GraphEdge *edge, edges)
	{
		addItem(edge);
		edge->updatePosition();
	}

	update();
}

void GraphScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent)
{
	QGraphicsItem *item = itemAt(contextMenuEvent->scenePos());
	
	if(item != 0 && m_mode == Manipulate)
	{

		if(item->type() == GraphNode::Type || item->type() == GraphNodeText::Type)
		{
			clearSelection();

			if(item->type() == GraphNode::Type)
				item->setSelected(true);
			else if(item->type() == GraphNodeText::Type)
				item->parentItem()->setSelected(true);

			m_nodeMenu->exec(contextMenuEvent->screenPos());
		}

		return;
	}

	QGraphicsScene::contextMenuEvent(contextMenuEvent);
}

void GraphScene::updateEdgesPositions()
{
	foreach(QGraphicsItem *item, items())
	{
		if(item->type() == GraphEdge::Type)
		{
			GraphEdge *edge = qgraphicsitem_cast<GraphEdge*>(item);
			edge->updatePosition();
		}
	}
}

void GraphScene::nodeDragged()
{
	emit changed();
}

QGraphicsDropShadowEffect *GraphScene::createDropShadowEffect()
{
	QGraphicsDropShadowEffect *dropShadowEffect = new QGraphicsDropShadowEffect();
	QColor dropShadowColor(0, 0, 0, 100);
	dropShadowEffect->setColor(dropShadowColor);
	dropShadowEffect->setBlurRadius(6.0);
	dropShadowEffect->setOffset(2.0);

	return dropShadowEffect;
}

