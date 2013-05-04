#include "configurationmanager.h"

#include <fstream>

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

namespace Common
{

namespace Configuration
{

namespace exceptions
{

KeyDoesNotExistException::KeyDoesNotExistException(const std::string& keyName)
  : keyName_(keyName),
    message_("Requested key - " + keyName + " does not exist in configuration.")
{}

const char* KeyDoesNotExistException::what() const throw()
{
  return message_.c_str();
}

std::string KeyDoesNotExistException::getKeyName() const
{
  return keyName_;
}

/******************************************************************************/

ModuleDoesNotExistException
::ModuleDoesNotExistException(const std::string& moduleName)
  : moduleName_(moduleName),
    message_("Requested module - " + moduleName
             + " does not exist in configuration.")
{}

const char* ModuleDoesNotExistException::what() const throw()
{
  return message_.c_str();
}

std::string ModuleDoesNotExistException::getModuleName() const
{
  return moduleName_;
}

/******************************************************************************/

ModuleAlreadyExistsException
::ModuleAlreadyExistsException(const std::string& moduleName)
  : moduleName_(moduleName),
    message_("Module " + moduleName + " already exists in configuration. "
             "Remove current to add new.")
{}

const char* ModuleAlreadyExistsException::what() const throw()
{
  return message_.c_str();
}

std::string ModuleAlreadyExistsException::getModuleName() const
{
  return moduleName_;
}

/******************************************************************************/

ConfigurationFileCorruptedException
::ConfigurationFileCorruptedException(const std::string& filename)
  : fileName_(filename),
    message_("The given configuration file " + filename
             + " is not accessible/is corrupted.")
{}

const char* ConfigurationFileCorruptedException::what() const throw()
{
  return message_.c_str();
}

std::string ConfigurationFileCorruptedException::getFileName() const
{
  return fileName_;
}

} // namespace exceptions

ConfigurationManager::ModuleConfiguration::ModuleConfiguration(
    ConfigurationManager::KeyValueMap& map)
  : map_(std::move(map))
{}

std::string
  ConfigurationManager::ModuleConfiguration::operator[](const std::string& key)
{
  KeyValueMap::const_iterator iter = map_.find(key);
  if (iter == map_.end())
    throw exceptions::KeyDoesNotExistException(key);

  return iter->second;
}

bool ConfigurationManager
::ModuleConfiguration::isInConfiguration(const std::string& key)
{
  KeyValueMap::const_iterator iter = map_.find(key);
  if (iter == map_.end())
    return false;

  return true;
}

std::string ConfigurationManager::ModuleConfiguration::toString() const
{
  std::string result;
  for (auto& i : map_)
  {
    result += (i.first + " = " + i.second + "\n");
  }

  return result;
}

/******************************************************************************/

ConfigurationManager& ConfigurationManager::getInstance()
{
  // Meyer's singleton design pattern. Be carefull in multithreaded environment!
  static ConfigurationManager instance;
  return instance;
}

std::string ConfigurationManager::toString() const
{
  std::string result;
  for (auto& i : map_)
  {
    result += (i.first + ":\n" + i.second.toString() + "\n");
  }

  return result;
}

void ConfigurationManager::parseKeyValueMapIntoConfiguration(
    KeyValueMap& configuration)
{
  std::map<std::string,KeyValueMap> modulesConfiguration;

  // obtain all configurations per module (for all modules)
  for (auto& i : configuration)
  {
    const std::string& key = i.first;
    const std::string& value = i.second;

    std::size_t pos = key.find(".");
    if (pos == std::string::npos)
      continue; // no module given, so skip this entry
    std::string module = key.substr(0,pos); // get everything till first "."
    std::string configurationKey = key.substr(pos+1); // get the rest

    modulesConfiguration[module].insert(
          std::make_pair(configurationKey,value));
  }

  // add already obtained configurations to manager
  for (auto& c : modulesConfiguration)
  {
    addConfigurationForModule(c.first,c.second);
  }
}

void
  ConfigurationManager::addConfigurationForModule(const std::string& moduleName,
                                                  KeyValueMap& configuration)
{
  if (map_.find(moduleName) != map_.end())
    throw exceptions::ModuleAlreadyExistsException(moduleName);

  ModuleConfiguration moduleConfiguration(configuration);
  map_.insert(std::make_pair(moduleName,moduleConfiguration));
}

ConfigurationManager::ModuleConfiguration
  ConfigurationManager::operator[](const std::string& moduleName) const
{
  ModuleConfigurationMap::const_iterator iter = map_.find(moduleName);
  if (iter == map_.end())
    throw exceptions::ModuleDoesNotExistException(moduleName);

  return iter->second;
}

bool ConfigurationManager::isInConfiguration(const std::string& moduleName,
                                             const std::string& key) const
{
  if (!doesModuleExist(moduleName))
    return false;

  ModuleConfiguration moduleConfiguration = operator[](moduleName);
  return moduleConfiguration.isInConfiguration(key);
}

bool ConfigurationManager::doesModuleExist(const std::string& moduleName) const
{
  ModuleConfigurationMap::const_iterator iter = map_.find(moduleName);
  if (iter == map_.end())
    return false;

  return true;
}

/******************************************************************************/

ConfigurationManager::KeyValueMap
  getConfigurationFromFile(const std::string& filename)
{
  ConfigurationManager::KeyValueMap result;
  std::ifstream file;
  file.open(filename);
  if (!file.good())
  {
    throw exceptions::ConfigurationFileCorruptedException(filename);
  }

  bpo::variables_map vm;
  bpo::options_description desc("Tracker parameters");
  desc.add_options()
      ("Model.AlignmentProcessor.TimeDelta", bpo::value<std::string>(),
        "Maximum time duration that detection reports can be spaced, "
        "to be assumed as taken in the same time. DT in seconds.")
      ("Model.DataAssociator.Threshold", bpo::value<std::string>(),
        "The minimum value of the DR->Track similarity grade, "
        "that allows to associate them. Allowed values - 0.0 - 1.0.")
      ("Model.TrackManager.InitializationThreshold", bpo::value<std::string>(),
        "Threshold indicates which DRs can be merged for the same track. "
        "It is based on Euclidean distance between detection reports. "
        "The closer DRs are - the higher rate is. "
        "Eg. when 1 DR is at distance of 1km from second (on X axis), "
        "it's approximately 0.01 longitude from it. The rating for such a DRs "
        "is 10000.")
      ("Model.ResultComparator.MaximumPositionRate", bpo::value<std::string>(),
        "Factor used to normalize result "
        "of comparation distance between DR and Track. "
        "Indicates maximum value of distance rate. "
        "If set to too little value only DRs with almost the same position "
        "as Tracks could be associated. "
        "This option is connected with Model.DataAssociator.Threshold")
      ("Model.ReportManager.PacketSize", bpo::value<std::string>(),
        "How many DRs obtain from DB at once.")
      ("Model.DataManager.TTL", bpo::value<std::string>(),
        "How long tracks are valid (not expired) without refreshing. "
        "Time is calculated from the latest track refresh time.")
      ("Controller.WorkMode", bpo::value<std::string>(),
        "batch - compute as fast as possible. When no more data is available, "
        "poll DB periodically to check for new data."
        "online - compute in real time.")
      ;

  bpo::store(bpo::parse_config_file(file, desc), vm);
  file.close();
  bpo::notify(vm);

  for (auto& i : vm)
  {
    result.insert(std::make_pair(i.first,i.second.as<std::string>()));
  }

  return result;
}

} // namespace Configuration

} // namespace Common
