# NESferatu

## Core
- [x] Implement 16 pixel mode for sprites
- [x] APU
- [x] Save states
- [ ] Remove memory as separate entity and integrate into the CPU
- [x] Use `NESFERATU_THROW` everywhere
- [x] Tidy up stacktrace usage on throw
- [x] Implement proper reset option
- [ ] Gate every debug function under debug define
- [ ] Debug: CPU/PPU/APU do not expose internal registers, but return a state object
- [ ] Proper logging: system is responsible
- [ ] Implement extra mappers

## App
- [x] Proper frame timing
- [x] No hardcoded ROM
- [ ] ImGui integration
    - [x] Load ROM from file
    - [x] Execution commands: start, stop, reset, step
    - [x] Volatite save states
    - [ ] Export/import save states to file
- [x] Reset audio buffer on reset/constructor

## Release
- [x] Compilation setup for Windows
- [ ] Setup CI build system to release
- [ ] Move tasks somewhere else and write proper README

## Bugs
- [ ] When dragging the SDL window, the emulator should pause, but it catches up on release.
    - Same thing happens when opening a file, pause the emulator while doing so.
- [ ] Do not run audio processes if there's no cartridge loaded
- [x] Loading a ROM while another ROM is running breaks the emulation
- [ ] The stepping on the application is awful to see, to be refactored
- [ ] It's not possible to disable sound