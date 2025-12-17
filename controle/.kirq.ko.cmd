cmd_/src/kirq.ko := arm-linux-gnueabihf-ld -r -EL -z noexecstack --no-warn-rwx-segments --build-id=sha1  -T scripts/module.lds -o /src/kirq.ko /src/kirq.o /src/kirq.mod.o;  true
