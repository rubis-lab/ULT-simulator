#include "Estimator.h"

Estimator::Estimator()
{
	measurementList = NULL;
	input = NULL;
	filterManager = NULL;
	reset();
}

Estimator::~Estimator()
{
	destructor();	
}
void Estimator::destructor()
{
	if (measurementList != NULL)
		delete measurementList;
	measurementList = NULL;
	
	if (input != NULL)
		delete input;
	input = NULL;

	if (filterManager != NULL)
		delete filterManager;
	filterManager = NULL;
}

void Estimator::reset()
{
	destructor();

	prevLocation = Vector();
}


void Estimator::setEstimator(EstimatorArgument estimatorArgument)
{
	reset();
	args = estimatorArgument;
	args.setCutThreshold();

	setMeasurementList();
	setSolver();
	setInput();
	setFilterManager();

}


void Estimator::setMeasurementList()
{

	measurementList = new MeasurementList(args.lid, args.beacons, args.planes);


	MeasurementCondition measurementCondition;

	measurementCondition.minSize = args.minBeaconSize;
	measurementCondition.validSize = args.validSize;
	measurementCondition.strictValidSize = args.strictValidSize;
	measurementCondition.timeWindow = args.timeWindow;

	measurementCondition.shortDistanceFirst = (bool)(!args.estimatorMode == EST::TRADITIONAL);
	measurementCondition.smallerNVSSFirst = (bool)(args.optimization & OPT::SELECTION);

	measurementCondition.minValidDistance = args.minValidDistance;

	
	measurementList->setMeasurementCondition(measurementCondition);

	

}

void Estimator::setSolver()
{
	SolverCondition condition;

	condition.solveNaive = (bool)(args.estimatorMode == EST::TRADITIONAL);

	condition.cutBranch1 = (bool)(args.optimization & 4);
	condition.cutBranch2 = (bool)((int)args.optimization & (int)OPT::BRANCHCUT_2);

	condition.maxMeasError = args.maxMeasError;


	solver.setSolverCondition(condition);
}

void Estimator::setInput()
{
	input = new SolverInput(measurementList, args.timeWindow);
}

void Estimator::setFilterManager()
{
	KFArgument kfArgs;
	kfArgs.mode = args.kfMode;
	kfArgs.timeSlot = args.timeSlot;
	kfArgs.KFMeasError = args.kfMeasError;
	kfArgs.KFSystemError = args.kfSystemError;
	filterManager = new FilterManager(kfArgs);
}


void Estimator::measure(unsigned long timestamp, int userBid, double distance)
{
	measurementList->measure(userBid, timestamp, distance);
}


EstimatorResult Estimator::solve(long currentTime)
{
	measurementList->makeSnapshot(currentTime, args.timeWindow);
	input->setup(currentTime, prevLocation);

	SolverResultList results;
	
	solver.solve(input, &results);

	if (args.optimization & OPT::THRESHOLD)
	{
		results.cutThreshold(pow(args.cutThreshold, 2));
	}

	EstimatorResult ret;

	if (args.estimatorMode == EST::KFONLY || args.estimatorMode == EST::PROPOSED1)
	{
		filterManager->getCorrectedResult(&results);
		SolverResult filteredResult = results.getFilteredResult();
		
		ret.location = filteredResult.getCorrectedLocation();
		ret.error = filteredResult.getCorrectedError();

		if (!filteredResult.isValid())
		{
			ret.location = prevLocation;
			ret.error = filteredResult.getError(prevLocation);
		}
	}
	else
	{
		if (results.size() == 0)
		{
			printf("unexpected error. results size is zero. in Estimator::solve\n");
			exit(20);
		}
		SolverResult result = results.getFirstResult();
			
		if (result.isValid())
		{
			ret.location = result.location;
			ret.error = result.error;
		}
		else
		{
			ret.location = prevLocation;
			ret.error = result.getError(prevLocation);
		}
	}

	return ret;

}









