#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include <memory>
#include <set>

#include "dyndbdriver.h"
#include "detectionreport.h"

class ReportManager
{
public:
  ReportManager(std::shared_ptr<DB::DynDBDriver> dbdriver,
                std::size_t packetSize = 20);

  /**
   * @brief Method returns collection of detection reports.
   *  Each invokation gives another part of DRs.
   * @return Collection of detection reports, ordered by time.
   */
  std::set<DetectionReport> getDRs();

private:
  void setupNextCursor();

  std::shared_ptr<DB::DynDBDriver> dbdriver_;
  DB::DynDBDriver::DRCursor drCursor_;
  std::size_t packetSize_; // how many DRs to obtain at once

  DetectionReport lastDR_;
};

#endif // REPORTMANAGER_H
