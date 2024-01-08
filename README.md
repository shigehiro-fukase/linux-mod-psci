# mod-psci

## 概要

PSCI (Power State Coordination Interface) を操作するための Linux 用ドライバモジュールです。

#### ビルド

`Makefile` 内の `KERNEL_HEADERS=` をターゲット用のビルド済み Linux カーネルソースのパスに変えてください。
また、ターゲット用の Linux カーネルビルドと同様に環境変数を設定し、ツールチェインのパスなどを整えてください。

```
$ make all
```

`mod-psci.ko` が作成されます。

#### ロード
操作前に `insmod` コマンドでモジュールのロードを行います。

```
# insmod mod-psci.ko
```

#### 操作
ロードすると `/proc/psci` が作成されます。
`/proc/psci` は `X0 X1 X2 X3` 形式の文字列を受け付けます。

- `X0`: PSCI function を名称又は数値で指定します
  - 名称: 以下の23種が定義済みです
    - `"version"`,
     `"cpu_suspend"`,
     `"cpu_off"`,
     `"cpu_on"`,
     `"affinity_info"`,
     `"migrate"`,
     `"migrate_info_type"`,
     `"migrate_info_up_cpu"`,
     `"system_off"`,
     `"system_reset"`,
     `"features"`,
     `"cpu_freeze"`,
     `"cpu_default_suspend"`,
     `"node_hw_state"`,
     `"system_suspend"`,
     `"set_suspend_mode"`,
     `"stat_residency"`,
     `"stat_count"`,
     `"system_reset2"`,
     `"mem_protect"`,
     `"mem_protect_check_range"`,
     `"clean_inv_memregion"`,
     `"clean_inv_memregion_attributes"`
    - 23以下の数値: 定義済みの23種のインデックスとみなします
    - 23以上の数値: `Function ID` 直値とみなします
      - 例）`0xC4000003` は 64bitモードでの `"cpu_on"` の `Function ID` です
- `X1` `X2` `X3` : 名称又は数値で指定します
  - 名称: 以下の3種を定義済みです
    - "cluster=数値", "core=数値", "address=数値"
  - 数値: パラメータ値とみなします
    - 注）`X3` を使用する Function は現状存在しないようです。無指定は `0` 扱いになります

モジュール内で保持する変数の確認方法

```
# ls /sys/module/mod_psci/parameters/
```

とすると各変数名がファイルとして見えます。`cat`コマンドで値を確認できます。

#### アンロード

```
# rmmod mod_psci
```

## CPU ON の例

CPU4 (cluster=1 core=0) を `0x730000000` で開始したい場合

名称形式の場合:
```
# echo "cpu_on cluster=1 core=0 address=0x730000000" > /proc/psci
```

数値直指定の場合: (CPU4 は `0x100`)
```
# echo "cpu_on 0x100 0x730000000" > /proc/psci
```

あるいは

```
# echo "0xC4000003 0x100 0x730000000" > /proc/psci
```

でも同様です。


## ほかの PSCI 操作について

有用に使える機能は無さそうです。

```
# echo "version" > /proc/psci
--> retvalue 0x10000
```

```
# echo "cpu_suspend 0x100 0x730000000" > /proc/psci
--> retvalue 0xfffffffffffffffe "INVALID_PARAMETERS"
```

```
# echo "cpu_off 1 0 0x730000000" > /proc/psci
--> Never return
```

```
# echo "affinity_info 0x100 0" > /proc/psci
--> retvalue 0x0 "SUCCESS"
a=0x0 a1=0x100 a2=0x0 a3=0x0
```

```
# echo "migrate 0 0 0" > /proc/psci
--> retvalue 0xffffffff "NOT_SUPPORTED"
```

```
# echo "migrate_info_type 0 0 0" > /proc/psci
--> retvalue 0x1
```

```
# echo "migrate_info_up_cpu 0 0 0" > /proc/psci
--> retvalue 0x0 "SUCCESS"
a=0x0 a1=0x0 a2=0x0 a3=0x0
```

```
# echo "system_suspend 0 0 0" > /proc/psci
--> retvalue 0xfffffffffffffffd "DENIED"
```


## References

### Arm Power State Coordination Interface Platform Design Document
- https://developer.arm.com/documentation/den0022/f/?lang=en
  - DEN0022F_Power_State_Coordination_Interface-alp0.pdf


### Xilinx による日本語資料
- PSCI (Power State Coordination Interface)
  - https://docs.xilinx.com/r/ja-JP/ug1304-versal-acap-ssdg/PSCI-Power-State-Coordination-Interface


