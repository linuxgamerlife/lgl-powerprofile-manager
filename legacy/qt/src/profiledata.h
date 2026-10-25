#pragma once
#include <QString>
#include <QList>

// profiledata.h — Static metadata for all built-in tuned profiles.
//
// Profiles are ordered by most common use case: Desktop first, then
// Light Gaming, Gaming Performance, Virtualisation, and Others.
// This order is used in both the Profiles tab and the Reference tab.

struct ProfileInfo {
    QString name;
    QString label;
    QString description;
    QString whenToUse;
    QString whenNotToUse;
};

inline const QList<ProfileInfo>& builtinProfiles() {
    static const QList<ProfileInfo> list = {
        // ── Desktop ──────────────────────────────────────────────────────────
        {
            "balanced",
            "Safe Default",
            "Balanced performance and power saving for most systems.",
            "Everyday use, workstations, safe default for most systems.",
            "When you specifically need maximum performance or maximum battery life.",
        },
        {
            "desktop",
            "Daily Driver",
            "Snappy, responsive desktop for everyday use.",
            "General desktop usage, light gaming, daily productivity.",
            "Servers, headless systems, or heavy compute workloads.",
        },
        // ── Gaming ───────────────────────────────────────────────────────────
        {
            "latency-performance",
            "Light Gaming",
            "Low latency, CPU always ready, no power saving.",
            "Competitive gaming, audio production, real-time workloads.",
            "Laptops, power-sensitive setups, or general everyday use.",
        },
        {
            "accelerator-performance",
            "Gaming Performance",
            "Maximum CPU and hardware throughput, no power saving.",
            "GPU workloads, AI inference, heavy compute, hardware acceleration tasks.",
            "General desktop use, laptops, anything needing power efficiency.",
        },
        {
            "throughput-performance",
            "Heavy Tasks",
            "Best for video encoding, compiling, and other sustained CPU workloads.",
            "Heavy multitasking and sustained workloads on mains power.",
            "Battery-powered devices or thermally constrained systems.",
        },
        // ── Network ──────────────────────────────────────────────────────────
        {
            "network-latency",
            "Low Latency Network",
            "Reduces network latency, good for Parsec and other remote desktop/streaming clients.",
            "Remote desktop streaming, trading systems, real-time network applications.",
            "Normal home networking - overhead is unnecessary for typical use.",
        },
        // ── Virtualisation ───────────────────────────────────────────────────
        {
            "virtual-guest",
            "VM Guest",
            "Install this app inside your QEMU/KVM virtual machine and apply this profile there.",
            "When running as a guest inside QEMU/KVM.",
            "Bare metal installations.",
        },
        {
            "virtual-host",
            "VM Host",
            "Run this profile on your machine when primarily running QEMU/KVM virtual machines.",
            "When hosting QEMU/KVM virtual machines on bare metal.",
            "Normal desktop use without active virtual machine hosting.",
        },
        // ── Battery ──────────────────────────────────────────────────────────
        {
            "balanced-battery",
            "Battery Saving",
            "Keep your laptop alive longer without killing performance entirely.",
            "Laptops on battery power with mixed workloads.",
            "Gaming, rendering, or any performance-heavy task.",
        },
        {
            "powersave",
            "Max Battery",
            "Squeeze every last drop of battery life out of your laptop.",
            "Laptops on battery, low-power always-on devices.",
            "Gaming, rendering, or any performance-sensitive task.",
        },
        // ── Others ───────────────────────────────────────────────────────────
        {
            "hpc-compute",
            "HPC / Scientific",
            "Parallel compute and scientific simulations.",
            "Scientific simulations, parallel compute, HPC cluster nodes.",
            "Desktop use, gaming, or general single-user workloads.",
        },
        {
            "network-throughput",
            "High Throughput Network",
            "Large data transfers, 10GbE+.",
            "Servers pushing large volumes of data, 10GbE+ networking environments.",
            "Home users or standard broadband connections.",
        },
        {
            "optimize-serial-console",
            "Headless / Serial",
            "Servers managed via serial console.",
            "Headless servers managed exclusively via serial console.",
            "Any normal desktop or GUI-based system.",
        },
        {
            "intel-sst",
            "Intel SST",
            "Intel enterprise CPUs with Speed Select only.",
            "Supported Intel enterprise CPUs with SST capability only.",
            "AMD systems or standard consumer Intel CPUs without SST.",
        },
        {
            "aws",
            "AWS EC2",
            "Cloud virtual machines on AWS only.",
            "Only when running inside AWS EC2 instances.",
            "Any local machine or non-AWS virtual machine.",
        },
    };
    return list;
}

// Returns the ProfileInfo for the given name, or a stub with empty fields
// if the name is not in the built-in list (e.g. a custom user profile).
inline ProfileInfo findProfile(const QString& name) {
    for (const ProfileInfo& p : builtinProfiles()) {
        if (p.name == name) return p;
    }
    return ProfileInfo{name, {}, {}, {}, {}};
}
