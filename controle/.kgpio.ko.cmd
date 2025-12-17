cmd_/src/kgpio.ko := arm-linux-gnueabihf-ld -r -EL -z noexecstack --no-warn-rwx-segments --build-id=sha1  -T scripts/module.lds -o /src/kgpio.ko /src/kgpio.o /src/kgpio.mod.o;  true
