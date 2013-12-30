#include "edgepairalgorithm.h"

using namespace Algorithm;

void EdgePairAlgorithm::onCompute()
{
	foreach(Node *node1, nodes())
	{
		foreach(Node *node2, node1->links())
		{
			foreach(Node *node3, node2->links())
			{
				Path *path = new Path();
				path->appendNode(node1);
				path->appendNode(node2);
				path->appendNode(node3);
				addReqResult(path);
			}
		}
	}
}
