#include "simplepathsalgorithm.h"

#include <QDebug>

using namespace Algorithm;

void SimplePathsAlgorithm::continuePath(Path *path)
{
	addReqResult(path);

	if(path->nodes().first() == path->nodes().last() && path->nodeCount() > 1)
		return;

	Node *last = path->nodes().last();

	foreach(Node *node, last->links())
	{
		if(!path->containsNode(node) || path->nodes().first() == node)
		{
			Path *newPath = new Path(path);
			newPath->appendNode(node);
			continuePath(newPath);
		}
	}
}

void SimplePathsAlgorithm::onCompute()
{
	foreach(Node *node, nodes())
	{
		continuePath(new Path(node));
	}
}
