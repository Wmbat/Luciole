# The Wombat Engine
A Cross-Platform Vulkan game engine.

## GNU/Linux
The library does not currently support Wayland nor Mir. It only support XCB.

You will have to find the XCB library and link against it. Here is an example on how it can be done:
```
    find_package( XCB REQUIRED )

    if( NOT XCB_FOUND )
        message( FATAL_ERROR "XCB development package not found" )
    else( )
        message( STATUS ${XCB_LIBRARIES} )
    endif( )
```
and then, link against it
```
    target_link_libraries( your_project
            ${XCB_LIBRARIES} )
```
It does not have to be done that way though.

You will also have to include the GLM headers as well as the spdlog headers from https://github.com/gabime/spdlog or you can simply grab them from the include external folder

You can then include the TWE library headers located in the TWE folder and link against the libTWE.so that you build.

## How to use

To start using the engine. You simply have to create a class that inherits from *TWE::application* and call its constructor. You will also have to override the *run()* function where your main loop will be located.
```
class demo : public TWE::application
{
public:
    demo( const std::string& title )
        :
        TWE::application( title )
    {

    }
    ~demo( ) override
    {
    }
    
    void run( ) override
    {
        while( p_wnd_->is_open() )
        {
        
        }
    }
};
```
Finally, you will have to implement the function *TWE::create_application*. Here is an example
```
std::unique_ptr<TWE::application> TWE::create_application( )
{
    auto p_demo = std::make_unique<demo>( "Demo" );
    
    return std::move( p_demo );
}
```
