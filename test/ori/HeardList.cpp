#include "HeardList.h"



DistanceInfo::DistanceInfo(int bid, BeaconInfo *beacon, double distance, int seq)
{
	this->bid = bid;
	this->location = beacon->location;
	this->distance = distance;
	this->seq = seq;
}

HeardList::HeardList()
{
	l_id = 0;
	reset();
}

HeardList::HeardList(int l_id)
{
	this->l_id = l_id;
	reset();
}

HeardList::~HeardList(void)
{
}

void HeardList::reset()
{
	heardInfo.clear();
/*
	heardBeacon.clear();
	heardDistance.clear();
	heardSeq.clear();
*/
}


void HeardList::insertHeardBeacon(int seq, int bid, BeaconInfo* beacon, double distance)
{	
	heardInfo.push_back(DistanceInfo(bid, beacon, distance, seq));
//	setHeardBeacon(seq, beacon->location, distance);
	last_seq = seq;
	heardInfo[heardInfo.size()-1].seq = beacon->temp2;
}

/*
void HeardList::setHeardBeacon(int seq, Vector location, double distance)
{
	heardSeq.push_back(seq);
	heardBeacon.push_back(location);
	heardDistance.push_back(distance);
}
*/
/*
int HeardList::GetValidList(HeardList &heardList)
{
	if (heardSeq[0] == heardSeq[heardSeq.size()-1]) return 0;
	int cur_seq = heardSeq[0];
	heardList.reset();
	if (heardSeq.size() > 5)
	{
		int a = 0;
		a ++;
	}
	while (heardSeq[0] == cur_seq)
	{
		heardList.heardSeq.push_back(heardSeq[0]);
		heardList.heardBeacon.push_back(heardBeacon[0]);
		heardList.heardDistance.push_back(heardDistance[0]);

		heardSeq.erase(heardSeq.begin());
		heardBeacon.erase(heardBeacon.begin());
		heardDistance.erase(heardDistance.begin());
	}
	return 1;
}
*/



void HeardList::Duplicate(HeardList& dupInstance)
{
	dupInstance.reset();
//	for (int i = 0; i<(int)heardBeacon.size(); i++)
	for (unsigned int i = 0; i < heardInfo.size(); i++)
	{
		dupInstance.heardInfo.push_back(heardInfo[i]);
/*
		dupInstance.heardBeacon.push_back(heardBeacon[i]);
		dupInstance.heardSeq.push_back(heardSeq[i]);
		dupInstance.heardDistance.push_back(heardDistance[i]);
*/
//		dupInstance.setHeardBeacon(heardSeq[i], heardBeacon[i], heardDistance[i]);
	}	
	dupInstance.n_valid = n_valid;
}

int HeardList::Compare(DistanceInfo &a, DistanceInfo &b)
{
	if (a.nVSS < b.nVSS) return -1;
	if (a.nVSS > b.nVSS) return 1;
	if (a.distance < b.distance) return -1;
	if (a.distance > b.distance) return 1;
	return 0;
}


void HeardList::Sort()
{
	for (unsigned int i = 0; i < heardInfo.size() - 1; i++)
	{
		for (unsigned int j = i + 1; j < heardInfo.size(); j++)
		{
			if (Compare(heardInfo[i], heardInfo[j]) > 0)
			{
				DistanceInfo temp = heardInfo[i];
				heardInfo[i] = heardInfo[j];
				heardInfo[j] = temp;
			}
		}
	}
	n_valid = 0;
	for (int i = 0; i < (int)heardInfo.size(); i++)
		heardInfo[i].valid = false;
}

void HeardList::Cut(size_t n)
{	
	for (int i = 0; i < (int) heardInfo.size(); i++)
	{
		if (!heardInfo[i].valid)
		{
			if (n_valid < (int)n)
			{
				heardInfo[i].valid = true;
				n_valid ++;
			}
			else			
			{
				heardInfo.erase(heardInfo.begin() + i);
				i--;
			}
		}
	}
	for (size_t i = 0; i < heardInfo.size() - 1; i++)
	{
		for (size_t j = i+1; j < heardInfo.size(); j++)
		{
			if (heardInfo[i].nVSS > heardInfo[j].nVSS)
			{
				DistanceInfo temp = heardInfo[i];
				heardInfo[i] = heardInfo[j];
				heardInfo[j] = temp;
			}
		}
	}
}

// Check whether finding solution without reflection is possible.
bool HeardList::CheckReflectionFitting(std::vector<Plane> &planeList)
{
	int availableBeacons = 0;
	
	for (unsigned int i = 0; i < heardInfo.size(); i++)
	{
//		heardInfo[i].fgRefFit = true;
		heardInfo[i].nVSS = 1;
		for (size_t j = 0; j < planeList.size(); j++)
		{
			double dist = planeList[j].getDistanceToPoint(heardInfo[i].location);
			if (dist > MIN_SEP && dist < heardInfo[i].distance)
			{
//				heardInfo[i].fgRefFit = false;
//				break;
				heardInfo[i].nVSS ++;
			}
		}
//		if (heardInfo[i].fgRefFit)
		if (heardInfo[i].nVSS == 1)
			availableBeacons ++;

	}

	if (availableBeacons < 4) return false;

	return true;
}