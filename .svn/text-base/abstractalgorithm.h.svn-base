#ifndef ABSTRACTALGORITHM_H
#define ABSTRACTALGORITHM_H

#include <QList>

#include "algorithmnode.h"
#include "algorithmpath.h"

namespace Algorithm
{
	class AbstractAlgorithm
	{
		private:
			QList<Node*> m_nodes;
			QList<Path*> m_covResults;
			QList<Path*> m_reqResults;

			void addCovResult(Path *path);
			void computeCoverage();


		protected:
			void filterSubpaths(QList<Path*> &paths, Path *currentPath);
			void addReqResult(Path *path);

			QList<Node*> &nodes() { return m_nodes; }

			virtual void onCompute() = 0;

		public:
			const QList<Path*> &coverageResults() const;
			const QList<Path*> &requirementsResults() const;

			void clearResults();
			void compute(const QList<Node*> &nodes, bool doComputeCoverage = true);

			virtual ~AbstractAlgorithm();
	};
}

#endif // ABSTRACTALGORITHM_H
