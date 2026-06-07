# -*- coding: utf-8 -*-
"""
AIRA Cellular Infrastructure Core Orchestrator & Multi-Protocol Packet Router
Copyright (c) 2026 Adam Eehan. All rights reserved.
STRICTLY CLOSED SOURCE - PROPRIETARY INTERFACES
FEATURES DEPLOYED: Offline Local Loopback Routing, VoIP/SIP NAT Matrix
DESIGNED FOR: Offline Educational Node (South Sudan Project) - MADE IN INDIA 🇮🇳
"""

import os
import sys
import subprocess
import time

# Packet Crafting ഫീച്ചറിനായി Scapy ഇമ്പോർട്ട് ചെയ്യുന്നു
try:
    from scapy.all import IP, UDP, TCP, Raw, send
except ImportError:
    print("[INFO] Scapy module missing. Installing requirements...")
    subprocess.run("sudo pip3 install scapy", shell=True, check=True)
    from scapy.all import IP, UDP, TCP, Raw, send

def trigger_system_routine(command, profile_name):
    print(f"[INFO] Initializing Execution: {profile_name}")
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError as err:
        print(f"[CRITICAL] Error Detected During Execution Of {profile_name}: {err}")
        sys.exit(1)

def inject_custom_internet_packet(target_ip="10.45.0.1", protocol="UDP", payload_data="AIRA Network Packet"):
    """
    സ്വന്തമായി ഇന്റർനെറ്റ് ഡാറ്റാ പാക്കറ്റുകൾ ക്രാഫ്റ്റ് ചെയ്ത് 
    നമ്മുടെ 4G/5G കോറിലേക്ക് ഇൻജക്ട് ചെയ്യുന്ന മാസ്റ്റർ ഫങ്ഷൻ! 😏🔥
    """
    print(f"\n[PACKET ENGINE] Crafting Custom Raw Local Packet to {target_ip} ({protocol})...")
    
    # 1. IP Layer - നമ്മുടെ പ്രൈവറ്റ് 5G സബ്‌സ്‌ക്രൈബർ ഐപിയിൽ നിന്ന് ലോക്കൽ കോർ ഗേറ്റ്‌വേയിലേക്ക് (10.45.0.1) വിലാസം വെക്കുന്നു
    ip_layer = IP(src="10.45.0.5", dst=target_ip)
    
    # 2. Protocol Layer - VoIP/SIP ആവശ്യങ്ങൾക്ക് UDP, സാധാരണ ഡാറ്റയ്ക്ക് TCP
    if protocol.upper() == "UDP":
        proto_layer = UDP(sport=5060, dport=5060) # SIP സിഗ്നലിംഗ് പോർട്ട്
    else:
        proto_layer = TCP(sport=80, dport=80) # നമ്മുടെ ഓഫ്‌ലൈൻ ലോക്കൽ വെബ് പോർട്ട്
        
    # 3. Payload Layer - നമ്മൾ പാക്കറ്റിന്റെ ഉള്ളിൽ ഒളിപ്പിച്ചു വെക്കുന്ന യഥാർത്ഥ ഡാറ്റ
    payload = Raw(load=payload_data)
    
    # 4. പാക്കറ്റ് എൻകാപ്സുലേഷൻ (Encapsulation)
    custom_packet = ip_layer / proto_layer / payload
    
    try:
        # 5. പാക്കറ്റ് നമ്മുടെ 'ogstun' വെർച്വൽ ടണൽ വഴി ലോക്കൽ നെറ്റ്‌വർക്കിലേക്ക് പായിക്കുന്നു
        send(custom_packet, verbose=False)
        print(f"[SUCCESS] Custom Packet Injected: '{payload_data}' -> Route Complete.")
    except Exception as e:
        print(f"[ERROR] Packet Injection Failed: {e}")

