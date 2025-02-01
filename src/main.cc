#include "log.h"
#include "networktables/NetworkTableInstance.h"
#include "wiiuse.h"
#include <mutex>
#include <string_view>
#include <thread>
#include "motion_plus.h"

// Configure me!
constexpr unsigned int TEAM_NUMBER = 2234;
constexpr bool DEBUG = true; // (enable to connect to local simulation)

using namespace std::chrono_literals;

void WiimoteThread();

struct WiimoteData {
    double pitch, roll, yaw;
    bool a, b, one, two, plus, minus, home;
};

WiimoteData wiimote_data;
std::mutex wiimote_data_lock;

int main() {
    Log("ntwii 0.1");

    Log("Connecting to robot...");
    auto instance = nt::NetworkTableInstance::Create();
    const auto port = nt::NetworkTableInstance::kDefaultPort4;
    
    if (DEBUG) {
        instance.SetServer("127.0.0.1");
    } else {
        instance.SetServerTeam(TEAM_NUMBER);
    }
    instance.StartClient4("ntwii!");

    while (!instance.IsConnected()) {
        using namespace std::chrono_literals;

        std::this_thread::sleep_for(500ms);
    }

    Log("Connected to NT!");
    std::thread wiimote_thread {WiimoteThread};

    while (true) {
        auto table = instance.GetTable("ntwii");
        {
            std::lock_guard lg {wiimote_data_lock};
            table->PutBoolean("a", wiimote_data.a);
            table->PutBoolean("b", wiimote_data.b);
            table->PutBoolean("one", wiimote_data.one);
            table->PutBoolean("two", wiimote_data.two);
            table->PutBoolean("plus", wiimote_data.plus);
            table->PutBoolean("minus", wiimote_data.minus);
            table->PutBoolean("home", wiimote_data.home);

            table->PutNumber("pitch", wiimote_data.pitch);
            table->PutNumber("roll", wiimote_data.roll);
            table->PutNumber("yaw", wiimote_data.yaw);
        }
        std::this_thread::sleep_for(21ms);
    }
}

bool any_wiimote_connected(wiimote** wm, int number) {
    for (int i = 0; i < number; i++) {
        if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i])) {
            return true;
        }
    }
    return false;
}

void WiimoteThread() {
    Log("Wiimote thread active...");

    auto wiimotes = wiiuse_init(1);
    while (true) {
        Log("Trying to find wiimotes...");
        int conn_result = wiiuse_find(wiimotes, 1, 5);
        if (!conn_result) {
            Log("Failed to find any wiimotes! Trying again in 3s...");

            std::this_thread::sleep_for(3s);

            continue;
        }
        break;
    }
    Log("Connected to wiimotes!");
    wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1 | WIIMOTE_LED_4);
    // sometimes it bugs, double check!
    wiiuse_probe_motion_plus(wiimotes[0]);
    wiiuse_set_motion_plus(wiimotes[0], true);
    wiiuse_motion_sensing(wiimotes[0], true);

    wiiuse_rumble(wiimotes[0], true);
    std::this_thread::sleep_for(250ms);
    wiiuse_rumble(wiimotes[0], false);

    while (any_wiimote_connected(wiimotes, 1)) {
        wiiuse_poll(wiimotes, 1);

        auto wm = wiimotes[0];
        {
            std::lock_guard lg {wiimote_data_lock};
            
            wiimote_data.a = wm->btns & WIIMOTE_BUTTON_A;
            wiimote_data.b = wm->btns & WIIMOTE_BUTTON_B;
            wiimote_data.one = wm->btns & WIIMOTE_BUTTON_ONE;
            wiimote_data.two = wm->btns & WIIMOTE_BUTTON_TWO;
            wiimote_data.plus = wm->btns & WIIMOTE_BUTTON_PLUS;
            wiimote_data.minus = wm->btns & WIIMOTE_BUTTON_MINUS;
            wiimote_data.home = wm->btns & WIIMOTE_BUTTON_HOME;

            wiimote_data.pitch = wm->orient.pitch;
            wiimote_data.roll = wm->orient.roll;
            wiimote_data.yaw = wm->orient.yaw;
        }

        std::this_thread::sleep_for(20ms);
    }
}