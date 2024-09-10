Add  in CMakeLists.txt

```cmake
if(DEFINED CONFIG_ENABLE_MY_SAMPLE)
    add_subdirectory_if_exist(demo)  
endif()
```

Add in Kconfig


```cmake
config ENABLE_MY_SAMPLE
    bool
    prompt "Enable the Sample of demo."
    default n
    depends on SAMPLE_ENABLE
    help
        This option means enable the sample of products.

if ENABLE_MY_SAMPLE
osource "application/samples/demo/Kconfig"
endif
```

CLICK Kconfig Select APP -> Enable Sample -> Enable Demo