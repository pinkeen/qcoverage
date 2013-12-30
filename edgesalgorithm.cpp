#include "edgesalgorithm.h"

using namespace Algorithm;

void EdgesAlgorithm::onCompute()
{
	foreach(Node *node, nodes())
	{
		foreach(Node *nodeTo, node->links())
		{
			Path *path = new Path(node);
			path->appendNode(nodeTo);
			addReqResult(path);
		}
	}
}
