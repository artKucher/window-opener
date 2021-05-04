/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C" {
  #include "user_interface.h"
}

const char HTTP_HEADER[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>Device configuration</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px; font-size:1em; margin-bottom: 10px} input{width:100%; box-sizing: border-box;} body{text-align: center;font-family:verdana;} button{border:1px solid #2b2a28;background-color:transparent;color:#2b2a28;font-size:17px;padding:16px; width: 100%;} button:hover{background-color:#f3f3f3} .q{float: right;width: 64px;text-align: right;}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEADER_END[] PROGMEM      = "</head><body><div style='text-align:left;display:inline-block;min-width:330px; max-width:500px; width: 50%;'>";
const char HTTP_LOGO_IMAGE[] PROGMEM      = "<div style='text-align: center; margin-bottom: 40px;'><svg style='display: block; margin-left: auto; margin-right: auto;' width='100' version='1.0' xmlns='http://www.w3.org/2000/svg' viewBox='0 0 906.000000 695.000000'  preserveAspectRatio='xMidYMid meet'>  <g transform='translate(0.000000,695.000000) scale(0.100000,-0.100000)' fill='#000000' stroke='none'> <path d='M4479 6597 c-20 -12 -995 -754 -2166 -1650 -2309 -1764 -2162 -1644 -2171 -1753 -6 -70 23 -133 79 -173 37 -27 50 -31 111 -31 l68 0 2072 1582 c1139 870 2081 1583 2092 1585 12 2 35 -5 51 -15 17 -11 950 -724 2075 -1586 l2045 -1566 68 0 c124 0 187 56 195 171 9 124 102 40 -1158 1049 l-1130 905 0 395 c0 228 -4 410 -10 430 -20 74 -108 130 -203 130 -78 0 -135 -32 -169 -95 l-28 -50 0 -253 0 -253 -27 16 c-16 9 -365 264 -778 566 -412 302 -781 565 -819 584 -84 43 -143 46 -197 12z'/> <path d='M4004 4878 c-38 -18 -44 -50 -44 -233 l0 -175 29 -32 29 -33 197 0 197 0 29 33 29 32 0 176 c0 182 -8 223 -45 238 -28 10 -398 5 -421 -6z'/> <path d='M4700 4868 c-19 -20 -20 -34 -20 -219 l0 -197 31 -26 c30 -26 31 -26 219 -26 104 0 196 3 205 6 37 15 45 56 45 247 0 171 -1 186 -21 211 l-20 26 -209 0 c-202 0 -210 -1 -230 -22z'/> <path d='M4013 4238 c-43 -21 -53 -64 -53 -230 0 -84 5 -170 10 -190 19 -68 19 -68 243 -68 143 1 207 4 218 13 34 26 39 54 39 232 0 186 -7 222 -49 245 -26 13 -379 12 -408 -2z'/> <path d='M4709 4221 l-29 -29 0 -196 c0 -183 1 -197 20 -216 19 -19 33 -20 224 -20 191 0 205 1 230 21 l26 20 0 195 0 196 -29 29 -29 29 -192 0 -192 0 -29 -29z'/> <path d='M7318 3473 c-53 -32 -80 -63 -96 -112 -9 -25 -12 -364 -12 -1291 0 -691 -3 -1305 -7 -1363 l-6 -107 -546 0 c-300 0 -551 4 -557 8 -6 4 -15 26 -18 48 -6 36 -11 43 -38 48 -51 10 -69 7 -89 -16 -17 -21 -18 -43 -19 -256 l0 -232 784 2 c773 3 785 3 813 24 15 11 39 42 53 70 l25 49 0 1517 0 1517 -21 27 c-11 15 -37 41 -58 58 -30 25 -48 31 -99 34 -54 3 -68 -1 -109 -25z'/> <path d='M1635 3476 c-37 -17 -69 -51 -87 -91 -10 -24 -14 -314 -16 -1521 l-2 -1491 28 -54 c16 -31 46 -67 71 -86 l43 -33 769 0 769 0 0 823 c0 798 -1 824 -19 841 -28 26 -79 17 -105 -17 -21 -28 -21 -37 -24 -638 l-2 -609 -560 0 -560 0 0 1328 c0 879 -4 1349 -11 1392 -12 76 -39 124 -82 147 -43 22 -172 28 -212 9z'/> <path d='M4445 3204 c-388 -39 -735 -218 -988 -509 -142 -164 -313 -459 -325 -561 -4 -35 -1 -44 23 -62 53 -42 91 -26 124 50 112 253 192 379 326 513 282 283 622 420 1000 402 287 -14 542 -95 752 -239 104 -70 255 -223 325 -328 75 -113 172 -314 202 -422 38 -133 46 -253 46 -692 0 -397 0 -402 23 -446 18 -37 27 -46 54 -48 25 -3 35 2 50 24 16 26 18 62 21 429 4 421 -4 548 -40 714 -61 283 -178 491 -403 716 -233 233 -478 370 -775 431 -112 23 -323 37 -415 28z'/> <path d='M4435 2874 c-150 -21 -261 -56 -399 -127 -339 -173 -572 -473 -631 -814 -13 -73 -15 -197 -15 -742 0 -360 3 -699 7 -753 7 -110 17 -125 74 -113 61 14 58 -17 64 785 5 617 8 747 22 810 17 84 58 202 82 240 37 59 232 308 268 341 83 80 201 139 370 186 93 26 110 28 313 29 118 0 220 1 227 2 12 2 33 54 33 83 0 23 -30 39 -105 56 -63 13 -258 24 -310 17z'/> <path d='M4981 2756 c-7 -8 -14 -30 -18 -49 -5 -30 -1 -37 28 -59 74 -55 186 -108 227 -108 45 0 52 10 52 70 0 45 -23 67 -134 125 -72 37 -135 46 -155 21z'/> <path d='M4435 2553 c-185 -33 -322 -106 -476 -251 -107 -102 -142 -160 -119 -201 16 -30 53 -43 87 -30 15 5 70 54 122 108 77 78 112 106 178 139 156 81 198 92 339 92 109 0 136 -3 202 -26 229 -78 379 -208 473 -408 56 -119 59 -155 59 -687 0 -469 1 -487 20 -511 30 -39 56 -41 93 -8 l32 28 0 509 c0 501 0 509 -23 593 -48 175 -128 308 -253 424 -138 127 -282 196 -488 230 -111 19 -139 19 -246 -1z'/> <path d='M5423 2404 c-37 -37 -28 -83 36 -177 66 -96 125 -253 142 -372 9 -64 14 -317 16 -880 3 -609 6 -791 16 -797 20 -13 71 -9 94 7 12 9 26 30 31 48 5 18 7 377 5 802 -3 619 -7 786 -19 852 -34 192 -102 365 -188 481 -39 52 -51 62 -76 62 -20 0 -40 -9 -57 -26z'/> <path d='M4421 2224 c-177 -47 -315 -183 -382 -376 l-29 -83 0 -707 0 -706 31 -26 c39 -33 69 -33 100 0 l24 26 5 711 c5 630 7 717 22 753 49 123 101 187 189 234 52 27 77 34 139 38 84 4 186 -15 252 -49 61 -31 134 -105 163 -163 39 -81 45 -156 45 -566 0 -435 -1 -430 69 -430 41 0 77 26 86 63 4 12 5 203 3 422 l-3 400 -34 88 c-77 202 -205 325 -386 372 -74 19 -222 19 -294 -1z'/> <path d='M4507 1910 c-52 -13 -87 -33 -119 -67 -54 -57 -58 -80 -58 -350 0 -136 3 -258 6 -269 6 -23 42 -44 73 -44 10 0 29 9 42 19 24 19 24 21 29 271 l5 252 28 24 c53 45 122 25 147 -42 6 -15 10 -305 10 -734 0 -691 0 -710 19 -720 31 -16 97 -12 112 7 12 13 14 144 14 757 l0 742 -26 49 c-20 41 -35 55 -80 77 -60 31 -143 42 -202 28z'/> <path d='M3707 1743 c-4 -3 -7 -64 -7 -134 0 -118 2 -130 23 -154 l22 -26 48 17 47 16 0 144 0 144 -63 0 c-35 0 -67 -3 -70 -7z'/> <path d='M3737 1259 c-43 -25 -44 -49 -38 -567 6 -473 7 -494 25 -508 29 -21 103 -18 115 4 5 10 7 227 3 535 -4 430 -7 521 -19 533 -17 16 -60 18 -86 3z'/> <path d='M4354 989 c-18 -20 -19 -44 -22 -409 -2 -353 -1 -388 14 -401 21 -18 99 -18 116 -1 10 10 14 98 18 380 3 202 2 381 -2 398 -13 52 -88 72 -124 33z'/> <path d='M5024 712 c-30 -19 -34 -55 -34 -274 0 -123 4 -228 8 -234 26 -40 89 -38 122 3 19 24 20 41 20 240 l0 215 -29 29 c-28 28 -64 37 -87 21z'/> <path d='M5317 541 c-14 -24 -17 -55 -17 -180 0 -167 2 -171 69 -171 52 0 71 29 71 108 0 38 3 103 6 145 6 72 6 76 -19 101 -37 37 -87 36 -110 -3z'/> </g> </svg>ymdom.ru</div>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<br/><form action=\"/wifi\" method=\"get\"><button>Настройки</button></form><br/><form action=\"/r\" method=\"post\"><button>Перезагрузить устройство</button></form>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM      = "<form style='width: 100%' method='get' action='wifisave'><div style='marggin-top: 30px;'>Выберите вашу домашнюю Wi-Fi сеть из списка, а затем введите пароль для подключения к ней.</div><input id='s' name='s' length=32 placeholder='Имя вашей Wi-Fi сети'><input id='p' name='p' length=64 type='password' placeholder='Пароль вашей Wi-Fi сети'><div>Оставьте эти поля пустыми,если хотите использовать вместе с Apple HomeKit.</div>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<button type='submit'>Сохранить</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM       = "<div class=\"c\"><a href=\"/wifi\">Сканировать Wi-Fi сети</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Параметры сохранены.<br/>Сейчас девайс попытается подключиться к заданной Wi-Fi сети.<br/>Если подключение не удасться, девайс создаст свою Wi-Fi сеть вновь и будет ожидать конфигурирования.</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

class WiFiManagerParameter {
  public:
    /** 
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ~WiFiManagerParameter();

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class WiFiManager;
};


class WiFiManager
{
  public:
    WiFiManager();
    ~WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter, returns false on failure
    bool          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEADER = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    //String        getEEPROMString(int start, int len);
    //void          setEEPROMString(int start, int len, String string);

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleInfo();
    void          handleReset();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();
    boolean       configPortalHasTimeout();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    int                    _max_params;
    WiFiManagerParameter** _params;

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
