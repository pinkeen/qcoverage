#include "primepathsalgorithm.h"
#include <QDebug>

using namespace Algorithm;

void PrimePathsAlgorithm::onCompute()
{
	simplePathsAlgorithm.compute(nodes(), false);

	QList<Path*> paths = simplePathsAlgorithm.requirementsResults();

	foreach(Path *patha, paths)
	{
		bool prime = true;

		foreach(Path *pathb, paths)
		{
			if(patha != pathb && pathb->containsPath(patha))
			{
				prime = false;
				break;
			}


		}

		if(prime)
			addReqResult(new Path(patha));
	}
}
