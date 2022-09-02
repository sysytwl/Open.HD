//
// Created by consti10 on 31.07.22.
//

#ifndef OPENHD_OPENHD_OHD_TELEMETRY_SRC_AIRTELEMETRYSETTINGS_H_
#define OPENHD_OPENHD_OHD_TELEMETRY_SRC_AIRTELEMETRYSETTINGS_H_

#include "openhd-settings.hpp"
#include "openhd-settings2.hpp"
#include <map>

// Settings for telemetry, only valid on an air pi (since only on the air pi we connect the FC)
// Note that we do not have any telemetry settings r.n for the ground (since forwarding is a task ov ohd_interface, not ohd_telemetry)
namespace openhd{

static const std::string TELEMETRY_SETTINGS_DIRECTORY=std::string(BASE_PATH)+std::string("telemetry/");

// Default for ardupilot and more
static constexpr int DEFAULT_UART_BAUDRATE=115200;
static constexpr int UART_CONNECTION_TYPE_DISABLE=0;
static constexpr int DEFAULT_UART_CONNECTION=UART_CONNECTION_TYPE_DISABLE; // Default to UART disabled (FC)

struct AirTelemetrySettings{
  // 0: Disable
  // 1: RPI UART0 (/dev/serial0)
  // 2: RPI UART1 (/dev/serial1)
  // 3: UART USB ADAPTER (/dev/ttyUSB0)
  //  My ardupilot shows up as either /dev/ttyACM0 or /dev/ttyACM1 if I connect it via
  // usb cable (micro usb on FC to rpi USB port)
  // 4: /dev/ttyACM0
  // 5: /dev/ttyACM1
  int fc_uart_connection_type=DEFAULT_UART_CONNECTION;
  int fc_uart_baudrate=DEFAULT_UART_BAUDRATE;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AirTelemetrySettings,fc_uart_connection_type,fc_uart_baudrate);

static bool validate_uart_connection_type(int type){
  return type >=0 && type <=5;
}

// If disabled, return nullopt
// If enabled, return the linux fd string for this (UART) connection type
static std::optional<std::string> uart_fd_from_connection_type(int connection_type){
  assert(validate_uart_connection_type(connection_type));
  switch (connection_type) {
	case 0: return std::nullopt;
	case 1:return "/dev/serial0";
	case 2:return "/dev/serial1";
	case 3:return "/dev/ttyUSB0";
	case 4:return "/dev/ttyACM0";
	case 5:return "/dev/ttyACM1";
	default:
	  assert(true);
	  return std::nullopt;
  }
}

// based on mavsdk and what linux allows setting
// if a value is in the map, we allow the user to set it
static std::map<int,void*> valid_uart_baudrates(){
  std::map<int,void*> ret;
  ret[9600]=nullptr;
  ret[19200]=nullptr;
  ret[38400]=nullptr;
  ret[57600]=nullptr;
  ret[115200]=nullptr;
  ret[230400]=nullptr;
  ret[460800]=nullptr;
  ret[500000]=nullptr;
  ret[576000]=nullptr;
  ret[921600]=nullptr;
  ret[1000000]=nullptr;
  // I think it is sane to stop here, I doubt anything higher makes sense
  return ret;
}

static bool validate_uart_baudrate(int baudrate){
  const auto supported=valid_uart_baudrates();
  if(supported.find(baudrate)!=supported.end()){
	return true;
  }
  return false;
}

// 16 chars limit !
static constexpr auto FC_UART_CONNECTION_TYPE="FC_UART_CONN";
static constexpr auto FC_UART_BAUD_RATE="FC_UART_BAUD";

class AirTelemetrySettingsHolder:public openhd::settings::PersistentSettings<AirTelemetrySettings>{
 public:
  AirTelemetrySettingsHolder():
	  openhd::settings::PersistentSettings<AirTelemetrySettings>(TELEMETRY_SETTINGS_DIRECTORY){
	init();
  }
 private:
  [[nodiscard]] std::string get_unique_filename()const override{
	std::stringstream ss;
	ss<<"fc_uart.json";
	return ss.str();
  }
  [[nodiscard]] AirTelemetrySettings create_default()const override{
	return AirTelemetrySettings{};
  }
};


}

#endif //OPENHD_OPENHD_OHD_TELEMETRY_SRC_AIRTELEMETRYSETTINGS_H_
