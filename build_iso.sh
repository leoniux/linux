#!/bin/bash
# Build ISO script
set -e

WORKDIR="/tmp/bad-apple-iso"
KERNEL="arch/x86/boot/bzImage"
OUTPUT_ISO="/tmp/BadAppleLinux.iso"

echo "Bad Apple Linux ISO Builder"
echo "Cleaning previous build..."
rm -rf "$WORKDIR"
mkdir -p "$WORKDIR"/{isofiles/boot/grub,initramfs/{bin,sbin,etc,proc,sys,dev}}
echo "Copying kernel..."
cp "$KERNEL" "$WORKDIR/isofiles/boot/vmlinuz"
echo "Creating initramfs with BusyBox shell..."
cd "$WORKDIR/initramfs"

# Cp busybox
if [ -f /bin/busybox-static ]; then
    cp /bin/busybox-static bin/busybox
elif [ -f /usr/bin/busybox-static ]; then
    cp /usr/bin/busybox-static bin/busybox
else
    cp "$(which busybox)" bin/busybox
fi

chmod +x bin/busybox
for cmd in sh ash mount umount echo cat ls ps kill insmod lsmod rmmod mkdir ln; do
    ln -sf /bin/busybox bin/$cmd
done

# init script
cat > init << 'EOFFF'
#!/bin/busybox sh
/bin/busybox echo "Mounting filesystems..."
/bin/busybox mount -t proc none /proc
/bin/busybox mount -t sysfs none /sys
/bin/busybox mount -t devtmpfs none /dev 2>/dev/null || true
/bin/busybox echo 1 > /proc/sys/kernel/sysrq
/bin/busybox clear
/bin/busybox echo "========================================"
/bin/busybox echo "            Bad Apple Kernel            "
/bin/busybox echo "========================================"
/bin/busybox echo ""
/bin/busybox echo "You now have a shell. To trigger kernel"
/bin/busybox echo "panic and play Bad Apple, run:"
/bin/busybox echo ""
/bin/busybox echo "    echo c > /proc/sysrq-trigger"
/bin/busybox echo ""
/bin/busybox echo "========================================"
/bin/busybox echo ""

# Export PATH
export PATH=/bin:/sbin

# Start shell
exec /bin/busybox sh
EOFFF

chmod +x init

echo "Packing initramfs..."
find . | cpio -o -H newc 2>/dev/null | gzip > "$WORKDIR/isofiles/boot/initramfs.gz"
cd "$WORKDIR"

echo "Creating GRUB configuration..."
cat > "$WORKDIR/isofiles/boot/grub/grub.cfg" << 'EOF'
set timeout=5
set default=0

menuentry "Bad Apple Linux" {
    linux /boot/vmlinuz console=tty0 panic=0 loglevel=4 nomodeset
    initrd /boot/initramfs.gz
}
menuentry "Bad Apple Linux Auto Panic" {
    linux /boot/vmlinuz console=tty0 panic=0 loglevel=7 nomodeset
}
EOF
echo "Building bootable ISO..."
grub-mkrescue -o "$OUTPUT_ISO" "$WORKDIR/isofiles" 2>&1 | grep -v "warning:"
# Cleanup
echo "Cleaning up temporary files..."
rm -rf "$WORKDIR"
echo "ISO created."
echo "ISO location: $OUTPUT_ISO"
echo "Size: $(du -h "$OUTPUT_ISO" | cut -f1)"