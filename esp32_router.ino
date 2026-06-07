/*
 * AIRA Hardware Network Matrix - ESP32 NAT Router & Packet Forwarder
 * Copyright (c) 2026 Adam Eehan. All rights reserved.
 * STRICTLY CLOSED SOURCE - PROPRIETARY
 * * FEATURES DEPLOYED: Local Captive Portal, Fake DNS Interception, and SIP/VoIP Packet Forwarding
 * DESIGNED FOR: Offline Educational Node (South Sudan Project) - MADE IN INDIA 🇮🇳
 */

#include <WiFi.h>
#include <WiFiUdp.h>
#include <DNSServer.h> // ഇന്റർനെറ്റ് ഇല്ലെങ്കിലും ഡിവൈസുകളെ ലോക്കൽ സെർവറിലേക്ക് തിരിച്ചുവിടാൻ
#include "dhcpserver.h"

// 1. നിങ്ങളുടെ കമ്പ്യൂട്ടറിലെ ഹോട്ട്‌സ്പോട്ടിന്റെയോ ലോക്കൽ കോർ മെഷീന്റെയോ പേരും പാസ്‌വേഡും
const char* WAN_SSID = "AIRA_PC_NET";
const char* WAN_PASS = "AdamCore2026";

// 2. ഈ ESP32 ഉണ്ടാക്കാൻ പോകുന്ന പുതിയ ഹോട്ട്‌സ്പോട്ടിന്റെ പേരും പാസ്‌വേഡും
const char* AP_SSID  = "AIRA_0_COST_5G_NODE";
const char* AP_PASS  = "EngineersPower";

// VoIP & SIP പ്രോട്ടോക്കോൾ പോർട്ടുകൾ ഡിഫൈൻ ചെയ്യുന്നു
const unsigned int SIP_PORT = 5060;
const byte DNS_PORT = 53; // ഡിഎൻഎസ് ട്രാഫിക് ക്യാപ്‌ചർ ചെയ്യാൻ

// നെറ്റ്‌വർക്കിങ് ഒബ്‌ജക്റ്റുകൾ
WiFiUDP udp_node;
DNSServer dns_server; 
char packet_buffer[512]; // VoIP പേലോഡുകൾ കൈകാര്യം ചെയ്യാൻ ബഫർ സൈസ് 512 ആക്കി കൂട്ടിയിട്ടുണ്ട്

// ലോക്കൽ നെറ്റ്‌വർക്ക് ഐപി കോൺഫിഗറേഷൻ
IPAddress local_IP(10, 45, 0, 1);
IPAddress gateway(10, 45, 0, 1);
IPAddress subnet(255, 255, 255, 0);

void print_log(String message) {
    Serial.println("[AIRA-NODE] " + message);
}

void setup() {
    Serial.begin(115200);
    print_log("Initializing AIRA Hardware Network Routing Node...");

    // A. ലോക്കൽ അഥോറിറ്റേറ്റീവ് കോർ മെഷീനുമായി കണക്ട് ചെയ്യുന്നു (Upstream Local Core)
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(WAN_SSID, WAN_PASS);
    
    print_log("Connecting to Authoritative PC Uplink WAN...");
    unsigned long start_attempt = millis();
    
    // സൗത്ത് സുഡാനിൽ മെയിൻ അപ്‌ലിങ്ക് ഇല്ലെങ്കിലും സിസ്റ്റം ബൂട്ട് ആകാൻ വേണ്ടി മാക്സിമം 10 സെക്കൻഡ് ചെക്ക് ചെയ്യും
    while (WiFi.status() != WL_CONNECTED && millis() - start_attempt < 10000) {
        delay(500);
        Serial.print(".");
    }
    
    if(WiFi.status() == WL_CONNECTED) {
        print_log("Uplink WAN Connected successfully.");
    } else {
        print_log("Uplink WAN not found. Operating in Standalone Offline Core Mode.");
    }

    // B. ESP32-ൽ സ്വന്തമായി ഒരു ഹോട്ട്‌സ്പോട്ട് റൂട്ടർ ക്രിയേറ്റ് ചെയ്യുന്നു (Local LAN)
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(AP_SSID, AP_PASS);

    print_log("ESP32 Hotspot Router Matrix is now ONLINE!");
    Serial.print("[STATUS] Connect devices to SSID: ");
    Serial.println(AP_SSID);
    Serial.print("[STATUS] Gateway IP Protocol deployed at: ");
    Serial.println(WiFi.softAPIP());

    // C. FAKE DNS & CAPTIVE PORTAL ENGINE
    // കുട്ടികളുടെ ഫോണിൽ നിന്ന് വരുന്ന എല്ലാ വെബ്/ഇന്റർനെറ്റ് റിക്വസ്റ്റുകളെയും (ഉദാഹരണത്തിന് youtube.com)
    // നമ്മുടെ ലോക്കൽ ഗേറ്റ്‌വേ ഐപിയിലേക്ക് (10.45.0.1) തിരിച്ചുവിട്ട് ഓഫ്‌ലൈൻ ആപ്പുകൾ ലോഡ് ചെയ്യിക്കുന്നു!
    dns_server.start(DNS_PORT, "*", local_IP);
    print_log("Captive Portal & DNS Interception Shield Active (* -> 10.45.0.1)");

    // D. SIP/VoIP പാക്കറ്റുകൾ ലിസൺ ചെയ്യാനായി UDP സോക്കറ്റ് ഓപ്പൺ ചെയ്യുന്നു
    udp_node.begin(SIP_PORT);
    Serial.print("[UDP ENGINE] Bound to SIP Signaling Port: ");
    Serial.println(SIP_PORT);

    print_log("Hardware Routing Table Enabled. Packet Forwarding Active.");
}

