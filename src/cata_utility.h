#pragma once
#ifndef CATA_SRC_CATA_UTILITY_H
#define CATA_SRC_CATA_UTILITY_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iosfwd>
#include <map>
#include <string> // IWYU pragma: keep
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

class JsonIn;
class JsonOut;
class translation;

/**
 * Greater-than comparison operator; required by the sort interface
 */
struct pair_greater_cmp_first {
    template< class T, class U >
    bool operator()( const std::pair<T, U> &a, const std::pair<T, U> &b ) const {
        return a.first > b.first;
    }

};

/**
 * For use with smart pointers when you don't actually want the deleter to do
 * anything.
 */
struct null_deleter {
    template<typename T>
    void operator()( T * ) const {}
};

/**
 * Type of object that a measurement is taken on.  Used, for example, to display wind speed in m/s
 * while displaying vehicle speed in km/h.
 */
enum units_type {
    VU_VEHICLE,
    VU_WIND
};

/**
 * Round a floating point value down to the nearest integer
 *
 * Optimized floor function, similar to std::floor but faster.
 */
inline int fast_floor( double v )
{
    return static_cast<int>( v ) - ( v < static_cast<int>( v ) );
}

/**
 * Round a value up at a given decimal place.
 *
 * @param val Value to be rounded.
 * @param dp Decimal place to round the value at.
 * @return Rounded value.
 */
double round_up( double val, unsigned int dp );

/** Divide @p num by @p den, rounding up
*
* @p num must be non-negative, @p den must be positive, and @c num+den must not overflow.
*/
template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
T divide_round_up( T num, T den )
{
    return ( num + den - 1 ) / den;
}

int divide_round_down( int a, int b );

/**
 * Determine whether a value is between two given boundaries.
 *
 * @param test Value to be tested.
 * @param down Lower boundary for value.
 * @param up Upper boundary for value.
 *
 * @return True if test value is greater than lower boundary and less than upper
 *         boundary, otherwise returns false.
 */
bool isBetween( int test, int down, int up );

/**
 * Perform case sensitive search for a query string inside a subject string.
 *
 * Searches for string given by qry inside a subject string given by str.
 *
 * @param str Subject to search for occurrence of the query string.
 * @param qry Query string to search for in str
 *
 * @return true if the query string is found at least once within the subject
 *         string, otherwise returns false
 */
bool lcmatch( const std::string &str, const std::string &qry );
bool lcmatch( const translation &str, const std::string &qry );

/**
 * Matches text case insensitive with the include/exclude rules of the filter
 *
 * Multiple includes/excludes are possible
 *
 * Examle: bank,-house,tank,-car
 * Will match text containing tank or bank while not containing house or car
 *
 * @param text String to be matched
 * @param filter String with include/exclude rules
 *
 * @return true if include/exclude rules pass. See Example.
 */
bool match_include_exclude( const std::string &text, std::string filter );

/**
 * Basic logistic function.
 *
 * Calculates the value at a single point on a standard logistic curve.
 *
 * @param t Point on logistic curve to retrieve value for
 *
 * @return Value of the logistic curve at the given point
 */
double logarithmic( double t );

/**
 * Normalized logistic function
 *
 * Generates a logistic curve on the domain [-6,6], then normalizes such that
 * the value ranges from 1 to 0.  A single point is then calculated on this curve.
 *
 * @param min t-value that should yield an output of 1 on the scaled curve.
 * @param max t-value that should yield an output of 0 on the scaled curve.
 * @param pos t-value to calculate the output for.
 *
 * @return The value of the scaled logistic curve at point pos.
 */
double logarithmic_range( int min, int max, int pos );

