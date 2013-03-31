This is a simple example of publish-subscribe message broker based on zeromq.

Compile
-------

```
  mkdir build
  cd build
  cmake .. -G "Unix Makefiles"
  make
```
or if you prefer to use ninja
```
  mkdir build
  cd build
  cmake .. -G "Ninja"
  ninja
```


Usage
-----

```
  ./broker &
  ./pub <topic> --broker &
  ./sub <topic> --broker &
```
