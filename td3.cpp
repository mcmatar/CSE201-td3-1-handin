#include <iostream>
#include "td3.hpp"
#include "support.hpp"
#include <stdlib.h>
#include <math.h>       // sin, cos
#include <assert.h>
#include <vector>
#include <algorithm>

using namespace std;

using namespace support;

double* extend_array(double* array, int length, int new_size) {
    double* narr = new double[new_size];

    for (int i = 0; i < length; i++) {
        narr[i] = array[i];
    }

    for (int j = length; j < new_size; j++) {
        narr[j] = 0;
    }

    delete array;

  return narr; // YOU CAN CHANGE THIS
}

double* shrink_array(double* array, int length, int new_size) {
  double* narr = new double[new_size];

  for (int i = 0; i < new_size; i++) {
        narr[i] = array[i];
  }

  delete array;

  return narr; // YOU CAN CHANGE THIS
}

double* append_to_array(double element,
                        double* array,
                        int &current_size,
                        int &max_size) {

  if (current_size == max_size){
        int integer = current_size + 5;
        double* narr = extend_array(array, current_size, integer);
        narr[current_size] = element;
        current_size += 1;
        max_size += 5;
        return narr;
  } else {
        array[current_size] = element;
        current_size += 1;
        return array;

  }
}

double* remove_from_array(double* array,
                          int &current_size,
                          int &max_size) {
  current_size -= 1;
  double* narr = new double[current_size];
  if ((max_size - current_size) >= 5){
        narr = shrink_array(narr, max_size, (max_size - 5));
        for (int i = 0; i < current_size; i++) {
            narr[i] = array[i];
        }

        max_size -= 5;

  } else {
        for (int i = 0; i < current_size; i++) {
            narr[i] = array[i];
        }
  }

  delete array;
  return narr;
}

bool simulate_projectile(const double magnitude, const double angle,
                         const double simulation_interval,
                         double *targets, int &tot_targets,
                         int *obstacles, int tot_obstacles,
                         double* &telemetry,
                         int &telemetry_current_size,
                         int &telemetry_max_size) {
  bool hit_target, hit_obstacle;
  double v0_x, v0_y, x, y, t;
  double PI = 3.14159265;
  double g = 9.8;

  double old_x;
  double old_y;

  v0_x = magnitude * cos(angle * PI / 180);
  v0_y = magnitude * sin(angle * PI / 180);

  t = 0;
  x = 0;
  y = 0;
  old_x = 0;
  old_y = 0;

  hit_target = false;
  hit_obstacle = false;
  while (y >= 0 && (! hit_target) && (! hit_obstacle)) {
    double * target_coordinates = find_collision(old_x, old_y, targets, tot_targets);
    if (target_coordinates != NULL) {
      remove_target(targets, tot_targets, target_coordinates);
      hit_target = true;
    } else if (find_collision(old_x, old_y, obstacles, tot_obstacles) != NULL) {
      hit_obstacle = true;
    } else {

      // Check if there is enough space in the telemetry array
      if (telemetry_current_size + 3 >= telemetry_max_size) {
          // Extend the telemetry array if needed
          int new_max_size = telemetry_max_size * 2;
          double* new_telemetry = new double[new_max_size];

          if (new_telemetry == nullptr) {
              // Failed to allocate memory for the extended telemetry array
              return false;
          }

          // Copy the existing data to the new array
          for (int i = 0; i < telemetry_current_size; i++) {
              new_telemetry[i] = telemetry[i];
          }

          // Delete the old telemetry array and update the pointers and sizes
          delete[] telemetry;
          telemetry = new_telemetry;
          telemetry_max_size = new_max_size;
      }

      // Record telemetry data: time, x, and y
      telemetry[telemetry_current_size++] = t;
      telemetry[telemetry_current_size++] = x;
      telemetry[telemetry_current_size++] = y;

      old_x = x;
      old_y = y;

      t = t + simulation_interval;
      y = v0_y * t  - 0.5 * g * t * t;
      x = v0_x * t;
    }

  }

  return hit_target;
}

//This sorting function was inspired from an outside source.
void sort(double *array, int array_size) {
  for (int j = 0; j < array_size; j += 3){
    for (int i = j; i < array_size; i += 3){
      double t,x,y;
      t = array [j];
      x = array [j+1];
      y = array [j+2];
      if (array [i]<=t){
          array [j] = array [i];
          array [j+1] = array [i+1];
          array [j+2] = array [i+2];
          array [i] = t;
          array [i+1] = x;
          array [i+2] = y;

      }
    }
  }
}

void merge_telemetry(double **telemetries,
                     int tot_telemetries,
                     int *telemetries_sizes,
                     double* &global_telemetry,
                     int &global_telemetry_current_size,
                     int &global_telemetry_max_size) {
  // Calculate the total number of elements in all telemetries
  global_telemetry_current_size = 0;
  for (int i = 0; i < tot_telemetries; ++i) {
    global_telemetry_current_size += telemetries_sizes[i];
  }

  // Ensure global_telemetry_max_size is sufficient, or allocate more memory if needed
  if (global_telemetry_max_size < global_telemetry_current_size) {
    if (global_telemetry) {
      delete[] global_telemetry;
    }
    global_telemetry = new double[global_telemetry_current_size];
    global_telemetry_max_size = global_telemetry_current_size;
  }

  // Copy data from individual telemetries to the global_telemetry
  int current_index = 0;
  for (int i = 0; i < tot_telemetries; ++i) {
    std::copy(telemetries[i], telemetries[i] + telemetries_sizes[i], global_telemetry + current_index);
    current_index += telemetries_sizes[i];
  }
  sort(global_telemetry,global_telemetry_current_size);
}






















