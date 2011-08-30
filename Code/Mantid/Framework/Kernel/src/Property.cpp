#include "MantidKernel/Property.h"
#include "MantidKernel/Exception.h"
#include <string>
#include <sstream>
#include <utility>

namespace Mantid
{
namespace Kernel
{

/** Constructor
 *  @param name :: The name of the property
 *  @param type :: The type of the property
 *  @param direction :: Whether this is a Direction::Input, Direction::Output or Direction::InOut (Input & Output) property
 */
Property::Property( const std::string &name, const std::type_info &type, const unsigned int direction ) :
  m_name( name ),
  m_documentation( "" ),
  m_typeinfo( &type ),
  m_direction( direction ),
  m_units(""),
  m_settings(NULL),
  m_group("")
{
    // Make sure a random int hasn't been passed in for the direction
    // Property & PropertyWithValue destructors will be called in this case
    if (m_direction > 2) throw std::out_of_range("direction should be a member of the Direction enum");
}

/// Copy constructor
Property::Property( const Property& right ) :
  m_name( right.m_name ),
  m_documentation( right.m_documentation ),
  m_typeinfo( right.m_typeinfo ),
  m_direction( right.m_direction ),
  m_units( right.m_units),
  m_settings(NULL),
  m_group( right.m_group)
{
  if (right.m_settings)
    m_settings = right.m_settings->clone();
}

/// Virtual destructor
Property::~Property()
{
  if (m_settings)
    delete m_settings;
}

/** Copy assignment operator. Does nothing.
* @param right :: The right hand side value
* @return pointer to this
*/
//Property& Property::operator=( const Property& right )
//{
//  UNUSED_ARG(right);
//  return *this;
//}

/** Get the property's name
 *  @return The name of the property
 */
const std::string& Property::name() const
{
  return m_name;
}

/** Get the property's documentation string
 *  @return The documentation string
 */
const std::string& Property::documentation() const
{
  return m_documentation;
}

/** Get the property type_info
 *  @return The type of the property
 */
const std::type_info* Property::type_info() const
{
  return m_typeinfo;
}

/** Returns the type of the property as a string.
 *  Note that this is implementation dependent.
 *  @return The property type
 */
const std::string Property::type() const
{
  return Mantid::Kernel::getUnmangledTypeName(*m_typeinfo);
}

/** Overridden functions checks whether the property has a valid value.
 *  
 *  @return empty string ""
 */
std::string Property::isValid() const
{
  // the no error condition
  return "";
}
/**
* Whether to remember this property input
* @return whether to remember this property's input
*/
bool Property::remember() const
{
  return true;
}

/** Sets the property's (optional) documentation string
 *  @param documentation :: The string containing the descriptive comment
 */
void Property::setDocumentation( const std::string& documentation )
{
  m_documentation = documentation;
}

/** Returns the set of valid values for this property, if such a set exists.
 *  If not, it returns an empty set.
 * @return the set of valid values for this property or an empty set
 */
std::set<std::string> Property::allowedValues() const
{
  return std::set<std::string>();
}

/// Create a PropertyHistory object representing the current state of the Property.
const PropertyHistory Property::createHistory() const
{
  return PropertyHistory(this->name(),this->value(),this->type(),this->isDefault(),this->direction());
}

//-------------------------------------------------------------------------------------------------
/** Return the size of this property.
 * Single-Value properties return 1.
 * TimeSeriesProperties return the # of entries.
 * @return the size of the property
 */
int Property::size() const
{
  return 1;
}

//-------------------------------------------------------------------------------------------------
/** Returns the units of the property, if any, as a string.
 * Units are optional, and will return empty string if they have
 * not been set before.
 * @return the property's units
 */
std::string Property::units() const
{
  return m_units;
}

//-------------------------------------------------------------------------------------------------
/** Sets the units of the property, as a string. This is optional.
 *
 * @param unit :: string to set for the units.
 */
void Property::setUnits(std::string unit)
{
  m_units = std::string(unit); //force the copy constructor
}

//-------------------------------------------------------------------------------------------------
/** Filter out a property by time. Will be overridden by TimeSeriesProperty (only)
 * @param start :: the beginning time to filter from
 * @param stop :: the ending time to filter to
 * */
void Property::filterByTime(const Kernel::DateAndTime start, const Kernel::DateAndTime stop)
{
  UNUSED_ARG(start);
  UNUSED_ARG(stop);
  //Do nothing in general
  return;
}


//-----------------------------------------------------------------------------------------------
/** Split a property by time. Will be overridden by TimeSeriesProperty (only)
 * For any other property type, this does nothing.
 * @param splitter :: time splitter
 * @param outputs :: holder for splitter output
 */
void Property::splitByTime(TimeSplitterType& splitter, std::vector< Property * > outputs) const
{
  UNUSED_ARG(splitter);
  UNUSED_ARG(outputs);
  return;
}

} // End Kernel namespace


//-------------------------- Utility function for class name lookup -----------------------------

// MG 16/07/09: Some forward declarations.I need this so
// that the typeid function in getUnmangledTypeName knows about them
// This way I don't need to actually include the headers and I don't
// introduce unwanted dependencies
namespace API
{
  class Workspace;
  class MatrixWorkspace;
  class ITableWorkspace;
  class IMDEventWorkspace;
  class IMDWorkspace;
  class IEventWorkspace;
}
namespace DataObjects
{
  class EventWorkspace;
  class PeaksWorkspace;
  class GroupingWorkspace;
  class OffsetsWorkspace;
  class SpecialWorkspace2D;
  class Workspace2D;
}
namespace MDDataObjects
{
  class MDWorkspace;
}

namespace Kernel
{
/**
 * Get the unmangled name of the given typestring for some common types that we use. Note that
 * this is just a lookup and NOT an unmangling algorithm
 * @param type :: A pointer to the type_info object for this type
 * @returns An unmangled version of the name
 */
std::string getUnmangledTypeName(const std::type_info& type)
{
  using std::string;
  using std::make_pair;
  using namespace Mantid::API;
  using namespace Mantid::DataObjects;
  using namespace Mantid::MDDataObjects;
  // Compile a lookup table. This is a static local variable that
  // will get initialized when the function is first used
  static std::map<string, string> typestrings;
  if( typestrings.empty() ) 
  {
    typestrings.insert(make_pair(typeid(char).name(), string("letter")));
    typestrings.insert(make_pair(typeid(int).name(), string("number")));
    typestrings.insert(make_pair(typeid(long long).name(), string("number")));
    typestrings.insert(make_pair(typeid(double).name(), string("number")));
    typestrings.insert(make_pair(typeid(bool).name(), string("boolean")));
    typestrings.insert(make_pair(typeid(string).name(), string("string")));
    typestrings.insert(make_pair(typeid(std::vector<string>).name(), string("str list")));
    typestrings.insert(make_pair(typeid(std::vector<int>).name(), string("int list")));
    typestrings.insert(make_pair(typeid(std::vector<double>).name(), string("dbl list")));

    //Workspaces
    typestrings.insert(make_pair(typeid(boost::shared_ptr<Workspace>).name(), 
                                      string("Workspace")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<MatrixWorkspace>).name(), 
                                      string("MatrixWorkspace")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<ITableWorkspace>).name(), 
                                      string("TableWorkspace")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<IMDWorkspace>).name(), 
                                      string("IMDWorkspace")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<IMDEventWorkspace>).name(), 
                                      string("MDEventWorkspace")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<IEventWorkspace>).name(), 
                                      string("IEventWorkspace")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<Workspace2D>).name(), 
                                      string("Workspace2D")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<EventWorkspace>).name(), 
                                      string("EventWorkspace")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<PeaksWorkspace>).name(), 
                                      string("PeaksWorkspace")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<GroupingWorkspace>).name(), 
                                      string("GroupingWorkspace")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<OffsetsWorkspace>).name(), 
                                      string("OffsetsWorkspace")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<SpecialWorkspace2D>).name(), 
                                      string("SpecialWorkspace2D")));
    typestrings.insert(make_pair(typeid(boost::shared_ptr<MDWorkspace>).name(), 
                                      string("MDWorkspace")));


  }
  std::map<std::string, std::string>::const_iterator mitr = typestrings.find(type.name());
  if( mitr != typestrings.end() )
  {
    return mitr->second;
  }

  return type.name();

}

} // namespace Kernel

} // namespace Mantid
