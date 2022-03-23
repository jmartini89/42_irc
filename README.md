![irc](https://user-images.githubusercontent.com/25162290/159374311-d8ab4c91-6011-4df0-a1f1-83d9ec4c7ada.png)

# 42_irc

### Internet Relay Chat server

[RFC](https://datatracker.ietf.org/doc/html/rfc1459) compliant IRC protocol server written in C++ using BSD libc

#
### Features

* Kernel event notification mechanism provided by `kevent` & `kqueue`

* Efficient single-threaded synchronous operations

* Flood-safe execution

* Empathetic buffer management for clients struggling with data transmission

#
### POSIX?

No, unfortunately

We wanted to take advantage of a modern and smart kernel notification system that could handle UNIX-signals as well

This led us to `GNU epoll` and `BSD kevent`. Both of these notification systems use non-POSIX compliant syscalls and therefore are not portable-friendly

Interesting discussions on the topic:
* [Choosing between portability and innovation](https://lwn.net/Articles/430793/)
* [kqueue vs epoll](https://news.ycombinator.com/item?id=3028687)

#
Made by jmartini & mpezzull
