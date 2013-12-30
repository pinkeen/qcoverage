#include "algorithmnode.h"

#include "algorithmpath.h"

using namespace Algorithm;

Node::Node(GraphNode *graphNode)
	: m_graphNode(graphNode)
{

}

void Node::addLink(Node *node, GraphEdge *edge)
{
	m_links.append(node);
	m_graphEdges.append(edge);
}

const QList<Node*> &Node::links() const
{
	return m_links;
}

QString Node::label() const
{
	return m_graphNode->label();
}

GraphNode::NodeType Node::type() const
{
	return m_graphNode->nodeType();
}

int Node::linkCount() const
{
	return m_links.size();
}

GraphNode *Node::graphNode() const
{
	return m_graphNode;
}

Node *Node::getEdgeToNode(Node *node)
{
	int i = m_links.indexOf(node);

	if(i == -1)
		return 0;

	return m_links.at(i);
}

GraphEdge *Node::getGraphEdgeToNode(Node *node)
{
	int i = m_links.indexOf(node);

	if(i == -1)
		return 0;

	return m_graphEdges.at(i);
}

void Node::addBackLink(Node *node)
{
	m_backLinks.append(node);
}

int Node::distanceToNearestEndNode(Path *path)
{
	if(path == 0)
		path = new Path();

	if(type() == GraphNode::EndNode || type() == GraphNode::StartEndNode)
	{
		int dist = path->edgeCount() + 1;
		delete path;
		return dist;
	}

	if(path->containsNode(this))
	{
		delete path;
		return -1;
	}

	path->appendNode(this);

	int minDist = -1;

	foreach(Node *node, m_links)
	{
		int dist = node->distanceToNearestEndNode(new Path(path));
		if(dist > minDist)
			minDist = dist;
	}

	delete path;

	return minDist;
}

int Node::distanceToNearestStartNode(Path *path)
{
	if(path == 0)
		path = new Path();

	if(type() == GraphNode::StartNode || type() == GraphNode::StartEndNode)
	{
		int dist = path->edgeCount() + 1;
		delete path;
		return dist;
	}

	if(path->containsNode(this))
	{
		delete path;
		return -1;
	}

	path->appendNode(this);

	int minDist = -1;

	foreach(Node *node, m_backLinks)
	{
		int dist = node->distanceToNearestStartNode(new Path(path));

		if(dist > minDist)
			minDist = dist;
	}

	delete path;

	return minDist;
}

bool Node::hasLink(Node *node)
{
	foreach(Node *linkNode, m_links)
	{
		if(linkNode == node)
			return true;
	}

	return false;
}

