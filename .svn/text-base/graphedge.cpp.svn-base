#include "graphedge.h"

#include <QLineF>
#include <QPainterPath>
#include <QVector>
#include <QVectorIterator>
#include <QSettings>
#include <QGraphicsDropShadowEffect>
#include <QCursor>

#include <QDebug>

#include <math.h>

#include "graphnode.h"
#include "graphscene.h"


GraphEdge::GraphEdge(GraphNode *fromNode, GraphNode *toNode,
		     QGraphicsItem *parent, QGraphicsScene *scene)
	: QGraphicsLineItem(parent, scene), m_fromNode(fromNode), m_toNode(toNode)
{
	initSettings();

	m_highlight = false;

	setColor(Qt::black);
	setPen(QPen(pen().color(), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setZValue(-2.0f);
	m_arrowSize = 15.0f;
	setCursor(QCursor(Qt::UpArrowCursor));

	//setGraphicsEffect(GraphScene::createDropShadowEffect());
}

void GraphEdge::initSettings()
{
	m_selectedColor = m_settings.value("selectedColor").toString();
	m_dropShadowColor = m_settings.value("dropShadowColor").toString();
	m_highlightColor = m_settings.value("highlightColor").toString();
}

void GraphEdge::bringToFront()
{
	setZValue(-1.0f);

	foreach(QGraphicsItem *item, collidingItems(Qt::IntersectsItemBoundingRect))
	{
		if(item->type() == GraphEdge::Type)
			item->setZValue(-2.0f);
	}

	scene()->update();
}

void GraphEdge::highlight()
{
	m_highlight = true;

	QPen tmpPen(pen());
	tmpPen.setColor(m_highlightColor);
	//tmpPen.setStyle(Qt::DashDotDotLine);
	setPen(tmpPen);
	bringToFront();
}

void GraphEdge::clearHighlight()
{
	m_highlight = false;

	QPen tmpPen(pen());
	tmpPen.setColor(Qt::black);
	tmpPen.setStyle(Qt::SolidLine);
	setPen(tmpPen);
}

void GraphEdge::updatePosition()
{
	QPointF centerFrom(m_fromNode->pos() + m_fromNode->boundingRect().center());
	QPointF centerTo(m_toNode->pos() + m_toNode->boundingRect().center());

	QLineF centerLine(centerFrom, centerTo);

	QPointF pFrom;
	QPointF pTo;

	if(m_fromNode != m_toNode)
	{
		pFrom = findPolygonIntersection(m_fromNode->polygon(), m_fromNode->pos(), centerLine);
		pTo = findPolygonIntersection(m_toNode->polygon(), m_toNode->pos(), centerLine);
		setLine(QLineF(pTo, pFrom));
	}
	else
	{
		float margin = pen().width() * 2.0;
		QRectF rect;
		QRectF bbox(m_fromNode->boundingRect());
		QSizeF size(bbox.width() / 2.0f + margin, bbox.height() + margin);
		QPointF pos(m_fromNode->scenePos() + bbox.topLeft());

		rect.setSize(size);
		rect.moveCenter(QPointF(pos.x(), pos.y() + (bbox.height() / 2.0)));

		float angle = asinf(bbox.height() / size.height()) / M_PI * 180.0f;

		m_startAngle = angle * 16;
		m_spanAngle = (360 - 2 * angle) * 16;

		m_arcRect = rect;

		setLine(centerLine);
	}

	//updateSimpleShape();
}

void GraphEdge::setColor(const QColor &color)
{
	QPen tmpPen(pen());
	tmpPen.setColor(color);
	setPen(tmpPen);
}

void GraphEdge::updateSimpleShape()
{
	m_simpleShape.clear();
	QLineF normal(line().normalVector().unitVector());

	qreal size = m_arrowSize;

	QPointF pa = normal.pointAt(-1.0 * size);
	QPointF pb = normal.pointAt(1.0 * size);
	QPointF sz = QPointF(line().dx(), line().dy());

	m_simpleShape << pa << pb << pb + sz << pa + sz;
}

QPainterPath GraphEdge::shape() const
{
	QPainterPath path;

	if(m_fromNode != m_toNode)
	{
		path = QGraphicsLineItem::shape();
		path.addPolygon(m_arrowHead);
		//path.addPolygon(m_simpleShape);
		return path;
	}

	path.addEllipse(m_arcRect);

	return path;
}

QRectF GraphEdge::boundingRect() const
{
	if(m_fromNode != m_toNode)
	{
		qreal a = pen().width() + m_arrowSize;
		return QGraphicsLineItem::boundingRect().adjusted(-a, -a, a, a);
	}

	return m_arcRect;
}

QPointF GraphEdge::findPolygonIntersection(const QPolygonF &polygon, const QPointF &polyPos, const QLineF &line)
{
	QPointF intersectPoint;
	QLineF polyLine;

	polyLine.setP1(polygon.last() + polyPos);

	foreach(QPointF polyPoint, polygon)
	{
		polyLine.setP2(polyPoint + polyPos);

		QLineF::IntersectType intersectType =
				polyLine.intersect(line, &intersectPoint);

		if(intersectType == QLineF::BoundedIntersection)
			break;

		polyLine.setP1(polyLine.p2());
	}

	return intersectPoint;
}

void GraphEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	if(m_fromNode != m_toNode &&
	   m_fromNode->collidesWithItem(m_toNode, Qt::IntersectsItemBoundingRect))
		return;

	painter->setPen(pen());
	painter->setBrush(pen().color());

	if(isSelected())
	{
		painter->setPen(QPen(m_selectedColor, pen().width()));
		painter->setBrush(QBrush(m_selectedColor));
	}

	double angle = ::acos(line().dx() / line().length());

	if (line().dy() >= 0)
		angle = (M_PI * 2) - angle;

	QPointF arrowP1 = line().p1() + QPointF(sin(angle + M_PI / 3) * m_arrowSize,
						cos(angle + M_PI / 3) * m_arrowSize);
	QPointF arrowP2 = line().p1() + QPointF(sin(angle + M_PI - M_PI / 3) * m_arrowSize,
						cos(angle + M_PI - M_PI / 3) * m_arrowSize);

	m_arrowHead.clear();
	m_arrowHead << line().p1() << arrowP1 << arrowP2;

	if(m_fromNode != m_toNode)
	{
		painter->drawLine(line());
		painter->drawPolygon(m_arrowHead);
	}
	else
	{
		painter->setBrush(Qt::NoBrush);
		painter->drawArc(m_arcRect, m_startAngle, m_spanAngle);


	}

	/*painter->setBrush(QBrush());
	painter->setPen(QPen(m_selectedColor, pen().width(), Qt::DashLine));
	painter->drawPolygon(m_simpleShape);*/
}

void GraphEdge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsLineItem::mousePressEvent(event);

	if(isSelected())
		bringToFront();
}