/**
 * Clamp the value of a modifier in order to bound the resulting value
 *
 * Ensures that a modifier value will not cause a base value to exceed given
 * bounds when applied.  If necessary, the given modifier value is increased or
 * reduced to meet this constraint.
 *
 * Giving a value of zero for min or max indicates that there is no minimum or
 * maximum boundary, respectively.
 *
 * @param val The base value that the modifier will be applied to
 * @param mod The desired modifier to be added to the base value
 * @param max The desired maximum value of the base value after modification, or zero.
 * @param min The desired minimum value of the base value after modification, or zero.
 *
 * @returns Value of mod, possibly altered to respect the min and max boundaries
 */
int bound_mod_to_vals( int val, int mod, int max, int min );

/**
 * Create a units label for a velocity value.
 *
 * Gives the name of the velocity unit in the user selected unit system, either
 * "km/h", "ms/s" or "mph".  Used to add abbreviated unit labels to the output of
 * @ref convert_velocity.
 *
 * @param vel_units type of velocity desired (i.e. wind or vehicle)
 *
 * @return name of unit.
 */
const char *velocity_units( units_type vel_units );

/**
 * Convert internal velocity units to units defined by user.
 *
 * @param velocity A velocity value in internal units.
 * @param vel_units General type of item this velocity is for (e.g. vehicles or wind)
 *
 * @returns Velocity in the user selected measurement system and in appropriate
 *          units for the object being measured.
 */
double convert_velocity( int velocity, units_type vel_units );

/**
 * Convert a temperature from degrees Fahrenheit to degrees Celsius.
 *
 * @return Temperature in degrees C.
 */
double temp_to_celsius( double fahrenheit );

/**
 * Convert a temperature from degrees Fahrenheit to Kelvin.
 *
 * @return Temperature in degrees K.
 */
double temp_to_kelvin( double fahrenheit );

/**
 * Convert a temperature from degrees Celsius to Kelvin.
 *
 * @return Temperature in degrees K.
 */
double celsius_to_kelvin( double celsius );

/**
 * Convert a temperature from Kelvin to degrees Fahrenheit.
 *
 * @return Temperature in degrees C.
 */
double kelvin_to_fahrenheit( double kelvin );

/**
 * Clamp (number and space wise) value to with,
 * taking into account the specified preferred scale,
 * returning the adjusted (shortened) scale that best fit the width,
 * optionally returning a flag that indicate if the value was truncated to fit the width
 */
/**@{*/
double clamp_to_width( double value, int width, int &scale );
double clamp_to_width( double value, int width, int &scale, bool *out_truncated );
/**@}*/

/**
 * Clamp first argument so that it is no lower than second and no higher than third.
 * Does not check if min is lower than max.
 */
template<typename T>
constexpr T clamp( const T &val, const T &min, const T &max )
{
    return std::max( min, std::min( max, val ) );
}

/**
 * Linear interpolation: returns first argument if t is 0, second if t is 1, otherwise proportional to t.
 * Does not clamp t, meaning it can return values lower than min (if t<0) or higher than max (if t>1).
 */
template<typename T>
constexpr T lerp( const T &min, const T &max, float t )
{
    return ( 1.0f - t ) * min + t * max;
}

/** Linear interpolation with t clamped to [0, 1] */
template<typename T>
constexpr T lerp_clamped( const T &min, const T &max, float t )
{
    return lerp( min, max, clamp( t, 0.0f, 1.0f ) );
}

/**
 * From `points`, finds p1 and p2 such that p1.first < x < p2.first
 * Then linearly interpolates between p1.second and p2.second and returns the result.
 * `points` should be sorted by first elements of the pairs.
 * If x is outside range, returns second value of the first (if x < points[0].first) or last point.
 */
float multi_lerp( const std::vector<std::pair<float, float>> &points, float x );

/**
 * @brief Class used to access a list as if it were circular.
 *
 * Some times you just want to have a list loop around on itself.
 * This wrapper class allows you to do that. It requires the list to exist
 * separately, but that also means any changes to the list get propagated (both ways).
 */
