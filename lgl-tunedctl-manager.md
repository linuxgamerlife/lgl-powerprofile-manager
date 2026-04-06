# tuned-adm Profiles Explained

## Is this Fedora only?

`tuned-adm` is not exclusive to Fedora, but it is most commonly used and best integrated there.

It is part of the **tuned** service, which is available on other distributions (mainly Red Hat-based ones like RHEL, CentOS, Rocky, AlmaLinux). Some other distros may include it, but profiles and behaviour can vary.

So:
- The concept is cross-distro
- The exact profiles and tuning may differ slightly

---

## Profiles Overview

| Profile | What it means (simple) | When to use it | When NOT to use it |
|--------|----------------------|----------------|--------------------|
| accelerator-performance | Pushes hardware for maximum throughput with minimal latency states | GPU workloads, AI, heavy compute, hardware acceleration tasks | General desktop use, laptops, anything needing power efficiency |
| aws | Pre-tuned for AWS EC2 environments | Only if running inside AWS EC2 | Any local machine or non-AWS VM |
| balanced | Default mix of performance and power saving | Everyday use, safe baseline | If you specifically need max performance or max battery |
| balanced-battery | Balanced but leans toward saving power | Laptops on battery | Gaming, rendering, anything performance heavy |
| desktop | Tuned for responsiveness and UI smoothness | General desktop usage, light gaming | Servers or heavy compute workloads |
| hpc-compute | Optimised for high performance computing clusters | Scientific workloads, simulations, parallel compute | Desktop use, gaming, general workloads |
| intel-sst | Uses Intel Speed Select features | Only on supported Intel enterprise CPUs | AMD systems or normal consumer CPUs |
| latency-performance | Minimises latency, keeps CPU ready at all times | Real-time workloads, audio production, competitive gaming | Laptops, power-sensitive setups, general use |
| network-latency | Focuses on reducing network delay | Trading systems, real-time network apps, low-latency servers | Normal home networking, gaming (usually unnecessary) |
| network-throughput | Maximises data transfer rates | Servers pushing large volumes of data, high-speed networking (10G+) | Typical home users or standard broadband |
| optimize-serial-console | Improves responsiveness for serial console access | Headless servers managed via serial | Any normal desktop or GUI system |
| powersave | Minimises power usage aggressively | Laptops, low-power systems, always-on devices | Gaming, performance tasks |
| throughput-performance | High performance without going full low-latency | Servers, mixed workloads, heavy multitasking | Battery-powered devices or thermally limited systems |
| virtual-guest | Optimised for running inside a VM | If running inside VirtualBox, KVM, VMware, etc. | Bare metal installs |
| virtual-host | Optimised for hosting VMs | If running virtual machines on your system | Normal desktop use without VMs |

---

## Quick Guidance

- Use `balanced` or `desktop` for general use  
- Use `latency-performance` when you want maximum responsiveness  
- Use `powersave` for battery-focused setups  
- Only use specialised profiles (network, HPC, virtual) if your workload actually requires them  

---

## Key Point

`tuned-adm` does not add performance by magic.

It adjusts things like:
- CPU governor behaviour  
- Power states  
- Disk and network tuning  
- Kernel parameters  

The right profile depends entirely on your workload.
