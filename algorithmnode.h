#ifndef ALGORITHMNODE_H
#define ALGORITHMNODE_H

#include <QList>
#include <QString>

#include "graphnode.h"
#include "graphedge.h"

namespace Algorithm
{
	class Path;

	class Node
	{
		private:
			QList<Node*> m_links;
			QList<Node*> m_backLinks;
			QList<GraphEdge*> m_graphEdges;
			GraphNode *m_graphNode;

		public:
			Node(GraphNode *graphNode);

			void addBackLink(Node *node);
			void addLink(Node *node, GraphEdge *edge);
			bool hasLink(Node *node);

			const QList<Node*> &links() const;
			QString label() const;

			GraphNode::NodeType type() const;
			int linkCount() const;

			GraphEdge *getGraphEdgeToNode(Node *node);
			Node *getEdgeToNode(Node *node);

			GraphNode *graphNode() const;

			int distanceToNearestStartNode(Path *path = 0);
			int distanceToNearestEndNode(Path *path = 0);
	};
}

#endif // ALGORITHMNODE_H
