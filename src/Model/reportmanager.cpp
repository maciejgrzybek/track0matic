#include "reportmanager.h"

ReportManager::ReportManager(DB::DynDBDriver& dbdriver, std::size_t packetSize)
  : dbdriver_(dbdriver),
    drCursor_(dbdriver_.getDRCursor()), // TODO add parametrization for this
    packetSize_(packetSize)
{
}

std::set<DetectionReport> ReportManager::getDRs()
{
  std::set<DetectionReport> result;
  for (std::size_t i = 0;i<packetSize_;++i)
  {
    try
    {
      DB::DynDBDriver::DRCursor::DR_row row = drCursor_.fetchRow();
      // TODO get features for DR
      DetectionReport dr(row);
      result.insert(dr);
    }
    catch (const DB::exceptions::NoResultAvailable& /*ex*/)
    {
      break; // if no more results available, end loop
    }
  }

  return result;
}
