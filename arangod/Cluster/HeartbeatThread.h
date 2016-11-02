////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_CLUSTER_HEARTBEAT_THREAD_H
#define ARANGOD_CLUSTER_HEARTBEAT_THREAD_H 1

#include "Basics/Thread.h"

#include "Basics/ConditionVariable.h"
#include "Basics/Mutex.h"
#include "Basics/asio-helper.h"
#include "Cluster/AgencyComm.h"
#include "Cluster/DBServerAgencySync.h"
#include "Logger/Logger.h"

namespace arangodb {

struct AgencyVersions {
  uint64_t plan;
  uint64_t current;

  AgencyVersions(uint64_t _plan, uint64_t _current)
      : plan(_plan), current(_plan) {}

  explicit AgencyVersions(const DBServerAgencySyncResult& result)
      : plan(result.planVersion), current(result.currentVersion) {}
};

class AgencyCallbackRegistry;

class HeartbeatThread : public Thread,
                        public std::enable_shared_from_this<HeartbeatThread> {
 public:
  HeartbeatThread(AgencyCallbackRegistry*, uint64_t interval,
                  uint64_t maxFailsBeforeWarning, boost::asio::io_service*);
  ~HeartbeatThread();

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief initializes the heartbeat
  //////////////////////////////////////////////////////////////////////////////

  bool init();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the thread is ready
  //////////////////////////////////////////////////////////////////////////////

  bool isReady() const { return _ready.load(); }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief set the thread status to ready
  //////////////////////////////////////////////////////////////////////////////

  void setReady() { _ready.store(true); }

  void dispatchedJobResult(DBServerAgencySyncResult);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the thread has run at least once.
  /// this is used on the coordinator only
  //////////////////////////////////////////////////////////////////////////////

  static bool hasRunOnce() { return HasRunOnce.load(); }

 protected:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief heartbeat main loop
  //////////////////////////////////////////////////////////////////////////////

  void run() override;

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief heartbeat main loop, coordinator version
  //////////////////////////////////////////////////////////////////////////////

  void runCoordinator();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief heartbeat main loop, dbserver version
  //////////////////////////////////////////////////////////////////////////////

  void runDBServer();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handles a plan change, coordinator case
  //////////////////////////////////////////////////////////////////////////////

  bool handlePlanChangeCoordinator(uint64_t);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handles a plan change, DBServer case
  //////////////////////////////////////////////////////////////////////////////

  bool handlePlanChangeDBServer(uint64_t);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief handles a state change
  //////////////////////////////////////////////////////////////////////////////

  bool handleStateChange(AgencyCommResult&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief sends the current server's state to the agency
  //////////////////////////////////////////////////////////////////////////////

  bool sendState();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief bring the db server in sync with the desired state
  //////////////////////////////////////////////////////////////////////////////

  bool syncDBServerStatusQuo();

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief AgencyCallbackRegistry
  //////////////////////////////////////////////////////////////////////////////

  AgencyCallbackRegistry* _agencyCallbackRegistry;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief status lock
  //////////////////////////////////////////////////////////////////////////////

  arangodb::Mutex _statusLock;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief AgencyComm instance
  //////////////////////////////////////////////////////////////////////////////

  AgencyComm _agency;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief condition variable for heartbeat
  //////////////////////////////////////////////////////////////////////////////

  arangodb::basics::ConditionVariable _condition;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief this server's id
  //////////////////////////////////////////////////////////////////////////////

  std::string const _myId;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief heartbeat interval
  //////////////////////////////////////////////////////////////////////////////

  uint64_t _interval;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief number of fails in a row before a warning is issued
  //////////////////////////////////////////////////////////////////////////////

  uint64_t _maxFailsBeforeWarning;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief current number of fails in a row
  //////////////////////////////////////////////////////////////////////////////

  uint64_t _numFails;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief last successfully dispatched version
  //////////////////////////////////////////////////////////////////////////////

  uint64_t _lastSuccessfulVersion;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief currently dispatching
  //////////////////////////////////////////////////////////////////////////////

  bool _isDispatchingChange;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief current plan version
  //////////////////////////////////////////////////////////////////////////////

  uint64_t _currentPlanVersion;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the thread is ready
  //////////////////////////////////////////////////////////////////////////////

  std::atomic<bool> _ready;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the heartbeat thread has run at least once
  /// this is used on the coordinator only
  //////////////////////////////////////////////////////////////////////////////

  static std::atomic<bool> HasRunOnce;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief keeps track of the currently installed versions
  //////////////////////////////////////////////////////////////////////////////
  AgencyVersions _currentVersions;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief keeps track of the currently desired versions
  //////////////////////////////////////////////////////////////////////////////
  AgencyVersions _desiredVersions;

  bool _wasNotified;

  std::unique_ptr<boost::asio::io_service::strand> _strand;
};
}

#endif