template<typename T>
class list_circularizer
{
    private:
        unsigned int _index = 0;
        std::vector<T> *_list;
    public:
        /** Construct list_circularizer from an existing std::vector. */
        explicit list_circularizer( std::vector<T> &_list ) : _list( &_list ) {
        }

        /** Advance list to next item, wrapping back to 0 at end of list */
        void next() {
            _index = ( _index == _list->size() - 1 ? 0 : _index + 1 );
        }

        /** Advance list to previous item, wrapping back to end at zero */
        void prev() {
            _index = ( _index == 0 ? _list->size() - 1 : _index - 1 );
        }

        /** Return list element at the current location */
        T &cur() const {
            // list could be null, but it would be a design time mistake and really, the callers fault.
            return ( *_list )[_index];
        }
};

/**
 * Open a file for writing, calls the writer on that stream.
 *
 * If the writer throws, or if the file could not be opened or if any I/O error
 * happens, the function shows a popup containing the
 * \p fail_message, the error text and the path.
 *
 * @return Whether saving succeeded (no error was caught).
 * @throw The void function throws when writing failes or when the @p writer throws.
 * The other function catches all exceptions and returns false.
 */
///@{
bool write_to_file( const std::string &path, const std::function<void( std::ostream & )> &writer,
                    const char *fail_message );
void write_to_file( const std::string &path, const std::function<void( std::ostream & )> &writer );
///@}

class JsonDeserializer;

/**
 * Try to open and read from given file using the given callback.
 *
 * The file is opened for reading (binary mode), given to the callback (which does the actual
 * reading) and closed.
 * Any exceptions from the callbacks are caught and reported as `debugmsg`.
 * If the stream is in a fail state (other than EOF) after the callback returns, it is handled as
 * error as well.
 *
 * The callback can either be a generic `std::istream`, a @ref JsonIn stream (which has been
 * initialized from the `std::istream`) or a @ref JsonDeserializer object (in case of the later,
 * it's `JsonDeserializer::deserialize` method will be invoked).
 *
 * The functions with the "_optional" prefix do not show a debug message when the file does not
 * exist. They simply ignore the call and return `false` immediately (without calling the callback).
 * They can be used for loading legacy files.
 *
 * @return `true` is the file was read without any errors, `false` upon any error.
 */
/**@{*/
bool read_from_file( const std::string &path, const std::function<void( std::istream & )> &reader );
bool read_from_file_json( const std::string &path, const std::function<void( JsonIn & )> &reader );
bool read_from_file( const std::string &path, JsonDeserializer &reader );

bool read_from_file_optional( const std::string &path,
                              const std::function<void( std::istream & )> &reader );
bool read_from_file_optional_json( const std::string &path,
                                   const std::function<void( JsonIn & )> &reader );
bool read_from_file_optional( const std::string &path, JsonDeserializer &reader );
/**@}*/

std::istream &safe_getline( std::istream &ins, std::string &str );

/** Apply fuzzy effect to a string like:
 * Hello, world! --> H##lo, wurl#!
 *
 * @param str the original string to be processed
 * @param f the function that guides how to mess the message
 * f() will be called for each character (lingual, not byte):
 * [-] f() == -1 : nothing will be done
 * [-] f() == 0  : the said character will be replace by a random character
 * [-] f() == ch : the said character will be replace by ch
 *
 * @return The processed string
 *
 */

std::string obscure_message( const std::string &str, const std::function<char()> &f );

/**
 * @group JSON (de)serialization wrappers.
 *
 * The functions here provide a way to (de)serialize objects without actually
 * including "json.h". The `*_wrapper` function create the JSON stream instances
 * and therefore require "json.h", but the caller doesn't. Callers should just
 * forward the stream reference to the actual (de)serialization function.
 *
 * The inline function do this by calling `T::(de)serialize` (which is assumed
 * to exist with the proper signature).
 *
 * @throws std::exception Deserialization functions may throw upon malformed
 * JSON or unexpected/invalid content.
 */
