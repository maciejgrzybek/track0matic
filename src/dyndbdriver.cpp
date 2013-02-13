#include <iostream>
#include <fstream>
#include <string>

#include <boost/archive/xml_archive_exception.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <pqxx/connection>
#include <pqxx/transaction>

#include "sensorfactory.h"

#include "dyndbdriver.h"

namespace DB
{

DynDBDriver::DynDBDriver(const std::string& options_path)
{
  { // read configuration from file
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
  db_connection_ = new pqxx::connection(options_.toString());
}

DynDBDriver::~DynDBDriver()
{
  delete db_connection_;
}

DynDBDriver::DRCursor DynDBDriver::getDRCursor(time_t timestamp,
                                               unsigned packetSize)
{
  return DRCursor(*this,timestamp,packetSize);
}

std::vector<Sensor*> DynDBDriver::getSensors(SensorFactory* producer)
{
  const std::string sql
      = "SELECT s.sensorid,s.lon,s.lat,s.mos,s.range,st.sensortype FROM sensors as s, sensortypes as st "
      "WHERE s.typeid = st.typeid";

  pqxx::work t(*db_connection_,"Sensors fetcher");
  pqxx::result result = t.exec(sql);

  pqxx::result::const_iterator resultIterator = result.begin();
  if (resultIterator == result.end()) // if after fetching, result is empty - tell interested ones.
    throw DB::exceptions::NoResultAvailable();

  std::vector<Sensor*> resultVector;

  for (; resultIterator != result.end(); ++resultIterator)
  {
    pqxx::result::const_iterator row = resultIterator;
    Sensor* sensor = producer->produce(row[0].as<int>(),
                                       row[1].as<double>(),
                                       row[2].as<double>(),
                                       row[3].as<double>(),
                                       row[4].as<double>(),
                                       row[5].as<std::string>());
    resultVector.push_back(sensor);
  }

  return resultVector;
}

DynDBDriver::DBDriverOptions::DBDriverOptions(const std::string& host,
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
{
}

std::string DynDBDriver::DBDriverOptions::toString() const
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

std::string DynDBDriver::DBDriverOptions::sslModeToString() const
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

} // namespace DB

// FIXME remove this, only for testing purpose
/*#include <iostream>
int main(void)
{
  DB::DynDBDriver dbdrv("options.xml");
  DB::DynDBDriver::DRCursor cursor(dbdrv);

  std::cout << "DRs:" << std::endl;
  try
  {
    while (1)
    {
      DB::DynDBDriver::DRCursor::DR_row row = cursor.fetchRow();
      std::cout << row.sensor_id << " " << row.dr_id << " "
                << row.lon << " " << row.lat << " "
                << row.mos << " " << row.sensor_time << " "
                << row.upload_time << std::endl;
    }
  }
  catch (const DB::exceptions::NoResultAvailable& ex)
  {
    std::cout << ex.what() << std::endl;
  }
  try
  {
    std::cout << "Sensors:" << std::endl;
    std::vector<DB::DynDBDriver::Sensor_row> sensors = dbdrv.getSensors();
    for (auto it : sensors)
    {
      std::cout << it.sensor_id << " " << it.lon << " " << it.lat << " " << it.mos << " " << it.range << " " << it.type << std::endl;
    }
  }
  catch (const DB::exceptions::NoResultAvailable& ex)
  {
    std::cout << ex.what() << std::endl;
  }
}
*/