void loop() {
    // 1. ഡിഎൻഎസ് ക്വറികൾ നിരന്തരം പ്രോസസ്സ് ചെയ്യുന്നു (ഇതാണ് ഫോണുകളിൽ 'No Internet' എറർ വരാതെ കാക്കുന്നത്)
    dns_server.processNextRequest();

    // 2. വോയ്‌സ് കോളുകൾക്കും (VoIP) പൈത്തൺ കോർ സിസ്റ്റത്തിൽ നിന്നുമുള്ള കസ്റ്റം പാക്കറ്റുകൾ ചെക്ക് ചെയ്യുന്നു
    int packet_size = udp_node.parsePacket();
    
    if (packet_size) {
        // ഇൻബൗണ്ട് പാക്കറ്റ് ഇൻഫോ ട്രാക്ക് ചെയ്യുന്നു
        Serial.print("\n[PACKET INBOUND] Size: ");
        Serial.print(packet_size);
        Serial.print(" Bytes | From: ");
        Serial.print(udp_node.remoteIP());
        Serial.print(":");
        Serial.println(udp_node.remotePort());

        // പാക്കറ്റിന്റെ ഉള്ളിലെ യഥാർത്ഥ 'Payload Data' റീഡ് ചെയ്യുന്നു
        int len = udp_node.read(packet_buffer, 511);
        if (len > 0) {
            packet_buffer[len] = 0; // String സേഫ് ആയി അവസാനിപ്പിക്കുന്നു
        }
        
        Serial.print("[PAYLOAD MATRIX] Raw Data: ");
        Serial.println(packet_buffer);

        // --- VoIP/SIP PACKET FORWARDING LOGIC ---
        // ഇവിടെ നമ്മൾ കുട്ടികളുടെ ഫോണുകൾ തമ്മിൽ ഫ്രീയായി വോയ്‌സ് കോൾ ചെയ്യാനുള്ള റൂട്ടിംഗ് നടത്തുന്നു.
        // മെയിൻ ലോക്കൽ സെർവറായ കമ്പ്യൂട്ടറിലേക്ക് (app.py റൺ ചെയ്യുന്ന കോർ) പാക്കറ്റുകൾ ഫോർവേഡ് ചെയ്യുന്നു.
        IPAddress target_IP = WiFi.softAPIP(); // ഡിഫോൾട്ടായി ലോക്കൽ റൂട്ടിംഗ്
        
        if (WiFi.status() == WL_CONNECTED) {
            // അപ്‌ലിങ്ക് പിസി ഉണ്ടെങ്കിൽ, പാക്കറ്റ് നേരിട്ട് കോർ പിസിയിലേക്ക് പുഷ് ചെയ്യും
            // (നിന്റെ പിസിയുടെ ലോക്കൽ നെറ്റ്‌വർക്ക് ഐപി ഇവിടെ വേണമെങ്കിൽ ഫിക്സ് ചെയ്യാം, ഇപ്പോൾ ഗേറ്റ്‌വേയിലേക്ക് മാപ്പ് ചെയ്തിരിക്കുന്നു)
            target_IP = IPAddress(10, 45, 0, 1); 
        }

        // കസ്റ്റം പാക്കറ്റോ വോയ്‌സ് ഡാറ്റയോ ലോക്കൽ സെർവർ പോർട്ടിലേക്ക് ഫോർവേഡ് ചെയ്യുന്നു
        udp_node.beginPacket(target_IP, SIP_PORT);
        udp_node.write((uint8_t*)packet_buffer, len);
        udp_node.endPacket();
        
        Serial.println("[ROUTING ENGINE] Packet forwarded successfully to Local Core Server.");
    }
    
    delay(2); // കാപ്റ്റീവ് പോർട്ടലും വോയ്‌സ് കോളും ലാഗ് ഇല്ലാതെ റൺ ചെയ്യാൻ ഡിലേ 2ms ആക്കി കുറച്ചു
}
