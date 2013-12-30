#include "algorithmpath.h"

#include<QDebug>
#include "graphnode.h"

using namespace Algorithm;

Path::Path()
{
	initSettings();
}

Path::Path(const Path* path)
{
	m_nodes.append(path->m_nodes);
	initSettings();
}

Path::Path(const QList<Node*> &nodes)
{
	m_nodes = nodes;
	initSettings();
}

Path::Path(Node* node)
{
	m_nodes.append(node);
	initSettings();
}

void Path::initSettings()
{
	m_startNodeColor = m_settings.value("startNodeColor").toString();
	m_endNodeColor = m_settings.value("endNodeColor").toString();
	m_startEndNodeColor = m_settings.value("startEndNodeColor").toString();
}

void Path::appendNode(Node *node)
{
	m_nodes.append(node);
}

void Path::appendPath(Path *path)
{
	foreach(Node *node, path->nodes())
	{
		m_nodes.append(node);
	}
}

bool Path::isTestPath()
{
	return (m_nodes.first()->type() == GraphNode::StartEndNode ||
		m_nodes.first()->type() == GraphNode::StartNode) &&
	       (m_nodes.last()->type() == GraphNode::StartEndNode ||
		m_nodes.last()->type() == GraphNode::EndNode);
}


void Path::prependNode(Node *node)
{
	m_nodes.prepend(node);
}

void Path::prependPath(Path *path)
{
	for(int i = path->m_nodes.size() - 1; i >= 0; --i)
	{
		m_nodes.prepend(path->m_nodes.at(i));
	}
}


int Path::nodeCount() const
{
	return m_nodes.size();
}

int Path::edgeCount() const
{
	return m_nodes.size() - 1;
}

void Path::intersectionLeftPos(const Path *path, int &start, int &end, int &len) const
{
	int maxLen = 0;
	int maxStart = -1;
	int maxEnd = -1;

	int tmpStart = m_nodes.indexOf(path->nodes().first());

	while(tmpStart != -1)
	{
		int j = 1;

		int tmpEnd = -1;

		for(int i = tmpStart + 1; i < nodeCount(); ++i)
		{
			if(j == path->nodeCount() || nodes().at(i) != path->nodes().at(j))
			{
				tmpEnd = i - 1;
				break;
			}

			++j;
		}

		if(tmpEnd == -1)
			tmpEnd = nodeCount() - 1;

		int len = tmpEnd - tmpStart + 1;

		/* >= because in case we've got many intersections of the same length
		   we want the last one
		*/
		if(len >= maxLen)
		{
			maxLen = len;
			maxStart = tmpStart;
			maxEnd = tmpEnd;
		}

		tmpStart = m_nodes.indexOf(path->nodes().first(), tmpStart + 1);
	}

	start = maxStart;
	end = maxEnd;
	len = maxLen;
}

void Path::intersectionRightPos(const Path *path, int &start, int &end, int &len) const
{
	path->intersectionLeftPos(this, start, end, len);
}

bool Path::isEndReachable(Node *fromNode, Path *touredPath)
{
	if(touredPath == 0)
		touredPath = new Path();

	if(touredPath->countNode(fromNode))
	{
		delete touredPath;
		return false;
	}

	touredPath->appendNode(fromNode);

	foreach(Node *node, fromNode->links())
	{
		if(node->type() == GraphNode::EndNode ||
		   node->type() == GraphNode::StartEndNode)
		{
			delete touredPath;
			return true;
		}
	}

	foreach(Node *node, fromNode->links())
	{
		if(isEndReachable(node, new Path(touredPath)))
		{
			delete touredPath;
			return true;
		}
	}

	delete touredPath;
	return false;
}

int Path::intersectsLeft(const Path *path) const
{
	int start, end, len;

	intersectionLeftPos(path, start, end, len);

	if(end == (nodeCount() - 1))
		return len;

	return 0;

	return len;
}

int Path::intersectsRight(const Path *path) const
{
	return path->intersectsLeft(this);
}

int Path::intersects(const Path *path) const
{
	int sza = intersectsLeft(path);
	int szb = intersectsRight(path);

	if(sza > szb)
		return sza;

	return szb;
}

Path *Path::mergedWith(const Path *path) const
{
	int startLeft, endLeft, lenLeft;
	int startRight, endRight, lenRight;

	intersectionLeftPos(path, startLeft, endLeft, lenLeft);
	intersectionRightPos(path, startRight, endRight, lenRight);


	bool leftMergePossible = (endLeft == (nodeCount() - 1)) && (lenLeft != 0);
	bool rightMergePossible = (endRight == (path->nodeCount() - 1)) && (lenRight != 0);

	bool mergeLeft = false;
	bool mergeRight = false;

	if(leftMergePossible && !rightMergePossible)
		mergeLeft = true;

	if(rightMergePossible && !leftMergePossible)
		mergeRight = true;

	if(rightMergePossible && leftMergePossible)
	{
		if(lenLeft >= lenRight)
			mergeLeft = true;
		else
			mergeRight = true;
	}

	if(mergeLeft)
	{
		Path *newPath = new Path(this);

		int from = nodeCount() - startLeft;

		for(int i = from; i < path->nodeCount(); ++i)
			newPath->appendNode(path->nodes().at(i));

		return newPath;
	}

	if(mergeRight)
	{

		Path *newPath = new Path(path);

		int from = path->nodeCount() - startRight;

		for(int i = from; i < nodeCount(); ++i)
			newPath->appendNode(nodes().at(i));

		return newPath;
	}

	return 0;
}


bool Path::containsPath(const Path *path) const
{
	int start, end, len;

	intersectionLeftPos(path, start, end, len);

	if(len == path->nodeCount())
		return true;

	return false;
}

bool Path::containsEdge(Node *fromNode, Node *toNode)
{
	int index = m_nodes.indexOf(fromNode);

	if(index == -1)
		return false;

	// we're at end
	if((index + 1) == m_nodes.size())
		return false;

	if(toNode == m_nodes.at(index + 1))
		return true;

	return false;
}

bool Path::isBorderPath()
{
	return lastNode()->type() == GraphNode::StartEndNode ||
	       lastNode()->type() == GraphNode::EndNode ||
	       firstNode()->type() == GraphNode::StartNode ||
	       firstNode()->type() == GraphNode::StartEndNode;
}

bool Path::containsNode(Node *node)
{
	int index = m_nodes.indexOf(node);

	if(index == -1)
		return false;
	return true;
}

bool Path::countNode(Node *node)
{
	return m_nodes.count(node);
}

QString Path::toText() const
{
	QStringList list;

	foreach(Node *node, m_nodes)
	{
		QString pattern = "%1";

		/*if(node->type() == GraphNode::StartNode)
			pattern += "->";
		else if(node->type() == GraphNode::EndNode)
			pattern = "->" + pattern;
		else if(node->type() == GraphNode::StartEndNode)
			pattern = "->" + pattern + "->";*/

		list << pattern.arg(node->label());
	}

	return "[" + list.join(", ") + "]";
}

QString Path::toRichText() const
{
	QStringList list;

	foreach(Node *node, m_nodes)
	{
		QString pattern = "<font color=\"%2\">%1</font>";
		QString color = "#000000";

		if(node->type() == GraphNode::StartNode)
			color = m_startNodeColor;
		else if(node->type() == GraphNode::EndNode)
			color = m_endNodeColor;
		else if(node->type() == GraphNode::StartEndNode)
			color = m_startEndNodeColor;

		list << pattern.arg(node->label(), color);
	}

	return "[" + list.join(", ") + "]";
}
