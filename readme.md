# Introduction

IOElectrify is a macOS kext which enables always-on power to Intel Thunderbolt hardware.

## In Depth

macOS expects the Intel Thunderbolt controller to be always powered on.
This opposed to the Windows implementation where Thunderbolt devices are configured through the BIOS.

Most ACPI implementations on Windows machines expose a Thunderbolt `force-power` method.

This method is published using the [Windows Management Instrumentation ACPI interface](https://msdn.microsoft.com/en-us/library/windows/hardware/dn614028(v=vs.85).aspx).

macOS does not support Windows Management Instrumentation out of the box, this is where IOElectrify comes in.

IOElectrify attaches to the `WTBT` ACPI object and determines the method with [guid](https://patchwork.kernel.org/patch/9942739/) `86ccfd48-205e-4a77-9c48-2021cbede341`.

For both the awake and sleep events, IOElectrify will respectively activate and de-activate the Intel Thunderbolt `force-power` setting.

## Configuration

IOElectrify attaches to ACPI identity `PNP0C14` with an `_UID` of `TBFP` by default.
This can be modified in the `Info.plist` as required.

## Tested

* Dell XPS 9360 - Alpine Ridge 2C `8086:1716`

## Credits

* [goodwin_c](https://www.tonymacx86.com/threads/usb-c-hotplug-not-working.223534/page-5#post-1580114): Thunderbolt force-power method
* [IOWMIFamily by dolnor](https://github.com/Dolnor/IOWMIFamily): macOS WMI interface