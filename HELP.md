# LGL Power Profile Manager Help

This guide covers LGL Power Profile Manager 2.0.0.

LGL Power Profile Manager switches between tuned performance profiles on Fedora
and RHEL-based systems. It runs as a normal user and asks Polkit for
authorization when a profile is applied.

The window follows your desktop's GTK theme, including light or dark mode, the
accent colour, the monospace font, and your own `gtk.css`. Theme changes apply
while the app is open.

## Main window

- **Status** shows the active profile and whether tuned is responding. It refreshes every five seconds.
- **Profiles** lists every available tuned profile with a friendly label and a description. Custom profiles installed outside the built-in set appear after the known profiles.
- **Log** records the commands that were run and their output.
- **Reference** explains what each built-in profile is best for and when to avoid it.
- **Setup** opens automatically when `tuned-adm` is not installed.

## Applying a profile

- Open **Profiles** and select a profile.
- Choose **Apply Profile** in the header bar, from the main menu, or press `Ctrl+Enter`.
- Polkit asks for authorization. A cancelled prompt leaves the current profile unchanged.
- The result appears in a notice at the bottom of the window, and the command output is written to **Log**.

The automatic five-second refresh updates the active profile but does not
overwrite your pending selection. Use **Refresh** or `F5` to reload both the
status and the profile list.

## If tuned is not installed

The **Setup** tab shows the installation commands. On Fedora, RHEL, and CentOS:

- `sudo dnf install tuned`
- `sudo systemctl enable --now tuned`

Install tuned with your distribution's package manager on other systems, enable
the service, then restart this application.

## System tray and closing

When a compatible StatusNotifierItem tray host is available, closing the window
hides it in the tray instead of quitting. The tray menu provides:

- **Profile** - the active tuned profile
- **Refresh Status** - update the active profile
- **Show Window** - bring the window back
- **Quit** - exit the application

Without a tray host, closing the window quits the application. Launching the app
again presents the existing window instead of starting a second copy.

## Main menu and title bar

Use the menu button, `F10`, or tap `Alt` to open the main menu. The menu contains
Refresh, Apply Profile, Use System Title Bar, Help, About LGL Power Profile
Manager, and Quit.

**Use System Title Bar** lets the compositor draw the title bar and moves the
application header bar inside the window. Choose it again, or press
`Ctrl+Shift+M`, to return to the application title bar. The choice is remembered.

## Keyboard shortcuts

- `Ctrl+Q` - quit
- `Ctrl+Shift+M` - switch between application and system title bars
- `Ctrl+Enter` - apply the selected profile
- `F5` - refresh the active profile and profile list
- `F1` - open this help
- `F10` - open or close the main menu
- `Alt` (tap) - open or close the main menu

## Settings

Preferences are stored in
`~/.config/lgl-powerprofile-manager/settings.ini`. The only setting for now is
`[window] systemTitleBar`, which records the title bar choice. The file is
created when a setting is saved.

## power-profiles-daemon

When a tuned profile is applied it overrides power-profiles-daemon settings. If
your profile is reverted, mask power-profiles-daemon so the two services stop
competing:

- Disable: `sudo systemctl mask power-profiles-daemon`
- Re-enable: `sudo systemctl unmask power-profiles-daemon`

## More profile guidance

The **Reference** tab covers every built-in profile. The repository also
includes `docs/tuned-profiles.md` with the same guidance in a longer table.
