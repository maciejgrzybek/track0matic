#ifndef DYNDBDRIVER_H
#define DYNDBDRIVER_H

#include <vector>
#include <set>
#include <string>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/uuid/uuid.hpp>

#include <pqxx/connection>
#include <pqxx/transaction>
#include <pqxx/prepared_statement>

#include <Common/logger.h>

class Sensor;

namespace DB
{

namespace exceptions
{

class DBException : public std::exception
{
public:
  virtual ~DBException() throw();
  virtual const char* what() const throw();
};

class NoResultAvailable : public DBException
{
public:
  virtual ~NoResultAvailable() throw();
  virtual const char* what() const throw();
};

} // namespace exceptions

class DBDriverOptions
{
public:
  /* SSLMode can be:
      DisableSSL - SSL is not used when connecting to DB
      AllowSSL - non-SSL connection is tried first, on fail, SSL goes on
      PreferSSL - first SSL connection is tried, when failed, non-SSL connection is tried [DEFAULT VALUE]
      RequireSSL - when SSL connection fails, error is raised
  */
  enum SSLMode { DisableSSL, AllowSSL, PreferSSL, RequireSSL };

  DBDriverOptions(const std::string& host = "host",
                  const std::string& port = "5432",
                  const std::string& dbname = "",
                  const std::string& user = "",
                  const std::string& password = "",
                  unsigned int connection_timeout = 0,
                  const std::string& additional_options = "",
                  SSLMode ssl_mode = PreferSSL,
                  const std::string& service = "");

  std::string toString() const;

  // if any option is empty, default will be used
  std::string host; // DB hostname; if starts with slash ("/"), means unix socket; default is "localhost"
  std::string port; // port or socket file name extension for Unix-domain connections.
  std::string dbname; // dbname; default is the same as username
  std::string username; // username used to authenticate to DB
  std::string password; // password used to authenticate to DB
  unsigned int connection_timeout; // connection timeout, when empty (0) - connector will wait indefinitely. timeout less than 2s is not recommended
  std::string additional_options; // additional options to pass to DB when connecting
  enum SSLMode ssl_mode; // read above explaination of SSLMode enum
  std::string service; // service name to use for additional parameters (maintained centrally)

private:
  friend class boost::serialization::access;

  std::string sslModeToString() const;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    using boost::serialization::make_nvp;
    ar & make_nvp("Host",host);
    ar & make_nvp("Port",port);
    ar & make_nvp("DBName",dbname);
    ar & make_nvp("Username",username);
    ar & make_nvp("Password",password);
    ar & make_nvp("Connection_timeout",connection_timeout);
    ar & make_nvp("Additional_options",additional_options);
    ar & make_nvp("SSL_mode",ssl_mode);
    ar & make_nvp("Service",service);
  }
};

class DynDBDriver
{
public:
  struct DR_row
  {
    DR_row(int sensor_id, int dr_id,
           double lon, double lat, double mos,
           time_t sensor_time, time_t upload_time = 0);

    int sensor_id;
    int dr_id;
    double lon;
    double lat;
    double mos;
    time_t upload_time;
    time_t sensor_time;
    Sensor* sensor; // TODO implement this, when needed: should fetch Sensor data from DB and produce it with SensorFactory
  };

  struct Track_row
  {
    Track_row(const boost::uuids::uuid& uuid,
              double lon, double lat, double mos,
              double lonVelocity, double latVelocity, double mosVelocity,
              double predictedLon, double predictedLat, double predictedMos,
              time_t refreshTime);

    static std::string uuidToString(const boost::uuids::uuid& uuid);

    boost::uuids::uuid uuid;

    double lon;
    double lat;
    double mos;

    double lonVelocity;
    double latVelocity;
    double mosVelocity;

    double predictedLon;
    double predictedLat;
    double predictedMos;

    time_t refreshTime;
  };

  class DRCursor
  {
  public:
    /**
     * @brief DRCursor
     * @param dbdriver reference to DB driver, to be used for fetching rows
     * @param timestamp number of seconds from epoch start (01.01.1970) from where DRs are fetched
     * @param packetSize how many DRs are selected at once
     * @param ID of last retrieved DR - Cursor will obtain DRs with ID higher than given
     *  if -1 value given - option is disabled.
     *  This option is used to actively poll DB for new DRs,
     *  when no more result are yet available.
     */
    DRCursor(DynDBDriver* dbdriver,
             time_t timestamp = 0,
             unsigned packetSize = 20,
             int beforeFirstDRId = -1);

    /**
     * @brief fetchRow - return one row from iterator and advances.
     * @return DR_row from db, selected according startingTime_ and packetSize_
     */
    DR_row fetchRow();
    unsigned getPacketSize() const;

  private:
    /**
     * @brief timeToInt64 converts point time into int64 type,
     *  assuming ptime is in unix epoch.
     * @param pt ptime to convert
     * @return int64 containing timestamp.
     */
    static boost::int64_t timeToInt64(const boost::posix_time::ptime& pt);

    /**
     * @brief advancePacket method moves window for another packet of data
     *  It only sets offset, but does not affect DB
     *  If you would like to fetch data from next pack,
     *  use advancePacket() and fetchRow() later
     */
    void advancePacket();

    /**
     * @brief fetchRows executes prepared select with given parameters
     *  which are startingTime_ and packetSize_ (limit)
     */
    void fetchRows();

    DynDBDriver* dbdriver_;
    boost::posix_time::ptime startingTime_;
    unsigned packetSize_;
    unsigned offset_;

    bool resultInitialized_;
    pqxx::result::const_iterator resultIterator_;
    pqxx::result result_;
  };

  struct Sensor_row
  {
    Sensor_row(int sensor_id,
               double lon, double lat, double mos,
               double range,
               const std::string& type);

    int sensor_id;
    double lon;
    double lat;
    double mos;
    double range;
    std::string type;
  };

  /**
   * @brief C-tor for DynDBDriver, basing on external options file
   * @param path to configuration file
   * @deprecated use DynDBDriver(const DBDriverOptions&) instead.
   */
  DynDBDriver(const std::string& options_path);

  /**
   * @brief C-tor for DynDBDriver, basing on given options,
   *  loaded from configuration file
   * @param options
   */
  DynDBDriver(const DBDriverOptions& options);
  ~DynDBDriver();

  DRCursor getDRCursor(time_t timestamp = 0,
                       unsigned packetSize = 20,
                       int beforeFirstDRId = -1);

  void insertDR(const DR_row& dr);
  void insertTrack(const Track_row& track);

  std::set<class Sensor*> getSensors();

private:
  void loadOptions(const std::string& options_path);

  DBDriverOptions options_;
  pqxx::connection* db_connection_;
};

} // namespace DB

#endif // DYNDBDRIVER_H