/**@{*/
std::string serialize_wrapper( const std::function<void( JsonOut & )> &callback );
void deserialize_wrapper( const std::function<void( JsonIn & )> &callback,
                          const std::string &data );

template<typename T>
inline std::string serialize( const T &obj )
{
    return serialize_wrapper( [&obj]( JsonOut & jsout ) {
        obj.serialize( jsout );
    } );
}

template<typename T>
inline void deserialize_from_string( T &obj, const std::string &data )
{
    deserialize_wrapper( [&obj]( JsonIn & jsin ) {
        obj.deserialize( jsin );
    }, data );
}
/**@}*/

/**
 * \brief Returns true iff s1 starts with s2
 */
bool string_starts_with( const std::string &s1, const std::string &s2 );

/**
 * Returns true iff s1 starts with s2.
 * This version accepts constant string literals and is ≈1.5 times faster than std::string version.
 * Note: N is (size+1) for null-terminated strings.
 */
template <std::size_t N>
inline bool string_starts_with( const std::string &s1, const char( &s2 )[N] )
{
    return s1.compare( 0, N - 1, s2, N - 1 ) == 0;
}

/**
 * \brief Returns true iff s1 ends with s2
 */
bool string_ends_with( const std::string &s1, const std::string &s2 );

/**
 *  Returns true iff s1 ends with s2.
 *  This version accepts constant string literals and is ≈1.5 times faster than std::string version.
 *  Note: N is (size+1) for null-terminated strings.
 */
template <std::size_t N>
inline bool string_ends_with( const std::string &s1, const char( &s2 )[N] )
{
    return s1.size() >= N - 1 && s1.compare( s1.size() - ( N - 1 ), std::string::npos, s2, N - 1 ) == 0;
}

/** Used as a default filter in various functions */
template<typename T>
bool return_true( const T & )
{
    return true;
}

/**
 * Joins a vector of `std::string`s into a single string with a delimiter/joiner
 */
std::string join( const std::vector<std::string> &strings, const std::string &joiner );

/**
 * Append all arguments after the first to the first.
 *
 * This provides a way to append several strings to a single root string
 * in a single line without an expression like 'a += b + c' which can cause an
 * unnecessary allocation in the 'b + c' expression.
 */
template<typename... T>
std::string &str_append( std::string &root, T &&...a )
{
    // Using initializer list as a poor man's fold expression until C++17.
    static_cast<void>(
    std::array<bool, sizeof...( T )> { {
            ( root.append( std::forward<T>( a ) ), false )...
        }
    } );
    return root;
}

/**
 * Concatenates a bunch of strings with append, to minimze unnecessary
 * allocations
 */
template<typename T0, typename... T>
std::string str_cat( T0 &&a0, T &&...a )
{
    std::string result( std::forward<T0>( a0 ) );
    return str_append( result, std::forward<T>( a )... );
}

/**
 * Erases elements from a set that match given predicate function.
 * Will work on vector, albeit not optimally performance-wise.
 * @return true if set was changed
 */
//bool erase_if( const std::function<bool( const value_type & )> &predicate ) {
template<typename Col, class Pred>
bool erase_if( Col &set, Pred predicate )
{
    bool ret = false;
    auto iter = set.begin();
    for( ; iter != set.end(); ) {
        if( predicate( *iter ) ) {
            iter = set.erase( iter );
            ret = true;
        } else {
            ++iter;
        }
    }
    return ret;
}

/**
 * Checks if two sets are equal, ignoring specified elements.
 * Works as if `ignored_elements` were temporarily erased from both sets before comparison.
 * @tparam Set type of the set (must be ordered, i.e. std::set, cata::flat_set)
 * @param set first set to compare
 * @param set2 second set to compare
 * @param ignored_elements elements from both sets to ignore
 * @return true, if sets without ignored elements are equal, false otherwise
 */
