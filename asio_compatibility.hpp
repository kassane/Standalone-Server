#ifndef SIMPLE_WEB_ASIO_HPP
#define SIMPLE_WEB_ASIO_HPP

#include <memory>

#ifdef USE_STANDALONE_ASIO
#include <asio.hpp>
#include <asio/steady_timer.hpp>
namespace SimpleWeb {
  using error_code = std::error_code;
  using errc = std::errc;
  using system_error = std::system_error;
  namespace make_error_code = std;
} // namespace SimpleWeb
#else
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
namespace SimpleWeb {
  namespace asio = boost::asio;
  using error_code = boost::system::error_code;
  namespace errc = boost::system::errc;
  using system_error = boost::system::system_error;
  namespace make_error_code = boost::system::errc;
} // namespace SimpleWeb
#endif

namespace SimpleWeb {
#if(USE_STANDALONE_ASIO && ASIO_VERSION >= 101300) || BOOST_ASIO_VERSION >= 101300
  using io_context = asio::io_context;
  using resolver_results = asio::ip::tcp::resolver::results_type;
  using async_connect_endpoint = asio::ip::tcp::endpoint;

  inline void restart(io_context &context) noexcept {
    context.restart();
  }
  inline asio::ip::address make_address(const std::string &str) noexcept {
    return asio::ip::make_address(str);
  }
  template <typename socket_type>
  io_context &get_socket_context(socket_type &socket) {
    return socket.get_executor().context();
  }
  template <typename handler_type>
  void async_resolve(asio::ip::tcp::resolver &resolver, const std::pair<std::string, std::string> &host_port, handler_type &&handler) {
    resolver.async_resolve(host_port.first, host_port.second, handler);
  }
  template <typename executor_type, typename handler_type>
  void post(executor_type &executor, handler_type &&handler) {
    asio::post(executor, handler);
  }
  template <typename executor_type, typename handler_type>
  asio::executor_binder<typename asio::decay<handler_type>::type, executor_type> bind_executor(executor_type &executor, handler_type &&handler) {
    return asio::bind_executor(executor, handler);
  }
#else
  using io_context = asio::io_service;
  using resolver_results = asio::ip::tcp::resolver::iterator;
  using async_connect_endpoint = asio::ip::tcp::resolver::iterator;

  inline void restart(io_context &context) noexcept {
    context.reset();
  }
  inline asio::ip::address make_address(const std::string &str) noexcept {
    return asio::ip::address::from_string(str);
  }
  template <typename socket_type>
  io_context &get_socket_context(socket_type &socket) {
    return socket.get_io_service();
  }
  template <typename handler_type>
  void async_resolve(asio::ip::tcp::resolver &resolver, const std::pair<std::string, std::string> &host_port, handler_type &&handler) {
    resolver.async_resolve(asio::ip::tcp::resolver::query(host_port.first, host_port.second), handler);
  }
  template <typename executor_type, typename handler_type>
  void post(executor_type &executor, handler_type &&handler) {
    executor.post(handler);
  }
  template <typename executor_type, typename handler_type>
  asio::detail::wrapped_handler<asio::io_service::strand, handler_type, asio::detail::is_continuation_if_running> bind_executor(executor_type &executor, handler_type &&handler) {
    return executor.wrap(handler);
  }
#endif
} // namespace SimpleWeb

#endif /* SIMPLE_WEB_ASIO_HPP */
