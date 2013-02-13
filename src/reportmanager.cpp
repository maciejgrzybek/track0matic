#include "reportmanager.h"

ReportManager::ReportManager(DB::DynDBDriver& dbdriver)
  : dbdriver_(dbdriver),
    drCursor_(dbdriver_.getDRCursor()) // TODO add parametrization for this
{
}

std::vector<DetectionReport> ReportManager::getDRs() const
{
  // TODO implement this
}
