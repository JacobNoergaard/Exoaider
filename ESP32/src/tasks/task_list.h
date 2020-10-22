#include <map>
#include <string>

#include "TaskInterface.h"
#include "tasks/DemoTask.h"
#include "tasks/DemoMotorControllerTask.h"
#include "tasks/DemoSensorBandTask.h"
#include "tasks/MySensorbandTask.h"
#include "tasks/simulatedSignal.h"

using namespace std;

void get_potential_tasks(std::map<string, TaskInterface*> &potential_tasks){
  potential_tasks["demo_task"] = new DemoTask();
  potential_tasks["demo_motor_controller"] = new DemoMotorControllerTask();
  potential_tasks["demo_left_sensor_band"] = new DemoSensorBandTask(true); // Left specialization
  potential_tasks["demo_right_sensor_band"] = new DemoSensorBandTask(false); // Right specialization
  potential_tasks["my_left_sensor_band_task"] = new MySensorbandTask(true);
  potential_tasks["my_right_sensor_band_task"] = new MySensorbandTask(false);
  potential_tasks["simulated_signal"] = new simulatedSignal();
}