#include "graphscenememento.h"

using namespace GraphSceneMementoTypes;

QDataStream &operator<<(QDataStream& stream, const StoredEdge& edge)
{
	stream << edge.fromNodeIndex << edge.toNodeIndex;
	return stream;
}

QDataStream &operator>>(QDataStream& stream, StoredEdge &edge)
{
	stream >> edge.fromNodeIndex >> edge.toNodeIndex;
	return stream;
}

QDataStream &operator<<(QDataStream& stream, const StoredNode& node)
{
	int nodeType = node.type;
	stream << node.label << nodeType << node.pos;
	return stream;
}

QDataStream &operator>>(QDataStream& stream, StoredNode &node)
{
	int nodeType;
	stream >> node.label >> nodeType >> node.pos;
	node.type = (GraphNode::NodeType)nodeType;

	return stream;
}

GraphSceneMemento::GraphSceneMemento()
{
}

bool GraphSceneMemento::isEmpty() const
{
	return m_storedNodes.isEmpty();
}

void GraphSceneMemento::clear()
{
	m_storedNodes.clear();
	m_storedEdges.clear();
}

void GraphSceneMemento::write(QDataStream &stream)
{
	stream << m_storedNodes;
	stream << m_storedEdges;
}

void GraphSceneMemento::read(QDataStream &stream)
{
	clear();
	stream >> m_storedNodes;
	stream >> m_storedEdges;
}

void GraphSceneMemento::storeItems(const QList<GraphEdge*> &edges, const QList<GraphNode*> &nodes)
{
	clear();

	foreach(GraphNode* node, nodes)
	{
		StoredNode storedNode;
		storedNode.label = node->label();
		storedNode.type = node->nodeType();
		storedNode.pos = node->pos();

		m_storedNodes.append(storedNode);
	}

	foreach(GraphEdge* edge, edges)
	{
		StoredEdge storedEdge;
		storedEdge.fromNodeIndex = nodes.indexOf(edge->fromNode());
		storedEdge.toNodeIndex = nodes.indexOf(edge->toNode());

		m_storedEdges.append(storedEdge);

#ifdef DEBUG
		if(storedEdge.fromNodeIndex == -1 || storedEdge.toNodeIndex)
			qWarning() << "storeItems: Data is inconsistent";
#endif

	}
}

void GraphSceneMemento::createItems(QList<GraphEdge*> &edges, QList<GraphNode*> &nodes)
{
	edges.clear();
	nodes.clear();

	foreach(StoredNode storedNode, m_storedNodes)
	{
		GraphNode *node = new GraphNode(storedNode.label);

		node->setNodeType(storedNode.type);
		node->setPos(storedNode.pos);

		nodes.append(node);
	}

	foreach(StoredEdge storedEdge, m_storedEdges)
	{
		GraphNode *fromNode = nodes.at(storedEdge.fromNodeIndex);
		GraphNode *toNode = nodes.at(storedEdge.toNodeIndex);
		GraphEdge *edge = new GraphEdge(fromNode, toNode);

		fromNode->addEdge(edge);
		toNode->addEdge(edge);

		edges.append(edge);
	}
}

