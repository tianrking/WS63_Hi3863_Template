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

SLE 30ms 全双工收发 2个长度 AA BB 没有问题  10ms 可能会死机
发现只要数据量一大 就有接收延迟 似乎
对于 90 个长度的 需要把延迟拉到 300ms 全双工才没有问题


#define CONFIG_SLE_UART_BUS  0
#define CONFIG_UART_TXD_PIN  17
#define CONFIG_UART_RXD_PIN  18
#define CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER 1
// #define CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT 1