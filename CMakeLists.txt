cmake_minimum_required(VERSION 3.16)
project(Bookstore)

set(CMAKE_CXX_STANDARD 17)

add_executable(Bookstore main.cpp cmake-build-debug BlockedLinkList.h Books.h Users.h Finance.h log.h processor.h Bookstore.h)
