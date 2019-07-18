---
title: 'Simple-Web-Server: a fast and flexible HTTP/1.1 C++ client and server library'
tags:
  - C++
  - web
  - http
  - client
  - server
  - library
  - asio
authors:
  - name: Ole Christian Eidheim
    orcid: 0000-0001-5355-6326
    affiliation: 1
affiliations:
  - name: Department of Computer Science, Norwegian University of Science and Technology
    index: 1
date: 18 July 2019
bibliography: paper.bib
---

# Summary

The programming language C++ is commonly used for resource intensive tasks.
Simple-Web-Server is a library that can be utilized in C++ applications to
implement web-resources or perform HTTP(S) requests in a simple manner across OS
platforms compared to using a networking library directly. 

The main features, apart from speed and ease of use, are flexibility and safety.
The asynchronous I/O library Asio C++ Library [@asio] is used to implement
networking and asynchronous event handling. The sending of outgoing messages has
been made thread safe, and event handling in one or several threads is
supported. The default event handling strategy is using one thread, commonly
called event-loop, which makes accessing shared resources safe without using
resource locking through for instance mutexes. Although, accessing shared
resources in a multithreaded event-handling strategy can be made safer by
utilizing the annotation offered in Clang Thread Safety Analysis
[@clang_thread_safety]. In some cases, however, processing requests
sequentially, in an event-loop scheme, can be faster than processing the
requests in several threads where shared resources must be protected from
simultaneous use.

An additional safety feature is stopping of asynchronous handlers when the
associated client or server object has been destroyed. An atomic instruction
based class, ScopeRunner, was implemented to achieve this since reader-writer
locks proved more resource intensive for this specific task.

The Asio C++ Library [@asio] is currently proposed to the C++ standard library
[@wakely]. If accepted in one of the future revisions of the C++ programming
language, C++ applications can make use of a standardized event handling system.
Until then, efforts are made to support old and new versions of the Asio C++
Library, as well as both the standalone and Boost [@boost] variants of the
library.

Simple-Web-Server is used in teaching at the Norwegian University of Science and
Technology, and used in many external projects, for instance in the
multi-purpose emulation framework MAME [@mame]. The library was also used in the
senior thesis by Chung and Callin [@chung].

There are several alternatives to Simple-Web-Server. Most notably
Boost.Beast [@beast], but this library is made for library authors and is thus
harder to utilize in a C++ application. Additionally, Boost.Beast does not support
standalone Asio. Another alternative is H2O [@h20] that supports several event
handling systems, however, Asio is not yet supported. Both Boost.Beast, and to a
lesser degree H2O, supports the WebSocket protocol [@websocket_protocol]. In the
case of Simple-Web-Server, WebSocket is supported through a related external
project named Simple-WebSocket-Server [@simple_websocket_server].

Based on Simple-Web-Server, a new C++ library supporting HTTP/2 is under
development. HTTP/2 is very different from HTTP/1.1, but the experiences from
developing Simple-Web-Server, and some its implementations, such as the
ScopeRunner class, can be helpful when writing an HTTP/2 library.

# Acknowledgments

I would like to thank all those who have contributed to the Simple-Web-Server
project.

# References
