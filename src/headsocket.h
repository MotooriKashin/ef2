/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

***** HeadSocket v0.1, created by Jan Pinter **** Minimalistic header only WebSocket server implementation in C++ *****
                    Sources: https://github.com/P-i-N/HeadSocket, contact: Pinter.Jan@gmail.com
                     PUBLIC DOMAIN - no warranty implied or offered, use this at your own risk

-----------------------------------------------------------------------------------------------------------------------

Usage:
- use this as a regular header file, but in EXACTLY one of your C++ files (ie. main.cpp) you must define
  HEADSOCKET_IMPLEMENTATION beforehand, like this:

    #define HEADSOCKET_IMPLEMENTATION
    #include <headsocket.h>

/*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __HEADSOCKET_H__
#define __HEADSOCKET_H__

#include <memory>
#include <string>
#include <functional>
#include <map>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace headsocket
{

  /* Forward declarations */
  class connection;
  class basic_tcp_server;
  class basic_tcp_client;
  class tcp_client;
  class async_tcp_client;

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  template <typename T>
  using ptr = std::shared_ptr<T>;
  typedef size_t id_t;

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  namespace detail
  {

    /* Forward declarations */
    struct connection_impl;
    struct basic_tcp_server_impl;
    struct basic_tcp_client_impl;
    struct async_tcp_client_impl;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static bool handshake_websocket(connection &conn);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct less_comparator : std::binary_function<std::string, std::string, bool>
    {
      bool operator()(const std::string &s1, const std::string &s2) const
      {
        return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), [](char c1, char c2) -> bool
                                            { return tolower(c1) < tolower(c2); });
      }
    };

  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class connection
  {
  public:
    connection(const detail::connection_impl &impl);
    ~connection();

    detail::connection_impl *impl() const { return _p.get(); }

    bool is_valid() const;
    id_t id() const;

    size_t write(const void *ptr, size_t length);
    size_t write(const std::string &text) { return write(text.c_str(), text.length()); }
    size_t read(void *ptr, size_t length);

    bool force_write(const void *ptr, size_t length);
    bool force_read(void *ptr, size_t length);

    bool read_line(std::string &output);

  private:
    std::unique_ptr<detail::connection_impl> _p;
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  enum class opcode
  {
    continuation = 0x00,
    text = 0x01,
    binary = 0x02,
    connection_close = 0x08,
    ping = 0x09,
    pong = 0x0A
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  struct data_block
  {
    opcode op;
    size_t offset;
    size_t length = 0;
    bool is_completed = false;

    data_block(opcode opc, size_t off)
        : op(opc), offset(off)
    {
    }
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class basic_tcp_server : public std::enable_shared_from_this<basic_tcp_server>
  {
  public:
    int port() const;
    void stop();
    bool is_running() const;
    bool disconnect(ptr<basic_tcp_client> client);
    bool disconnect(id_t id);

  protected:
    struct protected_tag
    {
    };
    void init() {}

    explicit basic_tcp_server(int port);
    virtual ~basic_tcp_server();

    virtual bool handshake(connection &conn) = 0;
    virtual ptr<basic_tcp_client> accept(connection &conn) = 0;
    virtual void client_connected(ptr<basic_tcp_client> client) = 0;
    virtual void client_disconnected(ptr<basic_tcp_client> client) = 0;

    std::unique_ptr<detail::basic_tcp_server_impl> _p;

  private:
    template <typename T>
    friend class tcp_server;

    void remove_disconnected() const;

    size_t acquire_clients() const;
    void release_clients() const;

    ptr<basic_tcp_client> client_at(size_t index) const;
    size_t num_clients() const;

    void accept_thread();
    void disconnect_thread();
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HEADSOCKET_SERVER(className, baseClassName)                                                                 \
protected:                                                                                                          \
  explicit className(int port) : baseClassName(port) { init(); }                                                    \
                                                                                                                    \
public:                                                                                                             \
  typedef baseClassName base_t;                                                                                     \
  className(const protected_tag &, int port) : className(port) {}                                                   \
  static headsocket::ptr<className> create(int port) { return std::make_shared<className>(protected_tag{}, port); } \
                                                                                                                    \
protected:                                                                                                          \
  void init()

  template <typename T>
  class tcp_server : public basic_tcp_server
  {
    HEADSOCKET_SERVER(tcp_server, basic_tcp_server) {}

  public:
    typedef T client_t;
    typedef ptr<client_t> client_ptr;

    virtual ~tcp_server()
    {
      base_t::stop();
    }

    class enumerator
    {
    public:
      explicit enumerator(const tcp_server &server)
          : _server(server), _count(server.acquire_clients())
      {
      }

      ~enumerator()
      {
        _server.release_clients();
      }

      const tcp_server &server() const { return _server; }
      size_t size() const { return _count; }

      struct iterator
      {
        enumerator &e;
        size_t index;

        iterator(enumerator &enu, size_t idx)
            : e(enu), index(idx)
        {
        }

        bool operator==(const iterator &iter) const { return iter.index == index && &iter.e == &e; }
        bool operator!=(const iterator &iter) const { return iter.index != index || &iter.e != &e; }
        ptr<T> operator*() const { return std::dynamic_pointer_cast<T>(e.server().client_at(index)); }

        iterator &operator++()
        {
          ++index;
          return *this;
        }
      };

      iterator begin() { return iterator(*this, 0); }
      iterator end() { return iterator(*this, _count); }

    private:
      const tcp_server &_server;
      size_t _count;
    };

    enumerator clients() const { return enumerator(*this); }

  protected:
    bool handshake(connection &conn) override { return true; }

    virtual void client_connected(client_ptr client) {}

    virtual void client_disconnected(client_ptr client) {}

  private:
    enum
    {
      needs_basic_tcp_client = T::is_basic_tcp_client
    };

    ptr<basic_tcp_client> accept(connection &conn) override
    {
      ptr<basic_tcp_client> newClient = T::create(shared_from_this(), conn);
      return newClient->is_connected() ? newClient : nullptr;
    }

    void client_connected(ptr<basic_tcp_client> client) override
    {
      client_connected(std::dynamic_pointer_cast<T>(client));
    }

    void client_disconnected(ptr<basic_tcp_client> client) override
    {
      client_disconnected(std::dynamic_pointer_cast<T>(client));
    }
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class basic_tcp_client
  {
  public:
    enum
    {
      is_basic_tcp_client
    };

    static const size_t invalid_operation = static_cast<size_t>(-1);

    virtual ~basic_tcp_client();

    bool disconnect();
    bool is_connected() const;

    ptr<basic_tcp_server> server() const;
    id_t id() const;

  protected:
    struct protected_tag
    {
    };

    friend class basic_tcp_server;

    basic_tcp_client(const std::string &address, int port);
    basic_tcp_client(ptr<basic_tcp_server> server, connection &conn);

    std::unique_ptr<detail::basic_tcp_client_impl> _p;
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __HEADSOCKET_CLIENT_STATIC_CTORS(className)                                                                                                      \
  className(const protected_tag &, const std::string &address, int port) : className(address, port) {}                                                   \
  className(const protected_tag &, headsocket::ptr<headsocket::basic_tcp_server> server, headsocket::connection &conn) : className(server, conn) {}      \
  static headsocket::ptr<className> create(const std::string &address, int port) { return std::make_shared<className>(protected_tag{}, address, port); } \
  static headsocket::ptr<className> create(headsocket::ptr<headsocket::basic_tcp_server> server, headsocket::connection &conn) { return std::make_shared<className>(protected_tag{}, server, conn); }

#define HEADSOCKET_CLIENT_BASE(className)                    \
protected:                                                   \
  className(const std::string &address, int port);           \
  className(ptr<basic_tcp_server> server, connection &conn); \
                                                             \
public:                                                      \
  __HEADSOCKET_CLIENT_STATIC_CTORS(className)

#define HEADSOCKET_CLIENT(className, baseClassName)                                                                              \
protected:                                                                                                                       \
  className(const std::string &address, int port) : baseClassName(address, port) {}                                              \
  className(headsocket::ptr<headsocket::basic_tcp_server> server, headsocket::connection &conn) : baseClassName(server, conn) {} \
                                                                                                                                 \
public:                                                                                                                          \
  __HEADSOCKET_CLIENT_STATIC_CTORS(className)

  class tcp_client : public basic_tcp_client
  {
    HEADSOCKET_CLIENT_BASE(tcp_client);

  public:
    typedef basic_tcp_client base_t;
    enum
    {
      is_tcp_client
    };

    virtual ~tcp_client();

    virtual size_t write(const void *ptr, size_t length);
    virtual size_t read(void *ptr, size_t length);

    bool force_write(const void *ptr, size_t length);
    bool force_read(void *ptr, size_t length);

    bool read_line(std::string &output);
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class async_tcp_client : public basic_tcp_client
  {
    HEADSOCKET_CLIENT_BASE(async_tcp_client);

  public:
    typedef basic_tcp_client base_t;
    enum
    {
      is_async_tcp_client
    };

    virtual ~async_tcp_client();

    void push(const void *ptr, size_t length);
    void push(const std::string &text);
    size_t peek() const;
    size_t pop(void *ptr, size_t length);

  protected:
    virtual void init_threads();
    virtual size_t async_write_handler(uint8_t *ptr, size_t length);
    virtual size_t async_read_handler(uint8_t *ptr, size_t length);

    virtual bool async_received_data(const data_block &db, uint8_t *ptr, size_t length) { return false; }

    virtual void push(const void *ptr, size_t length, opcode opcode);

    void kill_threads();

    std::unique_ptr<detail::async_tcp_client_impl> _ap;

  private:
    void write_thread();
    void read_thread();
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class web_socket_client : public async_tcp_client
  {
    HEADSOCKET_CLIENT_BASE(web_socket_client);

  public:
    static const size_t frame_size_limit = 128 * 1024;

    typedef async_tcp_client base_t;
    enum
    {
      is_web_socket_client
    };

    virtual ~web_socket_client();

    size_t peek(opcode *op) const;

  protected:
    size_t async_write_handler(uint8_t *ptr, size_t length) override;
    size_t async_read_handler(uint8_t *ptr, size_t length) override;

  private:
    struct frame_header
    {
      bool fin;
      opcode op;
      bool masked;
      size_t payload_length;
      uint32_t masking_key;

      size_t write(uint8_t *ptr, size_t length) const;
      size_t read(const uint8_t *ptr, size_t length);
    };

    size_t _payload_size = 0;
    frame_header _current_header;
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  template <typename T>
  class web_socket_server : public tcp_server<T>
  {
    HEADSOCKET_SERVER(web_socket_server, tcp_server<T>) {}

  public:
    virtual ~web_socket_server()
    {
      base_t::stop();
    }

  protected:
    bool handshake(connection &conn) override { return detail::handshake_websocket(conn); }

  private:
    enum
    {
      needs_web_socket_client = T::is_web_socket_client
    };
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  class http_server : public tcp_server<tcp_client>
  {
    HEADSOCKET_SERVER(http_server, tcp_server<tcp_client>);

  public:
    ~http_server()
    {
      stop();
    }

    struct response
    {
      std::string content_type = "text/html";
      std::string message = "";
    };

    struct parameter
    {
      std::string name;
      std::string value;
      bool boolean;
      int integer;
      double real;
    };

    typedef std::map<std::string, parameter, detail::less_comparator> parameters_t;

  protected:
    virtual bool request(const std::string &path, const parameters_t &params, response &resp) { return false; }

  private:
    bool handshake(connection &conn) final override;

    ptr<basic_tcp_client> accept(connection &conn) final override { return nullptr; }
    void client_connected(client_ptr client) final override {}
    void client_disconnected(client_ptr client) final override {}
  };

}

#endif // __HEADSOCKET_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef HEADSOCKET_IMPLEMENTATION
#ifndef __HEADSOCKET_H_IMPL__
#define __HEADSOCKET_H_IMPL__

#include <thread>
#include <atomic>
#include <iomanip>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADSOCKET_PLATFORM_OVERRIDE
#ifdef _WIN32
#define HEADSOCKET_PLATFORM_WINDOWS
#elif __ANDROID__
#define HEADSOCKET_PLATFORM_ANDROID
#define HEADSOCKET_PLATFORM_NIX
#elif __APPLE__
#include "TargetConditionals.h"
#ifdef TARGET_OS_MAC
#define HEADSOCKET_PLATFORM_MAC
#endif
#elif __linux
#define HEADSOCKET_PLATFORM_NIX
#elif __unix
#define HEADSOCKET_PLATFORM_NIX
#elif __posix
#define HEADSOCKET_PLATFORM_NIX
#else
#error Unsupported platform!
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(HEADSOCKET_PLATFORM_WINDOWS)
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#elif defined(HEADSOCKET_PLATFORM_ANDROID) || defined(HEADSOCKET_PLATFORM_NIX)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <netdb.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HEADSOCKET_LOCK_SUFFIX(var, suffix) std::lock_guard<decltype(var)> __scope_lock##suffix(var);
#define HEADSOCKET_LOCK_SUFFIX2(var, suffix) HEADSOCKET_LOCK_SUFFIX(var, suffix)
#define HEADSOCKET_LOCK(var) HEADSOCKET_LOCK_SUFFIX2(var, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace headsocket
{

  namespace detail
  {
#if defined(HEADSOCKET_PLATFORM_WINDOWS)
    typedef SOCKET socket_type;
    static const int socket_error = SOCKET_ERROR;
    static const SOCKET invalid_socket = INVALID_SOCKET;
    void close_socket(socket_type s) { closesocket(s); }
#define HEADSOCKET_SPRINTF sprintf_s
#elif defined(HEADSOCKET_PLATFORM_ANDROID) || defined(HEADSOCKET_PLATFORM_NIX)
    typedef int socket_type;
    static const int socket_error = -1;
    static const int invalid_socket = -1;
    void close_socket(socket_type s) { close(s); }
#define HEADSOCKET_SPRINTF sprintf
#endif
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  namespace detail
  {

    class sha1
    {
    public:
      typedef uint32_t digest32_t[5];
      typedef uint8_t digest8_t[20];

      inline static uint32_t rotate_left(uint32_t value, size_t count) { return (value << count) ^ (value >> (32 - count)); }

      sha1()
      {
        _digest[0] = 0x67452301;
        _digest[1] = 0xEFCDAB89;
        _digest[2] = 0x98BADCFE;
        _digest[3] = 0x10325476;
        _digest[4] = 0xC3D2E1F0;
      }

      ~sha1()
      {
      }

      void process_byte(uint8_t octet)
      {
        _block[_block_byte_index++] = octet;
        ++_byte_count;

        if (_block_byte_index == 64)
        {
          _block_byte_index = 0;
          process_block();
        }
      }

      void process_block(const void *start, const void *end)
      {
        const uint8_t *begin = static_cast<const uint8_t *>(start);

        while (begin != end)
          process_byte(*begin++);
      }

      void process_bytes(const void *data, size_t len)
      {
        process_block(data, static_cast<const uint8_t *>(data) + len);
      }

      const uint32_t *get_digest(digest32_t digest)
      {
        size_t bitCount = _byte_count * 8;
        process_byte(0x80);

        if (_block_byte_index > 56)
        {
          while (_block_byte_index != 0)
            process_byte(0);

          while (_block_byte_index < 56)
            process_byte(0);
        }
        else
          while (_block_byte_index < 56)
            process_byte(0);

        process_byte(0);
        process_byte(0);
        process_byte(0);
        process_byte(0);

        for (int i = 24; i >= 0; i -= 8)
          process_byte(static_cast<unsigned char>((bitCount >> i) & 0xFF));

        memcpy(digest, _digest, 5 * sizeof(uint32_t));
        return digest;
      }

      const uint8_t *get_digest_bytes(digest8_t digest)
      {
        digest32_t d32;
        get_digest(d32);
        size_t s[] = {24, 16, 8, 0};

        for (size_t i = 0, j = 0; i < 20; ++i, j = i % 4)
          digest[i] = ((d32[i >> 2] >> s[j]) & 0xFF);

        return digest;
      }

    private:
      void process_block()
      {
        uint32_t w[80], s[] = {24, 16, 8, 0};

        for (size_t i = 0, j = 0; i < 64; ++i, j = i % 4)
          w[i / 4] = j ? (w[i / 4] | (_block[i] << s[j])) : (_block[i] << s[j]);

        for (size_t i = 16; i < 80; i++)
          w[i] = rotate_left((w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]), 1);

        digest32_t dig = {_digest[0], _digest[1], _digest[2], _digest[3], _digest[4]};

        for (size_t f, k, i = 0; i < 80; ++i)
        {
          if (i < 20)
            f = (dig[1] & dig[2]) | (~dig[1] & dig[3]), k = 0x5A827999;
          else if (i < 40)
            f = dig[1] ^ dig[2] ^ dig[3], k = 0x6ED9EBA1;
          else if (i < 60)
            f = (dig[1] & dig[2]) | (dig[1] & dig[3]) | (dig[2] & dig[3]), k = 0x8F1BBCDC;
          else
            f = dig[1] ^ dig[2] ^ dig[3], k = 0xCA62C1D6;

          uint32_t temp = rotate_left(dig[0], 5) + f + dig[4] + k + w[i];
          dig[4] = dig[3];
          dig[3] = dig[2];
          dig[2] = rotate_left(dig[1], 30);
          dig[1] = dig[0];
          dig[0] = temp;
        }

        for (size_t i = 0; i < 5; ++i)
          _digest[i] += dig[i];
      }

      digest32_t _digest;
      uint8_t _block[64];
      size_t _block_byte_index = 0;
      size_t _byte_count = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct utils
    {
      static std::string base64_encode(const void *ptr, size_t length)
      {
        static const char *encoding_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        static size_t mod_table[] = {0, 2, 1};

        std::string result(4 * ((length + 2) / 3), '=');

        if (ptr && length)
        {
          const uint8_t *input = reinterpret_cast<const uint8_t *>(ptr);

          for (size_t i = 0, j = 0, triplet = 0; i < length; triplet = 0)
          {
            for (size_t k = 0; k < 3; ++k)
              triplet = (triplet << 8) | (i < length ? static_cast<uint8_t>(input[i++]) : 0);

            for (size_t k = 4; k--;)
              result[j++] = encoding_table[(triplet >> k * 6) & 0x3F];
          }

          for (size_t i = 0; i < mod_table[length % 3]; i++)
            result[result.length() - 1 - i] = '=';
        }

        return result;
      }

      static size_t xor32(uint32_t key, void *ptr, size_t length)
      {
        uint8_t *data = reinterpret_cast<uint8_t *>(ptr);
        uint8_t *mask = reinterpret_cast<uint8_t *>(&key);

        for (size_t i = 0; i < length; ++i, ++data)
          *data = (*data) ^ mask[i % 4];

        return length;
      }

      static std::string url_encode(const std::string &str)
      {
        std::ostringstream result;
        result.fill('0');
        result << std::hex;

        for (std::string::const_iterator i = str.begin(), n = str.end(); i != n; ++i)
        {
          auto c = (*i);

          if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
            result << c;
          else
            result << '%' << std::setw(2) << static_cast<int>(c);
        }

        return result.str();
      }

      static std::string url_decode(const std::string &str)
      {
        std::ostringstream result;

        for (size_t i = 0, S = str.length(); i < S; ++i)
        {
          auto c = str[i];

          if (c == '%')
          {
            char hexBuff[3] = {0, 0, 0};
            hexBuff[0] = str[++i];
            hexBuff[1] = str[++i];

            int value;
            sscanf(hexBuff, "%x", &value);
            result << static_cast<char>(value);
          }
          else if (c == '+')
            result << ' ';
          else
            result << c;
        }

        return result.str();
      }

      static uint16_t swap16bits(uint16_t x) { return ((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8); }

      static uint32_t swap32bits(uint32_t x)
      {
        return ((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) | ((x & 0x00FF0000) >> 8) | ((x & 0xFF000000) >> 24);
      }

      static uint64_t swap64bits(uint64_t x)
      {
        return ((x & 0x00000000000000FFULL) << 56) | ((x & 0x000000000000FF00ULL) << 40) | ((x & 0x0000000000FF0000ULL) << 24) |
               ((x & 0x00000000FF000000ULL) << 8) | ((x & 0x000000FF00000000ULL) >> 8) | ((x & 0x0000FF0000000000ULL) >> 24) |
               ((x & 0x00FF000000000000ULL) >> 40) | ((x & 0xFF00000000000000ULL) >> 56);
      }

      static std::string trim(const std::string &str)
      {
        size_t trimLeft = 0, trimRight = str.length() - 1;

        while (trimLeft < str.length() && isspace(str[trimLeft]))
          ++trimLeft;

        while (trimRight < str.length() && isspace(str[trimRight]))
          --trimRight;

        return (trimRight >= str.length() || trimLeft >= str.length() || trimRight < trimLeft)
                   ? std::string("")
                   : str.substr(trimLeft, trimRight - trimLeft + 1);
      }

      static std::string cut_front(std::string &str, char delimiter = ' ', bool first = true, bool hungry = true)
      {
        std::string result;

        auto pos = first ? str.find(delimiter) : str.rfind(delimiter);
        if (pos == std::string::npos)
        {
          if (hungry)
          {
            result = str;
            str = "";
          }
        }
        else
        {
          result = str.substr(0, pos);
          str = str.substr(pos + 1);
        }

        return result;
      }

      static std::string cut_back(std::string &str, char delimiter = ' ', bool first = true, bool hungry = true)
      {
        std::string result;

        auto pos = first ? str.rfind(delimiter) : str.find(delimiter);
        if (pos == std::string::npos)
        {
          if (hungry)
          {
            result = str;
            str = "";
          }
        }
        else
        {
          result = str.substr(pos + 1);
          str = str.substr(0, pos);
        }

        return result;
      }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct critical_section
    {
      mutable std::atomic_bool consumer_lock;

      critical_section()
      {
        consumer_lock = false;
      }

      void lock() const
      {
        while (consumer_lock.exchange(true))
          ;
      }
      void unlock() const { consumer_lock = false; }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename T, typename M = critical_section>
    struct lockable_value : M
    {
      T value;

      T *operator->() { return &value; }
      const T *operator->() const { return &value; }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct semaphore
    {
      mutable std::atomic_size_t count;
      mutable std::mutex mutex;
      mutable std::condition_variable cv;

      semaphore()
      {
        count = 0;
      }

      void lock() const
      {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&]() -> bool
                { return count > 0; });
        lock.release();
      }

      void unlock()
      {
        mutex.unlock();
      }

      void notify()
      {
        {
          std::lock_guard<std::mutex> lock(mutex);
          ++count;
        }

        cv.notify_one();
      }

      void consume() const
      {
        if (count)
          --count;
      }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct data_block_buffer
    {
      std::vector<data_block> blocks;
      std::vector<uint8_t> buffer;

      data_block_buffer()
      {
        buffer.reserve(65536);
      }

      data_block &block_begin(opcode op)
      {
        blocks.emplace_back(op, buffer.size());
        return blocks.back();
      }

      data_block &block_end()
      {
        blocks.back().is_completed = true;
        return blocks.back();
      }

      void block_remove()
      {
        if (blocks.empty())
          return;

        buffer.resize(blocks.back().offset);
        blocks.pop_back();
      }

      void write(const void *ptr, size_t length)
      {
        if (!length)
          return;

        buffer.resize(buffer.size() + length);
        memcpy(buffer.data() + buffer.size() - length, reinterpret_cast<const char *>(ptr), length);
        blocks.back().length += length;
      }

      size_t read(void *ptr, size_t length)
      {
        if (!ptr || blocks.empty() || !blocks.front().is_completed)
          return 0;

        data_block &db = blocks.front();
        size_t result = db.length >= length ? length : db.length;

        if (result)
        {
          memcpy(ptr, buffer.data() + db.offset, result);
          buffer.erase(buffer.begin(), buffer.begin() + result);
        }

        if (!(db.length -= result))
          blocks.erase(blocks.begin());
        else
          blocks.front().op = opcode::continuation;

        if (result)
          for (auto &block : blocks)
            if (block.offset > db.offset)
              block.offset -= result;

        return result;
      }

      size_t peek(opcode *op = nullptr) const
      {
        if (blocks.empty() || !blocks.front().is_completed)
          return 0;

        if (op)
          *op = blocks.front().op;

        return blocks.front().length;
      }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //---------------------------------------------------------------------------------------------------------------------
    bool handshake_websocket(connection &conn)
    {
      std::string line, key;

      while (conn.read_line(line))
      {
        if (line.empty())
          break;

        if (!memcmp(line.c_str(), "Sec-WebSocket-Key: ", 19))
          key = line.substr(19);
      }

      if (key.empty())
        return false;

      key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

      detail::sha1 sha;
      detail::sha1::digest8_t digest;
      sha.process_bytes(key.c_str(), key.length());

      std::string response = "HTTP/1.1 101 Switching Protocols\nUpgrade: websocket\nConnection: Upgrade\nSec-WebSocket-Accept: ";
      response += detail::utils::base64_encode(sha.get_digest_bytes(digest), 20);
      response += "\n\n";

      return conn.force_write(response.c_str(), response.length());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef HEADSOCKET_PLATFORM_WINDOWS
    void set_thread_name(const char *name)
    {
#pragma pack(push, 8)
      typedef struct tagTHREADNAME_INFO
      {
        DWORD dwType;
        LPCSTR szName;
        DWORD dwThreadID;
        DWORD dwFlags;
      } THREADNAME_INFO;
#pragma pack(pop)
      THREADNAME_INFO info = {0x1000, name, static_cast<DWORD>(-1), 0};

      __try
      {
        RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR *>(&info));
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
      }
    }
#else
    void set_thread_name(const char *name)
    {
    }
#endif

  } // namespace detail;

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  namespace detail
  {

    struct connection_impl
    {
      detail::socket_type socket = detail::invalid_socket;
      sockaddr_in from;
      size_t id = 0;

      void assign(const connection_impl &impl)
      {
        socket = impl.socket;
        from = impl.from;
        id = impl.id;
      }

      void close()
      {
        if (socket != detail::invalid_socket)
        {
          detail::close_socket(socket);
          socket = detail::invalid_socket;
        }
      }
    };

  }

  //---------------------------------------------------------------------------------------------------------------------
  connection::connection(const detail::connection_impl &impl)
      : _p(std::make_unique<detail::connection_impl>())
  {
    _p->assign(impl);
  }

  //---------------------------------------------------------------------------------------------------------------------
  connection::~connection()
  {
  }

  //---------------------------------------------------------------------------------------------------------------------
  bool connection::is_valid() const { return _p->socket != detail::invalid_socket; }

  //---------------------------------------------------------------------------------------------------------------------
  size_t connection::id() const { return _p->id; }

  //---------------------------------------------------------------------------------------------------------------------
  size_t connection::write(const void *ptr, size_t length)
  {
    if (!is_valid())
      return detail::socket_error;

    if (!ptr || !length)
      return 0;

    int result = send(_p->socket, static_cast<const char *>(ptr), length, 0);

    if (!result || result == detail::socket_error)
      return 0;

    return static_cast<size_t>(result);
  }

  //---------------------------------------------------------------------------------------------------------------------
  bool connection::force_write(const void *ptr, size_t length)
  {
    if (!is_valid())
      return false;

    if (!ptr)
      return true;

    const char *chPtr = static_cast<const char *>(ptr);

    while (length)
    {
      int result = send(_p->socket, chPtr, length, 0);

      if (!result || result == detail::socket_error)
        return false;

      length -= static_cast<size_t>(result);
      chPtr += result;
    }

    return true;
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t connection::read(void *ptr, size_t length)
  {
    if (!is_valid())
      return detail::socket_error;

    if (!ptr || !length)
      return 0;

    int result = recv(_p->socket, static_cast<char *>(ptr), length, 0);

    if (!result || result == detail::socket_error)
      return 0;

    return static_cast<size_t>(result);
  }

  //---------------------------------------------------------------------------------------------------------------------
  bool connection::force_read(void *ptr, size_t length)
  {
    if (!is_valid())
      return false;

    if (!ptr)
      return true;

    char *chPtr = static_cast<char *>(ptr);

    while (length)
    {
      int result = recv(_p->socket, chPtr, length, 0);

      if (!result || result == detail::socket_error)
        return false;

      length -= static_cast<size_t>(result);
      chPtr += result;
    }

    return true;
  }

  //---------------------------------------------------------------------------------------------------------------------
  bool connection::read_line(std::string &output)
  {
    if (!is_valid())
      return false;

    output = "";

    while (true)
    {
      char ch;
      int r = recv(_p->socket, &ch, 1, 0);

      if (!r || r == detail::socket_error)
        return false;

      if (ch == '\n')
        break;
      else if (ch != '\r')
        output += ch;
    }

    return true;
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  namespace detail
  {

    struct basic_tcp_client_ref
    {
      size_t refCount = 0;
      ptr<basic_tcp_client> client;

      basic_tcp_client_ref(ptr<basic_tcp_client> c)
          : client(c)
      {
      }
    };

    struct basic_tcp_server_impl
    {
      std::atomic_bool isRunning;
      std::atomic_bool disconnectThreadQuit;
      sockaddr_in local;
      detail::lockable_value<std::vector<basic_tcp_client_ref>> connections;
      detail::semaphore disconnectSemaphore;
      int port = 0;
      detail::socket_type serverSocket = invalid_socket;
      std::unique_ptr<std::thread> acceptThread;
      std::unique_ptr<std::thread> disconnectThread;
      id_t nextClientID = 1;

      basic_tcp_server_impl()
      {
        isRunning = false;
        disconnectThreadQuit = false;
      }
    };

  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //---------------------------------------------------------------------------------------------------------------------
  basic_tcp_server::basic_tcp_server(int port)
      : _p(std::make_unique<detail::basic_tcp_server_impl>())
  {
#ifdef HEADSOCKET_PLATFORM_WINDOWS
    WSADATA wsaData;
    WSAStartup(0x101, &wsaData);
#endif

    _p->local.sin_family = AF_INET;
    _p->local.sin_addr.s_addr = INADDR_ANY;
    _p->local.sin_port = htons(static_cast<unsigned short>(port));

    _p->serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(_p->serverSocket, reinterpret_cast<sockaddr *>(&_p->local), sizeof(_p->local)) != 0)
      return;

    if (listen(_p->serverSocket, 8))
      return;

    _p->isRunning = true;
    _p->port = port;
    _p->acceptThread = std::make_unique<std::thread>(std::bind(&basic_tcp_server::accept_thread, this));
    _p->disconnectThread = std::make_unique<std::thread>(std::bind(&basic_tcp_server::disconnect_thread, this));
  }

  //---------------------------------------------------------------------------------------------------------------------
  basic_tcp_server::~basic_tcp_server()
  {
    stop();

#ifdef HEADSOCKET_PLATFORM_WINDOWS
    WSACleanup();
#endif
  }

  //---------------------------------------------------------------------------------------------------------------------
  int basic_tcp_server::port() const { return _p->port; }

  //---------------------------------------------------------------------------------------------------------------------
  void basic_tcp_server::stop()
  {
    if (_p->isRunning.exchange(false))
    {
      detail::close_socket(_p->serverSocket);

      {
        acquire_clients();

        for (size_t i = 0, S = num_clients(); i < S; ++i)
          client_at(i)->disconnect();

        release_clients();
      }

      if (_p->acceptThread)
      {
        _p->acceptThread->join();
        _p->acceptThread = nullptr;
      }

      if (_p->disconnectThread)
      {
        _p->disconnectThreadQuit = true;
        _p->disconnectSemaphore.notify();

        _p->disconnectThread->join();
        _p->disconnectThread = nullptr;
      }
    }
  }

  //---------------------------------------------------------------------------------------------------------------------
  bool basic_tcp_server::is_running() const { return _p->isRunning; }

  //---------------------------------------------------------------------------------------------------------------------
  bool basic_tcp_server::disconnect(ptr<basic_tcp_client> client)
  {
    bool found = false;

    if (client)
    {
      {
        HEADSOCKET_LOCK(_p->connections);
        for (size_t i = 0, S = _p->connections->size(); i < S; ++i)
          if (_p->connections->at(i).client == client)
          {
            found = true;
            break;
          }
      }

      if (found && !client->disconnect())
      {
        client_disconnected(client);
        _p->disconnectSemaphore.notify();
      }
    }

    return found;
  }

  //---------------------------------------------------------------------------------------------------------------------
  bool basic_tcp_server::disconnect(id_t id)
  {
    bool found = false;

    if (id)
    {
      ptr<basic_tcp_client> client;

      {
        HEADSOCKET_LOCK(_p->connections);
        for (size_t i = 0, S = _p->connections->size(); i < S; ++i)
          if (_p->connections->at(i).client->id() == id)
          {
            client = _p->connections->at(i).client;
            found = true;
            break;
          }
      }

      if (found && !client->disconnect())
      {
        client_disconnected(client);
        _p->disconnectSemaphore.notify();
      }
    }

    return found;
  }

  //---------------------------------------------------------------------------------------------------------------------
  ptr<basic_tcp_client> basic_tcp_server::client_at(size_t index) const
  {
    HEADSOCKET_LOCK(_p->connections);
    return index < _p->connections->size() ? _p->connections->at(index).client : nullptr;
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t basic_tcp_server::num_clients() const
  {
    HEADSOCKET_LOCK(_p->connections);
    return _p->connections->size();
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t basic_tcp_server::acquire_clients() const
  {
    HEADSOCKET_LOCK(_p->connections);

    for (auto &clientRef : _p->connections.value)
      ++clientRef.refCount;

    return _p->connections->size();
  }

  //---------------------------------------------------------------------------------------------------------------------
  void basic_tcp_server::release_clients() const
  {
    HEADSOCKET_LOCK(_p->connections);

    for (auto &clientRef : _p->connections.value)
      --clientRef.refCount;

    remove_disconnected();
  }

  //---------------------------------------------------------------------------------------------------------------------
  void basic_tcp_server::remove_disconnected() const
  {
    size_t i = 0;

    while (i < _p->connections->size())
    {
      auto &clientRef = _p->connections.value[i];

      if (!clientRef.client->is_connected() && clientRef.refCount == 0)
        _p->connections->erase(_p->connections->begin() + i);
      else
        ++i;
    }
  }

  //---------------------------------------------------------------------------------------------------------------------
  void basic_tcp_server::accept_thread()
  {
    detail::set_thread_name("BaseTcpServer::acceptThread");

    while (_p->isRunning)
    {
      detail::connection_impl conn_impl;
      conn_impl.socket = ::accept(_p->serverSocket, reinterpret_cast<struct sockaddr *>(&conn_impl.from), nullptr);
      conn_impl.id = _p->nextClientID++;

      if (!_p->nextClientID)
        ++_p->nextClientID;

      if (!_p->isRunning)
        break;

      if (conn_impl.socket != detail::invalid_socket)
      {
        connection conn(conn_impl);

        ptr<basic_tcp_client> newClient;
        bool failed = false;

        if (handshake(conn))
        {
          if (newClient = accept(conn))
          {
            HEADSOCKET_LOCK(_p->connections);
            _p->connections->push_back(newClient);
          }
          else
            failed = true;
        }
        else
          failed = true;

        if (failed)
        {
          conn_impl.close();
          --_p->nextClientID;

          if (!_p->nextClientID)
            --_p->nextClientID;
        }
        else
          client_connected(newClient);
      }
    }
  }

  //---------------------------------------------------------------------------------------------------------------------
  void basic_tcp_server::disconnect_thread()
  {
    detail::set_thread_name("BaseTcpServer::disconnectThread");

    while (!_p->disconnectThreadQuit)
    {
      {
        HEADSOCKET_LOCK(_p->disconnectSemaphore);
        HEADSOCKET_LOCK(_p->connections);

        remove_disconnected();
        _p->disconnectSemaphore.consume();
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  namespace detail
  {

    struct basic_tcp_client_impl
    {
      std::atomic_int refCount;
      std::atomic_bool isConnected;
      std::weak_ptr<basic_tcp_server> server;
      connection conn = detail::connection_impl();
      std::string address = "";
      int port = 0;

      basic_tcp_client_impl()
      {
        refCount = 0;
        isConnected = false;
      }
    };

  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //---------------------------------------------------------------------------------------------------------------------
  basic_tcp_client::basic_tcp_client(const std::string &address, int port)
      : _p(std::make_unique<detail::basic_tcp_client_impl>())
  {
    struct addrinfo *result = nullptr, *ptr = nullptr, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    char buff[16];
    HEADSOCKET_SPRINTF(buff, "%d", port);

    if (getaddrinfo(address.c_str(), buff, &hints, &result))
      return;

    for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
    {
      _p->conn.impl()->socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

      if (!_p->conn.is_valid())
        return;

      if (connect(_p->conn.impl()->socket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen)) == detail::socket_error)
      {
        detail::close_socket(_p->conn.impl()->socket);
        _p->conn.impl()->socket = detail::invalid_socket;
        continue;
      }

      break;
    }

    freeaddrinfo(result);

    if (!_p->conn.is_valid())
      return;

    _p->address = address;
    _p->port = port;
    _p->isConnected = true;
  }

  //---------------------------------------------------------------------------------------------------------------------
  basic_tcp_client::basic_tcp_client(ptr<basic_tcp_server> server, connection &conn)
      : _p(std::make_unique<detail::basic_tcp_client_impl>())
  {
    _p->server = server;
    _p->conn.impl()->assign(*(conn.impl()));
    _p->isConnected = true;
  }

  //---------------------------------------------------------------------------------------------------------------------
  basic_tcp_client::~basic_tcp_client()
  {
    disconnect();
  }

  //---------------------------------------------------------------------------------------------------------------------
  bool basic_tcp_client::disconnect()
  {
    bool wasConnected = _p->isConnected.exchange(false);

    if (wasConnected)
    {
      _p->conn.impl()->close();

      ptr<basic_tcp_server> s = server();

      if (s)
        s->disconnect(_p->conn.id());
    }

    return wasConnected;
  }

  //---------------------------------------------------------------------------------------------------------------------
  bool basic_tcp_client::is_connected() const { return _p->isConnected; }

  //---------------------------------------------------------------------------------------------------------------------
  ptr<basic_tcp_server> basic_tcp_client::server() const { return _p->server.lock(); }

  //---------------------------------------------------------------------------------------------------------------------
  id_t basic_tcp_client::id() const { return _p->conn.id(); }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //---------------------------------------------------------------------------------------------------------------------
  tcp_client::tcp_client(const std::string &address, int port)
      : base_t(address, port)
  {
  }

  //---------------------------------------------------------------------------------------------------------------------
  tcp_client::tcp_client(ptr<basic_tcp_server> server, connection &conn)
      : base_t(server, conn)
  {
  }

  //---------------------------------------------------------------------------------------------------------------------
  tcp_client::~tcp_client()
  {
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t tcp_client::write(const void *ptr, size_t length) { return _p->conn.write(ptr, length); }

  //---------------------------------------------------------------------------------------------------------------------
  size_t tcp_client::read(void *ptr, size_t length) { return _p->conn.read(ptr, length); }

  //---------------------------------------------------------------------------------------------------------------------
  bool tcp_client::force_write(const void *ptr, size_t length) { return _p->conn.force_write(ptr, length); }

  //---------------------------------------------------------------------------------------------------------------------
  bool tcp_client::force_read(void *ptr, size_t length) { return _p->conn.force_read(ptr, length); }

  //---------------------------------------------------------------------------------------------------------------------
  bool tcp_client::read_line(std::string &output) { return _p->conn.read_line(output); }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  namespace detail
  {

    struct async_tcp_client_impl
    {
      detail::semaphore writeSemaphore;
      detail::lockable_value<detail::data_block_buffer> writeBlocks;
      detail::lockable_value<detail::data_block_buffer> readBlocks;
      std::unique_ptr<std::thread> writeThread;
      std::unique_ptr<std::thread> readThread;
    };

  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //---------------------------------------------------------------------------------------------------------------------
  async_tcp_client::async_tcp_client(const std::string &address, int port)
      : base_t(address, port), _ap(std::make_unique<detail::async_tcp_client_impl>())
  {
    init_threads();
  }

  //---------------------------------------------------------------------------------------------------------------------
  async_tcp_client::async_tcp_client(ptr<basic_tcp_server> server, connection &conn)
      : base_t(server, conn), _ap(new detail::async_tcp_client_impl())
  {
    init_threads();
  }

  //---------------------------------------------------------------------------------------------------------------------
  async_tcp_client::~async_tcp_client()
  {
    disconnect();

    _ap->writeSemaphore.notify();
    _ap->writeThread->join();
    _ap->readThread->join();
  }

  //---------------------------------------------------------------------------------------------------------------------
  void async_tcp_client::push(const void *ptr, size_t length, opcode opcode)
  {
    if (!ptr)
      return;

    {
      HEADSOCKET_LOCK(_ap->writeBlocks);
      _ap->writeBlocks->block_begin(opcode);
      _ap->writeBlocks->write(ptr, length);
      _ap->writeBlocks->block_end();
    }

    _ap->writeSemaphore.notify();
  }

  //---------------------------------------------------------------------------------------------------------------------
  void async_tcp_client::push(const void *ptr, size_t length)
  {
    push(ptr, length, opcode::binary);
  }

  //---------------------------------------------------------------------------------------------------------------------
  void async_tcp_client::push(const std::string &text)
  {
    push(text.c_str(), text.length(), opcode::text);
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t async_tcp_client::peek() const
  {
    HEADSOCKET_LOCK(_ap->readBlocks);
    return _ap->readBlocks->peek(nullptr);
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t async_tcp_client::pop(void *ptr, size_t length)
  {
    if (!ptr)
      return invalid_operation;

    if (!length)
      return 0;

    HEADSOCKET_LOCK(_ap->readBlocks);
    return _ap->readBlocks->read(ptr, length);
  }

  //---------------------------------------------------------------------------------------------------------------------
  void async_tcp_client::init_threads()
  {
    _ap->writeThread = std::make_unique<std::thread>(std::bind(&async_tcp_client::write_thread, this));
    _ap->readThread = std::make_unique<std::thread>(std::bind(&async_tcp_client::read_thread, this));
  }

  //---------------------------------------------------------------------------------------------------------------------
  void async_tcp_client::write_thread()
  {
    detail::set_thread_name("AsyncTcpClient::writeThread");

    std::vector<uint8_t> buffer(1024 * 1024);

    while (_p->isConnected)
    {
      size_t written = 0;
      {
        HEADSOCKET_LOCK(_ap->writeSemaphore);

        if (!_p->isConnected)
          break;

        written = async_write_handler(buffer.data(), buffer.size());
      }

      if (written == invalid_operation)
        break;

      if (!written)
        buffer.resize(buffer.size() * 2);
      else
      {
        const char *cursor = reinterpret_cast<const char *>(buffer.data());

        while (written)
        {
          int result = send(_p->conn.impl()->socket, cursor, written, 0);

          if (!result || result == detail::socket_error)
            break;

          cursor += result;
          written -= static_cast<size_t>(result);
        }
      }
    }

    kill_threads();
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t async_tcp_client::async_write_handler(uint8_t *ptr, size_t length)
  {
    HEADSOCKET_LOCK(_ap->writeBlocks);

    size_t toWrite = _ap->writeBlocks->peek(nullptr);
    size_t toConsume = length > toWrite ? toWrite : length;
    _ap->writeBlocks->read(ptr, toConsume);

    if (toWrite == toConsume)
      _ap->writeSemaphore.consume();

    return toConsume;
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t async_tcp_client::async_read_handler(uint8_t *ptr, size_t length)
  {
    HEADSOCKET_LOCK(_ap->readBlocks);

    _ap->readBlocks->block_begin(opcode::binary);
    _ap->readBlocks->write(ptr, length);
    _ap->readBlocks->block_end();

    return length;
  }

  //---------------------------------------------------------------------------------------------------------------------
  void async_tcp_client::read_thread()
  {
    detail::set_thread_name("AsyncTcpClient::readThread");

    std::vector<uint8_t> buffer(1024 * 1024);
    size_t bufferBytes = 0, consumed = 0;

    while (_p->isConnected)
    {
      while (true)
      {
        int result = static_cast<size_t>(bufferBytes);

        if (!result || !consumed)
        {
          result = recv(_p->conn.impl()->socket, reinterpret_cast<char *>(buffer.data() + bufferBytes), buffer.size() - bufferBytes, 0);

          if (!result || result == detail::socket_error)
          {
            consumed = invalid_operation;
            break;
          }

          bufferBytes += static_cast<size_t>(result);
        }

        consumed = async_read_handler(buffer.data(), bufferBytes);

        if (!consumed)
        {
          if (bufferBytes == buffer.size())
            buffer.resize(buffer.size() * 2);
        }
        else
          break;
      }

      if (consumed == invalid_operation)
        break;

      bufferBytes -= consumed;

      if (bufferBytes)
        memcpy(buffer.data(), buffer.data() + consumed, bufferBytes);
    }

    kill_threads();
  }

  //---------------------------------------------------------------------------------------------------------------------
  void async_tcp_client::kill_threads()
  {
    if (std::this_thread::get_id() == _ap->readThread->get_id())
      _ap->writeSemaphore.notify();

    disconnect();
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //---------------------------------------------------------------------------------------------------------------------
  web_socket_client::web_socket_client(const std::string &address, int port)
      : base_t(address, port)
  {
  }

  //---------------------------------------------------------------------------------------------------------------------
  web_socket_client::web_socket_client(ptr<basic_tcp_server> server, connection &conn)
      : base_t(server, conn)
  {
  }

  //---------------------------------------------------------------------------------------------------------------------
  web_socket_client::~web_socket_client()
  {
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t web_socket_client::peek(opcode *op) const
  {
    HEADSOCKET_LOCK(_ap->readBlocks);
    return _ap->readBlocks->peek(op);
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t web_socket_client::async_write_handler(uint8_t *ptr, size_t length)
  {
    uint8_t *cursor = ptr;
    HEADSOCKET_LOCK(_ap->writeBlocks);

    while (length >= 16)
    {
      opcode op;
      size_t toWrite = _ap->writeBlocks->peek(&op);
      size_t toConsume = (length - 15) > frame_size_limit ? frame_size_limit : (length - 15);
      toConsume = toConsume > toWrite ? toWrite : toConsume;

      frame_header header;
      header.fin = (toWrite - toConsume) == 0;
      header.op = op;
      header.masked = false;
      header.payload_length = toConsume;

      size_t headerSize = header.write(cursor, length);
      cursor += headerSize;
      length -= headerSize;
      _ap->writeBlocks->read(cursor, toConsume);
      cursor += toConsume;
      length -= toConsume;

      if (header.fin)
        _ap->writeSemaphore.consume();

      if (!_ap->writeBlocks->peek(&op))
        break;
    }

    return cursor - ptr;
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t web_socket_client::async_read_handler(uint8_t *ptr, size_t length)
  {
    uint8_t *cursor = ptr;
    HEADSOCKET_LOCK(_ap->readBlocks);

    if (!_payload_size)
    {
      opcode prevOpcode = _current_header.op;
      size_t headerSize = _current_header.read(cursor, length);

      if (!headerSize)
        return 0;
      else if (headerSize == invalid_operation)
        return invalid_operation;

      _payload_size = _current_header.payload_length;
      cursor += headerSize;
      length -= headerSize;

      if (_current_header.op != opcode::continuation)
        _ap->readBlocks->block_begin(_current_header.op);
      else
        _current_header.op = prevOpcode;
    }

    if (_payload_size)
    {
      size_t toConsume = length >= _payload_size ? _payload_size : length;

      if (toConsume)
      {
        _ap->readBlocks->write(cursor, toConsume);
        _payload_size -= toConsume;
        cursor += toConsume;
        length -= toConsume;
      }
    }

    if (!_payload_size)
    {
      if (_current_header.masked)
      {
        data_block &db = _ap->readBlocks->blocks.back();
        size_t len = _current_header.payload_length;
        detail::utils::xor32(_current_header.masking_key, _ap->readBlocks->buffer.data() + _ap->readBlocks->buffer.size() - len, len);
      }

      if (_current_header.fin)
      {
        data_block &db = _ap->readBlocks->blocks.back();

        switch (_current_header.op)
        {
        case opcode::ping:
          push(_ap->readBlocks->buffer.data() + db.offset, db.length, opcode::pong);
          break;

        case opcode::text:
          _ap->readBlocks->buffer.push_back(0);
          ++db.length;
          break;

        case opcode::connection_close:
          kill_threads();
          break;

        default:
          break;
        }

        if (_current_header.op == opcode::text || _current_header.op == opcode::binary)
        {
          _ap->readBlocks->block_end();

          if (async_received_data(db, _ap->readBlocks->buffer.data() + db.offset, db.length))
            _ap->readBlocks->block_remove();
        }
      }
    }

    return cursor - ptr;
  }

//---------------------------------------------------------------------------------------------------------------------
#define HAVE_ENOUGH_BYTES(num) \
  if (length < num)            \
    return 0;                  \
  else                         \
    length -= num;
  size_t web_socket_client::frame_header::read(const uint8_t *ptr, size_t length)
  {
    const uint8_t *cursor = ptr;
    HAVE_ENOUGH_BYTES(2);
    this->fin = ((*cursor) & 0x80) != 0;
    this->op = static_cast<opcode>((*cursor++) & 0x0F);

    this->masked = ((*cursor) & 0x80) != 0;
    uint8_t byte = (*cursor++) & 0x7F;

    if (byte < 126)
      this->payload_length = byte;
    else if (byte == 126)
    {
      HAVE_ENOUGH_BYTES(2);
      this->payload_length = detail::utils::swap16bits(*(reinterpret_cast<const uint16_t *>(cursor)));
      cursor += 2;
    }
    else if (byte == 127)
    {
      HAVE_ENOUGH_BYTES(8);
      uint64_t length64 = detail::utils::swap64bits(*(reinterpret_cast<const uint64_t *>(cursor))) & 0x7FFFFFFFFFFFFFFFULL;
      this->payload_length = static_cast<size_t>(length64);
      cursor += 8;
    }

    if (this->masked)
    {
      HAVE_ENOUGH_BYTES(4);
      this->masking_key = *(reinterpret_cast<const uint32_t *>(cursor));
      cursor += 4;
    }

    return cursor - ptr;
  }

  //---------------------------------------------------------------------------------------------------------------------
  size_t web_socket_client::frame_header::write(uint8_t *ptr, size_t length) const
  {
    uint8_t *cursor = ptr;
    HAVE_ENOUGH_BYTES(2);
    *cursor = this->fin ? 0x80 : 0x00;
    *cursor++ |= static_cast<uint8_t>(this->op);

    *cursor = this->masked ? 0x80 : 0x00;

    if (this->payload_length < 126)
      *cursor++ |= static_cast<uint8_t>(this->payload_length);
    else if (this->payload_length < 65536)
    {
      HAVE_ENOUGH_BYTES(2);
      *cursor++ |= 126;
      *reinterpret_cast<uint16_t *>(cursor) = detail::utils::swap16bits(static_cast<uint16_t>(this->payload_length));
      cursor += 2;
    }
    else
    {
      HAVE_ENOUGH_BYTES(8);
      *cursor++ |= 127;
      *reinterpret_cast<uint64_t *>(cursor) = detail::utils::swap64bits(static_cast<uint64_t>(this->payload_length));
      cursor += 8;
    }

    if (this->masked)
    {
      HAVE_ENOUGH_BYTES(4);
      *reinterpret_cast<uint32_t *>(cursor) = this->masking_key;
      cursor += 4;
    }

    return cursor - ptr;
  }
#undef HAVE_ENOUGH_BYTES

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  namespace detail
  {

  }

  //---------------------------------------------------------------------------------------------------------------------
  void http_server::init()
  {
  }

  //---------------------------------------------------------------------------------------------------------------------
  bool http_server::handshake(connection &conn)
  {
    std::string requestLine;

    if (!conn.read_line(requestLine))
      return false;

    std::string headerLine;
    while (conn.read_line(headerLine))
    {
      if (headerLine.empty())
        break;
    }

    std::string method = detail::utils::cut_front(requestLine);
    std::string path = detail::utils::url_decode(detail::utils::cut_front(requestLine));

    if (!path.empty() && path.front() == '/')
      path = path.substr(1);
    if (!path.empty() && path.back() == '/')
      path = path.substr(0, path.length() - 1);

    std::string params_get = detail::utils::cut_back(path, '?', false, false);
    std::string version = detail::utils::cut_front(requestLine);

    parameters_t params;
    std::string param_str;
    while (!(param_str = detail::utils::cut_front(params_get, '&')).empty())
    {
      parameter param;
      param.name = detail::utils::cut_front(param_str, '=');
      param.value = param_str;
      param.integer = atoi(param_str.c_str());
      param.real = atof(param_str.c_str());
      param.boolean = (param.integer != 0) || (param_str == "true");

      params[param.name] = param;
    }

    response resp;
    if (path != "favicon.ico" && request(path, params, resp))
    {
      std::stringstream ss;
      ss << version << " 200 OK\n";
      ss << "Content-Type: " << resp.content_type << "\n";
      ss << "Content-Length: " << resp.message.length() << "\n\n";
      ss << resp.message;

      conn.write(ss.str());
    }
    else
    {
      conn.write(version);
      conn.write(" 404 Not Found\n");
    }

    return false;
  }

}
#endif
#endif