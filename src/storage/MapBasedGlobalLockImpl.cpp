#include "MapBasedGlobalLockImpl.h"

#include <mutex>

namespace Afina {
namespace Backend {

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Put(const std::string &key, const std::string &value) {
    std::unique_lock<std::mutex> guard(_lock);
    do
      {
        auto key_iter = _backend.find (key);
        if (key_iter != _backend.end ())
          {
            elements_order.remove (key);
            elements_order.push_back (key);
            _backend [key] = value;
            break;
          }

        while (_backend.size () >= _max_size)
          {
            _backend.erase (_backend.find (elements_order.front ()));
            elements_order.pop_front ();
          }

        _backend [key] = value;
        elements_order.push_back (key);
      } while (0);
    return true;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::PutIfAbsent(const std::string &key, const std::string &value) {
    std::unique_lock<std::mutex> guard(_lock);
    auto key_iter = _backend.find (key);
    if (key_iter == _backend.end ())
      {
        if (_backend.size () >= _max_size)
          {
            _backend.erase (_backend.find (elements_order.front ()));
            elements_order.pop_front ();
          }

        _backend [key] = value;
        elements_order.push_back (key);
        return true;
      }
    return false;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Set(const std::string &key, const std::string &value) {
    std::unique_lock<std::mutex> guard(_lock);
    if (_backend.find (key) != _backend.end ())
      {
        _backend[key] = value;
        return true;
      }
    return false;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Delete(const std::string &key) {
    std::unique_lock<std::mutex> guard(_lock);
    auto key_iter = _backend.find (key);
    if (key_iter != _backend.end ())
      {
        _backend.erase (key_iter);
        elements_order.remove (key);
//        _max_size--;
        return true;
      }
    return false;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Get(const std::string &key, std::string &value) const {
    std::unique_lock<std::mutex> guard(*const_cast<std::mutex *>(&_lock));
    if (_backend.find (key) != _backend.end ())
      {
        value = _backend.at (key);
        return true;
      }
    return false;
}

} // namespace Backend
} // namespace Afina