def run_telecom_core_pipeline():
    # Enforcing root access verification before system execution
    if os.geteuid() != 0:
        print("[CRITICAL] Privilege Validation Failure: Run the script using 'sudo python3 app.py'")
        sys.exit(1)

    print("\n=======================================================")
    print("  AIRA SECURE CELLULAR CORE NETWORK ORCHESTRATION NODE ")
    print("=======================================================")

    # Phase 1: Linux Kernel Configuration
    trigger_system_routine("sysctl -w net.ipv4.ip_forward=1", "Linux Kernel IP Forwarding Interface")

    # Phase 2: Firewall Table Architecture & Local Loopback Routing
    # സൗത്ത് സുഡാനിൽ പുറത്തേക്ക് ഇന്റർനെറ്റ് ഇല്ലാത്തതിനാൽ ട്രാഫിക് പൂർണ്ണമായും ഇന്റേണൽ ആയി മാപ്പ് ചെയ്യുന്നു
    GTP_TUNNEL = "ogstun"
    
    # ലോക്കൽ സബ്‌നെറ്റിൽ നിന്നുള്ള പാക്കറ്റുകൾ വെർച്വൽ ടണൽ വഴി റൂട്ട് ചെയ്യാനുള്ള iptables നിയമങ്ങൾ
    trigger_system_routine(f"iptables -t nat -A POSTROUTING -s 10.45.0.0/24 -j MASQUERADE", "Local Subnet Masquerading Rules")
    trigger_system_routine(f"iptables -A FORWARD -i {GTP_TUNNEL} -j ACCEPT", "Inbound Routing Data Filters")
    trigger_system_routine(f"iptables -A FORWARD -o {GTP_TUNNEL} -j ACCEPT", "Outbound Cellular Packet Redirection")

    # Phase 3: VoIP / SIP Voice Packet Routing Protocol Layer
    SIP_SIGNALING_PORT = 5060
    RTP_AUDIO_START = 10000
    RTP_AUDIO_END = 20000
    
    trigger_system_routine(f"iptables -A FORWARD -p udp --dport {SIP_SIGNALING_PORT} -j ACCEPT", "SIP Signaling Packet Firewall Exemption")
    trigger_system_routine(f"iptables -A FORWARD -p udp --dport {RTP_AUDIO_START}:{RTP_AUDIO_END} -j ACCEPT", "RTP Real-Time Voice Audio Packet Pipeline")

    # Phase 4: Initializing Core Daemon Subsystems
    trigger_system_routine("systemctl restart open5gs-smfd", "Session Management Function (SMF) Core Initialization")
    trigger_system_routine("systemctl restart open5gs-upfd", "User Plane Function (UPF) Packet Execution Pipeline")
    
    # Phase 5: Radio Access Network (srsRAN) Link Orchestration
    print("[INFO] Verifying Local RAN Profile Compatibility...")
    if os.path.exists("enb.conf"):
        print("[STATUS] Local Radio Station Template Verified via 'enb.conf'")
    else:
        print("[WARNING] Local 'enb.conf' profile target absent. Ensure path compliance post-clone.")

    # Phase 6: Database Profile Mapping & Cryptographic Key Storage
    trigger_system_routine("mongoimport --db open5gs --collection subscribers --file db_subscriber.json --jsonArray", "SIM Credential Profile Database Injection")

    print("\n=======================================================")
    print("[SUCCESS] Private Cellular Node Core Framework Is Online.")
    print("[STATUS] IP Tunnel Packets, srsRAN Templates, And VoIP Streams Associated Systematically.")
    print("=======================================================")

    # Phase 7: Live Packet Generation Test (ഹാൻഡ്‌സ്-ഓൺ മാജിക് 🧪)
    time.sleep(2) # സിസ്റ്റം സ്റ്റേബിൾ ആകാൻ ഒരു 2 സെക്കൻഡ് ഗ്യാപ്പ്
    
    # ഒരു ടെസ്റ്റ് VoIP/SIP സിഗ്നലിംഗ് പാക്കറ്റ് ലോക്കൽ കോറിലേക്ക് ജനറേറ്റ് ചെയ്യുന്നു
    inject_custom_internet_packet(target_ip="10.45.0.1", protocol="UDP", payload_data="AIRA_SIP_INVITE_CONTEXT")
    
    # നമ്മുടെ ലോക്കൽ വെബ് പോർട്ടിലേക്ക് ഒരു ഓഫ്‌ലൈൻ ഡാറ്റാ സ്ട്രീം പാക്കറ്റ് ടെസ്റ്റ് ചെയ്യുന്നു
    inject_custom_internet_packet(target_ip="10.45.0.1", protocol="TCP", payload_data="AIRA_CORE_5G_DATA_STREAM")

if __name__ == "__main__":
    run_telecom_core_pipeline()
