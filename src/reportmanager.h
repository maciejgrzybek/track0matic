#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include <vector>

#include "dyndbdriver.h"

class DetectionReport
{
  // TODO implement this
};

class ReportManager
{
public:
  ReportManager(DB::DynDBDriver& dbdriver);

  /**
   * @brief Method returns collection of detection reports.
   *  Each invokation gives another part of DRs.
   * @return Collection of detection reports, ordered by time.
   */
  std::vector<DetectionReport> getDRs() const;

private:
  DB::DynDBDriver& dbdriver_;
  DB::DynDBDriver::DRCursor drCursor_;
};

#endif // REPORTMANAGER_H
