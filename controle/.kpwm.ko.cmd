cmd_/src/kpwm.ko := arm-linux-gnueabihf-ld -r -EL -z noexecstack --no-warn-rwx-segments --build-id=sha1  -T scripts/module.lds -o /src/kpwm.ko /src/kpwm.o /src/kpwm.mod.o;  true
