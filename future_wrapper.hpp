// necessary since in c++11 >=, future is already available in std
#define BOOST_THREAD_PROVIDES_FUTURE
// be able to use .then and other goodies
#define BOOST_THREAD_VERSION 4
#include <boost/thread/future.hpp>

using boost::future;
using boost::future_status;
using boost::promise;
