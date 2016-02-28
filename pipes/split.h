#include <sstream>
#include <string>

#include "trim.h"

struct s_split
{
  enum empties_t { empties_ok, no_empties };
};

template <typename Container>
Container& split(
  Container&                                 result,
  const typename Container::value_type&      s,
  typename Container::value_type::value_type delimiter,
  s_split::empties_t                           empties = s_split::empties_ok )
{
  result.clear();
  std::istringstream ss( s );
  while (!ss.eof())
  {
    typename Container::value_type field;
    getline( ss, field, delimiter );
    if ((empties == s_split::no_empties) && field.empty()) continue;
    field = trim_copy(field);
    result.push_back( field );
  }
  return result;
}