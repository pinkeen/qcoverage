#ifndef GRAPHEDGE_H
#define GRAPHEDGE_H

#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QPolygonF>
#include <QColor>
#include <QPainterPath>
#include <QObject>
#include <QPainter>
#include <QWidget>
#include <QStyleOptionGraphicsItem>
#include <QSettings>
#include <QGraphicsSceneMouseEvent>

class GraphNode;

class GraphEdge : public QGraphicsLineItem
{
	public:
		GraphEdge(GraphNode *from, GraphNode *to,
			  QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

		void setColor(const QColor &color);

		GraphNode *fromNode() const { return m_fromNode; }
		GraphNode *toNode() const { return m_toNode; }
		QRectF boundingRect() const;
		QPainterPath shape() const;

	public:
		void updatePosition();
		void highlight();
		void clearHighlight();
		void bringToFront();

	private:
		GraphNode *m_fromNode;
		GraphNode *m_toNode;

		QPolygonF m_simpleShape;
		QPolygonF m_arrowHead;

		// these are for a recursive edge
		QRectF m_arcRect;
		int m_startAngle;
		int m_spanAngle;

		QSettings m_settings;
		QColor m_selectedColor;
		QColor m_dropShadowColor;
		QColor m_highlightColor;

		float m_arrowSize;

		bool m_highlight;

		void initSettings();
		void updateSimpleShape();
		static QPointF findPolygonIntersection(const QPolygonF &polygon, const QPointF &polyPos, const QLineF &line);

	protected:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // GRAPHEDGE_H
