# A Simple Linux HTTP Web Server

## Steps to build it-

Clone the repository-
```
$ git clone --recursive https://github.com/ss-221/web-server.git
```
Make a build folder and navigate to it-
```
web-server$ mkdir build
web-server$ cd build
```
Run CMake-
```
web-server/build$ cmake ..
web-server/build$ make
```

Navigate to the src folder and run the web-server (provide a port no. as the arg.)-

```
web-server/build$ cd src
web-server/build/src$ ./web-server 8080
```

Finally, open your browser and type in-
```
localhost:8080
```
