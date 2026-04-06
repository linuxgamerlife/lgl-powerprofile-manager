# Changelog

## [1.0.0] - 2026-04-05

### Initial Release

#### Core Features
- Qt6 QMainWindow-based GUI running as normal user
- pkexec used to apply tuned profiles with elevated privileges
- Auto-refresh of active profile status every 5 seconds (without overwriting pending user selection)
- System tray icon with square status indicator and active profile in tooltip

#### Tabs
- **Status** — Shows active profile and tuned service status. Includes note on power-profiles-daemon conflict with mask/unmask commands
- **Profiles** — Scrollable radio button list with human-friendly labels and descriptions. Apply button triggers pkexec tuned-adm
- **Log** — Output log from tuned-adm commands with colour-coded messages
- **Reference** — Full profile table with label and description columns
- **Setup** — Shown automatically when tuned-adm is not found on PATH

#### Profile Data
- 15 built-in tuned profiles with human-friendly labels and descriptions
- Profiles ordered by desktop/gaming relevance rather than alphabetically
- Same order maintained in both Profiles tab and Reference tab
- Custom/unknown profiles from tuned-adm list appended at the end

#### Build
- CMake 3.16+, C++20, Qt6 Core + Widgets
- Flat source layout (no src/ subdirectory) matching project conventions
- .clang-format and .clang-tidy configured
