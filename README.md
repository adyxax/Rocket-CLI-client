# Rocket CLI client

This is a collection of tools to interact with rocket chat.

## Content

- [Dependencies](#dependencies)
- [Manual installation](#manual-installation)
- [Configuration](#configuration)
- [Usage](#usage)

## Dependencies

This project has the following hard dependencies :
- the libconfig from http://www.hyperrealm.com/libconfig/libconfig.html
- the libcurl from https://curl.haxx.se/libcurl/
- the openssl libraries : libcrypto and libssl
- the libev event loop for websockets handling with libuwsc : http://software.schmorp.de/pkg/libev.html

The following dependencies are included as submodules :
- uthash from http://troydhanson.github.io/uthash/

The following dependency is automaticaly downloaded and built from github :
- libuwsc from https://github.com/zhaojh329/libuwsc

## Manual Installation

This project is built using cmake :
```
git submodule update --init
mkdir build
cd build
cmake ..
make
make install
```

You can customise the build with the following cmake flags :

- `CMAKE_BUILD_TYPE` : Debug|Release|RelWithDebInfo|MinSizeRel, defaults to Release if using a tarball, and Debug if using the git tree
- `CMAKE_INSTALL_PREFIX` : path, defaults to `/usr/local`
- `DEBUG_WEBSOCKETS` : ON|OFF, defaults to OFF

For example this activates websocket debugging for a debug build and installs the tools for your current user :

`cmake .. -DCMAKE_BUILD_TYPE=Debug -D CMAKE_INSTALL_PREFIX=$HOME/.local -DDEBUG_WEBSOCKETS=ON`

## Configuration

Here is the default configuration :
```
web_url = "http://hurricane:3000";
wss_url = "ws://hurricane:3000/websocket";
login = "test";
password = "test";
```

## Usage

### rocket_close_direct_conversations

This tools lists your active direct conversations and allows you to specify conversations to close from the cli. It is equivalent of the "Hide Room" feature in the gui, it will not make you
really leave a channel. I use it to quickly remove clutter. An empty line will exit the program.

Example use :
```
julien@hurricane ~/git/rocket-cli-client/build (master *%) $ close_direct_conversations/rocket_close_direct_conversations
Active direct conversations :
        foo.bar
        mr.somebody
Direct conversation to close: foo.bar
{"success":true}
Direct conversation to close:
julien@hurricane ~/git/rocket-cli-client/build (master *%) $
```

### rocket_maar

This tools marks all active subscriptions as read, not channels or groups at the moment. It is usefull when returning from a holliday.

Example use :
```
julien@hurricane ~/git/rocket-cli-client/build (master *%) $ maar/rocket_maar
The following active subscriptions are marked as read :
foo.bar
mr.somebody
julien@hurricane ~/git/rocket-cli-client/build (master *%) $
```
