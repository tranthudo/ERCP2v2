
#include "N2TMatcher.h"

N2TMatcher::N2TMatcher(void) : ratio(0.95f)
{
	
}


N2TMatcher::~N2TMatcher(void)
{
}



int N2TMatcher::ratioTest( std::vector<std::vector<cv::DMatch>>& matches)
{
	
	int removed=0;	
	// for all matches
	for (std::vector<std::vector<cv::DMatch>>::iterator matchIterator= matches.begin();
		matchIterator!= matches.end(); ++matchIterator) {

			// if 2 NN has been identified
			if (matchIterator->size() > 1) {

				// check distance ratio
				if ((*matchIterator)[0].distance/(*matchIterator)[1].distance > ratio) {

					matchIterator->clear(); // remove match
					removed++;
				}

			} else { // does not have 2 neighbours

				matchIterator->clear(); // remove match
				removed++;
			}
	}
	//for (std::vector<std::vector<cv::DMatch>>::iterator matchIterator= matches.begin();
	//	matchIterator!= matches.end(); ++matchIterator)
	//{
	//	if (matchIterator->size() == 2)
	//	{
	//		/*cv::DMatch match;
	//		match.distance = (*matchIterator)[0].distance;
	//		match.trainIdx = (*matchIterator)[0].trainIdx;*/
	//		ratioMatches.push_back((*matchIterator)[0]);
	//	}
	//}
	return removed;
}

void N2TMatcher::symmetryTest( const std::vector<std::vector<cv::DMatch>>& matches1, const std::vector<std::vector<cv::DMatch>>& matches2, std::vector<cv::DMatch>& symMatches )
{
	// for all matches image 1 -> image 2
	for (std::vector<std::vector<cv::DMatch>>::const_iterator matchIterator1= matches1.begin();
		matchIterator1!= matches1.end(); ++matchIterator1) {

			if (matchIterator1->size() < 2) // ignore deleted matches 
				continue;

			// for all matches image 2 -> image 1
			for (std::vector<std::vector<cv::DMatch>>::const_iterator matchIterator2= matches2.begin();
				matchIterator2!= matches2.end(); ++matchIterator2) {

					if (matchIterator2->size() < 2) // ignore deleted matches 
						continue;

					// Match symmetry test
					if ((*matchIterator1)[0].queryIdx == (*matchIterator2)[0].trainIdx  && 
						(*matchIterator2)[0].queryIdx == (*matchIterator1)[0].trainIdx) {

							// add symmetrical match
							symMatches.push_back(cv::DMatch((*matchIterator1)[0].queryIdx,
								(*matchIterator1)[0].trainIdx,
								(*matchIterator1)[0].distance));
							break; // next match in image 1 -> image 2
					}
			}
	}	
}

