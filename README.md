This is a simple example of publish-subscribe message broker based on zeromq.

Compile
-------

```
  mkdir build
  cd build
  cmake .. -G "Unix Makefiles"
  cd ..
  make
```


Usage
-----

```
  ./broker &
  ./pub <topic> --broker &
  ./sub <topic> --broker &
```
