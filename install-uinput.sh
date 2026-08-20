#!/bin/bash
set -e

if [ "$EUID" -ne 0 ]; then
    echo "Usage: sudo ./install-uinput.sh [--temp|--permanent]"
	echo " "
	echo "	--temp       Temp mode: perms until reboot"
	echo "	--permanent  Permanent mode: udev rule + group, survives on reboot"
	echo " "
    exit 1
fi

MODE="${1:---permanent}"
TARGET_USER="${SUDO_USER:-$USER}"

case "$MODE" in
  --temp)
    echo "Temp mode: perms until reboot only."
    chmod 666 /dev/uinput
    modprobe uinput 2>/dev/null || true
    echo "Done. Run ./ponio now, no sudo. Resets on reboot — rerun after restart."
    ;;

  --permanent)
    echo "Permanent mode: udev rule + group, survives reboot."
    groupadd -f uinput
    usermod -aG uinput "$TARGET_USER"

    cat > /etc/udev/rules.d/99-ponio-uinput.rules <<'EOF'
KERNEL=="uinput", MODE="0660", GROUP="uinput", OPTIONS+="static_node=uinput"
EOF

    echo "uinput" > /etc/modules-load.d/ponio-uinput.conf
    udevadm control --reload-rules
    udevadm trigger
    modprobe uinput

    echo "Done. Log out and back in, then run ./ponio (no sudo needed)."
    ;;

  *)
    echo "Unknown mode: $MODE"
    echo "Usage: sudo ./install-uinput.sh [--temp|--permanent]"
    exit 1
    ;;
esac
