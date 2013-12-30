#include "abstractalgorithm.h"

#include <QDebug>

using namespace Algorithm;

const QList<Path*> &AbstractAlgorithm::coverageResults() const
{
	return m_covResults;
}

const QList<Path*> &AbstractAlgorithm::requirementsResults() const
{
	return m_reqResults;
}

void AbstractAlgorithm::clearResults()
{
	foreach(Path *path, m_covResults)
	{
		delete path;
	}

	m_covResults.clear();

	foreach(Path *path, m_reqResults)
	{
		delete path;
	}

	m_reqResults.clear();
}

AbstractAlgorithm::~AbstractAlgorithm()
{
	clearResults();
}

void AbstractAlgorithm::addCovResult(Path *path)
{
	m_covResults.append(path);
}

void AbstractAlgorithm::addReqResult(Path *path)
{
	m_reqResults.append(path);
}

void AbstractAlgorithm::compute(const QList<Node*> &nodes, bool doComputeCoverage)
{
	clearResults();
	m_nodes.clear();
	m_nodes.append(nodes);

	onCompute();

	if(doComputeCoverage)
		computeCoverage();
}

void AbstractAlgorithm::filterSubpaths(QList<Path*> &paths, Path *currentPath)
{
	foreach(Path *path, paths)
	{
		if(currentPath->containsPath(path))
		{
			paths.removeOne(path);
			delete path;
		}
	}
}

void AbstractAlgorithm::computeCoverage()
{
	QList<Path*> tmpPaths;

	foreach(Path* path, m_reqResults)
	{
		tmpPaths.append(new Path(path));
	}


	do
	{
		Path *currentPath;

		//find longest path

		int maxlen = tmpPaths.first()->nodeCount();
		int maxi = 0;

		for(int i = 1; i < tmpPaths.size(); ++i)
		{
			Path *pathi = tmpPaths.at(i);
			if(pathi->nodeCount() > maxlen)
			{
				maxlen = pathi->nodeCount();
				maxi = i;
			}
		}

		currentPath = tmpPaths.at(maxi);
		tmpPaths.removeAt(maxi);


		if(currentPath->isTestPath())
		{
			filterSubpaths(tmpPaths, currentPath);
			m_covResults.append(currentPath);
			qDebug() << "currentPath: " << currentPath->toText();
			continue;
		}

		do
		{
			filterSubpaths(tmpPaths, currentPath);
			qDebug() << "currentPath: " << currentPath->toText();

			if(tmpPaths.size() > 0)
			{
				//search for longest overlap and merge

				int maxoverlap = tmpPaths.first()->intersects(currentPath);
				int maxi = 0;

				for(int i = 1; i < tmpPaths.size(); ++i)
				{
					Path *pathi = tmpPaths.at(i);

					int overlap = pathi->intersects(currentPath);

					qDebug() << pathi->toText() << " overlaps: " << overlap;


					// lets choose an open path over those which end
					if(maxoverlap > 0 && overlap == maxoverlap)
					{
						if(tmpPaths.at(maxi)->isBorderPath())
							maxi = i;
					}

					if(overlap > maxoverlap)
					{


						maxoverlap = overlap;
						maxi = i;
					}
				}

				if(maxoverlap != 0)
				{
					Path *maxPath = tmpPaths.at(maxi);
					Path *merged = currentPath->mergedWith(maxPath);

					tmpPaths.removeOne(maxPath);

					qDebug() << "merging path: " << maxPath->toText();

					delete maxPath;
					delete currentPath;

					currentPath = merged;

					continue;
				}

				qDebug() << "max overlap" << maxoverlap;
			}



			if(tmpPaths.size() > 0)
			{
				bool append = true;
				int maxi = -1;
				int maxlen = -1;

				//find longest path that can be appended

				for(int i = 0; i < tmpPaths.size(); ++i)
				{
					Path *pathi = tmpPaths.at(i);

					if(pathi->nodeCount() > maxlen)
					{
						if(currentPath->lastNode()->hasLink(pathi->firstNode()))
						{
							maxlen = pathi->nodeCount();
							maxi = i;
						}
					}
				}

				//find longest path that can be prepended

				for(int i = 0; i < tmpPaths.size(); ++i)
				{
					Path *pathi = tmpPaths.at(i);

					if(pathi->nodeCount() > maxlen)
					{
						if(pathi->lastNode()->hasLink(currentPath->firstNode()))
						{
							maxlen = pathi->nodeCount();
							maxi = i;
							append = false;
						}
					}
				}

				if(maxi != -1)
				{
					Path *pathi = tmpPaths.at(maxi);

					if(append)
					{
						currentPath->appendPath(pathi);
						qDebug() << "appending linked path: " << pathi->toText();
					}
					else
					{
						currentPath->prependPath(pathi);
						qDebug() << "prepending linked path: " << pathi->toText();
					}

					tmpPaths.removeOne(pathi);
					delete pathi;

					continue;
				}
			}


			bool append = true;
			int mindist = 0xFFFFFFF;
			int mini = -1;

			// finally search for nodes that are closest to end...

			for(int i = 0; i < m_nodes.size(); ++i)
			{
				Node *node = m_nodes.at(i);

				int dist = node->distanceToNearestEndNode();

				if(dist < mindist)
				{
					if(currentPath->lastNode()->hasLink(node) && currentPath->lastNode() != node)
					{
						mindist = dist;
						mini = i;
					}
				}
			}

			// ...or start

			for(int i = 0; i < m_nodes.size(); ++i)
			{
				Node *node = m_nodes.at(i);

				int dist = node->distanceToNearestStartNode();

				if(dist < mindist)
				{
					if(node->hasLink(currentPath->firstNode()) && node != currentPath->firstNode())
					{
						mindist = dist;
						mini = i;
						append = false;
					}
				}
			}

			if(mini != -1)
			{
				if(append)
				{
					currentPath->appendNode(m_nodes.at(mini));
					qDebug() << "appending node: " << m_nodes.at(mini)->label();
				}
				else
				{
					currentPath->prependNode(m_nodes.at(mini));
					qDebug() << "prepending node: " << m_nodes.at(mini)->label();
				}
			}


		}
		while(!currentPath->isTestPath());

		filterSubpaths(tmpPaths, currentPath);

		qDebug() << "currentPathend " << currentPath->toText();

		m_covResults.append(currentPath);

	} while(tmpPaths.size() > 0);



}
