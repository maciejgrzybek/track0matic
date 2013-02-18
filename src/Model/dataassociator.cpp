#include <utility>

#include "dataassociator.h"

DataAssociator::DataAssociator()
{
}

void DataAssociator::compute()
{
  /*
   foreach Track
   {
    DRs = getListForTrack(Track); // after this line, DRGroups are thiner of DRs (chosen ones)
    associatedTracks.append(DRs);
    //notAssociatedTracks are built by getListForTrack method.
   }
   */
}

void DataAssociator::setDRResultComparator(std::unique_ptr<ResultComparator> comparator)
{
  resultComparator = std::move(comparator);
}

void DataAssociator::setListResultComparator(std::unique_ptr<ListResultComparator> comparator)
{
  listResultComparator = std::move(comparator);
}

