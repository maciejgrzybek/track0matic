#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <boost/lexical_cast.hpp>

#include <Common/logger.h>

namespace Common
{

namespace Configuration
{

namespace exceptions
{

class KeyDoesNotExistException : public std::exception
{
public:
  KeyDoesNotExistException(const std::string& keyName);
  virtual ~KeyDoesNotExistException() throw() = default;

  virtual const char* what() const throw();

  std::string getKeyName() const;

private:
  const std::string keyName_;
  const std::string message_;
};

class ModuleDoesNotExistException : public std::exception
{
public:
  ModuleDoesNotExistException(const std::string& moduleName);
  virtual ~ModuleDoesNotExistException() throw() = default;

  virtual const char* what() const throw();

  std::string getModuleName() const;

private:
  const std::string moduleName_;
  const std::string message_;
};

class ModuleAlreadyExistsException : public std::exception
{
public:
  ModuleAlreadyExistsException(const std::string& moduleName);
  virtual ~ModuleAlreadyExistsException() throw() = default;

  virtual const char* what() const throw();

  std::string getModuleName() const;

private:
  const std::string moduleName_;
  const std::string message_;
};

class ConfigurationFileCorruptedException : public std::exception
{
public:
  ConfigurationFileCorruptedException(const std::string& filename);
  virtual ~ConfigurationFileCorruptedException() throw() = default;

  virtual const char* what() const throw();

  std::string getFileName() const;

private:
  const std::string fileName_;
  const std::string message_;
};

} // namespace exceptions

class ConfigurationManager
{
public:
  typedef std::unordered_map<std::string,std::string> KeyValueMap;

  class ModuleConfiguration
  {
  public:
    // moves map
    explicit ModuleConfiguration(KeyValueMap&);

    /**
     * @brief Returns value corresponding to given key.
     * @param key
     * @return value
     * @throw exceptions::KeyDoesNotExistException
     */
    std::string operator[](const std::string& key);

    /**
     * @brief Indicates whether given key exists in configuration map.
     * @param key
     * @return true - when value exists, false otherwise
     */
    bool isInConfiguration(const std::string& key);

    std::string toString() const;

  private:
    KeyValueMap map_;
  };

  static ConfigurationManager& getInstance();

  template <typename T>
  static T getCastedValue(const std::string& moduleName, const std::string& key,
                          const T& defaultValue)
  {
    ConfigurationManager& confMan = ConfigurationManager::getInstance();
    const std::string value = confMan[moduleName][key];
    T result = defaultValue;
    try
    {
      result = boost::lexical_cast<T>(value);
    }
    catch (const boost::bad_lexical_cast&)
    {
      Common::GlobalLogger& logger = Common::GlobalLogger::getInstance();
      std::stringstream msg;
      msg << "Unable to cast " << moduleName << "." << key << " value "
          << "\"" << value << "\", using default - " << result;
      logger.log("ConfigurationManager",msg.str());
    }

    return result;
  }

  std::string toString() const;

  // moves map
  void parseKeyValueMapIntoConfiguration(KeyValueMap& configuration);

  // moves map
  void addConfigurationForModule(const std::string& moduleName,
                                 KeyValueMap& configuration);

  /**
   * @brief Returns ModuleConfiguration - proxy for configuration per module.
   * @param module name
   * @return ModuleConfiguration
   */
  ModuleConfiguration operator[](const std::string& moduleName) const;

  /**
   * @brief Checks whether key exists in configuration of given module.
   *
   * This method is not reliable in multithreaded environment,
   * because of non-atomic checks.
   * Checks are performed not in the same "transaction".
   * @param module name
   * @param key
   * @return true or false
   */
  bool isInConfiguration(const std::string& moduleName,
                         const std::string& key) const;

  bool doesModuleExist(const std::string& moduleName) const;

private:
  typedef std::unordered_map<std::string,
                             ModuleConfiguration> ModuleConfigurationMap;

  ConfigurationManager() = default;
  ~ConfigurationManager() = default; // nobody can delete this
  ConfigurationManager(const ConfigurationManager&) = delete;

  ModuleConfigurationMap map_;
};

ConfigurationManager::KeyValueMap
  getConfigurationFromFile(const std::string& filename);

} // namespace Configuration

} // namespace Common

#endif // CONFIGURATION_MANAGER_H
