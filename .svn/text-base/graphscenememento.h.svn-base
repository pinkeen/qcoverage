#ifndef GRAPHSCENEMEMENTO_H
#define GRAPHSCENEMEMENTO_H

#include <QList>
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QPointF>

#include "graphedge.h"
#include "graphnode.h"

namespace GraphSceneMementoTypes
{
	typedef struct
	{
		int fromNodeIndex;
		int toNodeIndex;
	} StoredEdge;

	typedef struct
	{
		GraphNode::NodeType type;
		QString label;
		QPointF pos;
	} StoredNode;
}

QDataStream &operator<<(QDataStream& stream, const GraphSceneMementoTypes::StoredEdge& edge);
QDataStream &operator>>(QDataStream& stream, GraphSceneMementoTypes::StoredEdge &edge);

QDataStream &operator<<(QDataStream& stream, const GraphSceneMementoTypes::StoredNode& node);
QDataStream &operator>>(QDataStream& stream, GraphSceneMementoTypes::StoredNode &node);

class GraphSceneMemento
{
	public:
		GraphSceneMemento();

		bool isEmpty() const;

		void write(QDataStream &stream);
		void read(QDataStream &stream);

		void storeItems(const QList<GraphEdge*> &edges, const QList<GraphNode*> &nodes);
		void createItems(QList<GraphEdge*> &edges, QList<GraphNode*> &nodes);

		void clear();

	private:
		QList<GraphSceneMementoTypes::StoredEdge> m_storedEdges;
		QList<GraphSceneMementoTypes::StoredNode> m_storedNodes;

};

#endif // GRAPHSCENEMEMENTO_H
