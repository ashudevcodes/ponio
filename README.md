# Ponio

> A virtual gamepad Linux server for the [Ponio Mobile Application](https://github.com/ashudevcodes/ponio_android_controller)

Turns your Android phone into a wireless gamepad ony support Linux enviornment (ARCH Btw)

<kbd><img width="1919" height="1079" alt="ponioWithGame" src="https://github.com/user-attachments/assets/e1d49089-2e2d-4729-bc17-39385c2a4dd0" /></kbd>

## Features
- Wireless control over TCP (UDP coming soon... i guess)

## Pro Tips for Best Experience
- **Perfect for story-based games** - Works great with single-player adventures
- **No internet needed** - Turn on mobile hotspot or laptop hotspot, connect both devices to it
- **Zero latency** - Direct device-to-device connection = 0ms lag, no buffer

## Requirements
- Linux Base OS
- Android device with [Ponio](https://github.com/ashudevcodes/ponio_android_controller) app installed
- Both devices connected to the same WiFi network

## Usage
1. Run the server
2. Open the mobile app
3. Connect
4. Play

## Use Cases

- Couch gaming on your HTPC
- Retro gaming with emulators
- Playing story-driven games from your couch
- Testing games with controller input
- Gaming when you don't have a physical gamepad handy

### Download

> [!NOTE]
> This will download **ponio** to the current directory and make the downloaded file executable

```sh

curl -L -o ponio https://github.com/ashudevcodes/ponio/releases/latest/download/ponio.tar.gz

tar -xzvf ponio.tar.gz
cd ponio

```
## Setup (one-time)

Ponio create virtual gamepad via `/dev/uinput`. Kernel restrict this to root by default — one-time permission setup needed.

Two modes:

**Permanent (recommended)** — udev rule + group membership, survives reboot:
```bash
sudo ./install-uinput.sh --permanent
```
Log out and back in after (group change needs new session).

**Temp** — quick test, resets on reboot, must rerun each restart:
```bash
sudo ./install-uinput.sh --temp
```

```sh
# To run the server
chmod +x ponio
./ponio

```

## Troubleshooting

| Problem | Fix |
|---|---|
| `Permission denied /dev/uinput` | Run setup script, log out/in |
| Gamepad not detected in game | Check `evtest` list shows `ponio virtual gamepad` |
| Setup script fails | Ensure run with `sudo`, not as root user directly |

## Uninstall permissions

```bash

sudo rm /etc/udev/rules.d/99-ponio-uinput.rules
sudo rm /etc/modules-load.d/ponio-uinput.conf
sudo gpasswd -d $USER uinput

`````

## Built With
- [libevdev](https://www.freedesktop.org/software/libevdev/doc/latest/index.html) - To handle joystick command inputs

## License

[MIT](./LICENSE)

## Links

[Ponio Android App](https://github.com/ashudevcodes/ponio_android_controller)

## Contributing

Got ideas? Cool. Issues? Sure. Pull requests? Why not
