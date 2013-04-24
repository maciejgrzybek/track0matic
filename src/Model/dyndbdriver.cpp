#include "dyndbdriver.h"

#include <memory>
#include <fstream>
#include <string>

#include <boost/archive/xml_archive_exception.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <pqxx/connection>
#include <pqxx/transaction>

#include "sensorfactory.h"

namespace DB
{

namespace exceptions
{

DBException::~DBException() throw()
{}

const char* DBException::what() const throw()
{
  return "Unknown DB driver exception.";
}

NoResultAvailable::~NoResultAvailable() throw()
{}

const char* NoResultAvailable::what() const throw()
{
  return "No result available - maybe empty result.";
}

} // namespace exceptions

/******************************************************************************/

DynDBDriver::DR_row::DR_row(int sensor_id,
                                int dr_id,
                                double lon,
                                double lat,
                                double mos,
                                time_t sensor_time,
                                time_t upload_time)
  : sensor_id(sensor_id),dr_id(dr_id),
    lon(lon),lat(lat),mos(mos),
    upload_time(upload_time),sensor_time(sensor_time)
{}

/******************************************************************************/

DynDBDriver::DRCursor::DRCursor(DynDBDriver* dbdriver, time_t timestamp,
                                unsigned packetSize, int beforeFirstDRId)
  : dbdriver_(dbdriver),
    packetSize_(packetSize),
    offset_(0),
    resultInitialized_(false)
{
  startingTime_ = boost::posix_time::from_time_t(timestamp);

  std::stringstream sql;
  if (beforeFirstDRId > -1)
  {
    sql << "SELECT *,"
          "extract(epoch from upload_time) as upl_ts,"
          "extract(epoch from sensor_time) as sns_ts "
          "FROM detection_reports WHERE "
          "sensor_time >= to_timestamp($1) AND dr_id > "
          << beforeFirstDRId << " "
          << "ORDER BY sensor_time ASC, upload_time ASC "
             "LIMIT $2 OFFSET $3";
  }
  else
  {
    sql << "SELECT *,"
          "extract(epoch from upload_time) as upl_ts,"
          "extract(epoch from sensor_time) as sns_ts "
          "FROM detection_reports WHERE "
          "sensor_time >= to_timestamp($1) "
          "ORDER BY sensor_time ASC, upload_time ASC "
          "LIMIT $2 OFFSET $3";
  }

  // prepare statement (query) for connection
  dbdriver->db_connection_->unprepare("DR_select_statement");
  dbdriver->db_connection_->prepare("DR_select_statement",sql.str());
}

DynDBDriver::DR_row DynDBDriver::DRCursor::fetchRow()
{
  if (!resultInitialized_)
    fetchRows();
  else if (resultIterator_ == result_.end()) // no more rows to fetch
  {
    advancePacket();
    fetchRows();
  }
  pqxx::result::const_iterator row = resultIterator_++;
  return DR_row(row[0].as<int>(), // sensor_id
                row[1].as<int>(), // dr_id
                row[2].as<double>(), // lon
                row[3].as<double>(), // lat
                row[4].as<double>(), // meters_over_sea
                row[7].as<double>(), // upload_time - read as double, because of microseconds
                row[8].as<double>()); // sensor_time - like above
}

unsigned DynDBDriver::DRCursor::getPacketSize() const
{
  return packetSize_;
}

boost::int64_t
  DynDBDriver::DRCursor::timeToInt64(const boost::posix_time::ptime& pt)
{
  using namespace boost::posix_time;
  static ptime epoch(boost::gregorian::date(1970, 1, 1));
  time_duration diff(pt - epoch);
  return (diff.ticks() / diff.ticks_per_second());
}

void DynDBDriver::DRCursor::advancePacket()
{
  offset_ += packetSize_;
}

void DynDBDriver::DRCursor::fetchRows()
{
  pqxx::work t(*dbdriver_->db_connection_,"DRs fetcher");
  { // TODO rewrite this, when logger will be more sophisticated
    std::stringstream msg;
    msg << "Fetching rows with starting time = "
        << timeToInt64(startingTime_)
        << " packet size = " << packetSize_
        << " offset = " << offset_;
    Common::GlobalLogger::getInstance().log("DynDBDriver",msg.str());
  }
  result_
      = t.prepared("DR_select_statement")
      (pqxx::to_string(timeToInt64(startingTime_)))
      (packetSize_)
      (offset_).exec();

  { // TODO rewrite this, when logger will be more sophisticated
    std::stringstream msg;
    msg << "Fetched " << result_.size() << " rows";
    Common::GlobalLogger::getInstance().log("DynDBDriver",msg.str());
  }

  resultIterator_ = result_.begin();
  if (resultIterator_ == result_.end()) // if after fetching, result is empty - tell interested ones.
    throw DB::exceptions::NoResultAvailable();

  resultInitialized_ = true;
}

/******************************************************************************/

DynDBDriver::Sensor_row::Sensor_row(int sensor_id,
                                    double lon, double lat, double mos,
                                    double range,
                                    const std::string& type)
  : sensor_id(sensor_id),
    lon(lon),lat(lat),mos(mos),
    range(range),
    type(type)
{}

/******************************************************************************/

DynDBDriver::DynDBDriver(const std::string& options_path)
{
  loadOptions(options_path);
  db_connection_ = new pqxx::connection(options_.toString());
}

DynDBDriver::DynDBDriver(const DBDriverOptions& options)
  : options_(options)
{
  db_connection_ = new pqxx::connection(options_.toString());
}

DynDBDriver::~DynDBDriver()
{
  delete db_connection_;
}

DynDBDriver::DRCursor DynDBDriver::getDRCursor(time_t timestamp,
                                               unsigned packetSize,
                                               int beforeFirstDRId)
{
  return DRCursor(this,timestamp,packetSize,beforeFirstDRId);
}

void DynDBDriver::insertDR(const DR_row& dr)
{
  std::string sql;
  if (dr.dr_id == -1) // TODO change this magic constant
  { // get dr_id from DB's sequence
      sql = "INSERT INTO detection_reports "
        "(sensor_id,lon,lat,meters_over_sea,upload_time,sensor_time) "
        "VALUES("
          + pqxx::to_string(dr.sensor_id) + ","
          + pqxx::to_string(dr.lon) + ","
          + pqxx::to_string(dr.lat) + ","
          + pqxx::to_string(dr.mos) + ","
          + ((dr.upload_time == 0)
             ? "DEFAULT"
             : ("to_timestamp(" + pqxx::to_string(dr.upload_time) + ")"))
          + ",to_timestamp(" + pqxx::to_string(dr.sensor_time) + "))";
  }
  else
  { // dr_id given
    sql = "INSERT INTO detection_reports "
      "(sensor_id,dr_id,lon,lat,meters_over_sea,upload_time,sensor_time) "
      "VALUES("
        + pqxx::to_string(dr.sensor_id) + ","
        + pqxx::to_string(dr.dr_id) + ","
        + pqxx::to_string(dr.lon) + ","
        + pqxx::to_string(dr.lat) + ","
        + pqxx::to_string(dr.mos) + ","
        + ((dr.upload_time == 0)
           ? "DEFAULT"
           : ("to_timestamp(" + pqxx::to_string(dr.upload_time) + ")"))
        + ",to_timestamp(" + pqxx::to_string(dr.sensor_time) + "))";
  }

  pqxx::work t(*db_connection_,"DR inserter");
  t.exec(sql);
  t.commit();
}

std::set<Sensor*> DynDBDriver::getSensors()
{
  const std::string sql
      = "SELECT s.sensorid,s.lon,s.lat,s.mos,s.range,st.typename "
        "FROM sensors as s, sensortypes as st "
        "WHERE s.typeid = st.typeid";

  pqxx::work t(*db_connection_,"Sensors fetcher");
  pqxx::result result = t.exec(sql);

  pqxx::result::const_iterator resultIterator = result.begin();
  if (resultIterator == result.end()) // if after fetching, result is empty - tell interested ones.
    throw DB::exceptions::NoResultAvailable();

  std::set<Sensor*> resultSet;

  for (; resultIterator != result.end(); ++resultIterator)
  {
    pqxx::result::const_iterator row = resultIterator;
    Sensor* sensor
        = SensorFactory::getInstance()
            .produce(row[0].as<int>(),
                     row[1].as<double>(),
                     row[2].as<double>(),
                     row[3].as<double>(),
                     row[4].as<double>(),
                     row[5].as<std::string>());

    resultSet.insert(sensor);
  }

  return resultSet;
}

DBDriverOptions::DBDriverOptions(const std::string& host,
                                 const std::string& port,
                                 const std::string& dbname,
                                 const std::string& user,
                                 const std::string& password,
                                 unsigned int connection_timeout,
                                 const std::string& additional_options,
                                 SSLMode ssl_mode,
                                 const std::string& service)
  : host(host),
    port(port),
    dbname(dbname),
    username(user),
    password(password),
    connection_timeout(connection_timeout),
    additional_options(additional_options),
    ssl_mode(ssl_mode),
    service(service)
{}

std::string DBDriverOptions::toString() const
{
  std::string conn_timeout = boost::lexical_cast<std::string>(connection_timeout);
  std::string sslMode = sslModeToString();
  std::string result;
  if (!host.empty())
    result += "host='"+host+"' ";
  if (!port.empty())
    result += "port='"+port+"' ";
  if (!dbname.empty())
    result += "dbname='"+dbname+"' ";
  if (!username.empty())
    result += "user='"+username+"' ";
  if (!password.empty())
    result += "password='"+password+"' ";
  if (!conn_timeout.empty())
    result += "connect_timeout='"+conn_timeout+"' ";
  if (!additional_options.empty())
    result += "options='"+additional_options+"' ";
  if (!sslMode.empty())
    result += "sslmode='"+sslMode+"' ";
  if (!service.empty())
    result += "service='"+service+"'";

  return result;
}

std::string DBDriverOptions::sslModeToString() const
{
  switch (ssl_mode)
  {
    case DisableSSL:
      return "disable";
      break;
    case AllowSSL:
      return "allow";
      break;
    case RequireSSL:
      return "require";
      break;
    default:
    case PreferSSL:
      return "prefer";
      break;
  }
}

void DynDBDriver::loadOptions(const std::string& options_path)
{
  // read configuration from file
  std::ifstream ifs(options_path.c_str());
  boost::archive::xml_iarchive ia(ifs);
  try
  {
    ia >> boost::serialization::make_nvp("DB_options",options_);
  }
  catch (const boost::archive::xml_archive_exception& ex)
  {
    std::cerr << "Configuration file is corrupted: " << ex.what() << std::endl;
    throw;
  }
}

} // namespace DB
