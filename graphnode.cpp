#include "graphnode.h"

#include <QPainterPath>
#include <QPen>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QTextCursor>

#include <QDebug>

#include "graphscene.h"


GraphNode::GraphNode(const QString &text, QGraphicsItem *parent, QGraphicsScene *scene)
	: QGraphicsPolygonItem(parent, scene)
{
	initSettings();

	m_highlight = false;

	setNodeType(NormalNode);

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	//setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges);

	QPen pen;
	pen.setWidth(4);
	setPen(pen);

	m_padding = 5.0f;
	m_rounding = 15.0f;

	m_nodeText = new GraphNodeText(text, this, scene);
	setFiltersChildEvents(true);
	setBoundingRect(m_nodeText->boundingRect());

	connect(m_nodeText, SIGNAL(keyPressed()), this, SLOT(nodeTextKeyPressed()));
	connect(m_nodeText, SIGNAL(lostFocus()), this, SLOT(nodeTextLostFocus()));

	//setGraphicsEffect(GraphScene::createDropShadowEffect());

	setFocusProxy(m_nodeText);
}

bool GraphNode::hasEdge(GraphNode *toNode)
{
	foreach(GraphEdge *edge, m_edges)
	{
		if(edge->fromNode() == this && edge->toNode() == toNode)
			return true;
	}

	return false;
}

void GraphNode::highlight()
{
	m_highlight = true;

	QPen tmpPen(pen());
	tmpPen.setColor(m_highlightColor);
	//tmpPen.setStyle(Qt::DashDotDotLine);
	setPen(tmpPen);
}

void GraphNode::clearHighlight()
{
	m_highlight = false;
	setNodeType(m_nodeType);
}


void GraphNode::initSettings()
{
	m_normalNodeColor = m_settings.value("normalNodeColor").toString();

	m_startNodeColor = m_settings.value("startNodeColor").toString();
	m_endNodeColor = m_settings.value("endNodeColor").toString();
	m_startEndNodeColor = m_settings.value("startEndNodeColor").toString();

	m_selectedColor = m_settings.value("selectedColor").toString();
	m_highlightColor = m_settings.value("highlightColor").toString();
}


void GraphNode::setColor(const QColor &fillColor, const QColor &borderColor, bool dashed)
{
	QColor tmpColor(fillColor);
	tmpColor.setAlpha(200);
	setBrush(QBrush(tmpColor, Qt::SolidPattern));

	QPen tmpPen(pen());
	tmpPen.setColor(borderColor);

	if(dashed)
		tmpPen.setStyle(Qt::DotLine);
	else
		tmpPen.setStyle(Qt::SolidLine);

	setPen(tmpPen);
}

const QString GraphNode::label() const
{
	return m_nodeText->toPlainText();
}


void GraphNode::setNodeType(enum NodeType type)
{
	if(m_highlight)
		return;

	m_nodeType = type;

	switch(m_nodeType)
	{
		case NormalNode: setColor(m_normalNodeColor); break;
		case StartNode: setColor(m_startNodeColor, m_startNodeColor); break;
		case EndNode: setColor(m_endNodeColor, m_endNodeColor); break;
		case StartEndNode: setColor(m_startEndNodeColor, m_startEndNodeColor); break;
	}
}

void GraphNode::setBoundingRect(const QRectF &rect)
{
	QPainterPath path;
	path.addRoundedRect(rect.adjusted(-m_padding, -m_padding, m_padding, m_padding),
			    m_rounding, m_rounding, Qt::AbsoluteSize);

	m_polygon = path.toFillPolygon(QTransform());
	setPolygon(m_polygon);

	updateEdgesPositions();
}

void GraphNode::nodeTextKeyPressed()
{
	setBoundingRect(m_nodeText->boundingRect());
}

void GraphNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	m_nodeText->mouseDoubleClickEvent(event);
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}

void GraphNode::addEdge(GraphEdge *edge)
{
	m_edges.append(edge);
}

void GraphNode::removeEdge(GraphEdge *edge)
{
	int index = m_edges.indexOf(edge);

	if(index != -1)
		m_edges.removeAt(index);
}

void GraphNode::removeAllEdges()
{
	foreach(GraphEdge *edge, m_edges)
	{
		edge->fromNode()->removeEdge(edge);
		edge->toNode()->removeEdge(edge);
		scene()->removeItem(edge);
	}
}

void GraphNode::updateEdgesPositions()
{
	foreach(GraphEdge *edge, m_edges)
		edge->updatePosition();
}

void GraphNode::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	painter->setPen(pen());
	painter->setBrush(brush());

	if(isSelected())
		painter->setPen(QPen(m_selectedColor, pen().width()));

	painter->drawPolygon(polygon());
}

void GraphNode::nodeTextLostFocus()
{
	emit lostFocus(this);
}

QVariant GraphNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if(change == QGraphicsItem::ItemPositionHasChanged)
		updateEdgesPositions();

	return value;
}

void GraphNode::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if(mouseEvent->button() == Qt::LeftButton)
			m_startDragPosition = mouseEvent->scenePos();

	QGraphicsPolygonItem::mousePressEvent(mouseEvent);
}

void GraphNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if(mouseEvent->button() == Qt::LeftButton &&
	   mouseEvent->scenePos() != m_startDragPosition &&
	   isSelected() && contains(mouseEvent->pos()))
		emit dragFinish();

	QGraphicsPolygonItem::mouseReleaseEvent(mouseEvent);
}