template<typename Set, typename T = std::decay_t<decltype( *std::declval<const Set &>().begin() )>>
bool equal_ignoring_elements( const Set &set, const Set &set2, const Set &ignored_elements )
{
    // general idea: splits both sets into the ranges bounded by elements from `ignored_elements`
    // and checks that these ranges are equal

    // traverses ignored elements in
    if( ignored_elements.empty() ) {
        return set == set2;
    }

    using Iter = typename Set::iterator;
    Iter end = ignored_elements.end();
    Iter cur = ignored_elements.begin();
    Iter prev = cur;
    cur++;

    // first comparing the sets range [set.begin() .. ignored_elements.begin()]
    if( !std::equal( set.begin(), set.lower_bound( *prev ),
                     set2.begin(), set2.lower_bound( *prev ) ) ) {
        return false;
    }

    // compare ranges bounded by two elements: [`prev` .. `cur`]
    while( cur != end ) {
        if( !std::equal( set.upper_bound( *prev ), set.lower_bound( *cur ),
                         set2.upper_bound( *prev ), set2.lower_bound( *cur ) ) ) {
            return false;
        }
        prev = cur;
        cur++;
    }

    // compare the range after the last element of ignored_elements: [ignored_elements.back() .. set.end()]
    return static_cast<bool>( std::equal( set.upper_bound( *prev ), set.end(),
                                          set2.upper_bound( *prev ), set2.end() ) );
}

/**
 * Return a copy of a std::map with some keys removed.
 */
template<typename K, typename V>
std::map<K, V> map_without_keys( const std::map<K, V> &original, const std::vector<K> &remove_keys )
{
    std::map<K, V> filtered( original );
    for( const K &key : remove_keys ) {
        filtered.erase( key );
    }
    return filtered;
}

int modulo( int v, int m );

class on_out_of_scope
{
    private:
        std::function<void()> func;
    public:
        explicit on_out_of_scope( const std::function<void()> &func ) : func( func ) {
        }

        on_out_of_scope( const on_out_of_scope & ) = delete;
        on_out_of_scope( on_out_of_scope && ) = delete;
        on_out_of_scope &operator=( const on_out_of_scope & ) = delete;
        on_out_of_scope &operator=( on_out_of_scope && ) = delete;

        ~on_out_of_scope() {
            if( func ) {
                func();
            }
        }

        void cancel() {
            func = nullptr;
        }
};

template<typename T>
class restore_on_out_of_scope
{
    private:
        T &t;
        T orig_t;
        on_out_of_scope impl;
    public:
        // *INDENT-OFF*
        explicit restore_on_out_of_scope( T &t_in ) : t( t_in ), orig_t( t_in ),
            impl( [this]() { t = std::move( orig_t ); } ) {
        }

        explicit restore_on_out_of_scope( T &&t_in ) : t( t_in ), orig_t( std::move( t_in ) ),
            impl( [this]() { t = std::move( orig_t ); } ) {
        }
        // *INDENT-ON*

        restore_on_out_of_scope( const restore_on_out_of_scope<T> & ) = delete;
        restore_on_out_of_scope( restore_on_out_of_scope<T> && ) = delete;
        restore_on_out_of_scope &operator=( const restore_on_out_of_scope<T> & ) = delete;
        restore_on_out_of_scope &operator=( restore_on_out_of_scope<T> && ) = delete;
};

/** Add elements from one set to another */
template <typename T>
std::unordered_set<T> &operator<<( std::unordered_set<T> &lhv, const std::unordered_set<T> &rhv )
{
    lhv.insert( rhv.begin(), rhv.end() );
    return lhv;
}

/** Move elements from one set to another */
template <typename T>
std::unordered_set<T> &operator<<( std::unordered_set<T> &lhv, std::unordered_set<T> &&rhv )
{
    for( const T &value : rhv ) {
        lhv.insert( std::move( value ) );
    }
    rhv.clear();
    return lhv;
}

#endif // CATA_SRC_CATA_UTILITY_H
