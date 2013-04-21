#include "reportmanager.h"

#include <Common/logger.h>
#include <Common/time.h>

ReportManager::ReportManager(std::shared_ptr<DB::DynDBDriver> dbdriver,
                             std::size_t packetSize)
  : dbdriver_(dbdriver),
    drCursor_(dbdriver_->getDRCursor()), // TODO add parametrization for this
    packetSize_(packetSize),
    lastDR_(-1,-1,-1,-1,-1,0,0) // dummy DR
{}

std::set<DetectionReport> ReportManager::getDRs()
{
  std::set<DetectionReport> result;
  for (std::size_t i = 0; i<packetSize_; ++i)
  {
    try
    {
      DB::DynDBDriver::DR_row row = drCursor_.fetchRow();
      // TODO get features for DR
      DetectionReport dr(row);
      lastDR_ = dr;
      result.insert(dr);
    }
    catch (const DB::exceptions::NoResultAvailable& /*ex*/)
    { // if no more results available, prepare new Cursor and end loop
      Common::GlobalLogger
          ::getInstance().log("ReportManager","No result available from DB");

      setupNextCursor();
      break;
    }
  }

  return result;
}

void ReportManager::setupNextCursor()
{
  time_t lastDRTime = lastDR_.getRawSensorTime();

  drCursor_ = dbdriver_->getDRCursor(lastDRTime,
                                     drCursor_.getPacketSize(),
                                     lastDR_.getDrId());
}
