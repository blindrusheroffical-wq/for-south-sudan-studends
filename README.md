# 🚀 AIRA Private 4G/5G Cellular Core & Telecom Router

![Country: India](https://img.shields.io/badge/Made_In-India_🇮🇳-orange.svg?style=for-the-badge)
![License: Proprietary](https://img.shields.io/badge/License-Strictly_Closed_Source-red.svg?style=for-the-badge)
![Platform: Ubuntu Linux](https://img.shields.io/badge/Platform-Ubuntu_Linux-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)
![Language: Python](https://img.shields.io/badge/Language-Python_3.x-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Core: Open5GS](https://img.shields.io/badge/Core-Open5GS_Framework-005A9C?style=for-the-badge)
![Database: MongoDB](https://img.shields.io/badge/Database-MongoDB-47A248?style=for-the-badge&logo=mongodb&logoColor=white)

An advanced, low-latency private cellular core network manager and automated packet router. This infrastructure automates **Open5GS** and **srsRAN** topologies, allowing a local machine to act as an independent Mobile Network Operator (MNO) Core using Software Defined Radio (SDR) hardware.

> **💡 OFFLINE HUMANITARIAN DEPLOYMENT PROFILE:** > This repository is customized specifically as an offline digital educational node for children in South Sudan. It enables zero-cost local VoIP calling, offline YouTube/Hugging Face caching, and an independent local AI knowledge base without requiring an active global internet backhaul.

---

## 🔒 Proprietary Notice & Licensing
> ### 🛑 STRICTLY CLOSED SOURCE & PROPRIETARY INTERFACES
> **Copyright (c) 2026 Adam Eehan. All rights reserved.only for south_sudan**
> 
> - This source code, orchestration matrix (`app.py` / `offline_core.py`), database structure (`db_subscriber.json`), radio profiles (`enb.conf`), and system configurations (`core_smf.yaml`, `core_upf.yaml`) are **strictly closed source** and confidential.
> - No permission is granted for modification, distribution, public mirroring, or commercial exploitation.
> - Authorized exclusively for private security testing and internal educational audits.

---

## 🏗️ Architectural Topology (Offline Loopback Mode)

```text
  +---------------------------------------+
  |        User Equipment (UE)            |  <-- Client Devices (Smartphones/IoT)
  |   (Wi-Fi & Cellular Enabled Hardware) |      Subnet Address: 10.45.0.X Pool
  +---------------------------------------+      QoS Priority Level 1 (Conversational Voice)
                      │
                      ▼ [Local Wi-Fi / Radio Link Access Connection]
  +---------------------------------------+
  |    ESP32 Access Point & NAT Router    |  <-- Hardware Interface (esp32_router.ino)
  |   (Local Gateway Instance: 10.45.0.1) |      lwIP Stack & Fake DNS Captive Portal Active
  +---------------------------------------+
                      │
                      ▼ [GTP Tunnel / Embedded Pipeline Stream]
  +---------------------------------------+
  |       Open5GS Core Framework          |  <-- Session, Routing & Voice Orchestration
  |  (SMF on 127.0.0.5 | UPF on 127.0.0.7)|      Local Gateway DNS Resolution (10.45.0.1)
  +---------------------------------------+
                      │
                      ▼ [ogstun Virtual Tunnel Interface Layer]
  +---------------------------------------+
  |    Automated NAT Routing Engine       |  <-- Managed via app.py (SIP/VoIP Enabled)
  |   (Linux Kernel IP Forwarding Matrix) |      Unlimited Local Bandwidth Matrix (1 Gbps)
  +---------------------------------------+
                      │
                      ▼ [Internal Loopback Matrix]
  +---------------------------------------+
  |       AIRA Central Local Server       |  <-- Offline Content Delivery Network (CDN)
  |      (Offline Flask Micro-Core)       |      Serves Cached YouTube & Hugging Face AI Data
  +---------------------------------------+
