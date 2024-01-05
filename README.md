## load module

```
# insmod mod-psci.ko
```

## CPU ON

example) CPU4 (cluster=1 core=0)
```
# echo "cpu_on cluster=1 core=0 address=0x730000000" > /proc/psci
```

Or directly specify params.
- 1st parameter: psci function
- 2nd parameter: cpu number (CPU4 is 0x100)
- 3rd parameter: start address
```
# echo "cpu_on 0x100 0x730000000" > /proc/psci
```

## Check variables
```
# ls /sys/module/mod_psci/parameters/

### And cat valiable names
```

## unload module

```
# rmmod mod_psci
```

## Other operations

```
# echo "version" > /proc/psci
--> UNKNOWN
```

```
# echo "cpu_suspend 0x100 0x730000000" > /proc/psci
--> PSCI_RET_INVALID_PARAMS
```

```
# echo "cpu_off 1 0 0x730000000" > /proc/psci
--> Never return
```

```
# echo "affinity_info 0x100 0" > /proc/psci
--> PSCI_RET_SUCCESS
a=0x0 a1=0x100 a2=0x0 a3=0x0
```

```
# echo "migrate 0 0 0" > /proc/psci
--> UNKNOWN
# echo "migrate_info_type 0 0 0" > /proc/psci
--> UNKNOWN
# echo "migrate_info_up_cpu 0 0 0" > /proc/psci
--> PSCI_RET_SUCCESS
a=0x0 a1=0x0 a2=0x0 a3=0x0
```
```
# echo "system_suspend 0 0 0" > /proc/psci
--> PSCI_RET_DENIED
a=0xFFFFFFFFFFFFFFFD a1=0x0 a2=0x0 a3=0x0
```
