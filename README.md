# The Wombat Engine

## How to use.
You have to build the library using Cmake yourself.

### GNU/Linux
You have to link 

You currently need to use the XCB library. The engine does not support currently Wayland or Mir. If XCB is installed in your system, it can be done like so in your cmake file: 
```
    find_package( XCB REQUIRED )

    if( NOT XCB_FOUND )
        message( FATAL_ERROR "XCB development package not found" )
    else( )
        message( STATUS ${XCB_LIBRARIES} )
    endif( )
```
You will also have to include the GLM headers as well as the spdlog headers from https://github.com/gabime/spdlog or you can simply grab them from the include external folder

You can then include the TWE library headers located in the TWE folder and link against the libTWE.so.
