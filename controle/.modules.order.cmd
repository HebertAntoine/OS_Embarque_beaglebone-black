cmd_/src/modules.order := {   echo /src/kgpio.ko;   echo /src/kirq.ko;   echo /src/kpwm.ko; :; } | awk '!x[$$0]++' - > /src/modules.order
