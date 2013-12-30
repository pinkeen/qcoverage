#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QRectF>
#include <QString>
#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QMenu>
#include <QColor>
#include <QList>
#include <QSettings>

#include "graphnodetext.h"
#include "graphedge.h"


class GraphNode : public QObject, public QGraphicsPolygonItem
{
	Q_OBJECT

	public:
		enum NodeType { NormalNode, StartNode, EndNode, StartEndNode };

	public:
		GraphNode(const QString &text, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

		void setColor(const QColor &fillColor, const QColor &borderColor = Qt::black, bool dashed = false);
		void setBoundingRect(const QRectF &rect);
		void setNodeType(NodeType type);

		void addEdge(GraphEdge *edge);
		void removeEdge(GraphEdge *edge);
		void removeAllEdges();

		NodeType nodeType() { return m_nodeType; }
		const QString label() const;

		void updateEdgesPositions();

		void highlight();
		void clearHighlight();
		bool hasEdge(GraphNode *toNode);


	private slots:
		void nodeTextKeyPressed();
		void nodeTextLostFocus();

	signals:
		void lostFocus(GraphNode *node);
		void dragFinish();

	private:
		NodeType m_nodeType;
		GraphNodeText *m_nodeText;
		QPolygonF m_polygon;
		QList<GraphEdge*> m_edges;

		QSettings m_settings;

		QColor m_startNodeColor;
		QColor m_endNodeColor;
		QColor m_startEndNodeColor;
		QColor m_normalNodeColor;
		QColor m_selectedColor;
		QColor m_highlightColor;

		QPointF m_startDragPosition;

		float m_padding;
		float m_rounding;

		bool m_highlight;

		void initSettings();

	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
		void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
		void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
		QVariant itemChange(GraphicsItemChange change, const QVariant &value);



};

#endif // GRAPHNODE_H
