#ifndef FUSIONEXECUTOR_H
#define FUSIONEXECUTOR_H

#include <map>
#include <memory>
#include <set>

#include "detectionreport.h"
#include "track.h"

class FusionExecutor
{
public:
  FusionExecutor();

  /**
   * @brief Makes detection reports fusion on given collection.
   *  Collection will be changed after this call!
   *  Some of DRs could be removed and others could be replaced!
   *
   *  Method can be overloaded to achieve special type of fusion.
   *  Default fusion invokes correct() on estimation filter,
   *  connected with Track for each DR of it's interest.
   */
  virtual void fuseDRs(std::map<std::shared_ptr<Track>,std::set<DetectionReport> >&);
};

#endif // FUSIONEXECUTOR_H
